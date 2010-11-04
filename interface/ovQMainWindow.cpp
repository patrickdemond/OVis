/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQMainWindow.h"

#include "ui_ovQMainWindow.h"

#include <QActionGroup>
#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QListWidget>

#include "vtkCommand.h"
#include "vtkGlyphSource2D.h"
#include "vtkGraph.h"
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
//#include "vtkDataSetAttributes.h"
//#include "vtkVariantArray.h"

#include "source/ovOrlandoReader.h"
#include "source/ovOrlandoTagInfo.h"
#include "source/ovRestrictGraph.h"

class ovQMainWindowProgressCommand : public vtkCommand
{
public:
  static ovQMainWindowProgressCommand *New() { return new ovQMainWindowProgressCommand; }
void Execute( vtkObject *caller, unsigned long eventId, void *callData )
  {
    if( this->ui )
    {
      // display the progress
      double progress = *( static_cast<double*>( callData ) );
      int value = vtkMath::Floor( 100 * progress );
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
        else if( ovRestrictGraph::SafeDownCast( caller ) )
        {
          message = QString( "Resolving visible edges..." );
        }
      
        if( message.length() ) this->ui->statusbar->showMessage( message );
      }
    }
  }

  //QProgressBar *progressBar;
  Ui_ovQMainWindow *ui;

protected:
  ovQMainWindowProgressCommand() { this->ui = NULL; }
};


//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::ovQMainWindow( QWidget* parent )
  : QMainWindow( parent )
{
  QMenu *menu;
  this->AutoUpdateGraphView = false;
  
  this->ui = new Ui_ovQMainWindow;
  this->ui->setupUi( this );

  // connect the file menu items
  QObject::connect(
    this->ui->actionFileOpen, SIGNAL( triggered() ),
    this, SLOT( slotFileOpen() ) );
  QObject::connect(
    this->ui->actionFileExit, SIGNAL( triggered() ),
    this, SLOT( slotFileExit() ) );
  
  // connect the view menu items
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
  QObject::connect(
    this->ui->actionSetLayoutStrategyToTree, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToTree() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToCosmicTree, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToCosmicTree() ) );
  QObject::connect(
    this->ui->actionSetLayoutStrategyToCone, SIGNAL( triggered() ),
    this, SLOT( slotSetLayoutStrategyToCone() ) );
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
  
  this->layoutStrategyActionGroup = new QActionGroup( this );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToRandom );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToForceDirected );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToSimple2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToClustering2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCommunity2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToFast2D );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCircular );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToTree );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCosmicTree );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToCone );
  this->layoutStrategyActionGroup->addAction( this->ui->actionSetLayoutStrategyToSpanTree );

  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategyToClustering2D();
  this->GraphLayoutView->DisplayHoverTextOn();
  this->GraphLayoutView->IconVisibilityOff();
  this->GraphLayoutView->SetGlyphType( VTK_VERTEX_GLYPH );
  this->GraphLayoutView->SetEdgeColorArrayName( "colors" );
  this->GraphLayoutView->ColorEdgesOn();
  this->GraphLayoutView->SetScalingArrayName( "sizes" );
  this->GraphLayoutView->ScaledGlyphsOff();
  vtkRenderedGraphRepresentation* rep = vtkRenderedGraphRepresentation::SafeDownCast(
    this->GraphLayoutView->GetRepresentation() );
  rep->SetVertexHoverArrayName( "pedigrees" );
  this->GraphLayoutView->SetInteractor( this->ui->graphLayoutWidget->GetInteractor() );
  this->ui->graphLayoutWidget->SetRenderWindow( this->GraphLayoutView->GetRenderWindow() );

  this->GraphLayoutViewTheme = vtkSmartPointer< vtkViewTheme >::New();
  this->GraphLayoutViewTheme->SetBackgroundColor( 0.0, 0.0, 0.0 );
  this->GraphLayoutViewTheme->SetBackgroundColor2( 0.0, 0.0, 0.0 );
  this->GraphLayoutViewTheme->SetPointSize( this->ui->vertexSizeSlider->value() );
  this->GraphLayoutViewTheme->SetLineWidth( this->ui->edgeSizeSlider->value() );
  vtkLookupTable *lut =
    vtkLookupTable::SafeDownCast( this->GraphLayoutViewTheme->GetCellLookupTable() );
  lut->SetAlphaRange( 1.0, 1.0 );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  
  // set up the reader and filters
  vtkSmartPointer< ovQMainWindowProgressCommand > observer;
  observer = vtkSmartPointer< ovQMainWindowProgressCommand >::New();
  observer->ui = this->ui;
  this->OrlandoReader = vtkSmartPointer< ovOrlandoReader >::New();
  this->OrlandoReader->AddObserver( vtkCommand::ProgressEvent, observer );
  this->RestrictFilter = vtkSmartPointer< ovRestrictGraph >::New();
  this->RestrictFilter->AddObserver( vtkCommand::ProgressEvent, observer );
  this->RestrictFilter->SetInput( this->OrlandoReader->GetOutput() );
  
  // set up the display property widgets
  QObject::connect(
    this->ui->vertexSizeSlider, SIGNAL( valueChanged( int ) ),
    this, SLOT( slotVertexSizeSliderValueChanged( int ) ) );
  QObject::connect(
    this->ui->edgeSizeSlider, SIGNAL( valueChanged( int ) ),
    this, SLOT( slotEdgeSizeSliderValueChanged( int ) ) );

  // set up the tag list
  this->ui->tagListWidget->setSortingEnabled( 1 );
  QObject::connect(
    this->ui->tagListCheckAllButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagListCheckAllButtonClicked() ) );
  QObject::connect(
    this->ui->tagListCheckNoneButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotTagListCheckNoneButtonClicked() ) );
  QObject::connect(
    this->ui->tagListWidget, SIGNAL( itemChanged( QListWidgetItem* ) ),
    this, SLOT( slotTagListItemChanged( QListWidgetItem* ) ) );
  QObject::connect(
    this->ui->tagListPresetComboBox, SIGNAL( currentIndexChanged( int ) ),
    this, SLOT( slotTagListPresetComboBoxIndexChanged( int ) ) );

  // start updating the graph view automatically
  this->AutoUpdateGraphView = true;
};

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::~ovQMainWindow()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFileOpen()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Orlando File" ), "data", tr( "Orlando XML Files (*.xml)" ) );

  if( "" != fileName )
  {
    // define the tag list based on orlando tags
    char buffer[64];
    this->ui->tagListPresetComboBox->clear();
    for( int rank = 1, total = ovOrlandoTagInfo::GetInfo()->GetNumberOfRanks(); rank <= total; rank++ )
    {
      sprintf( buffer, "Preset List #%d", rank );
      this->ui->tagListPresetComboBox->addItem( buffer, rank );
    }
    // ovQMainWindow::slotTagListPresetComboBoxIndexChanged is triggered by setting the current
    // index which will populate the tagListWidget
    this->ui->tagListPresetComboBox->setCurrentIndex( 0 );
    
    // load the orlando file and render
    this->OrlandoReader->SetFileName( fileName.toStdString() );
    this->GraphLayoutView->SetRepresentationFromInput( this->RestrictFilter->GetOutput() );
    this->UpdateGraphView( true );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFileExit()
{
  qApp->exit();
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
  this->GraphLayoutView->SetLayoutStrategy( strategy );
  this->GraphLayoutView->ResetCamera();
  this->GraphLayoutView->Render();
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
void ovQMainWindow::slotEdgeSizeSliderValueChanged( int value )
{
  this->GraphLayoutViewTheme->SetLineWidth( value );
  this->GraphLayoutView->ApplyViewTheme( this->GraphLayoutViewTheme );
  this->GraphLayoutView->Render();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListCheckAllButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->AutoUpdateGraphView = false;

  QListWidgetItem *item;
  for( int row = 0; row < this->ui->tagListWidget->count(); row++ )
  {
    item = this->ui->tagListWidget->item( row );
    if( Qt::Unchecked == item->checkState() ) item->setCheckState( Qt::Checked );
  }

  // manually update the graph view
  this->UpdateGraphView();
  this->AutoUpdateGraphView = true;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListCheckNoneButtonClicked()
{
  // don't update the graph view until we're done checking tags
  this->AutoUpdateGraphView = false;

  QListWidgetItem *item;
  for( int row = 0; row < this->ui->tagListWidget->count(); row++ )
  {
    item = this->ui->tagListWidget->item( row );
    if( Qt::Checked == item->checkState() ) item->setCheckState( Qt::Unchecked );
  }

  // manually update the graph view
  this->UpdateGraphView();
  this->AutoUpdateGraphView = true;
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListItemChanged( QListWidgetItem* item )
{
  // the checked state of the item may have changed, update the tag info singleton
  ovTag *tag = ovOrlandoTagInfo::GetInfo()->FindTag( item->text().toStdString() );
  if( tag ) tag->active = Qt::Checked == item->checkState() ? true : false;
  if( this->AutoUpdateGraphView ) this->UpdateGraphView();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListPresetComboBoxIndexChanged( int index )
{
  // don't update the graph view until we're done checking tags
  this->AutoUpdateGraphView = false;

  QVariant data = this->ui->tagListPresetComboBox->itemData( index );

  if( data.isValid() )
  {
    bool test = false;
    int tagListId = data.toInt( &test );
    if( test )
    {
      ovTagVector tags;
      ovTagVector::iterator it;
      QListWidgetItem *item;
      ovOrlandoTagInfo::GetInfo()->GetTags( tags, tagListId );

      this->ui->tagListWidget->clear();
      for( it = tags.begin(); it != tags.end(); it++ )
      {
        item = new QListWidgetItem( (*it)->name.c_str(), this->ui->tagListWidget );
        item->setCheckState( (*it)->active ? Qt::Checked : Qt::Unchecked );
        item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        this->ui->tagListWidget->addItem( item );
      }
    }
  }

  // manually update the graph view
  this->UpdateGraphView();
  this->AutoUpdateGraphView = true;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::UpdateGraphView( bool resetCamera )
{
  // build a string array from all the currently active tags
  vtkSmartPointer< vtkStringArray > tags = vtkSmartPointer< vtkStringArray >::New();

  QListWidgetItem *item;
  for( int row = 0; row < this->ui->tagListWidget->count(); row++ )
  {
    item = this->ui->tagListWidget->item( row );
    if( Qt::Checked == item->checkState() )
      tags->InsertNextValue( item->text().toStdString() );
  }
  
  this->RestrictFilter->SetIncludeTags( tags );
  if( resetCamera ) this->GraphLayoutView->ResetCamera();
  this->GraphLayoutView->Render();

/*
  vtkLookupTable *lut = vtkLookupTable::SafeDownCast(
    this->GraphLayoutViewTheme->GetCellLookupTable() );
  if( NULL == lut ) return;

  vtkstd::vector< double > hashArray;
  vtkstd::vector< double >::iterator it;
  for( vtkIdType i = 0; i < tags->GetNumberOfValues(); ++i )
  {
    hashArray.push_back( ovHash( tags->GetValue( i ) ) );
  }

  double* rng = lut->GetRange();
  double minVal = rng[0];
  double maxVal = rng[1];
  minVal = VTK_DOUBLE_MAX;
  maxVal = VTK_DOUBLE_MIN;
  
  for( it = hashArray.begin(); it != hashArray.end(); ++it )
  {
    if( *it > maxVal )
    {
      maxVal = *it;
    }
    if( *it < minVal )
    {
      minVal = *it;
    }
  }

  double scale = 1.0;
  if( minVal != maxVal )
  {
    scale = ( rng[1] - rng[0] ) / ( maxVal - minVal );
  }

  int i = 0;
  double rgb[3];
  for( it = hashArray.begin(); it != hashArray.end(); ++it )
  {
    double value = rng[0] + scale * ( *it - minVal );
    lut->GetColor( value, rgb );
    cout << tags->GetValue( i++ ) << " is (" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ")" << endl;
  }
*/
}
