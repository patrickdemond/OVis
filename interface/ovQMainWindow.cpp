/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQMainWindow.h"

#include "ui_ovQMainWindow.h"

#include <QComboBox>
#include <QFileDialog>
#include <QListWidget>

#include "vtkCommand.h"
#include "vtkGraph.h"
#include "vtkGraphLayoutView.h"
#include "vtkMath.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"

#include "source/ovOrlandoReader.h"
#include "source/ovOrlandoTagInfo.h"

class ovQMainWindowProgressCommand : public vtkCommand
{
public:
  static ovQMainWindowProgressCommand *New() { return new ovQMainWindowProgressCommand; }
  virtual void Execute( vtkObject *, unsigned long eventId, void *callData )
  {
    if( this->progressBar )
    {
      double progress = *( static_cast<double*>( callData ) );
      int value = vtkMath::Floor( 100 * progress );
      if( progressBar->value() != value ) this->progressBar->setValue( value );
    }
  }

  QProgressBar *progressBar;

protected:
  ovQMainWindowProgressCommand() { this->progressBar = NULL; }
};


//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::ovQMainWindow( QWidget* parent )
  : QMainWindow( parent )
{
  QMenu *menu;
  
  this->ui = new Ui_ovQMainWindow;
  this->ui->setupUi( this );

  // create file open menu item
  this->actionFileOpen = new QAction( tr( "&Open Data" ), this );
  this->actionFileOpen->setShortcut( tr( "Ctrl+O" ));
  this->actionFileOpen->setStatusTip( tr( "Open a data file" ) );
  QObject::connect(
    this->actionFileOpen, SIGNAL( triggered() ),
    this, SLOT( slotFileOpen() ));

  // create file exit menu item
  this->actionFileExit = new QAction( tr( "&Exit" ), this );
  this->actionFileExit->setShortcut( tr( "Ctrl+Q" ));
  this->actionFileExit->setStatusTip( tr( "Exit the application" ));
  QObject::connect(
    this->actionFileExit, SIGNAL( triggered() ),
    this, SLOT( slotFileExit() ));

  // create file menu
  menu = this->menuBar()->addMenu( tr( "&File" ));
  menu->addAction( this->actionFileOpen );
  menu->addAction( this->actionFileExit );
  
  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategyToClustering2D();
  this->GraphLayoutView->SetInteractor( this->ui->graphLayoutWidget->GetInteractor() );
  this->ui->graphLayoutWidget->SetRenderWindow( this->GraphLayoutView->GetRenderWindow() );
  vtkRenderer *renderer =
    this->ui->graphLayoutWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  renderer->SetBackground( 0, 0, 0 );
  renderer->GradientBackgroundOff();

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
};

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQMainWindow::~ovQMainWindow()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFileOpen()
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Orlando File" ), ".", tr( "Orlando XML Files (*.xml)" ) );

  if( "" != fileName )
  {
    // open the xml file using the orlando reader
    vtkSmartPointer< ovOrlandoReader > reader = vtkSmartPointer< ovOrlandoReader >::New();
    
    // watch the graph layout view for progress events
    vtkSmartPointer< ovQMainWindowProgressCommand > observer =
      vtkSmartPointer< ovQMainWindowProgressCommand >::New();
    observer->progressBar = this->ui->progressBar;
    reader->AddObserver( vtkCommand::ProgressEvent, observer );
    
    // define the tag list based on orlando tags
    char buffer[64];
    for( int rank = 1, total = ovOrlandoTagInfo::GetInfo()->GetNumberOfRanks(); rank <= total; rank++ )
    {
      sprintf( buffer, "Preset List #%d", rank );
      this->ui->tagListPresetComboBox->addItem( buffer, rank );
    }
    // ovQMainWindow::slotTagListPresetComboBoxIndexChanged is triggered by setting the current
    // index which will populate the tagListWidget
    this->ui->tagListPresetComboBox->setCurrentIndex( 0 );
    
    // load the orlando file and render
    reader->SetFileName( fileName.toStdString() );
    this->GraphLayoutView->SetRepresentationFromInput( reader->GetOutput() );
    this->GraphLayoutView->ResetCamera();
    this->GraphLayoutView->Render();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotFileExit()
{
  qApp->exit();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListCheckAllButtonClicked()
{
  QListWidgetItem *item;
  for( int row = 0; row < this->ui->tagListWidget->count(); row++ )
  {
    item = this->ui->tagListWidget->item( row );
    if( Qt::Unchecked == item->checkState() ) item->setCheckState( Qt::Checked );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListCheckNoneButtonClicked()
{
  QListWidgetItem *item;
  for( int row = 0; row < this->ui->tagListWidget->count(); row++ )
  {
    item = this->ui->tagListWidget->item( row );
    if( Qt::Checked == item->checkState() ) item->setCheckState( Qt::Unchecked );
  }
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListItemChanged( QListWidgetItem* item )
{
  // the checked state of the item may have changed, update the tag info singleton
  ovTag *tag = ovOrlandoTagInfo::GetInfo()->FindTag( item->text().toStdString() );
  if( tag ) tag->active = Qt::Checked == item->checkState() ? true : false;
}
 
//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQMainWindow::slotTagListPresetComboBoxIndexChanged( int index )
{
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
}
