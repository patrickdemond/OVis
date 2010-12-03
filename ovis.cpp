/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovis.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .SECTION Description
// The main function which launches the application.
//

#include <QApplication>
#include "interface/ovQMainWindow.h"

// main function
int main( int argc, char** argv )
{
  // create application and set some defaults
  QApplication application( argc, argv );
  
  // set main widget for the application to the main window
  ovQMainWindow mainWindow;
  application.setMainWidget( &mainWindow );  
  mainWindow.show();

  // return the result of the executed application
  return application.exec();
}
