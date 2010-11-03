/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQMainWindow.h"

#include "ui_ovQMainWindow.h"

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
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkViewTheme.h"

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
    this->ui->actionViewSetBackgroundSolid, SIGNAL( triggered() ),
    this, SLOT( slotViewSetBackgroundSolid() ) );
  QObject::connect(
    this->ui->actionViewSetBackgroundTop, SIGNAL( triggered() ),
    this, SLOT( slotViewSetBackgroundTop() ) );
  QObject::connect(
    this->ui->actionViewSetBackgroundBottom, SIGNAL( triggered() ),
    this, SLOT( slotViewSetBackgroundBottom() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToNone, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleNone() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToVertex, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleVertex() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToDash, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleDash() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToCross, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleCross() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToThickCross, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleThickcross() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToTriangle, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleTriangle() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToSquare, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleSquare() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToCircle, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleCircle() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToDiamond, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleDiamond() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToArrow, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleArrow() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToThickArrow, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleThickarrow() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToHookedArrow, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleHookedarrow() ) );
  QObject::connect(
    this->ui->actionViewSetVertexStyleToEdgeArrow, SIGNAL( triggered() ),
    this, SLOT( slotViewSetVertexStyleEdgearrow() ) );

  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategyToClustering2D();
  this->GraphLayoutView->SetEdgeLayoutStrategyToPassThrough();
  this->GraphLayoutView->IconVisibilityOff();
  this->GraphLayoutView->SetEdgeScalarBarVisibility( false );
  this->GraphLayoutView->SetGlyphType( VTK_VERTEX_GLYPH );
  this->GraphLayoutView->SetEdgeColorArrayName( "colors" );
  this->GraphLayoutView->ColorEdgesOn();
  this->GraphLayoutView->SetScalingArrayName( "sizes" );
  this->GraphLayoutView->ScaledGlyphsOff();
  this->GraphLayoutView->SetInteractor( this->ui->graphLayoutWidget->GetInteractor() );
  this->ui->graphLayoutWidget->SetRenderWindow( this->GraphLayoutView->GetRenderWindow() );

  this->GraphLayoutViewTheme = vtkSmartPointer< vtkViewTheme >::New();
  this->GraphLayoutViewTheme->SetBackgroundColor( 0.0, 0.0, 0.0 );
  this->GraphLayoutViewTheme->SetBackgroundColor2( 0.0, 0.0, 0.0 );
  this->GraphLayoutViewTheme->SetPointSize( 6 );
  this->GraphLayoutViewTheme->SetLineWidth( 3 );
  vtkLookupTable *lut =
    vtkLookupTable::SafeDownCast( this->GraphLayoutViewTheme->GetCellLookupTable() );
  lut->SetValueRange( 0.5, 1.0 );
  lut->SetAlphaRange( 0.5, 1.0 );
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
void ovQMainWindow::slotViewSetBackgroundSolid()
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
void ovQMainWindow::slotViewSetBackgroundTop()
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
void ovQMainWindow::slotViewSetBackgroundBottom()
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
  // make sure only the selected vertex style menu item is checked
  this->ui->actionViewSetVertexStyleToNone->setChecked( type == VTK_NO_GLYPH );
  this->ui->actionViewSetVertexStyleToVertex->setChecked( type == VTK_VERTEX_GLYPH );
  this->ui->actionViewSetVertexStyleToDash->setChecked( type == VTK_DASH_GLYPH );
  this->ui->actionViewSetVertexStyleToCross->setChecked( type == VTK_CROSS_GLYPH );
  this->ui->actionViewSetVertexStyleToThickCross->setChecked( type == VTK_THICKCROSS_GLYPH );
  this->ui->actionViewSetVertexStyleToTriangle->setChecked( type == VTK_TRIANGLE_GLYPH );
  this->ui->actionViewSetVertexStyleToSquare->setChecked( type == VTK_SQUARE_GLYPH );
  this->ui->actionViewSetVertexStyleToCircle->setChecked( type == VTK_CIRCLE_GLYPH );
  this->ui->actionViewSetVertexStyleToDiamond->setChecked( type == VTK_DIAMOND_GLYPH );
  this->ui->actionViewSetVertexStyleToArrow->setChecked( type == VTK_ARROW_GLYPH );
  this->ui->actionViewSetVertexStyleToThickArrow->setChecked( type == VTK_THICKARROW_GLYPH );
  this->ui->actionViewSetVertexStyleToHookedArrow->setChecked( type == VTK_HOOKEDARROW_GLYPH );
  this->ui->actionViewSetVertexStyleToEdgeArrow->setChecked( type == VTK_EDGEARROW_GLYPH );
  
  this->GraphLayoutView->SetGlyphType( type );
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
}
