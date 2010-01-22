#include <qapplication.h>
#include <qfiledialog.h>
#include "Orlando4.h"
#include "graph.h"
#include "Words.h"

//Orlando constructor
Orlando::Orlando(QWidget* parent)
  : QMainWindow(parent)
{
  //set up user interface
  setupUi(this);

  //create file open menu item
  QAction* a_fileOpen = new QAction(tr("&Open"), this);
  a_fileOpen->setShortcut(tr("Ctrl+O"));
  a_fileOpen->setStatusTip(tr("Open an orlando file"));
  connect(a_fileOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

  //create file open menu item
  QAction* a_fileLoad = new QAction(tr("&Load"), this);
  a_fileLoad->setShortcut(tr("Ctrl+L"));
  a_fileLoad->setStatusTip(tr("Load an XML file"));
  connect(a_fileLoad, SIGNAL(triggered()), this, SLOT(fileLoad()));

  //create file save menu item
  QAction* a_fileSave = new QAction(tr("&Save"), this);
  a_fileSave->setShortcut(tr("Ctrl+S"));
  a_fileSave->setStatusTip(tr("Save a file"));
  connect(a_fileSave, SIGNAL(triggered()), this, SLOT(fileSave()));

  //create file exit menu item
  QAction* a_fileExit = new QAction(tr("&Exit"), this);
  a_fileExit->setShortcut(tr("Ctrl+Q"));
  a_fileExit->setStatusTip(tr("Exit"));
  connect(a_fileExit, SIGNAL(triggered()), this, SLOT(fileExit()));

  //create file menu
  QMenu* file_menu = this->menuBar()->addMenu(tr("&File"));
  file_menu->addAction(a_fileOpen);
  file_menu->addAction(a_fileLoad);
  file_menu->addAction(a_fileSave);
  file_menu->addAction(a_fileExit);

  //create name tags menu item
  a_names = new QAction(tr("Name Tags On"), this);
  a_names->setStatusTip(tr("Turn Name Tags On"));
  connect(a_names, SIGNAL(triggered()), this, SLOT(nameTagsOn()));
  a_names->setCheckable(true);

  a_names2 = new QAction(tr("Name Tags Off"), this);
  a_names2->setStatusTip(tr("Turn Name Tags Off"));
  connect(a_names2, SIGNAL(triggered()), this, SLOT(nameTagsOff()));
  a_names2->setCheckable(true);

  QAction* a_names3 = new QAction(tr("Clear Name Tags"), this);
  a_names3->setStatusTip(tr("Clear Existing Name Tags"));
  connect(a_names3, SIGNAL(triggered()), this, SLOT(nameTagsClear()));
  
  QAction* a_names4 = new QAction(tr("Show All Name Tags"), this);
  a_names4->setStatusTip(tr("Turns On All Name Tags"));
  connect(a_names4, SIGNAL(triggered()), this, SLOT(allNameTagsOn()));

  //create file menu for name tags
  QMenu* file_menu2 = this->menuBar()->addMenu(tr("&Labels"));
  file_menu2->addAction(a_names);
  file_menu2->addAction(a_names2);
  file_menu2->addAction(a_names3);
  file_menu2->addAction(a_names4);

  //create path menu item
  QAction* a_path = new QAction(tr("&Path Search"), this);
  a_path->setStatusTip(tr("Search for a Path Between Two Nodes"));
  connect(a_path, SIGNAL(triggered()), this, SLOT(findPath()));
  
  //create file menu for path
  QMenu* file_menu3 = this->menuBar()->addMenu(tr("&Paths"));
  file_menu3->addAction(a_path);

  //create tags menu item
  QAction* a_tags = new QAction(tr("&Choose Tags"), this);
  a_tags->setStatusTip(tr("Choose the Tags to Use"));
  connect(a_tags, SIGNAL(triggered()), this, SLOT(popUpTags()));
  
  //create file menu for tags
  QMenu* file_menu4 = this->menuBar()->addMenu(tr("&Tags"));
  file_menu4->addAction(a_tags);

  //create text menu item
  QAction* a_text = new QAction(tr("&Get XML Text"), this);
  a_text->setStatusTip(tr("Retrieve the XML Data Text"));
  connect(a_text, SIGNAL(triggered()), this, SLOT(popUpText()));
  
  //create file menu for tags
  QMenu* file_menu5 = this->menuBar()->addMenu(tr("T&ext"));
  file_menu5->addAction(a_text);

  bool testTrue = true;

  //connect signals to slots
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(search()));
  connect(pushButton, SIGNAL(pressed()), this, SLOT(search()));
  connect(toolButton_2, SIGNAL(pressed()), this, SLOT(graphButton()));
  connect(toolButton, SIGNAL(pressed()), this, SLOT(toggleMode()));
  connect(toolButton_3, SIGNAL(pressed()), this, SLOT(cameraMode()));  
  connect(toolButton_4, SIGNAL(pressed()), this, SLOT(highlightMode()));
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleClick(QListWidgetItem*)));

  toolButton_3->setAutoRaise(true);

  //set selection mode to an extended selection
  listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
};

void Orlando::graphButton()
{
  
  graphMode(true);
}

//when item is double-clicked
void Orlando::doubleClick(QListWidgetItem* item)
{
  progressBar->setValue(0);

  //char* to hold item's string
  char* str = (char*) calloc(1000, sizeof(char));
  //get item's string
  sprintf(str, item->text());
  //set selected to string
  graph->setSelected(str);
  //free string
  free(str);
  
  //if toggle is on
  if(style->getToggle())
    {
      //set toggle
      style->setToggle(false);
    }
  //if hightight it on
  else if(style->getHighlight())
    {
      //set highlight
      style->setHighlight(false);
    }
  //if graph is on
  else
    {
      //set graph
      style->setGraph(false);
    }
}

//Return progress bar
QProgressBar* Orlando::getProgressBar()
{
  return progressBar;
}

//Set the graph to gra
void Orlando::setGraph(Graph* gra)
{
  graph = gra;
}

//return widget
QWidget* Orlando::getWidget()
{
  return this;
}

//return button
QPushButton* Orlando::getButton()
{
  return pushButton;
}

//return window 2
vtkRenderWindow* Orlando::getWindow2()
{
  return qvtkWidget->GetRenderWindow();
}

//return window
vtkRenderWindow* Orlando::getWindow()
{
  return qvtkWidget_2->GetRenderWindow();
}

//return interactor
QVTKInteractor* Orlando::getInteractor()
{
  return qvtkWidget_2->GetInteractor();
}

//return interactor 2
QVTKInteractor* Orlando::getInteractor2()
{
  return qvtkWidget->GetInteractor();
}

//return list
QListWidget* Orlando::getList()
{
  return listWidget;
}

//return label
QLabel* Orlando::getLabel()
{
  return labelSelected;
}

//return line edit
QLineEdit* Orlando::getLineEdit()
{
  return lineEdit;
}

void Orlando::fileSave()
{  
  QString str = QFileDialog::getSaveFileName(this, tr("Save File"), "/home", tr("Orlando Files (*.orlando)")); 

  if(str != "")
    {
      char* fname = (char*) calloc(1000, sizeof(char));
      sprintf(fname, str);
      graph->saveFile(fname);
    }
}

//when file open menu item is pressed
void Orlando::fileOpen()
{
  QString str = QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Orlando Files (*.orlando)"));

  if(str != "")
    {
      char* fname = (char*) calloc(1000, sizeof(char));
      sprintf(fname, str);
      graph->loadFile(fname);
    }
}

void Orlando::fileLoad()
{
  QString str = QFileDialog::getOpenFileName(this, tr("Load File"), "/home", tr("XML Files (*.xml)")); 

  if(str != "")
    {
      Words wd;
      wd.exec();

      char* fname = (char*) calloc(1000, sizeof(char));
      sprintf(fname, str);
      graph->loadXML(fname);
    }
}

//when file exit menu item is pressed exit application
void Orlando::fileExit()
{
  qApp->exit();
}

void Orlando::popUpTags()
{
  graph->tagWindowOn();

  graphMode(true);
}

void Orlando::popUpText()
{
  graph->textWindowOn();
}

//when name tags menu item is pressed
void Orlando::nameTagsOn()
{
  //turn all names on
  graph->allNamesOn(false);    
  a_names->setChecked(true);
  a_names2->setChecked(false);
}

void Orlando::allNameTagsOn()
{
  graph->allNamesOn(true);  
  a_names->setChecked(true);
  a_names2->setChecked(false);
}

void Orlando::nameTagsOff()
{
  progressBar->setValue(0);  

  //turn all names off
  graph->allNamesOff(true);
  a_names->setChecked(false);
  a_names2->setChecked(true);
}

void Orlando::nameTagsClear()
{
  graph->allNamesOff(false);
}

//return label 1
QLabel* Orlando::getLabel1()
{
  return label;
}

//return label 2
QLabel* Orlando::getLabel2()
{
  return labelConnected;
}

//when search button is pressed
void Orlando::search()
{
  //char* str to hold searchbox's string
  char* str = (char*) calloc(1000, sizeof(char));
  //get searchbox's string
  sprintf(str, lineEdit->text());
  //search the string
  graph->search(str);
  //free string
  free(str);
}

//set user style to st
void Orlando::setUserStyle(userStyle* st)
{
  style = st;
}

//when camera mode button is pressed
void Orlando::cameraMode()
{
  toolButton->setAutoRaise(false);
  toolButton_2->setAutoRaise(false);
  toolButton_3->setAutoRaise(true);
  toolButton_4->setAutoRaise(false);
  
  //change interactor to camera
  graph->changeInteractorToCamera();
  //set camera to true in user style
  style->setCamera();
}

//when toggle mode button is pressed
void Orlando::toggleMode()
{
  toolButton->setAutoRaise(true);
  toolButton_2->setAutoRaise(false);
  toolButton_3->setAutoRaise(false);
  toolButton_4->setAutoRaise(false);

  graph->resetGraphCons();

  //if camera mode is on change it to graph mode
  graph->changeInteractorToGraph();
  
  //set toggle to true in user style
  style->setToggle(true);
}

//when graph mode button is pressed
void Orlando::graphMode(bool draw)
{
  toolButton->setAutoRaise(false);
  toolButton_2->setAutoRaise(true);
  toolButton_3->setAutoRaise(false);
  toolButton_4->setAutoRaise(false);

  graph->resetGraphCons();

  //change it to graph mode
  graph->changeInteractorToGraph();

  //set graph to true
  style->setGraph(draw);
}

//when highlight mode button is pressed
void Orlando::highlightMode()
{  
  (getInteractor())->Disable();

  toolButton->setAutoRaise(false);
  toolButton_2->setAutoRaise(false);
  toolButton_3->setAutoRaise(false);
  toolButton_4->setAutoRaise(true);

  graph->resetGraphCons();

  //change it to graph mode
  graph->changeInteractorToGraph();
  
  //set highlight to true in user style
  style->setHighlight(true);
  
  (getInteractor())->Enable();
}

void Orlando::findPath()
{
  progressBar->setValue(0);
  graph->popUpPath();
}
