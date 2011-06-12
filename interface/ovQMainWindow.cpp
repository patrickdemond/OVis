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
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>
#include <QTreeWidget>

#include "vtkAnnotationLink.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkCornerAnnotation.h"
#include "vtkGlyphSource2D.h"
#include "vtkGraph.h"
#include "vtkGraphLayoutStrategy.h"
#include "vtkGraphLayoutView.h"
#include "vtkIdTypeArray.h"
#include "vtkInteractorStyleRubberBand2D.h"
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
#include "vtkTextProperty.h"
#include "vtkViewTheme.h"
#include "vtkWindowToImageFilter.h"

// experimental
#include "vtkDataSetAttributes.h"
#include "vtkVariantArray.h"

#include "ovQDateSpanDialog.h"
#include "ovQTextSearchDialog.h"
#include "ovQAuthorSearchDialog.h"
#include "source/ovOrlandoReader.h"
#include "source/ovOrlandoTagInfo.h"
#include "source/ovRestrictGraphFilter.h"
#include "source/ovSearchPhrase.h"
#include "source/ovSession.h"
#include "source/ovSessionReader.h"
#include "source/ovSessionWriter.h"

#include "vtksys/SystemTools.hxx"
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
        message = QString( "Resolving visible nodes and links..." );
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
    
    ovTagVector *tags = ovOrlandoTagInfo::GetInfo()->GetTags();

    vtkStringArray *labelArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetVertexData()->GetAbstractArray( "label" ) );
    vtkStringArray *pedigreeArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetVertexData()->GetAbstractArray( "pedigree" ) );
    vtkStringArray *contentArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetEdgeData()->GetAbstractArray( "content" ) );
    vtkStringArray *tagsArray = 
      vtkStringArray::SafeDownCast(
        this->Graph->GetEdgeData()->GetAbstractArray( "tags" ) );

    if( NULL == contentArray ||
        NULL == pedigreeArray ||
        NULL == contentArray ||
        NULL == tagsArray )
    {
      this->ui->graphSelectTextEdit->setHtml( QString( "" ) );
      return;
    }

    // process the selected edge and vertex ids
    vtkstd::ostringstream stream;
    
    // reset the label array to all empty strings
    int numValues = labelArray->GetNumberOfValues();
    for( int index = 0; index < numValues; index++ ) labelArray->SetValue( index, "" );

    numValues = vertexIds->GetNumberOfTuples();
    if( 0 < numValues )
    {
      stream << "<h3>Selected Nodes (" << numValues
             << ( 100 < numValues ? ", showing first 100" : "" )
             << ")</h3><ul>";
      for( int index = 0; index < numValues; index++ )
      {
        vtkIdType id = vertexIds->GetValue( index );
        ovString name = pedigreeArray->GetValue( id );
        labelArray->SetValue( id, name );
        if( index < 100 ) stream << "<li>" << name << "</li>";
      }
      stream << "</ul>";

      if( 100 < numValues ) stream << "<p>List cropped at 100 nodes</p>";
    }

    numValues = edgeIds->GetNumberOfTuples();
    if( 0 < numValues )
    {
      stream << "<h3>Selected Links (" << numValues
             << ( 100 < numValues ? ", showing first 100" : "" )
             << ")</h3><ul>";
      for( int index = 0; index < numValues && index < 100; index++ )
      {
        vtkIdType eId = edgeIds->GetValue( index );
        vtkIdType sId = this->Graph->GetSourceVertex( eId );
        vtkIdType tId = this->Graph->GetTargetVertex( eId );
        stream << "<li><b>" << pedigreeArray->GetValue( sId ) << "</b> is connected to <b>"
               << pedigreeArray->GetValue( tId ) << "</b>:<br />"
               << "<b>Tags</b>: ";
        
        ovString edgeTags = tagsArray->GetValue( eId );
        int tagIndex = 0;
        bool first = true;
        for( int tagIndex = 0; tagIndex < edgeTags.length(); tagIndex++ )
        {
          if( '1' == edgeTags.at( tagIndex ) )
          {
            stream << ( first ? "" : ", " ) << tags->at( tagIndex )->title;
            first = false;
          }
        }
        stream << "<br />"
               << "<b>Text</b>:\"" << contentArray->GetValue( eId ) << "\"</li>";
      }
      stream << "</ul>";

      if( 100 < numValues ) stream << "<p>List cropped at 100 links</p>";
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
  this->CurrentLayoutStrategy = "SpanTree";
  this->CustomAnnotationText = "";
  this->Session = vtkSmartPointer< ovSession >::New();
  
  this->ui = new Ui_ovQMainWindow;
  this->ui->setupUi( this );
  
  // make the tag link
  this->ui->tagLinkLabel->setOpenExternalLinks( true );
  this->ui->tagLinkLabel->setText( "<a href=\"http://medusa.arts.ualberta.ca/testing/protected/svDocumentation?formname=t&d_id=ABOUTTHETAGS\">link type (tag) documentation</a>" );

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
    this->ui->actionShowAnnotation, SIGNAL( triggered() ),
    this, SLOT( slotShowAnnotation() ) );
  QObject::connect(
    this->ui->actionSetAnnotation, SIGNAL( triggered() ),
    this, SLOT( slotSetAnnotation() ) );

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
  this->GraphLayoutView->SetVertexLabelArrayName( "label" );
  this->GraphLayoutView->VertexLabelVisibilityOn();
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
  
  // set up the corner annotations
  vtkTextProperty *textProp;
  this->TopAnnotation = vtkSmartPointer< vtkCornerAnnotation >::New();
  textProp = this->TopAnnotation->GetTextProperty();
  textProp->SetColor( 0, 0, 0 );
  this->GraphLayoutView->GetRenderer()->AddViewProp( this->TopAnnotation );
  this->TopAnnotation->SetVisibility( true );

  this->BottomAnnotation = vtkSmartPointer< vtkCornerAnnotation >::New();
  textProp = this->BottomAnnotation->GetTextProperty();
  textProp->SetColor( 0, 0, 0 );
  this->GraphLayoutView->GetRenderer()->AddViewProp( this->BottomAnnotation );
  this->BottomAnnotation->SetVisibility( true );

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
  
  // set up the display property widgets
  QObject::connect(
    this->ui->writersCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotWritersCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->othersCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotOthersCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->femaleCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotFemaleCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->maleCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotMaleCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->BRWTypeCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotBRWTypeCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->writerTypeCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotWriterTypeCheckBoxStateChanged( int ) ) );
  QObject::connect(
    this->ui->IBRTypeCheckBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( slotIBRTypeCheckBoxStateChanged( int ) ) );
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
    this->ui->setTextSearchPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotSetTextSearchPushButtonClicked() ) );
  QObject::connect(
    this->ui->clearTextSearchPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotClearTextSearchPushButtonClicked() ) );
  QObject::connect(
    this->ui->setAuthorSearchPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotSetAuthorSearchPushButtonClicked() ) );
  QObject::connect(
    this->ui->clearAuthorSearchPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotClearAuthorSearchPushButtonClicked() ) );

  // set up the date restriction widgets
  QObject::connect(
    this->ui->setDateSpanPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotSetDateSpanPushButtonClicked() ) );
  QObject::connect(
    this->ui->clearDateSpanPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotClearDateSpanPushButtonClicked() ) );

  // set up the tag tree
  //this->ui->tagTreeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
  this->ui->tagTreeWidget->setExpandsOnDoubleClick( false );
  QObject::connect(
    this->ui->tagTreeCheckAllButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeCheckAllButtonClicked() ) );
  QObject::connect(
    this->ui->tagTreeUnCheckAllButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagTreeUnCheckAllButtonClicked() ) );
  /* NOTE: Functionality removed by request
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
  */
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
  
  ovTag *tag, *parentTag;
  QTreeWidgetItem *item, *parent;
  ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
  ovTagVector *tags = tagInfo->GetTags();
  
  int numberOfGeneralTags = 0; // we'll need this for colouring below
  bool tagParentError = false; // we only want to display the tag tree error once
  this->ui->tagTreeWidget->clear();
  for( ovTagVector::iterator it = tags->begin(); it != tags->end(); it++ )
  {
    tag = *it;
    if( tag->parent.length() )
    {
      // count the number of general tags
      if( tag->parent == "General" ) numberOfGeneralTags++;

      // find the item with the parent's name
      parent = NULL;
      QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
      while( *treeIt )
      {
        parentTag = tagInfo->FindTagFromName( tag->parent );
        if( parentTag->title == (*treeIt)->text( 0 ).toStdString() ) parent = *treeIt;
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
    item->setText(0, tag->title.c_str() );
    item->setCheckState( 0, tag->active ? Qt::Checked : Qt::Unchecked );
    item->setExpanded( tag->expanded );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
    
    // top level items have to be explicitely added to the tree
    if( 0 == tag->parent.length() ) this->ui->tagTreeWidget->addTopLevelItem( item );
  }
  
  // TODO-NEXT: need to put a gap in the colour spectrum between "WEALTH" and "WRITING"

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

  // make the general tags greyscale
  double generalCount = 0;
  lut->SetTableValue( tags->size() - numberOfGeneralTags - 1, 1.0, 1.0, 1.0 );
  for( int index = tags->size() - numberOfGeneralTags; index <= tags->size(); index++ )
  {
    double shade = 0.3 + 0.4 * ( generalCount / ( numberOfGeneralTags - 1 ) );
    lut->SetTableValue( index, shade, shade, shade );
    generalCount++;
  }
  
  // color tag names in the tree widget based on the LUT (expand the tab while we're at it
  int generalTagCount = 0;
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    int index = tagInfo->FindTagIndexFromTitle( (*treeIt)->text( 0 ).toStdString().c_str() );
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
  this->slotTagTreeCheckAllButtonClicked();
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

  // set the annotation color
  double average = ( rgba[0] + rgba[1] + rgba[2] ) / 3.0;
  double annotation = average > 0.5 ? 0.0 : 1.0;
  this->TopAnnotation->GetTextProperty()->SetColor( annotation, annotation, annotation );
  this->BottomAnnotation->GetTextProperty()->SetColor( annotation, annotation, annotation );

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

  // set the annotation color
  double average = ( rgba[0] + rgba[1] + rgba[2] ) / 3.0;
  double annotation = average > 0.5 ? 0.0 : 1.0;
  this->TopAnnotation->GetTextProperty()->SetColor( annotation, annotation, annotation );

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

  // set the annotation color
  double average = ( rgba[0] + rgba[1] + rgba[2] ) / 3.0;
  double annotation = average > 0.5 ? 0.0 : 1.0;
  this->BottomAnnotation->GetTextProperty()->SetColor( annotation, annotation, annotation );

  this->GraphLayoutViewTheme->SetBackgroundColor( rgba );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetAnnotation()
{
  bool ok;
  QString text = QInputDialog::getText(
    this, tr( "Set Custom Annotation" ), tr( "Annotation:" ), QLineEdit::Normal,
    tr( this->CustomAnnotationText.c_str() ), &ok );

  if( ok )
  {
    this->CustomAnnotationText = text.toStdString();
    this->TopAnnotation->SetText( 2, this->GetAnnotationText().c_str() );
    this->GraphLayoutView->Render();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotShowAnnotation()
{
  bool show = this->ui->actionShowAnnotation->isChecked();
  this->TopAnnotation->SetVisibility( show );
  this->BottomAnnotation->SetVisibility( show );
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
  // emulate a left mouse click in the render window (hack to fix session loading problem)
  unsigned int rect[] = { 1, 1, 1, 1, 0 };
  vtkInteractorStyleRubberBand2D *iactStyle = 
    vtkInteractorStyleRubberBand2D::SafeDownCast(
    this->GraphLayoutView->GetRenderWindow()->GetInteractor()->GetInteractorStyle() );
  iactStyle->InvokeEvent( vtkCommand::SelectionChangedEvent, reinterpret_cast<void*>(rect) );
  iactStyle->InvokeEvent( vtkCommand::EndInteractionEvent );
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
  this->CustomAnnotationText = this->Session->GetCustomAnnotationText();
  this->TopAnnotation->SetVisibility( this->Session->GetShowAnnotation() );
  this->BottomAnnotation->SetVisibility( this->Session->GetShowAnnotation() );
  this->ui->actionShowAnnotation->setChecked( this->Session->GetShowAnnotation() );
  this->SetVertexStyle( this->Session->GetVertexStyle() );
  this->SetLayoutStrategy( this->Session->GetLayoutStrategy() );
  this->RestrictGraphFilter->SetIncludeWriters( this->Session->GetIncludeWriters() );
  this->RestrictGraphFilter->SetIncludeOthers( this->Session->GetIncludeOthers() );
  this->RestrictGraphFilter->SetIncludeFemale( this->Session->GetIncludeFemale() );
  this->RestrictGraphFilter->SetIncludeMale( this->Session->GetIncludeMale() );
  this->RestrictGraphFilter->SetIncludeBRWType( this->Session->GetIncludeBRWType() );
  this->RestrictGraphFilter->SetIncludeWriterType( this->Session->GetIncludeWriterType() );
  this->RestrictGraphFilter->SetIncludeIBRType( this->Session->GetIncludeIBRType() );
  this->SetVertexSize( this->Session->GetVertexSize() ); 
  this->SetEdgeSize( this->Session->GetEdgeSize() ); 
  this->SetAuthorVertexColor( this->Session->GetAuthorVertexColor() );
  this->SetAssociationVertexColor( this->Session->GetAssociationVertexColor() );
  vtkSmartPointer< ovSearchPhrase > textPhrase = vtkSmartPointer< ovSearchPhrase >::New();
  textPhrase->Parse( this->Session->GetTextSearchPhrase() );
  this->SetTextSearchPhrase( textPhrase, this->Session->GetTextSearchNarrow() );
  vtkSmartPointer< ovSearchPhrase > authorPhrase = vtkSmartPointer< ovSearchPhrase >::New();
  authorPhrase->Parse( this->Session->GetAuthorSearchPhrase() );
  this->SetAuthorSearchPhrase( authorPhrase );
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
  this->SetSelectedVertexList( this->Session->GetSelectedVertexList() );
  this->SetSelectedEdgeList( this->Session->GetSelectedEdgeList() );
  this->RenderGraph();
  this->GraphLayoutView->GetRepresentation()->GetAnnotationLink()->InvokeEvent(
    vtkCommand::AnnotationChangedEvent );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::ApplyStateToSession()
{
  this->Session->SetDataFile( this->CurrentDataFileName );
  this->Session->SetBackgroundColor1(
    this->GraphLayoutViewTheme->GetBackgroundColor() );
  this->Session->SetBackgroundColor2(
    this->GraphLayoutViewTheme->GetBackgroundColor2() );
  this->Session->SetCustomAnnotationText( this->CustomAnnotationText );
  this->Session->SetShowAnnotation( this->TopAnnotation->GetVisibility() );
  this->Session->SetVertexStyle( this->GraphLayoutView->GetGlyphType() );
  this->Session->SetLayoutStrategy( this->CurrentLayoutStrategy );
  this->Session->SetIncludeWriters( this->RestrictGraphFilter->GetIncludeWriters() );
  this->Session->SetIncludeOthers( this->RestrictGraphFilter->GetIncludeOthers() );
  this->Session->SetIncludeFemale( this->RestrictGraphFilter->GetIncludeFemale() );
  this->Session->SetIncludeMale( this->RestrictGraphFilter->GetIncludeMale() );
  this->Session->SetIncludeBRWType( this->RestrictGraphFilter->GetIncludeBRWType() );
  this->Session->SetIncludeWriterType( this->RestrictGraphFilter->GetIncludeWriterType() );
  this->Session->SetIncludeIBRType( this->RestrictGraphFilter->GetIncludeIBRType() );
  this->Session->SetVertexSize(
    this->GraphLayoutViewTheme->GetPointSize() );
  this->Session->SetEdgeSize(
    this->GraphLayoutViewTheme->GetLineWidth() );
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetPointLookupTable() );
  this->Session->SetAuthorVertexColor( lut->GetTableValue( 1 ) );
  this->Session->SetAssociationVertexColor( lut->GetTableValue( 0 ) );
  ovSearchPhrase *phrase = this->RestrictGraphFilter->GetTextSearchPhrase();
  this->Session->SetTextSearchPhrase(
    NULL == phrase ? "" : this->RestrictGraphFilter->GetTextSearchPhrase()->ToString() );
  this->Session->SetTextSearchNarrow(
    this->RestrictGraphFilter->GetTextSearchNarrow() );
  phrase = this->RestrictGraphFilter->GetAuthorSearchPhrase();
  this->Session->SetAuthorSearchPhrase(
    NULL == phrase ? "" : this->RestrictGraphFilter->GetAuthorSearchPhrase()->ToString() );
  this->Session->SetStartDateRestriction(
    this->RestrictGraphFilter->GetStartDate()->ToInt() );
  this->Session->SetEndDateRestriction(
    this->RestrictGraphFilter->GetEndDate()->ToInt() );
  this->GetSelectedVertexList( this->Session->GetSelectedVertexList() );
  this->GetSelectedEdgeList( this->Session->GetSelectedEdgeList() );
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
void ovQMainWindow::slotWritersCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeWriters( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotOthersCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeOthers( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFemaleCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeFemale( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotMaleCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeMale( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotBRWTypeCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeBRWType( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotWriterTypeCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeWriterType( 0 != state );
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotIBRTypeCheckBoxStateChanged( int state )
{
  this->RestrictGraphFilter->SetIncludeIBRType( 0 != state );
  this->RenderGraph( true );
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
    this, "Select author node color" );

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
    this, "Select association node color" );

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
void ovQMainWindow::SetTextSearchPhrase( ovSearchPhrase *phrase, bool narrow )
{
  // update the GUI
  ovString text = phrase
                ? phrase->ToString() + ( narrow ? " (narrow)" : " (broad)" )
                : "(none set)";
  this->ui->textSearchLabel->setText( text.c_str() );
  
  // update the graph
  this->RestrictGraphFilter->SetTextSearchPhrase( phrase );
  this->RestrictGraphFilter->SetTextSearchNarrow( narrow );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetAuthorSearchPhrase( ovSearchPhrase *phrase )
{
  // update the GUI
  this->ui->authorSearchLabel->setText( phrase ? phrase->ToString().c_str() : "(none set)" );
  
  // update the graph
  this->RestrictGraphFilter->SetAuthorSearchPhrase( phrase );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetTextSearchPushButtonClicked()
{
  ovQTextSearchDialog dialog( this, true );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select text search" ) );
  
  ovSearchPhrase *phrase = this->RestrictGraphFilter->GetTextSearchPhrase();
  dialog.setSearchPhrase( phrase );
  dialog.setNarrow( this->RestrictGraphFilter->GetTextSearchNarrow() );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    // update the text search from the dialog
    vtkSmartPointer< ovSearchPhrase > phrase = vtkSmartPointer< ovSearchPhrase >::New();
    dialog.getSearchPhrase( phrase );
    this->SetTextSearchPhrase( phrase, dialog.isNarrow() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotClearTextSearchPushButtonClicked()
{
  this->SetTextSearchPhrase( NULL );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetAuthorSearchPushButtonClicked()
{
  ovQAuthorSearchDialog dialog( this, false );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select author search" ) );
  
  ovSearchPhrase *phrase = this->RestrictGraphFilter->GetTextSearchPhrase();
  dialog.setSearchPhrase( phrase );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    // update the author search from the dialog
    vtkSmartPointer< ovSearchPhrase > phrase = vtkSmartPointer< ovSearchPhrase >::New();
    dialog.getSearchPhrase( phrase );
    this->SetAuthorSearchPhrase( phrase );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotClearAuthorSearchPushButtonClicked()
{
  this->SetAuthorSearchPhrase( NULL );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetStartDate( const ovDate &date )
{
  // update the GUI
  ovString dateString;
  date.ToString( dateString );
  if( 0 == dateString.length() ) dateString = "612 BCE (default)";
  this->ui->startDateLabel->setText( dateString.c_str() );
  
  // update the graph
  this->RestrictGraphFilter->SetStartDate( date );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetDateSpanPushButtonClicked()
{
  ovQDateSpanDialog dialog( this );
  dialog.setModal( true );
  dialog.setWindowTitle( tr( "Select date span" ) );

  dialog.setDateSpan(
    *( this->RestrictGraphFilter->GetStartDate() ),
    *( this->RestrictGraphFilter->GetEndDate() ) );
  
  if( QDialog::Accepted == dialog.exec() )
  {
    this->SetStartDate( dialog.getStartDate() );
    this->SetEndDate( dialog.getEndDate() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotClearDateSpanPushButtonClicked()
{
  this->SetStartDate( ovDate() );
  this->SetEndDate( ovDate() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetEndDate( const ovDate &date )
{
  // determine the year
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  timeinfo->tm_year;
  char year[10];
  sprintf( year, "%d", 1900 + timeinfo->tm_year );

  // update the GUI
  ovString dateString;
  date.ToString( dateString );
  if( 0 == dateString.length() )
  {
    dateString = year;
    dateString += " CE (default)";
  }
  this->ui->endDateLabel->setText( dateString.c_str() );
  
  // update the graph
  this->RestrictGraphFilter->SetEndDate( date );
  this->RestrictGraphFilter->Modified();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
// NOTE: 
void ovQMainWindow::slotTagTreeCheckAllButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->IsCheckingMultipleTags = true;
  
  // check all tree items
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    if( Qt::Unchecked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Checked );
    treeIt++;
  }

  // manually update the graph view
  this->IsCheckingMultipleTags = false;
  this->UpdateActiveTags();
  this->RenderGraph( true );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeUnCheckAllButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->IsCheckingMultipleTags = true;

  // check all tree items
  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    if( Qt::Checked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Unchecked );
    treeIt++;
  }

  // manually update the graph view
  this->IsCheckingMultipleTags = false;
  this->UpdateActiveTags();
  this->RenderGraph( true );
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
/* NOTE: Functionality removed by request
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
*/
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
/* NOTE: Functionality removed by request
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
*/

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
/* NOTE: Functionality removed by request
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
*/

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
/* NOTE: Functionality removed by request
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
*/
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeItemChanged( QTreeWidgetItem* item, int column )
{
  // the checked state of the item may have changed, update the tag info singleton
  ovTag *tag = ovOrlandoTagInfo::GetInfo()->FindTagFromTitle( item->text( column ).toStdString() );
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
      ovOrlandoTagInfo::GetInfo()->FindTagIndexFromTitle(
        item->text( column ).toStdString().c_str() ),
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
void ovQMainWindow::GetSelectedVertexList( ovIntVector *ids )
{
  if( NULL == ids ) return;
  ids->clear();

  vtkSelection *selection =
    this->GraphLayoutView->GetRepresentation()->GetAnnotationLink()->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  vtkIdTypeArray *array = vtkSelectionNode::VERTEX == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );

  int numValues = array->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      ids->push_back( array->GetValue( index ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetSelectedVertexList( ovIntVector *ids )
{
  if( NULL == ids ) return;
  vtkAnnotationLink *annotation = this->GraphLayoutView->GetRepresentation()->GetAnnotationLink();
  vtkSelection *selection = annotation->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  vtkIdTypeArray *array = vtkSelectionNode::VERTEX == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );

  array->Initialize();
  for( ovIntVector::iterator it = ids->begin(); it != ids->end(); ++it )
    array->InsertNextValue( *it );
}
  
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::GetSelectedEdgeList( ovIntVector *ids )
{
  if( NULL == ids ) return;
  ids->clear();

  vtkSelection *selection =
    this->GraphLayoutView->GetRepresentation()->GetAnnotationLink()->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  vtkIdTypeArray *array = vtkSelectionNode::EDGE == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );

  int numValues = array->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      ids->push_back( array->GetValue( index ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::SetSelectedEdgeList( ovIntVector *ids )
{
  if( NULL == ids ) return;
  
  vtkAnnotationLink *annotation = this->GraphLayoutView->GetRepresentation()->GetAnnotationLink();
  vtkSelection *selection = annotation->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  vtkIdTypeArray *array = vtkSelectionNode::EDGE == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );

  array->Initialize();
  for( ovIntVector::iterator it = ids->begin(); it != ids->end(); ++it )
    array->InsertNextValue( *it );

  annotation->Modified();
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
    tag->DeepCopy( ovOrlandoTagInfo::GetInfo()->FindTagFromTitle(
      item->text( 0 ).toStdString().c_str() ) );
    tag->active = Qt::Checked == item->checkState( 0 );
    tag->expanded = item->isExpanded();

    // get the tag color from the LUT
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetCellLookupTable() );
    rgba = lut->GetTableValue(
      ovOrlandoTagInfo::GetInfo()->FindTagIndexFromTitle( tag->title.c_str() ) );
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
        if( 0 == strcmp( item->text( 0 ).toStdString().c_str(), tag->title.c_str() ) )
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
            ovOrlandoTagInfo::GetInfo()->FindTagIndexFromTitle(
              item->text( 0 ).toStdString().c_str() ),
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
  
  ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
  ovTag *tag;

  QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
  while( *treeIt )
  {
    QTreeWidgetItem *item = *treeIt;
    if( Qt::Checked == item->checkState( 0 ) )
    {
      tag = tagInfo->FindTagFromTitle( item->text( 0 ).toStdString() );
      array->InsertNextValue( tag->name );
    }
    treeIt++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovString ovQMainWindow::GetAnnotationText()
{
  if( 0 < this->CustomAnnotationText.length() ) return this->CustomAnnotationText;

  // determine the corner annotation text
  size_t pos;
  ovString annotation, startString, endString, textString, authorString;

  bool writers = this->RestrictGraphFilter->GetIncludeWriters();
  bool others = this->RestrictGraphFilter->GetIncludeOthers();
  bool female = this->RestrictGraphFilter->GetIncludeFemale();
  bool male = this->RestrictGraphFilter->GetIncludeMale();
  bool BRWType = this->RestrictGraphFilter->GetIncludeBRWType();
  bool writerType = this->RestrictGraphFilter->GetIncludeWriterType();
  bool IBRType = this->RestrictGraphFilter->GetIncludeIBRType();
  ovDate *startDate = this->RestrictGraphFilter->GetStartDate();
  startDate->ToString( startString );
  ovDate *endDate = this->RestrictGraphFilter->GetEndDate();
  endDate->ToString( endString );
  ovSearchPhrase *textPhrase = this->RestrictGraphFilter->GetTextSearchPhrase();
  if( textPhrase ) textString = textPhrase->ToString( true );
  ovSearchPhrase *authorPhrase = this->RestrictGraphFilter->GetAuthorSearchPhrase();
  if( authorPhrase ) authorString = authorPhrase->ToString( true );
  
  if( female && male ) annotation += "all";
  else if( female ) annotation += "female";
  else if( male ) annotation += "male";
  
  if( BRWType && writerType && IBRType ) ; // no text
  else if( BRWType && writerType ) annotation += " British and international";
  else if( BRWType && IBRType ) annotation += " British and international-British";
  else if( writerType && IBRType ) annotation += " international and international-British";
  else if( BRWType ) annotation += " British";
  else if( writerType ) annotation += " international";
  else if( IBRType ) annotation += " international-British";

  if( writers && others ) annotation += " writers and non-writers";
  else if( writers ) annotation += " writers";
  else if( others ) annotation += " non-writers";

  if( startDate->IsSet() && endDate->IsSet() )
    annotation += " alive between " + startString + " and " + endString;
  else if( startDate->IsSet() && !endDate->IsSet() )
    annotation += " died after " + startString;
  else if( !startDate->IsSet() && endDate->IsSet() )
    annotation += " born before " + endString;
  
  if( 0 < textString.length() && 0 < authorString.length() )
    annotation += " whose entries contain the word " + textString
             + " and whose name contain " + authorString;
  if( 0 < textString.length() && 0 == authorString.length() )
    annotation += " whose entries contain the word " + textString;
  if( 0 == textString.length() && 0 < authorString.length() )
    annotation += " whose name contain " + authorString;
  
  // capitolize the first letter
  annotation[0] = toupper( annotation[0] );
  
  // insert new lines if the text is long
  int length = annotation.length();
  int index = 50;
  while( index < length )
  {
    if( vtkstd::string::npos != ( pos = annotation.find( ' ', index ) ) )
      annotation.replace( pos, 1, "\n" );
    index += 50;
  }
  
  return annotation;
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
  if( resetCamera )
  {
    vtkSelection *selection =
      this->GraphLayoutView->GetRepresentation()->GetAnnotationLink()->GetCurrentSelection();
    for( int i = 0; i < selection->GetNumberOfNodes(); i++ )
      selection->GetNode( i )->Initialize();
    this->GraphLayoutView->ResetCamera();
  }

  this->TopAnnotation->SetText( 2, this->GetAnnotationText().c_str() ); // top left
  this->TopAnnotation->SetText( 3, "" ); // top right
  this->BottomAnnotation->SetText( 0, "" ); // bottom left
  this->BottomAnnotation->SetText( 1, "" ); // bottom right

  this->GraphLayoutView->Render();
  clock_t end = clock();

  // update the cursor and report how many vertices and edges are currently visible
  this->setCursor( Qt::ArrowCursor );
  this->ui->graphLayoutWidget->setCursor( Qt::CrossCursor );
  char buffer[512];
  sprintf( buffer, "Processing time: %0.2fs    Number of nodes: %d     Number of links: %d",
    static_cast< double >( end - start ) / CLOCKS_PER_SEC,
    static_cast< int >( this->RestrictGraphFilter->GetOutput()->GetNumberOfVertices() ),
    static_cast< int >( this->RestrictGraphFilter->GetOutput()->GetNumberOfEdges() ) );
  this->ui->statusbar->showMessage( buffer );
}
