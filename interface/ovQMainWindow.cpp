#include <qapplication.h>
#include <qfiledialog.h>
#include "ovQMainWindow.h"
#include "ovQWords.h"
#include "ovQScreenshot.h"

#include "source/ovGraph.h"

ovQMainWindow::ovQMainWindow( QWidget* parent )
  : QMainWindow( parent )
{
  //set up user interface
  setupUi( this );

  //create file open menu item
  a_fileOpen = new QAction( tr( "&Open" ), this );
  a_fileOpen->setShortcut( tr( "Ctrl+O" ));
  a_fileOpen->setStatusTip( tr( "Open an orlando file" ));
  connect( a_fileOpen, SIGNAL( triggered() ), this, SLOT( fileOpen() ));

  //create file open menu item
  a_fileLoad = new QAction( tr( "&Load" ), this );
  a_fileLoad->setShortcut( tr( "Ctrl+L" ));
  a_fileLoad->setStatusTip( tr( "Load an XML file" ));
  connect( a_fileLoad, SIGNAL( triggered() ), this, SLOT( fileLoad() ));

  //create file save menu item
  a_fileSave = new QAction( tr( "&Save" ), this );
  a_fileSave->setShortcut( tr( "Ctrl+S" ));
  a_fileSave->setStatusTip( tr( "Save a file" ));
  a_fileSave->setEnabled( false );
  connect( a_fileSave, SIGNAL( triggered() ), this, SLOT( fileSave() ));

  //create file exit menu item
  a_fileExit = new QAction( tr( "&Exit" ), this );
  a_fileExit->setShortcut( tr( "Ctrl+Q" ));
  a_fileExit->setStatusTip( tr( "Exit" ));
  connect( a_fileExit, SIGNAL( triggered() ), this, SLOT( fileExit() ));

  //create file menu
  QMenu* file_menu = this->menuBar()->addMenu( tr( "&File" ));
  file_menu->addAction( a_fileLoad );
  file_menu->addAction( a_fileSave );
  file_menu->addAction( a_fileOpen );
  file_menu->addAction( a_fileExit );

 //create file open menu item
  a_editScreenshot = new QAction( tr( "&Save ovQScreenshot" ), this );
  a_editScreenshot->setStatusTip( tr( "Save a ovQScreenshot as an Image" ));
  a_editScreenshot->setEnabled( false );
  connect( a_editScreenshot, SIGNAL( triggered() ), this, SLOT( saveScreenshot() ));

 // Create Clear Graph in the Edit Menu
  a_editClearGraph = new QAction( tr( "Clear Graph" ), this );
  a_editClearGraph->setStatusTip( tr( "Clear the current Graph" ));
  a_editClearGraph->setEnabled( false );
  connect( a_editClearGraph, SIGNAL( triggered() ), this, SLOT( clearGraph() ));

  //create file menu
  QMenu* edit_menu = this->menuBar()->addMenu( tr( "&Edit" ));
  edit_menu->addAction( a_editScreenshot );
  edit_menu->addAction( a_editClearGraph );

  //create name tags menu item
  a_names = new QAction( tr( "Name ovQTags On" ), this );
  a_names->setStatusTip( tr( "Turn Name ovQTags On" ));
  a_names->setEnabled( false );
  connect( a_names, SIGNAL( triggered() ), this, SLOT( nameTagsOn() ));
  a_names->setCheckable( true );

  a_names2 = new QAction( tr( "Name ovQTags Off" ), this );
  a_names2->setStatusTip( tr( "Turn Name ovQTags Off" ));
  a_names2->setEnabled( false );
  connect( a_names2, SIGNAL( triggered() ), this, SLOT( nameTagsOff() ));
  a_names2->setCheckable( true );

  a_names3 = new QAction( tr( "Clear Name ovQTags" ), this );
  a_names3->setStatusTip( tr( "Clear Existing Name ovQTags" ));
  a_names3->setEnabled( false );
  connect( a_names3, SIGNAL( triggered() ), this, SLOT( nameTagsClear() ));
  
  a_names4 = new QAction( tr( "Show All Name ovQTags" ), this );
  a_names4->setStatusTip( tr( "Turns On All Name ovQTags" ));
  a_names4->setEnabled( false );
  connect( a_names4, SIGNAL( triggered() ), this, SLOT( allNameTagsOn() ));

  a_names5 = new QAction( tr( "Label Properties" ), this );
  a_names5->setStatusTip( tr( "Set Label ovQText Size, Color, etc." ));
  connect( a_names5, SIGNAL( triggered() ), this, SLOT( setLabelProperties() ));

  //create file menu for name tags
  QMenu* file_menu2 = this->menuBar()->addMenu( tr( "&Labels" ));
  file_menu2->addAction( a_names );
  file_menu2->addAction( a_names2 );
  file_menu2->addAction( a_names3 );
  file_menu2->addAction( a_names4 );
  file_menu2->addAction( a_names5 );

  //create path menu item
  a_path = new QAction( tr( "&Path Search" ), this );
  a_path->setEnabled( false );
  a_path->setStatusTip( tr( "Search for a ovQPath Between Two Nodes" ));
  connect( a_path, SIGNAL( triggered() ), this, SLOT( findPath() ));
  
  //create file menu for path
  QMenu* file_menu3 = this->menuBar()->addMenu( tr( "&Paths" ));
  file_menu3->addAction( a_path );

  //create tags menu item
  a_tags = new QAction( tr( "&Choose ovQTags" ), this );
  a_tags->setStatusTip( tr( "Choose the ovQTags to Use" ));
  connect( a_tags, SIGNAL( triggered() ), this, SLOT( popUpTags() ));
  
  //create file menu for tags
  QMenu* file_menu4 = this->menuBar()->addMenu( tr( "&Tags" ));
  file_menu4->addAction( a_tags );

  //create text menu item
  a_text = new QAction( tr( "&Get XML ovQText" ), this );
  a_text->setEnabled( false );
  a_text->setStatusTip( tr( "Retrieve the XML Data ovQText" ));
  connect( a_text, SIGNAL( triggered() ), this, SLOT( popUpText() ));
  
  //create file menu for tags
  QMenu* file_menu5 = this->menuBar()->addMenu( tr( "T&ext" ));
  file_menu5->addAction( a_text );

  bool testTrue = true;

  //connect signals to slots
  connect( lineEdit, SIGNAL( returnPressed() ), this, SLOT( search() ));
  connect( pushButton, SIGNAL( pressed() ), this, SLOT( search() ));
  connect( toolButton_2, SIGNAL( pressed() ), this, SLOT( graphButton() ));
  connect( toolButton, SIGNAL( pressed() ), this, SLOT( toggleMode() ));
  connect( toolButton_3, SIGNAL( pressed() ), this, SLOT( cameraMode() ));  
  connect( toolButton_4, SIGNAL( pressed() ), this, SLOT( highlightMode() ));
  connect( listWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* )), this, SLOT( doubleClick( QListWidgetItem* )) );

  toolButton_3->setAutoRaise( true );

  //set selection mode to an extended selection
  listWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
  
  
  /*  The two lines below are a fix specific to compiling this on a MAC that allows it to display
    the Menu Bar - however, it seems to require QT 4+ to compile and must be commented out for
      the Linux etc. */
   
  // this->menuBar()->setNativeMenuBar( false );
  // this->menuBar()->setVisible( true );
  
};

// Clears the visuals and main data for the graph without instantiating anything new
void ovQMainWindow::clearGraph()
{
  graph->clearGraph();
}


// Saves a screenshot of the graph
void ovQMainWindow::saveScreenshot()
{ 
  QFileDialog ssD( this );
  
  ssD.setAcceptMode( QFileDialog::AcceptSave );

  // Available formats for the screenshot
  QStringList filters;
  filters << "PNG ( *.png )"
    << "JPG ( *.jpg )"
    << "Bitmap ( *.bmp )"
    << "TIFF ( *.tif )";

  ssD.setNameFilters( filters );

  ( getInteractor() )->Disable();

  ssD.exec();

  QString str2= ssD.selectedNameFilter();
  QStringList strs = ssD.selectedFiles();

  QString str = strs.takeLast();

  ssD.hide();

  ovQScreenshot* screenshotWin = new ovQScreenshot();

  screenshotWin->exec();
  int mag = screenshotWin->getMagnification();

  ( getInteractor() )->Enable();

  
 /* Checks string for screenshot and saves into correct format, also ensures there is a filename */
  if( str != "" )
    {

      if( !str.contains( ".png" ) && !str.contains( ".jpg" ) && !str.contains( ".bmp" ) && !str.contains( ".tif" ))
  {
      
    if( str2.compare( "PNG ( *.png )" ) == 0 )
      {
        str.append( ".png" );
      }
    else if( str2.compare( "JPG ( *.jpg )" ) == 0 )
      {
        str.append( ".jpg" );
      }
    else if( str2.compare( "Bitmap ( *.bmp )" ) == 0 )
      {
        str.append( ".bmp" );
      }
    else if( str2.compare( "TIFF ( *.tif )" ) == 0 )
      {
        str.append( ".tif" );
      }
  }

      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );

      if( str.contains( ".png" ))
  {
    graph->saveScreenshot( fname, PNG, mag );
  }
      else if( str.contains( ".jpg" ))
  {
    graph->saveScreenshot( fname, JPG, mag );
  } 
      else if( str.contains( ".bmp" ))
  {
    graph->saveScreenshot( fname, BMP, mag );
  }
      else if( str.contains( ".tif" ))
  {
    graph->saveScreenshot( fname, TIF, mag );
  }
    }
}

void ovQMainWindow::setVisualizationText( char* text )
{
  labelTitle->setText( text );
}

void ovQMainWindow::setLabelProperties()
{      
  ovQFont* fntWin = new ovQFont( this, graph );
      
  ( getInteractor() )->Disable();
  
  fntWin->exec();
  
  ( getInteractor() )->Enable();
}

void ovQMainWindow::graphButton()
{
  graphMode( true );
}

//when item is double-clicked
void ovQMainWindow::doubleClick( QListWidgetItem* item )
{
  progressBar->setValue( 0 );

  //char* to hold item's string
  char* str = ( char* ) calloc( 1000, sizeof( char ));
  //get item's string
  sprintf( str, item->text() ); 
  //set selected to string
  graph->setSelected( str );
  //determine what name tag to show
  graph->nameOnOff( true, str );
  //free string
  free( str );
  
  //if toggle is on
  if( style->getToggle() )
    {
      //set toggle
      style->setToggle( false );
    }
  //if hightight it on
  else if( style->getHighlight() )
    {
      //set highlight
      style->setHighlight( false );
    }
  //if graph is on
  else
    {
      //set graph
      style->setGraph( false );
    }
}

//Return progress bar
QProgressBar* ovQMainWindow::getProgressBar()
{
  return progressBar;
}

//Set the graph to gra
void ovQMainWindow::setGraph( ovGraph* gra )
{
  graph = gra;
}

//return widget
QWidget* ovQMainWindow::getWidget()
{
  return this;
}

//return button
QPushButton* ovQMainWindow::getButton()
{
  return pushButton;
}

//return window 2
vtkRenderWindow* ovQMainWindow::getWindow2()
{
  return qvtkWidget->GetRenderWindow();
}

//return window
vtkRenderWindow* ovQMainWindow::getWindow()
{
  return qvtkWidget_2->GetRenderWindow();
}

//return interactor
QVTKInteractor* ovQMainWindow::getInteractor()
{
  return qvtkWidget_2->GetInteractor();
}

//return interactor 2
QVTKInteractor* ovQMainWindow::getInteractor2()
{
  return qvtkWidget->GetInteractor();
}

//return list
QListWidget* ovQMainWindow::getList()
{
  return listWidget;
}

//return label
QLabel* ovQMainWindow::getLabel()
{
  return labelSelected;
}

//return line edit
QLineEdit* ovQMainWindow::getLineEdit()
{
  return lineEdit;
}

void ovQMainWindow::fileSave()
{  
  QString str = QFileDialog::getSaveFileName( this, tr( "Save File" ), "/home", tr( "Orlando Files ( *.orlando )" )); 

  if( str != "" )
    {
      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );
      graph->saveFile( fname );
    }
}

//when file open menu item is pressed
void ovQMainWindow::fileOpen()
{
  // Opens an Orlando file
  QString str = QFileDialog::getOpenFileName( this, tr( "Open File" ), "/home", tr( "Orlando Files ( *.orlando )" ));

  if( str != "" )
    {
      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );
      graph->loadFile( fname );
    }
}

//when file open menue item is pressed
void ovQMainWindow::fileLoad()
{
  // Opens an XML file
  QString str = QFileDialog::getOpenFileName( this, tr( "Load File" ), "/home", tr( "XML Files ( *.xml )" )); 

  if( str != "" )
    {
      ovQWords wd( graph );
      wd.exec();

      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );
      graph->loadXML( fname );
    }
}

//when file exit menu item is pressed exit application
void ovQMainWindow::fileExit()
{
  qApp->exit();
}

void ovQMainWindow::popUpTags()
{
  graph->tagWindowOn();
}

void ovQMainWindow::popUpText()
{
  graph->textWindowOn();
}

//when name tags menu item is pressed
void ovQMainWindow::nameTagsOn()
{
  graph->selectedNodesOn(); //Turns only selected nodes on
  
  a_names->setChecked( true );
  a_names2->setChecked( false );
}

void ovQMainWindow::allNameTagsOn()
{
  // turns on all name tags
  graph->allNamesOn( true );  
  a_names->setChecked( true );
  a_names2->setChecked( false );
}

void ovQMainWindow::nameTagsOff()
{
  progressBar->setValue( 0 );  

  //turn all names off
  graph->allNamesOff( true );
  a_names->setChecked( false );
  a_names2->setChecked( true );
}

void ovQMainWindow::nameTagsClear()
{
  graph->allNamesOff( false );
}

//return label 1
QLabel* ovQMainWindow::getLabel1()
{
  return label;
}

//return label 2
QLabel* ovQMainWindow::getLabel2()
{
  return labelConnected;
}

//when search button is pressed
void ovQMainWindow::search()
{
  //char* str to hold searchbox's string
  char* str = ( char* ) calloc( 1000, sizeof( char ));
  //get searchbox's string
  sprintf( str, lineEdit->text() );
  //search the string
  graph->search( str );
  //free string
  free( str );
}

//set user style to st
void ovQMainWindow::setUserStyle( ovUserStyle* st )
{
  style = st;
}

//when camera mode button is pressed
void ovQMainWindow::cameraMode()
{

  // Set the Graph's Mode to Camera
  graph->setMode( 'c' );

  toolButton->setAutoRaise( false );
  toolButton_2->setAutoRaise( false );
  toolButton_3->setAutoRaise( true );
  toolButton_4->setAutoRaise( false );
  
  //change interactor to camera
  graph->changeInteractorToCamera();
  //set camera to true in user style
  style->setCamera();
}

//when toggle mode button is pressed
void ovQMainWindow::toggleMode()
{

  // Set the Graph's Mode to Toggle
  graph->setMode( 't' );

  toolButton->setAutoRaise( true );
  toolButton_2->setAutoRaise( false );
  toolButton_3->setAutoRaise( false );
  toolButton_4->setAutoRaise( false );

  graph->resetGraphCons();

  //if camera mode is on change it to graph mode
  graph->changeInteractorToGraph();
  
  //set toggle to true in user style
  style->setToggle( true );
}

//when graph mode button is pressed
void ovQMainWindow::graphMode( bool draw )
{
  // Set the Graph Mode to Graph
  graph->setMode( 'g' );

  toolButton->setAutoRaise( false );
  toolButton_2->setAutoRaise( true );
  toolButton_3->setAutoRaise( false );
  toolButton_4->setAutoRaise( false );

  graph->resetGraphCons();

  //change it to graph mode
  graph->changeInteractorToGraph();

  //set graph to true
  style->setGraph( draw );
}

//when highlight mode button is pressed
void ovQMainWindow::highlightMode()
{ 
  // Set the Graph Mode to Highlight 
  graph->setMode( 'h' );
 
  ( getInteractor() )->Disable();

  toolButton->setAutoRaise( false );
  toolButton_2->setAutoRaise( false );
  toolButton_3->setAutoRaise( false );
  toolButton_4->setAutoRaise( true );

  graph->resetGraphCons();

  //change it to graph mode
  graph->changeInteractorToGraph();
  
  //set highlight to true in user style
  style->setHighlight( true );
  
  ( getInteractor() )->Enable();
}

void ovQMainWindow::findPath()
{
  progressBar->setValue( 0 );
  graph->popUpPath();
}

// Turns menu items on
void ovQMainWindow::enableMenuItems( char mode ){
  a_fileOpen->setEnabled( true );
  a_fileLoad->setEnabled( true );
  a_fileSave->setEnabled( true );
  a_fileExit->setEnabled( true ); 
  a_editScreenshot->setEnabled( true ); 
  a_editClearGraph->setEnabled( true );
  a_names->setEnabled( true );
  a_names2->setEnabled( true );
  a_names3->setEnabled( true );
  a_names4->setEnabled( true );
  a_names5->setEnabled( true );
  a_path->setEnabled( true );
  a_tags->setEnabled( true );
  a_text->setEnabled( true );
}
