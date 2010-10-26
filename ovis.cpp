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
  // we are temporarily disabling the QT user-interface until the new reader/vtkGraph is incorperated
  /*
  ovOrlandoReader* reader = ovOrlandoReader::New();
  reader->SetFileName(
    "/home/emondpd/files/data/orlando/orlando_2010-08-04/orlando_entries_all_pub_c_2010-08-03.xml" );
  reader->AddAssociationType( "INTIMATERELATIONSHIPS" );
  reader->AddAssociationType( "FRIENDSANDASSOCIATES" );
  reader->AddAssociationType( "LEISUREANDSOCIETY" );
  reader->AddAssociationType( "MEMBER" );
  reader->AddAssociationType( "OCCUPATION" );
  reader->AddAssociationType( "PANTHOLOGIZATION" );
  reader->AddAssociationType( "PATTITUDES" );
  reader->AddAssociationType( "PAUTHORSHIP" );
  reader->AddAssociationType( "PDEDICATION" );
  reader->AddAssociationType( "PINFLUENCESHER" );
  reader->AddAssociationType( "PLITERARYSCHOOLS" );
  reader->AddAssociationType( "POLITICS" );
  reader->AddAssociationType( "PRODUCTION" );
  reader->AddAssociationType( "RECEPTION" );
  reader->AddAssociationType( "RELATIONSWITHPUBLISHERS" );
  reader->AddAssociationType( "RFICTIONALIZATION" );
  reader->AddAssociationType( "RPENALTIES" );
  reader->AddAssociationType( "RRESPONSES" );
  reader->AddAssociationType( "RRECOGNITIONS" );
  reader->AddAssociationType( "RSHEINFLUENCED" );
  reader->AddAssociationType( "TEXT" );
  reader->AddAssociationType( "TEXTUALFEATURES" );
  reader->AddAssociationType( "TINTERTEXTUALITY" );
  reader->AddAssociationType( "TTHEMEORTOPIC" );

  vtkGraphLayoutView *view = vtkGraphLayoutView::New();
  view->SetLayoutStrategyToClustering2D();

  view->SetRepresentationFromInput( reader->GetOutput() );
  view->ResetCamera();
  view->GetInteractor()->Start();
  
  reader->Delete();
  view->Delete();
  return 0;
  */

  // create application
  QApplication application( argc, argv );

  // create main window
  ovQMainWindow mainWindow;

  // set main widget for the application to the main window
  application.setMainWidget( &mainWindow );  
  
  // show the main window
  mainWindow.show();

  // return the result of the executed application
  return application.exec();
}
