/*
 * Main application file which launches ovis
 * Author: Patrick D. Emond <emondpd@mcmaster.ca>
 */

#include <QApplication>
#include "interface/ovQMainWindow.h"
#include "interface/ovQTags.h"
#include "source/ovGraph.h"

// main function
int main( int argc, char** argv )
{
  // create application
  QApplication application( argc, argv );

  // create main window
  ovQMainWindow mainWindow;

  ovQTags tagWindow;

  // create the graph
  ovGraph* graph = new ovGraph(
    mainWindow.getWindow(),
    mainWindow.getInteractor(),
    mainWindow.getList(),
    mainWindow.getLabel(),
    mainWindow.getLineEdit(),
    mainWindow.getWidget(),
    mainWindow.getButton(),
    mainWindow.getLabel1(),
    mainWindow.getLabel2(),
    mainWindow.getWindow2(),
    mainWindow.getInteractor2(),
    mainWindow.getProgressBar(),
    tagWindow.getTagList(),
    &mainWindow,
    &tagWindow
  );

  // set main widget for the application to the main window
  application.setMainWidget( &mainWindow );  
  
  // show the main window
  mainWindow.show();

  // set the graph in the main window
  mainWindow.setGraph( graph );

  list<char*> tagList;
  graph->setTags( tagList );

  // return the result of the executed application
  return application.exec();
}
