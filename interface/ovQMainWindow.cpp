/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQMainWindow.h"

#include "ui_ovQMainWindow.h"

#include <QFileDialog>

#include "vtkCommand.h"
#include "vtkGraph.h"
#include "vtkGraphLayoutView.h"
#include "vtkMath.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"

#include "source/ovOrlandoReader.h"

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
  this->ActionFileOpen = new QAction( tr( "&Open Data" ), this );
  this->ActionFileOpen->setShortcut( tr( "Ctrl+O" ));
  this->ActionFileOpen->setStatusTip( tr( "Open a data file" ) );
  QObject::connect( this->ActionFileOpen, SIGNAL( triggered() ), this, SLOT( slotFileOpen() ));

  // create file exit menu item
  this->ActionFileExit = new QAction( tr( "&Exit" ), this );
  this->ActionFileExit->setShortcut( tr( "Ctrl+Q" ));
  this->ActionFileExit->setStatusTip( tr( "Exit the application" ));
  QObject::connect( this->ActionFileExit, SIGNAL( triggered() ), this, SLOT( slotFileExit() ));

  // create file menu
  menu = this->menuBar()->addMenu( tr( "&File" ));
  menu->addAction( this->ActionFileOpen );
  menu->addAction( this->ActionFileExit );
  
  // set up the graph layout view
  this->GraphLayoutView = vtkSmartPointer< vtkGraphLayoutView >::New();
  this->GraphLayoutView->SetLayoutStrategyToClustering2D();
  this->GraphLayoutView->SetInteractor( this->ui->graphLayoutWidget->GetInteractor() );
  this->ui->graphLayoutWidget->SetRenderWindow( this->GraphLayoutView->GetRenderWindow() );
  vtkRenderer *renderer =
    this->ui->graphLayoutWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
  renderer->SetBackground( 0, 0, 0 );
  renderer->GradientBackgroundOff();
};

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
