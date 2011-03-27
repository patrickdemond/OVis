/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQMainWindow.h"

#include "ui_ovQMainWindow.h"

#include <stdio.h>
#include <time.h>

#include <QActionGroup>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>
#include <QTreeWidget>

#include "vtkAnnotationLink.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkGlyphSource2D.h"
#include "vtkGraph.h"
#include "vtkGraphLayoutStrategy.h"
#include "vtkGraphLayoutView.h"
#include "vtkIdTypeArray.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkPNGWriter.h"
#include "vtkRenderedGraphRepresentation.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkViewTheme.h"
#include "vtkWindowToImageFilter.h"

// experimental
#include "vtkDataSetAttributes.h"
#include "vtkVariantArray.h"

#include "ovQDateDialog.h"
#include "ovQSearchDialog.h"
#include "source/ovOrlandoReader.h"
#include "source/ovOrlandoTagInfo.h"
#include "source/ovRestrictGraphFilter.h"
#include "source/ovSearchPhrase.h"
#include "source/ovSession.h"
#include "source/ovSessionReader.h"
#include "source/ovSessionWriter.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>
#include <vtksys/ios/sstream>

class ovQMainWindowProgressCommand : public vtkCommand
{
public:
  static ovQMainWindowProgressCommand *New() { return new ovQMainWindowProgressCommand; }
  void Execute( vtkObject *caller, unsigned long eventId, void *callData );
  Ui_ovQMainWindow *ui;

protected:
  ovQMainWindowProgressCommand() { this->ui = NULL; }
};


//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindowProgressCommand::Execute(
  vtkObject *caller, unsigned long eventId, void *callData )
{
  if( this->ui )
  {
    // display the progress
    double progress = *( static_cast<double*>( callData ) );
    int value = vtkMath::Floor( 100 * progress ) + 1;
    if( 100 < value ) value = 100;
    if( this->ui->progressBar->value() != value ) this->ui->progressBar->setValue( value );

    // show what's happening in the status bar
    if( 100 == value )
    {
      this->ui->statusbar->clearMessage();
    }
    else
    {
      QString message = QString( "" );
      if( ovOrlandoReader::SafeDownCast( caller ) )
      {
        message = QString( "Reading data..." );
      }
      else if( ovRestrictGraphFilter::SafeDownCast( caller ) )
      {
        message = QString( "Resolving visible vertices and edges..." );
      }
      else if( vtkGraphLayoutStrategy::SafeDownCast( caller ) )
      {
        message = QString( "Determining graph layout..." );
      }
    
      if( message.length() ) this->ui->statusbar->showMessage( message );

      // if we want the status bar to look smooth then we can call repaint on it here
      // however, let's not do that since it substantially slows down processing
      //this->ui->statusbar->repaint();
    }
  }
}

class ovQMainWindowSelectionCommand : public vtkCommand
{
public:
  static ovQMainWindowSelectionCommand *New() { return new ovQMainWindowSelectionCommand; }
  void Execute( vtkObject *caller, unsigned long eventId, void *callData );
  Ui_ovQMainWindow *ui;
  vtkGraph *Graph;

protected:
  ovQMainWindowSelectionCommand() { this->ui = NULL; this->Graph = NULL; }
};


//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindowSelectionCommand::Execute(
  vtkObject *caller, unsigned long eventId, void *callData )
{
  if( this->ui && this->Graph )
  {
    vtkAnnotationLink *link = vtkAnnotationLink::SafeDownCast( caller );
    if( NULL == link ) return;
    
    vtkSelectionNode *node0, *node1;
    vtkIdTypeArray *edgeIds, *vertexIds;

    // there will be two nodes, one are vertices and the other are edges
    node0 = link->GetCurrentSelection()->GetNode( 0 );
    node1 = link->GetCurrentSelection()->GetNode( 1 );

    if( vtkSelectionNode::EDGE == node0->GetFieldType() )
    {
      edgeIds = vtkIdTypeArray::SafeDownCast( node0->GetSelectionList() );
      vertexIds = vtkIdTypeArray::SafeDownCast( node1->GetSelectionList() );
    }
    else
    {
      edgeIds = vtkIdTypeArray::SafeDownCast( node1->GetSelectionList() );
      vertexIds = vtkIdTypeArray::SafeDownCast( node0->GetSelectionList() );
    }
    
    vtkStringArray *pedigreeArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetVertexData()->GetAbstractArray( "pedigree" ) );

    vtkStringArray *contentArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetEdgeData()->GetAbstractArray( "content" ) );

    // process the selected edge and vertex ids
    vtkstd::ostringstream stream;

    int numValues = vertexIds->GetNumberOfTuples();
    if( 0 < numValues )
    {
      stream << "<h3>Selected Vertices (" << numValues
             << ( 100 < numValues ? ", showing first 100" : "" )
             << ")</h3><ul>";
      for( int index = 0; index < numValues && index < 100; index++ )
      {
        stream << "<li>" << pedigreeArray->GetValue( vertexIds->GetValue( index ) ) << "</li>";
      }
      stream << "</ul>";

      if( 100 < numValues ) stream << "<p>List cropped at 100 vertices</p>";
    }

    numValues = edgeIds->GetNumberOfTuples();
    if( 0 < numValues )
    {
      stream << "<h3>Selected Edges (" << numValues
             << ( 100 < numValues ? ", showing first 100" : "" )
             << ")</h3><ul>";
      for( int index = 0; index < numValues && index < 100; index++ )
      {
        vtkIdType eId = edgeIds->GetValue( index );
        vtkIdType sId = this->Graph->GetSourceVertex( eId );
        vtkIdType tId = this->Graph->GetTargetVertex( eId );
        stream << "<li><b>" << pedigreeArray->GetValue( sId ) << "</b> is connected to <b>"
               << pedigreeArray->GetValue( tId ) << "</b>:<br />\"" 
               << contentArray->GetValue( eId ) << "\"</li>";
      }
      stream << "</ul>";

      if( 100 < numValues ) stream << "<p>List cropped at 100 edges</p>";
    }

    this->ui->graphSelectTextEdit->setHtml( QString( stream.str().c_str() ) );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::ovQMainWindow( QWidget* parent )
  : QMainWindow( parent )
{
  QMenu *menu;
  this->IsLoadingData = false;
  this->IsLoadingSession = false;
  this->IsCheckingMultipleTags = false;
  this->IsCheckingMultipleTags = false;
  this->SnapshotMagnification = 1;
  this->CurrentDataFileName = "";
  this->CurrentSessionFileName = "";
  this->CurrentLayoutStrategy = "Clustering2D";
  this->Session = vtkSmartPointer< ovSession >::New();
  
  this->ui = new Ui_ovQMainWindow;
  this->ui->setupUi( this );

  // connect the file menu items
  QObject::connect(
    this->ui->actionOpenData, SIGNAL( triggered() ),
    this, SLOT( slotOpenData() ) );
  QObject::connect(
    this->ui->actionTakeScreenshot, SIGNAL( triggered() ),
    this, SLOT( slotTakeScreenshot() ) );
  //this->ui->actionExit->setShortcuts( QKeySequence::Quit );
  QObject::connect( this->ui->actionExit, SIGNAL( triggered() ),
    qApp, SLOT( closeAllWindows() ) );
  
  // connect the view menu items
  QObject::connect(
    this->ui->actionReCenterGraph, SIGNAL( triggered() ),
    this, SLOT( slotReCenterGraph() ) );
  QObject::connect(
    this->ui->actionSetBackgroundSolid, SIGNAL( triggered() ),
    this, SLOT( slotSetBackgroundSolid() ) );
  QObject::connect(
    this->ui->actionSetBackgroundTop, SIGNAL( triggered() ),
    this, SLOT( slotSetBackgroundTop() ) );
  QObject::connect(
    this->ui->actionSetBackgroundBottom, SIGNAL( triggered() ),
    this, SLOT( slotSetBackgroundBottom() ) );

  QObject::connect(
    this->ui->actionSetVertexStyleToNone, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToNone() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToVertex, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToVertex() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToDash, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToDash() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToCross, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToCross() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToThickCross, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToThickcross() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToTriangle, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToTriangle() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToSquare, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToSquare() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToCircle, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToCircle() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToDiamond, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToDiamond() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToArrow, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToArrow() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToThickArrow, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToThickarrow() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToHookedArrow, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToHookedarrow() ) );
  QObject::connect(
    this->ui->actionSetVertexStyleToEdgeArrow, SIGNAL( triggered() ),
    this, SLOT( slotSetVertexStyleToEdgearrow() ) );
  
  QObject::connect(
    this->ui->actionSetLayoutStrategyToRandom, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToRandom() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToForceDirected, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToForceDirected() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToSimple2D, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToSimple2D() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToClustering2D, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToClustering2D() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToCommunity2D, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToCommunity2D() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToFast2D, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToFast2D() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToCircular, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToCircular() ) );
  // Not sure if cosmic trees are actually useful for this project
  //QObject::connect(
  //  this->ui->actionSetLayoutStrategyToCosmicTree, SIGNAL( triggered() ),
  //  this, SLOT( slotSetLayoutStrategyToCosmicTree() ) );
  // Cone seems to crash the app from time to time, so leaving it out for now
  //QObject::connect(
  //  this->ui->actionSetLayoutStrategyToCone, SIGNAL( triggered() ),
  //  this, SLOT( slotSetLayoutStrategyToCone() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToSpanTree, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToSpanTree() ) );
  
  QObject::connect(
    this->ui->actionMag1, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo1() ) );
  QObject::connect(
    this->ui->actionMag2, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo2() ) );
  QObject::connect(
    this->ui->actionMag3, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo3() ) );
  QObject::connect(
    this->ui->actionMag4, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo4() ) );
  QObject::connect(
    this->ui->actionMag5, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo5() ) );
  QObject::connect(
    this->ui->actionMag6, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo6() ) );
  QObject::connect(
    this->ui->actionMag7, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo7() ) );
  QObject::connect(
    this->ui->actionMag8, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo8() ) );
  QObject::connect(
    this->ui->actionMag9, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo9() ) );
  QObject::connect(
    this->ui->actionMag10, SIGNAL( triggered() ),
    this, SLOT( slotSetSnapshotMagnificationTo10() ) );

  // set up the menu action groups
  this->vertexStyleActionGroup = new QActionGroup( this );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToNone );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToVertex );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToDash );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToCross );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToThickCross );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToTriangle );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToSquare );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToCircle );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToDiamond );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToArrow );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToThickArrow );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToHookedArrow );
  this->vertexStyleActionGroup->addAction( this->ui->actionSetVertexStyleToEdgeArrow );
  
  this->layoutStrategyActionGroup = new QActionGroup( this );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToRandom );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToForceDirected );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToSimple2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToClustering2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCommunity2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToFast2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCircular );
  //this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCosmicTree );
  //this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCone );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToSpanTree );
  
  this->snapshotMagnificationActionGroup = new QActionGroup( this );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag1 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag2 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag3 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag4 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag5 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag6 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag7 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag8 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag9 );
  this->snapshotMagnificationActionGroup->addAction( this->ui->actionMag10 );

  // connect the session menu items
  QObject::connect(
    this->ui->actionOpenSession, SIGNAL( triggered() ),
    this, SLOT( slotOpenSession() ) );
  QObject::connect(
    this->ui->actionReloadSession, SIGNAL( triggered() ),
    this, SLOT( slotReloadSession() ) );
  QObject::connect(
    this->ui->actionSaveSession, SIGNAL( triggered() ),
    this, SLOT( slotSaveSession() ) );
  QObject::connect(
    this->ui->actionSaveSessionAs, SIGNAL( triggered() ),
    this, SLOT( slotSaveSessionAs() ) );

  // set up the observer to update the progress bar
  this->ProgressObserver = vtkSmartPointer< ovQMainWindowProgressCommand >::New();
  this->ProgressObserver->ui = this->ui;

  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategy( this->CurrentLayoutStrategy.c_str() );
  this->GraphLayoutView->GetLayoutStrategy()->AddObserver(
    vtkCommand::ProgressEvent, this->ProgressObserver );
  this->GraphLayoutView->DisplayHoverTextOn();
  this->GraphLayoutView->IconVisibilityOff();
  this->GraphLayoutView->SetGlyphType( VTK_VERTEX_GLYPH );
  this->GraphLayoutView->SetVertexColorArrayName( "color" );
  this->GraphLayoutView->ColorVerticesOn();
  this->GraphLayoutView->SetEdgeColorArrayName( "color" );
  this->GraphLayoutView->ColorEdgesOn();
  this->GraphLayoutView->SetScalingArrayName( "size" );
  this->GraphLayoutView->ScaledGlyphsOff();
  vtkRenderedGraphRepresentation* rep = vtkRenderedGraphRepresentation::SafeDownCast(
    this->GraphLayoutView->GetRepresentation() );
  rep->SetVertexHoverArrayName( "pedigree" );
  this->GraphLayoutView->SetInteractor( this->ui->graphLayoutWidget->GetInteractor() );
  this->ui->graphLayoutWidget->SetRenderWindow( this->GraphLayoutView->GetRenderWindow() );
  
  // set up the observer to update the graph selection
  this->SelectionObserver = vtkSmartPointer< ovQMainWindowSelectionCommand >::New();
  this->SelectionObserver->ui = this->ui;

  // add an observer to watch for graph node/edge selection
  rep->GetAnnotationLink()->AddObserver(
    vtkCommand::AnnotationChangedEvent, this->SelectionObserver );

  this->GraphLayoutViewTheme = vtkSmartPointer< vtkViewTheme >::New();
  this->GraphLayoutViewTheme->SetBackgroundColor( 0.7, 0.7, 0.7 );
  this->GraphLayoutViewTheme->SetBackgroundColor2( 0.9, 0.9, 0.9 );
  this->GraphLayoutViewTheme->SetPointSize( this->ui->vertexSizeSlider->value() );
  this->GraphLayoutViewTheme->SetLineWidth( this->ui->edgeSizeSlider->value() );
  this->GraphLayoutViewTheme->ScalePointLookupTableOff();
  this->GraphLayoutViewTheme->ScaleCellLookupTableOff();
  // TODO: authors are set to 1 and associations to 0.  This isn't very well designed, but
  //       things have been left this way for now since likely vertex coloring will expand
  //       substantially from its current behaviour
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetPointLookupTable() );
  lut->SetTableRange( 0, 1 );
  lut->SetNumberOfTableValues( 2 );
  lut->Build();
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  
  // set the colors of the vertex butons
  double *tableColor;
  char buffer[512];
  
  tableColor = lut->GetTableValue( 0 );
  sprintf( buffer, "color: %s; background-color: rgba( %d, %d, %d, %d )",
    ovIsOppositeColorWhite( tableColor ) ? "white" : "black",
    static_cast< int >( tableColor[0]*255 ),
    static_cast< int >( tableColor[1]*255 ),
    static_cast< int >( tableColor[2]*255 ),
    static_cast< int >( tableColor[3]*255 ) );
  this->ui->associationVertexColorPushButton->setStyleSheet( buffer );

  tableColor = lut->GetTableValue( 1 );
  sprintf( buffer, "color: %s; background-color: rgba( %d, %d, %d, %d )",
    ovIsOppositeColorWhite( tableColor ) ? "white" : "black",
    static_cast< int >( tableColor[0]*255 ),
    static_cast< int >( tableColor[1]*255 ),
    static_cast< int >( tableColor[2]*255 ),
    static_cast< int >( tableColor[3]*255 ) );
  this->ui->authorVertexColorPushButton->setStyleSheet( buffer );
  
  // set up the reader and filters
  this->OrlandoReader = vtkSmartPointer< ovOrlandoReader >::New();
  this->OrlandoReader->AddObserver( vtkCommand::ProgressEvent, this->ProgressObserver );
  this->RestrictGraphFilter = vtkSmartPointer< ovRestrictGraphFilter >::New();
  this->RestrictGraphFilter->AddObserver( vtkCommand::ProgressEvent, this->ProgressObserver );
  this->RestrictGraphFilter->SetInput( this->OrlandoReader->GetOutput() );
  this->SelectionObserver->Graph = this->RestrictGraphFilter->GetOutput();
  
  // set up the search phrases
  this->TextSearchPhrase = vtkSmartPointer< ovSearchPhrase >::New();
  this->AuthorSearchPhrase = vtkSmartPointer< ovSearchPhrase >::New();
  
  // set up the display property widgets
  QObject::connect(
    this->ui->authorCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotAuthorCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->genderComboBox, SIGNAL( currentIndexChanged( const QString& ) ),
    this, SLOT( slotGenderComboBoxCurrentIndexChanged( const QString& ) ) );
  QObject::connect(
    this->ui->writerComboBox, SIGNAL( currentIndexChanged( const QString& ) ),
    this, SLOT( slotWriterComboBoxCurrentIndexChanged( const QString& ) ) );
  QObject::connect(
    this->ui->vertexSizeSlider, SIGNAL( valueChanged( int ) ),
    this, SLOT( slotVertexSizeSliderValueChanged( int ) ) );
  QObject::connect(
    this->ui->edgeSizeSlider, SIGNAL( valueChanged( int ) ),
    this, SLOT( slotEdgeSizeSliderValueChanged( int ) ) );
  QObject::connect(
    this->ui->authorVertexColorPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotAuthorVertexColorPushButtonClicked() ) );
  QObject::connect(
    this->ui->associationVertexColorPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotAssociationVertexColorPushButtonClicked() ) );
  
  // set up the search restriction widgets
  QObject::connect(
    this->ui->textSearchSetPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTextSearchSetPushButtonClicked() ) );
  QObject::connect(
    this->ui->authorSearchSetPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotAuthorSearchSetPushButtonClicked() ) );

  // set up the date restriction widgets
  QObject::connect(
    this->ui->startSetPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotStartSetPushButtonClicked() ) );
  QObject::connect(
    this->ui->endSetPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotEndSetPushButtonClicked() ) );

  // set up the tag tree
  this->ui->tagTreeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
  this->ui->tagTreeWidget->setExpandsOnDoubleClick( false );
  QObject::connect(
    this->ui->tagTreeCheckButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeCheckButtonClicked() ) );
  QObject::connect(
    this->ui->tagTreeUnCheckButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeUnCheckButtonClicked() ) );
  QObject::connect(
    this->ui->tagTreeExpandButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeExpandButtonClicked() ) );
  QObject::connect(
    this->ui->tagTreeCollapseButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeCollapseButtonClicked() ) );
  QObject::connect(
    this->ui->tagTreeWidget, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
    this, SLOT( slotTagTreeItemChanged( QTreeWidgetItem*, int ) ) );
  QObject::connect(
    this->ui->tagTreeWidget, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    this, SLOT( slotTagTreeItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  
  this->ReadSettings();
  this->ApplyStateToSession();
};

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::~ovQMainWindow()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::closeEvent( QCloseEvent *event )
{
  if( this->MaybeSave() )
  {
    WriteSettings();
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

bool ovQMainWindow::MaybeSave()
{
  if( false /* TODO: check if session has changed */ )
  {
    QMessageBox::StandardButton messageBox;
    messageBox = QMessageBox::warning( this, tr("Application"),
      tr("The document has been modified.\n"
         "Do you want to save your changes?"),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );

    if( messageBox == QMessageBox::Save )
    {
      // TODO: save session here
      return true;
    }
    else if (messageBox == QMessageBox::Cancel)
    {
      return false;
    }
  }
  return true;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::WriteSettings()
{
  QSettings settings( "sharcnet", "ovis" );
  
  settings.beginGroup( "MainWindow" );
  settings.setValue( "size", this->size() );
  settings.setValue( "pos", this->pos() );
  settings.setValue( "maximized", this->isMaximized() );
  settings.endGroup();

  settings.beginGroup( "Splitters" );
  settings.setValue( "displayQuery", this->ui->displayQuerySplitter->saveState() );
  settings.setValue( "graphText", this->ui->graphTextSplitter->saveState() );
  settings.endGroup();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::ReadSettings()
{
  QSettings settings( "sharcnet", "ovis" );
  
  settings.beginGroup( "MainWindow" );
  if( settings.contains( "size" ) ) this->resize( settings.value( "size" ).toSize() );
  if( settings.contains( "pos" ) ) this->move( settings.value( "pos" ).toPoint() );
  if( settings.contains( "maximized" ) && settings.value( "maximized" ).toBool() )
    this->showMaximized();
  settings.endGroup();

  settings.beginGroup( "Splitters" );
  if( settings.contains( "displayQuery" ) )
    this->ui->displayQuerySplitter->restoreState( settings.value("displayQuery" ).toByteArray() );
  if( settings.contains( "graphText" ) )
    this->ui->graphTextSplitter->restoreState( settings.value("graphText" ).toByteArray() );
  settings.endGroup();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotOpenData()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Orlando File" ), "data", tr( "Orlando XML Files (*.xml)" ) );
  
  if( "" != fileName )
  {
    this->CurrentDataFileName = fileName.toStdString().c_str();
    this->LoadData();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTakeScreenshot()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr( "Save Snapshot" ), ".", tr( "PNG Files (*.png)" ) );
  
  if( "" != fileName )
  {
    // add .png if it isn't already there
    if( !fileName.endsWith( ".png" ) ) fileName.append( ".png" );

    vtkSmartPointer< vtkWindowToImageFilter > filter =
      vtkSmartPointer< vtkWindowToImageFilter >::New();
    vtkSmartPointer< vtkPNGWriter > writer = vtkSmartPointer< vtkPNGWriter >::New();
  
    filter->SetInput( this->GraphLayoutView->GetRenderWindow() );
    filter->SetMagnification( this->SnapshotMagnification );
    filter->Update();
    writer->SetInput( filter->GetOutput() );
    writer->SetFileName( fileName.toStdString().c_str() );
    writer->Write();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::LoadData()
{
  if( "" == this->CurrentDataFileName ) return;

  // define the tag list based on orlando tags
  // don't update the graph view until we're done checking tags
  this->IsLoadingData = true;
  
  ovTag *tag;
  QTreeWidgetItem *item, *parent;
  ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
  ovTagVector *tags = tagInfo->GetTags();
  
  bool tagParentError = false; // we only want to display the tag tree error once
  this->ui->tagTreeWidget->clear();
  for( ovTagVector::iterator it = tags->begin(); it != tags->end(); it++ )
  {
    tag = *it;
    if( tag->parent.length() )
    {
      // find the item with the parent's name
      parent = NULL;
      QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
      while( *treeIt )
      {
        if( tag->parent == (*treeIt)->text( 0 ).toStdString() ) parent = *treeIt;
        treeIt++;
      }
      
      // If we can't find a parent then something is wrong, let the user know
      // Should this error ever occur then likely ovOrlandoTagInfo::Add() is being
      // called improperly somewhere (make sure parents are added before children)
      if( NULL == parent && !tagParentError )
      {
        tagParentError = true;
        QMessageBox errorMessage( this );
        errorMessage.setWindowModality( Qt::WindowModal );
        errorMessage.setIcon( QMessageBox::Warning );
        errorMessage.setText( "Some tags may be missing from the tag tree." );
        errorMessage.exec();
      }
      else
      {
        item = new QTreeWidgetItem( parent );
      }
    }
    else
    {
      item = new QTreeWidgetItem( this->ui->tagTreeWidget );
    }
    item->setText(0, tag->name.c_str() );
    item->setCheckState( 0, tag->active ? Qt::Checked : Qt::Unchecked );
    item->setExpanded( tag->expanded );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
    
    // top level items have to be explicitely added to the tree
    if( 0 == tag->parent.length() ) this->ui->tagTreeWidget->addTopLevelItem( item );
  }
  
  // define the range of the edge lookup table based on the number of available tags
  // we add 1 to the number of tags because of the artificial "unsorted" tag
  double rgba[4], range[] = { 0, tags->size() };
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetCellLookupTable() );
  lut->SetTableRange( range );
  lut->SetHueRange( 0, 1 );
  lut->SetSaturationRange( 1, 1 );
  lut->SetValueRange( 1, 1 );
  lut->SetAlphaRange( 1, 1 );
  lut->SetNumberOfTableValues( tags->size() + 1 );
  lut->Build();
  
  // color tag names in the tree widget based on the LUT (expand the tab while we're at it
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    int index = tagInfo->FindTagIndex( (*treeIt)->text( 0 ).toStdString().c_str() );
    lut->GetTableValue( index, rgba );
    (*treeIt)->setBackground( 0, QColor( rgba[0]*255, rgba[1]*255, rgba[2]*255, rgba[3]*255 ) );
    (*treeIt)->setForeground( 0, ovIsOppositeColorWhite( rgba )
      ? QColor(255, 255, 255, 255)
      : QColor(0, 0, 0, 255) );

    (*treeIt)->setExpanded( true );
    treeIt++;
  }

  this->IsLoadingData = false;
  
  // load the orlando file and render
  this->OrlandoReader->SetFileName( this->CurrentDataFileName );
  this->GraphLayoutView->SetRepresentationFromInput( this->RestrictGraphFilter->GetOutput() );
  this->UpdateActiveTags();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotReCenterGraph()
{
  this->GraphLayoutView->ResetCamera();
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetBackgroundSolid()
{
  double rgba[4];
  this->GraphLayoutViewTheme->GetBackgroundColor( rgba );
  QColor c( 255. * rgba[0], 255. * rgba[1], 255. * rgba[2], 255 );
  c = QColorDialog::getColor( c, this );
  rgba[0] = c.redF();
  rgba[1] = c.greenF();
  rgba[2] = c.blueF();
  rgba[3] = c.alphaF();

  this->GraphLayoutViewTheme->SetBackgroundColor( rgba );
  this->GraphLayoutViewTheme->SetBackgroundColor2( rgba );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetBackgroundTop()
{
  double rgba[4];
  this->GraphLayoutViewTheme->GetBackgroundColor2( rgba );
  QColor c( 255. * rgba[0], 255. * rgba[1], 255. * rgba[2], 255 );
  c = QColorDialog::getColor( c, this );
  rgba[0] = c.redF();
  rgba[1] = c.greenF();
  rgba[2] = c.blueF();
  rgba[3] = c.alphaF();

  this->GraphLayoutViewTheme->SetBackgroundColor2( rgba );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetBackgroundBottom()
{
  double rgba[4];
  this->GraphLayoutViewTheme->GetBackgroundColor( rgba );
  QColor c( 255. * rgba[0], 255. * rgba[1], 255. * rgba[2], 255 );
  c = QColorDialog::getColor( c, this );
  rgba[0] = c.redF();
  rgba[1] = c.greenF();
  rgba[2] = c.blueF();
  rgba[3] = c.alphaF();

  this->GraphLayoutViewTheme->SetBackgroundColor( rgba );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotOpenSession()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Ovis Session" ), ".", tr( "Ovis Session Files (*.ovx)" ) );
  
  if( "" != fileName )
  {
    vtkSmartPointer< ovSessionReader > reader = vtkSmartPointer< ovSessionReader >::New();
    try
    {
      reader->SetFileName( fileName.toStdString().c_str() );
      reader->Update();
      this->Session->DeepCopy( reader->GetOutput() );
      this->CurrentSessionFileName = fileName.toStdString().c_str();
      this->ApplySessionToState();
    }
    catch( vtkstd::exception &e )
    {
      QMessageBox errorMessage( this );
      errorMessage.setWindowModality( Qt::WindowModal );
      errorMessage.setIcon( QMessageBox::Warning );
      errorMessage.setText( "There was an error while attempting to open the session." );
      errorMessage.exec();
    }
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotReloadSession()
{
  this->ApplySessionToState();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSaveSession()
{
  // make sure we have a session file name
  if( "" == this->CurrentSessionFileName )
  {
    this->slotSaveSessionAs();
  }
  else
  {
    vtkSmartPointer< ovSessionWriter > writer = vtkSmartPointer< ovSessionWriter >::New();
    try
    {
      this->ApplyStateToSession();
      writer->SetFileName( this->CurrentSessionFileName );
      writer->SetInput( this->Session );
      writer->Update();
    }
    catch( vtkstd::exception &e )
    {
      QMessageBox errorMessage( this );
      errorMessage.setWindowModality( Qt::WindowModal );
      errorMessage.setIcon( QMessageBox::Warning );
      errorMessage.setText( "There was an error while attempting to save the session." );
      errorMessage.exec();
    }
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSaveSessionAs()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr( "Save Ovis Session" ), ".", tr( "Ovis Session Files (*.ovx)" ) );
  
  if( "" != fileName )
  {
    // add .ovx if it isn't already there
    if( !fileName.endsWith( ".ovx" ) ) fileName.append( ".ovx" );
    this->CurrentSessionFileName = fileName.toStdString().c_str();
    this->slotSaveSession();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::ApplySessionToState()
{
  this->IsLoadingSession = true;
  
  if( this->Session->GetDataFile() != this->CurrentDataFileName )
  {
    this->CurrentDataFileName = this->Session->GetDataFile();
    this->LoadData();
  }
  this->GraphLayoutViewTheme->SetBackgroundColor(
    this->Session->GetBackgroundColor1() );
  this->GraphLayoutViewTheme->SetBackgroundColor2(
    this->Session->GetBackgroundColor2() );
  this->SetVertexStyle( this->Session->GetVertexStyle() );
  this->SetLayoutStrategy( this->Session->GetLayoutStrategy() );
  this->RestrictGraphFilter->SetAuthorsOnly( this->Session->GetAuthorsOnly() );
  this->RestrictGraphFilter->SetGenderTypeRestriction(
    this->Session->GetGenderTypeRestriction() );
  this->RestrictGraphFilter->SetWriterTypeRestriction(
    this->Session->GetWriterTypeRestriction() );
  this->SetVertexSize( this->Session->GetVertexSize() ); 
  this->SetEdgeSize( this->Session->GetEdgeSize() ); 
  this->SetAuthorVertexColor( this->Session->GetAuthorVertexColor() );
  this->SetAssociationVertexColor( this->Session->GetAssociationVertexColor() );
  this->SetStartDate( this->Session->GetStartDateRestriction() );
  this->SetEndDate( this->Session->GetEndDateRestriction() );
  this->SetTagList( this->Session->GetTagList() );
  vtkCamera *camera = this->GraphLayoutView->GetRenderer()->GetActiveCamera();
  camera->SetPosition( this->Session->GetCamera()->GetPosition() );
  camera->SetFocalPoint( this->Session->GetCamera()->GetFocalPoint() );
  camera->SetViewUp( this->Session->GetCamera()->GetViewUp() );
  camera->SetClippingRange( this->Session->GetCamera()->GetClippingRange() );
  camera->SetParallelScale( this->Session->GetCamera()->GetParallelScale() );

  this->IsLoadingSession = false;
  this->UpdateActiveTags();
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::ApplyStateToSession()
{
  this->Session->SetDataFile( this->CurrentDataFileName );
  this->Session->SetBackgroundColor1(
    this->GraphLayoutViewTheme->GetBackgroundColor() );
  this->Session->SetBackgroundColor2(
    this->GraphLayoutViewTheme->GetBackgroundColor2() );
  this->Session->SetVertexStyle( this->GraphLayoutView->GetGlyphType() );
  this->Session->SetLayoutStrategy( this->CurrentLayoutStrategy );
  this->Session->SetAuthorsOnly( this->RestrictGraphFilter->GetAuthorsOnly() );
  this->Session->SetGenderTypeRestriction(
    this->RestrictGraphFilter->GetGenderTypeRestriction() );
  this->Session->SetWriterTypeRestriction(
    this->RestrictGraphFilter->GetWriterTypeRestriction() );
  this->Session->SetVertexSize(
    this->GraphLayoutViewTheme->GetPointSize() );
  this->Session->SetEdgeSize(
    this->GraphLayoutViewTheme->GetLineWidth() );
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetPointLookupTable() );
  this->Session->SetAuthorVertexColor( lut->GetTableValue( 1 ) );
  this->Session->SetAssociationVertexColor( lut->GetTableValue( 0 ) );
  this->Session->SetStartDateRestriction(
    this->RestrictGraphFilter->GetStartDate()->ToInt() );
  this->Session->SetEndDateRestriction(
    this->RestrictGraphFilter->GetEndDate()->ToInt() );
  this->GetTagList( this->Session->GetTagList() );
  vtkCamera *camera = this->GraphLayoutView->GetRenderer()->GetActiveCamera();
  this->Session->GetCamera()->SetPosition( camera->GetPosition() );
  this->Session->GetCamera()->SetFocalPoint( camera->GetFocalPoint() );
  this->Session->GetCamera()->SetViewUp( camera->GetViewUp() );
  this->Session->GetCamera()->SetClippingRange( camera->GetClippingRange() );
  this->Session->GetCamera()->SetParallelScale( camera->GetParallelScale() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetVertexStyle( int type )
{
  this->GraphLayoutView->SetGlyphType( type );
  
  // we need to double the point size for all but vertex type
  this->GraphLayoutViewTheme->SetPointSize(
    this->ui->vertexSizeSlider->value() * ( VTK_VERTEX_GLYPH == type ? 1 : 2 ) );

  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetLayoutStrategy( const ovString &strategy )
{
  if( this->CurrentLayoutStrategy != strategy )
  {
    // update the GUI
    if( "Random" == strategy )
      this->ui->actionSetLayoutStrategyToRandom->setChecked( true );
    else if( "Force Directed" == strategy )
      this->ui->actionSetLayoutStrategyToForceDirected->setChecked( true );
    else if( "Simple 2D" == strategy )
      this->ui->actionSetLayoutStrategyToSimple2D->setChecked( true );
    else if( "Clustering 2D" == strategy )
      this->ui->actionSetLayoutStrategyToClustering2D->setChecked( true );
    else if( "Community 2D" == strategy )
      this->ui->actionSetLayoutStrategyToCommunity2D->setChecked( true );
    else if( "Fast 2D" == strategy )
      this->ui->actionSetLayoutStrategyToFast2D->setChecked( true );
    else if( "Circular" == strategy )
      this->ui->actionSetLayoutStrategyToCircular->setChecked( true );
    //else if( "Cosmic Tree" == strategy )
    //  this->ui->actionSetLayoutStrategyToCosmicTree->setChecked( true );
    //else if( "Cone" == strategy )
    //  this->ui->actionSetLayoutStrategyToCone->setChecked( true );
    else if( "Span Tree" == strategy )
      this->ui->actionSetLayoutStrategyToSpanTree->setChecked( true );

    // update the graph
    this->CurrentLayoutStrategy = strategy;
    this->GraphLayoutView->GetLayoutStrategy()->RemoveObserver(
      vtkCommand::ProgressEvent );
    this->GraphLayoutView->SetLayoutStrategy( this->CurrentLayoutStrategy.c_str() );
    this->GraphLayoutView->GetLayoutStrategy()->AddObserver(
      vtkCommand::ProgressEvent, this->ProgressObserver );
    
    this->RenderGraph( true );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAuthorCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetAuthorsOnly( 0 != state );
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotGenderComboBoxCurrentIndexChanged( const QString& index )
{
  this->RestrictGraphFilter->SetGenderTypeRestriction(
    ovRestrictGraphFilter::GenderTypeRestrictionFromString( index.toStdString().c_str() ) );
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotWriterComboBoxCurrentIndexChanged( const QString& index )
{
  this->RestrictGraphFilter->SetWriterTypeRestriction(
    ovRestrictGraphFilter::WriterTypeRestrictionFromString( index.toStdString().c_str() ) );
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetVertexSize( int value )
{
  // we need to double the point size for all but the vertex type
  int type = this->GraphLayoutView->GetGlyphType();
  value *= VTK_VERTEX_GLYPH == type ? 1 : 2;
  this->GraphLayoutViewTheme->SetPointSize( value );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->ui->vertexSizeSlider->setValue( value );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotVertexSizeSliderValueChanged( int value )
{
  this->SetVertexSize( value );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetAuthorVertexColor( double rgba[4] )
{
  // change the button color
  char buffer[512];
  sprintf( buffer, "color: %s; background-color: rgba( %d, %d, %d, %d )",
    ovIsOppositeColorWhite( rgba ) ? "white" : "black",
    static_cast< int >( rgba[0]*255 ),
    static_cast< int >( rgba[1]*255 ),
    static_cast< int >( rgba[2]*255 ),
    static_cast< int >( rgba[3]*255 ) );
  this->ui->authorVertexColorPushButton->setStyleSheet( buffer );
    
  // change the LUT color
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetPointLookupTable() );
  lut->SetTableValue( 1, rgba );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAuthorVertexColorPushButtonClicked()
{
  QColor color = QColorDialog::getColor(
    this->ui->authorVertexColorPushButton->palette().color( QPalette::Active, QPalette::Button ),
    this, "Select author vertex color" );

  if( color.isValid() )
  {
    double rgba[] = { color.redF(), color.greenF(), color.blueF(), color.alphaF() };
    this->SetAuthorVertexColor( rgba );

    // Calling Render() should be enough to update the graph view, but for some reason
    // it is also necessary to simulate a left mouse button click.  This seems harmless
    // enough.
    this->GraphLayoutView->Render();
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonPressEvent );
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetAssociationVertexColor( double rgba[4] )
{
  // change the button color
  char buffer[512];
  sprintf( buffer, "color: %s; background-color: rgba( %d, %d, %d, %d )",
    ovIsOppositeColorWhite( rgba ) ? "white" : "black",
    static_cast< int >( rgba[0]*255 ),
    static_cast< int >( rgba[1]*255 ),
    static_cast< int >( rgba[2]*255 ),
    static_cast< int >( rgba[3]*255 ) );
  this->ui->associationVertexColorPushButton->setStyleSheet( buffer );
    
  // change the LUT color
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetPointLookupTable() );
  lut->SetTableValue( 0, rgba );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAssociationVertexColorPushButtonClicked()
{
  QColor color = QColorDialog::getColor(
    this->ui->associationVertexColorPushButton->palette().color( QPalette::Active, QPalette::Button ),
    this, "Select association vertex color" );

  if( color.isValid() )
  {
    double rgba[] = { color.redF(), color.greenF(), color.blueF(), color.alphaF() };
    this->SetAssociationVertexColor( rgba );

    // Calling Render() should be enough to update the graph view, but for some reason
    // it is also necessary to simulate a left mouse button click.  This seems harmless
    // enough.
    this->GraphLayoutView->Render();
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonPressEvent );
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetEdgeSize( int value )
{
  this->GraphLayoutViewTheme->SetLineWidth( value );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->ui->edgeSizeSlider->setValue( value );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotEdgeSizeSliderValueChanged( int value )
{
  this->SetEdgeSize( value );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTextSearchSetPushButtonClicked()
{
  ovQSearchDialog dialog( this );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select text search" ) );
  dialog.setSearchPhrase( this->TextSearchPhrase );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    // update the text search from the dialog
    dialog.getSearchPhrase( this->TextSearchPhrase );
  
    // update the GUI
    this->ui->textSearchLineEdit->setText( this->TextSearchPhrase->ToString().c_str() );
    
    // update the graph
    this->RestrictGraphFilter->SetTextSearchPhrase( this->TextSearchPhrase );
    this->RestrictGraphFilter->Modified();
    this->RenderGraph();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAuthorSearchSetPushButtonClicked()
{
  ovQSearchDialog dialog( this );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select text search" ) );
  dialog.setSearchPhrase( this->AuthorSearchPhrase );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    // update the text search from the dialog
    dialog.getSearchPhrase( this->AuthorSearchPhrase );
  
    // update the GUI
    this->ui->textSearchLineEdit->setText( this->AuthorSearchPhrase->ToString().c_str() );
    
    // update the graph
    this->RestrictGraphFilter->SetAuthorSearchPhrase( this->AuthorSearchPhrase );
    this->RestrictGraphFilter->Modified();
    this->RenderGraph();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetStartDate( const ovDate &date )
{
  // update the GUI
  ovString dateString;
  date.ToString( dateString );
  this->ui->startLineEdit->setText( dateString.c_str() );
  
  // update the graph
  this->RestrictGraphFilter->SetStartDate( date );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotStartSetPushButtonClicked()
{
  ovQDateDialog dialog( this );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select start date" ) );
  dialog.setDate( ovDate( this->ui->startLineEdit->text().toStdString().c_str() ) );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    this->SetStartDate( dialog.getDate() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetEndDate( const ovDate &date )
{
  // update the GUI
  ovString dateString;
  date.ToString( dateString );
  this->ui->endLineEdit->setText( dateString.c_str() );
  
  // update the graph
  this->RestrictGraphFilter->SetEndDate( date );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotEndSetPushButtonClicked()
{
  ovQDateDialog dialog( this );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select end date" ) );
  dialog.setDate( ovDate( this->ui->endLineEdit->text().toStdString().c_str() ) );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    this->SetEndDate( dialog.getDate() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeCheckButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->IsCheckingMultipleTags = true;
  
  // check all selected tree items
  QTreeWidgetItem *item;
  QList< QTreeWidgetItem* > list = this->ui->tagTreeWidget->selectedItems();

  for( int index = 0; index < list.size(); ++index )
  {
    item = list.at( index );
    if( Qt::Unchecked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Checked );
  }

  // manually update the graph view
  this->IsCheckingMultipleTags = false;
  this->UpdateActiveTags();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeUnCheckButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->IsCheckingMultipleTags = true;

  // check all selected tree items
  QTreeWidgetItem *item;
  QList< QTreeWidgetItem* > list = this->ui->tagTreeWidget->selectedItems();

  for( int index = 0; index < list.size(); ++index )
  {
    item = list.at( index );
    if( Qt::Checked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Unchecked );
  }

  // manually update the graph view
  this->IsCheckingMultipleTags = false;
  this->UpdateActiveTags();
  this->RenderGraph( true );
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeExpandButtonClicked()
{
  // expand all tree items
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    item->setExpanded( true );
    treeIt++;
  }
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeCollapseButtonClicked()
{
  // expand all tree items
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    item->setExpanded( false );
    treeIt++;
  }
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeItemChanged( QTreeWidgetItem* item, int column )
{
  // the checked state of the item may have changed, update the tag info singleton
  ovTag *tag = ovOrlandoTagInfo::GetInfo()->FindTag( item->text( column ).toStdString() );
  if( tag ) tag->active = Qt::Checked == item->checkState( column ) ? true : false;
  this->UpdateActiveTags();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeItemDoubleClicked( QTreeWidgetItem* item, int column )
{
  // open a color-selection dialog using the item's color
  char buffer[512];
  sprintf( buffer, "Select color for %s", item->text( column ).toStdString().c_str() );
  QColor color =
    QColorDialog::getColor( item->background( column ), this, buffer );

  if( color.isValid() )
  {
    // block the tree widget's signals, otherwise itemChanged() will signal the
    // slotTagTreeItemChanged() method which we don't want
    this->ui->tagTreeWidget->blockSignals( true );

    // change the item's color
    item->setBackground( 0, color );
    item->setForeground( 0, ovIsOppositeColorWhite( color.redF(), color.greenF(), color.blueF() )
      ? QColor(255, 255, 255, 255)
      : QColor(0, 0, 0, 255) );
    this->ui->tagTreeWidget->blockSignals( false );

    // update the LUT with the new color
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetCellLookupTable() );
    lut->SetTableValue(
      ovOrlandoTagInfo::GetInfo()->FindTagIndex( item->text( column ).toStdString().c_str() ),
      color.redF(), color.greenF(), color.blueF() );

    // Calling Render() should be enough to update the graph view, but for some reason
    // it is also necessary to simulate a left mouse button click.  This seems harmless
    // enough.
    this->GraphLayoutView->Render();
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonPressEvent );
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::GetTagList( ovTagVector *tags )
{
  if( NULL == tags ) return;
  double *rgba;
  QTreeWidgetItem *parent;
  
  vtkstd::for_each( tags->begin(), tags->end(), safe_delete() );
  tags->clear();
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    parent = item->parent();
    ovTag *tag = new ovTag;
    tag->parent = parent ? parent->text( 0 ).toStdString().c_str() : "";
    tag->name = item->text( 0 ).toStdString().c_str();
    tag->active = Qt::Checked == item->checkState( 0 );
    tag->expanded = item->isExpanded();

    // get the tag color from the LUT
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetCellLookupTable() );
    rgba = lut->GetTableValue(
      ovOrlandoTagInfo::GetInfo()->FindTagIndex( tag->name.c_str() ) );
    for( int i = 0; i < 4; i++ ) tag->color[i] = rgba[i];

    // store the tag and go to the next tree widget item
    tags->push_back( tag );
    treeIt++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetTagList( ovTagVector *tags )
{
  if( NULL == tags ) return;
  
  this->IsCheckingMultipleTags = true;
    
  // block the tree widget's signals, otherwise itemChanged() will signal the
  // slotTagTreeItemChanged() method which we don't want
  this->ui->tagTreeWidget->blockSignals( true );
  
  if( 0 == tags->size() )
  {
    this->ui->tagTreeWidget->clear();
  }
  else
  {
    // go through the tag list and apply it to the tree widget
    ovTag *tag;
    for( ovTagVector::iterator it = tags->begin(); it != tags->end(); ++it )
    {
      tag = *it;
      QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
      while( *treeIt )
      {
        QTreeWidgetItem *item = *treeIt;
        if( 0 == strcmp( item->text( 0 ).toStdString().c_str(), tag->name.c_str() ) )
        {
          item->setCheckState( 0, tag->active ? Qt::Checked : Qt::Unchecked );
          item->setExpanded( tag->expanded );
  
          // change the item's color
          item->setBackground( 0, QColor(
            tag->color[0]*255,
            tag->color[1]*255,
            tag->color[2]*255,
            tag->color[3]*255 ) );
          item->setForeground( 0, ovIsOppositeColorWhite( tag->color )
            ? QColor(255, 255, 255, 255)
            : QColor(0, 0, 0, 255) );
      
          // update the LUT with the new color
          vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
            this->GraphLayoutViewTheme->GetCellLookupTable() );
          lut->SetTableValue(
            ovOrlandoTagInfo::GetInfo()->FindTagIndex( item->text( 0 ).toStdString().c_str() ),
            tag->color );
          break;
        }
        treeIt++;
      }
    }
  }

  this->ui->tagTreeWidget->blockSignals( false );
  this->IsCheckingMultipleTags = false;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::UpdateActiveTags()
{
  if( this->IsCheckingMultipleTags ) return;
  
  vtkSmartPointer< vtkStringArray > tags = vtkSmartPointer< vtkStringArray >::New();
  this->GetActiveTags( tags );
  this->RestrictGraphFilter->SetActiveTags( tags );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::GetActiveTags( vtkStringArray* array )
{
  if( NULL == array ) return;
  
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    if( Qt::Checked == item->checkState( 0 ) )
    {
      array->InsertNextValue( item->text( 0 ).toStdString() );
    }
    treeIt++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::RenderGraph( bool resetCamera )
{
  if( this->IsLoadingSession || this->IsLoadingData || this->IsCheckingMultipleTags ) return;

  // we're about to do an operation that might take a while, so update the GUI and cursor
  this->repaint();
  this->setCursor( Qt::WaitCursor );
  this->ui->graphLayoutWidget->setCursor( Qt::WaitCursor );

  clock_t start = clock();
  if( resetCamera ) this->GraphLayoutView->ResetCamera();
  this->GraphLayoutView->Render();
  clock_t end = clock();

  // update the cursor and report how many vertices and edges are currently visible
  this->setCursor( Qt::ArrowCursor );
  this->ui->graphLayoutWidget->setCursor( Qt::CrossCursor );
  char buffer[512];
  sprintf( buffer, "Processing time: %0.2fs    Number of vertices: %d     Number of edges: %d",
    static_cast< double >( end - start ) / CLOCKS_PER_SEC,
    static_cast< int >( this->RestrictGraphFilter->GetOutput()->GetNumberOfVertices() ),
    static_cast< int >( this->RestrictGraphFilter->GetOutput()->GetNumberOfEdges() ) );
  this->ui->statusbar->showMessage( buffer );
}
