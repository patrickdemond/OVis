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
#include <QTreeWidget>

#include "vtkCommand.h"
#include "vtkGlyphSource2D.h"
#include "vtkGraph.h"
#include "vtkGraphLayoutStrategy.h"
#include "vtkGraphLayoutView.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkRenderedGraphRepresentation.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkViewTheme.h"

// experimental
#include "vtkDataSetAttributes.h"
#include "vtkVariantArray.h"

#include "ovQDateDialog.h"
#include "source/ovOrlandoReader.h"
#include "source/ovOrlandoTagInfo.h"
#include "source/ovRestrictGraphFilter.h"

#include <vtkstd/stdexcept>

class ovQMainWindowProgressCommand : public vtkCommand
{
public:
  static ovQMainWindowProgressCommand *New() { return new ovQMainWindowProgressCommand; }
  void Execute( vtkObject *caller, unsigned long eventId, void *callData );
  Ui_ovQMainWindow *ui;

  // Set to true when a file is being loaded
  bool IsLoadingFile;

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

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::ovQMainWindow( QWidget* parent )
  : QMainWindow( parent )
{
  QMenu *menu;
  this->AutoUpdateIncludeTags = false;
  
  this->ui = new Ui_ovQMainWindow;
  this->ui->setupUi( this );

  // connect the file menu items
  QObject::connect(
    this->ui->actionFileOpen, SIGNAL( triggered() ),
    this, SLOT( slotFileOpen() ) );

  this->ui->actionFileExit->setShortcuts( QKeySequence::Quit );
  QObject::connect( this->ui->actionFileExit, SIGNAL( triggered() ),
    qApp, SLOT( closeAllWindows() ) );
//  QObject::connect(
//    this->ui->actionFileExit, SIGNAL( triggered() ),
//    this, SLOT( slotFileExit() ) );
  
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

  // set up the observer to update the progress bar
  this->ProgressObserver = vtkSmartPointer< ovQMainWindowProgressCommand >::New();
  this->ProgressObserver = vtkSmartPointer< ovQMainWindowProgressCommand >::New();
  this->ProgressObserver->ui = this->ui;

  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategyToClustering2D();
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
  sprintf( buffer, "color: %s; background-color: rgb( %d, %d, %d )",
    ovIsOppositeColorWhite( tableColor[0], tableColor[1], tableColor[2] ) ? "white" : "black",
    static_cast< int >( tableColor[0]*255 ),
    static_cast< int >( tableColor[1]*255 ),
    static_cast< int >( tableColor[2]*255 ) );
  this->ui->associationVertexColorPushButton->setStyleSheet( buffer );

  tableColor = lut->GetTableValue( 1 );
  sprintf( buffer, "color: %s; background-color: rgb( %d, %d, %d )",
    ovIsOppositeColorWhite( tableColor[0], tableColor[1], tableColor[2] ) ? "white" : "black",
    static_cast< int >( tableColor[0]*255 ),
    static_cast< int >( tableColor[1]*255 ),
    static_cast< int >( tableColor[2]*255 ) );
  this->ui->authorVertexColorPushButton->setStyleSheet( buffer );
  
  // set up the reader and filters
  this->OrlandoReader = vtkSmartPointer< ovOrlandoReader >::New();
  this->OrlandoReader->AddObserver( vtkCommand::ProgressEvent, this->ProgressObserver );
  this->RestrictGraphFilter = vtkSmartPointer< ovRestrictGraphFilter >::New();
  this->RestrictGraphFilter->AddObserver( vtkCommand::ProgressEvent, this->ProgressObserver );
  this->RestrictGraphFilter->SetInput( this->OrlandoReader->GetOutput() );
  
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
  
  // set up the date restriction widgets
  QObject::connect(
    this->ui->startLineEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( slotStartLineEditTextChanged( const QString& ) ) );
  QObject::connect(
    this->ui->startSetPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotStartSetPushButtonClicked() ) );
  QObject::connect(
    this->ui->endLineEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( slotEndLineEditTextChanged( const QString& ) ) );
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
    this->ui->tagTreeWidget, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
    this, SLOT( slotTagTreeItemChanged( QTreeWidgetItem*, int ) ) );
  QObject::connect(
    this->ui->tagTreeWidget, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
    this, SLOT( slotTagTreeItemDoubleClicked( QTreeWidgetItem*, int ) ) );
  
  this->readSettings();

  // start updating the include tags automatically
  this->AutoUpdateIncludeTags = true;
};

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::~ovQMainWindow()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::closeEvent( QCloseEvent *event )
{
  if( this->maybeSave() )
  {
    writeSettings();
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

bool ovQMainWindow::maybeSave()
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
void ovQMainWindow::writeSettings()
{
  QSettings settings( "sharcnet", "ovis" );
  settings.beginGroup( "MainWindow" );
  settings.setValue( "size", this->size() );
  settings.setValue( "pos", this->pos() );
  settings.setValue( "maximized", this->isMaximized() );
  settings.endGroup();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::readSettings()
{
  QSettings settings( "sharcnet", "ovis" );
  settings.beginGroup( "MainWindow" );
  if( settings.contains( "size" ) )
  {
    this->resize( settings.value( "size" ).toSize() );
  }
  if( settings.contains( "pos" ) )
  {
    this->move( settings.value( "pos" ).toPoint() );
  }
  if( settings.contains( "maximized" ) && settings.value( "maximized" ).toBool() )
  {
    this->showMaximized();
  }
  settings.endGroup();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFileOpen()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Orlando File" ), "data", tr( "Orlando XML Files (*.xml)" ) );
  
  if( "" != fileName )
  {
    // define the tag list based on orlando tags
    // don't update the graph view until we're done checking tags
    this->AutoUpdateIncludeTags = false;
    
    ovTag *tag;
    ovTagVector tags;
    ovTagVector::iterator it;
    QTreeWidgetItem *item, *parent;
    ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
    tagInfo->GetTags( tags );
    
    bool tagParentError = false; // we only want to display the tag tree error once
    this->ui->tagTreeWidget->clear();
    for( it = tags.begin(); it != tags.end(); it++ )
    {
      tag = *it;
      if( tag->parent.length() )
      {
        // find the item with the parent's name

        parent = NULL;
        QTreeWidgetItemIterator it( this->ui->tagTreeWidget );
        while( *it )
        {
          if( tag->parent == (*it)->text( 0 ).toStdString() ) parent = *it;
          it++;
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
      item->setText(0, (*it)->name.c_str() );
      item->setCheckState( 0, (*it)->active ? Qt::Checked : Qt::Unchecked );
      item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
      
      // top level items have to be explicitely added to the tree
      if( 0 == tag->parent.length() ) this->ui->tagTreeWidget->addTopLevelItem( item );
    }
    
    // define the range of the edge lookup table based on the number of available tags
    double rgb[3], range[] = { 0, tags.size() - 1 };
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetCellLookupTable() );
    lut->SetTableRange( range );
    lut->SetNumberOfTableValues( tags.size() );
    lut->Build();
    
    // color tag names in the tree widget based on the LUT
    QTreeWidgetItemIterator treeIt( this->ui->tagTreeWidget );
    while( *treeIt )
    {
      int index = tagInfo->FindTagIndex( (*treeIt)->text( 0 ).toStdString().c_str() );
      lut->GetColor( index, rgb );
      (*treeIt)->setBackground( 0, QColor( rgb[0]*255, rgb[1]*255, rgb[2]*255, 255 ) );
      (*treeIt)->setForeground( 0, ovIsOppositeColorWhite( rgb[0], rgb[1], rgb[2] )
        ? QColor(255, 255, 255, 255)
        : QColor(0, 0, 0, 255) );
      treeIt++;
    }

    this->AutoUpdateIncludeTags = true;
    
    // load the orlando file and render
    this->OrlandoReader->SetFileName( fileName.toStdString() );
    this->GraphLayoutView->SetRepresentationFromInput( this->RestrictGraphFilter->GetOutput() );
    this->UpdateIncludeTags();
    this->RenderGraph( true );
  }
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
  double rgb[3];
  this->GraphLayoutViewTheme->GetBackgroundColor( rgb );
  QColor c( 255. * rgb[0], 255. * rgb[1], 255. * rgb[2] );
  c = QColorDialog::getColor( c, this );
  rgb[0] = static_cast< double >( c.red() ) / 255.;
  rgb[1] = static_cast< double >( c.green() ) / 255.;
  rgb[2] = static_cast< double >( c.blue() ) / 255.;

  this->GraphLayoutViewTheme->SetBackgroundColor( rgb[0], rgb[1], rgb[2] );
  this->GraphLayoutViewTheme->SetBackgroundColor2( rgb[0], rgb[1], rgb[2] );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetBackgroundTop()
{
  double rgb[3];
  this->GraphLayoutViewTheme->GetBackgroundColor2( rgb );
  QColor c( 255. * rgb[0], 255. * rgb[1], 255. * rgb[2] );
  c = QColorDialog::getColor( c, this );
  rgb[0] = static_cast< double >( c.red() ) / 255.;
  rgb[1] = static_cast< double >( c.green() ) / 255.;
  rgb[2] = static_cast< double >( c.blue() ) / 255.;

  this->GraphLayoutViewTheme->SetBackgroundColor2( rgb[0], rgb[1], rgb[2] );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotSetBackgroundBottom()
{
  double rgb[3];
  this->GraphLayoutViewTheme->GetBackgroundColor( rgb );
  QColor c( 255. * rgb[0], 255. * rgb[1], 255. * rgb[2] );
  c = QColorDialog::getColor( c, this );
  rgb[0] = static_cast< double >( c.red() ) / 255.;
  rgb[1] = static_cast< double >( c.green() ) / 255.;
  rgb[2] = static_cast< double >( c.blue() ) / 255.;

  this->GraphLayoutViewTheme->SetBackgroundColor( rgb[0], rgb[1], rgb[2] );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
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
void ovQMainWindow::SetLayoutStrategy( const char* strategy )
{
  this->GraphLayoutView->GetLayoutStrategy()->RemoveObserver(
    vtkCommand::ProgressEvent );
  this->GraphLayoutView->SetLayoutStrategy( strategy );
  this->GraphLayoutView->GetLayoutStrategy()->AddObserver(
    vtkCommand::ProgressEvent, this->ProgressObserver );
  
  this->RenderGraph( true );
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
void ovQMainWindow::slotVertexSizeSliderValueChanged( int value )
{
  // we need to double the point size for all but the vertex type
  int type = this->GraphLayoutView->GetGlyphType();
  this->GraphLayoutViewTheme->SetPointSize( value * ( VTK_VERTEX_GLYPH == type ? 1 : 2 ) );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAuthorVertexColorPushButtonClicked()
{
  QColor color = QColorDialog::getColor(
    this->ui->authorVertexColorPushButton->palette().color( QPalette::Active, QPalette::Button ),
    this, "Select author vertex color" );

  if( color.isValid() )
  {
    // change the button color
    char buffer[512];
    sprintf( buffer, "color: %s; background-color: rgb( %d, %d, %d )",
      ovIsOppositeColorWhite( color.redF(), color.greenF(), color.blueF() ) ? "white" : "black",
      color.red(), color.green(), color.blue() );
    this->ui->authorVertexColorPushButton->setStyleSheet( buffer );
    
    // change the LUT color
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetPointLookupTable() );
    lut->SetTableValue( 1, color.redF(), color.greenF(), color.blueF() );

    // Calling Render() should be enough to update the graph view, but for some reason
    // it is also necessary to simulate a left mouse button click.  This seems harmless
    // enough.
    this->GraphLayoutView->Render();
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonPressEvent );
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotAssociationVertexColorPushButtonClicked()
{
  QColor color = QColorDialog::getColor(
    this->ui->associationVertexColorPushButton->palette().color( QPalette::Active, QPalette::Button ),
    this, "Select association vertex color" );

  if( color.isValid() )
  {
    // change the button color
    char buffer[512];
    sprintf( buffer, "color: %s; background-color: rgb( %d, %d, %d )",
      ovIsOppositeColorWhite( color.redF(), color.greenF(), color.blueF() ) ? "white" : "black",
      color.red(), color.green(), color.blue() );
    this->ui->associationVertexColorPushButton->setStyleSheet( buffer );
    
    // change the LUT color
    vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
      this->GraphLayoutViewTheme->GetPointLookupTable() );
    lut->SetTableValue( 0, color.redF(), color.greenF(), color.blueF() );

    // Calling Render() should be enough to update the graph view, but for some reason
    // it is also necessary to simulate a left mouse button click.  This seems harmless
    // enough.
    this->GraphLayoutView->Render();
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonPressEvent );
    this->GraphLayoutView->GetInteractor()->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotEdgeSizeSliderValueChanged( int value )
{
  this->GraphLayoutViewTheme->SetLineWidth( value );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotStartLineEditTextChanged( const QString& text )
{
  this->RestrictGraphFilter->GetStartDate()->SetDate( text.toStdString().c_str() );
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
    ovString dateString;
    dialog.getDate().ToString( dateString );
    this->ui->startLineEdit->setText( dateString.c_str() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotEndLineEditTextChanged( const QString& text )
{
  this->RestrictGraphFilter->GetEndDate()->SetDate( text.toStdString().c_str() );
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
    ovString dateString;
    dialog.getDate().ToString( dateString );
    this->ui->endLineEdit->setText( dateString.c_str() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeCheckButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->AutoUpdateIncludeTags = false;
  
  // check all selected tree items
  QTreeWidgetItem *item;
  QList< QTreeWidgetItem* > list = this->ui->tagTreeWidget->selectedItems();

  for( int index = 0; index < list.size(); ++index )
  {
    item = list.at( index );
    if( Qt::Unchecked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Checked );
  }

  // manually update the graph view
  this->UpdateIncludeTags();
  this->RenderGraph( true );
  this->AutoUpdateIncludeTags = true;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeUnCheckButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->AutoUpdateIncludeTags = false;

  // check all selected tree items
  QTreeWidgetItem *item;
  QList< QTreeWidgetItem* > list = this->ui->tagTreeWidget->selectedItems();

  for( int index = 0; index < list.size(); ++index )
  {
    item = list.at( index );
    if( Qt::Checked == item->checkState( 0 ) ) item->setCheckState( 0, Qt::Unchecked );
  }

  // manually update the graph view
  this->UpdateIncludeTags();
  this->RenderGraph( true );
  this->AutoUpdateIncludeTags = true;
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagTreeItemChanged( QTreeWidgetItem* item, int column )
{
  // the checked state of the item may have changed, update the tag info singleton
  ovTag *tag = ovOrlandoTagInfo::GetInfo()->FindTag( item->text( column ).toStdString() );
  if( tag ) tag->active = Qt::Checked == item->checkState( column ) ? true : false;
  if( this->AutoUpdateIncludeTags )
  {
    this->UpdateIncludeTags();
    this->RenderGraph( true );
  }
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
void ovQMainWindow::UpdateIncludeTags()
{
  // build a string array from all the currently active tags
  vtkSmartPointer< vtkStringArray > tags = vtkSmartPointer< vtkStringArray >::New();
  
  QTreeWidgetItemIterator it( this->ui->tagTreeWidget );
  while( *it )
  {
    if( Qt::Checked == (*it)->checkState( 0 ) )
    {
      tags->InsertNextValue( (*it)->text( 0 ).toStdString() );
    }
    it++;
  }

  this->RestrictGraphFilter->SetIncludeTags( tags );
}

void ovQMainWindow::RenderGraph( bool resetCamera )
{
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
    this->RestrictGraphFilter->GetOutput()->GetNumberOfVertices(),
    this->RestrictGraphFilter->GetOutput()->GetNumberOfEdges() );
  this->ui->statusbar->showMessage( buffer );
}
