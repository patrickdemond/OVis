#include "ovUserStyle.h"
#include "vtkCommand.h"
#include <qapplication.h>
#include <qfiledialog.h>
#include "QVTKWidget.h"

//constructor for ovUserStyle
ovUserStyle::ovUserStyle( QVTKInteractor* interact, vtkRenderWindow* window )
{
  //set the bool variables to false
  act = false;
  dim = false;
  toggle = false;
  search = false;
  path = false;
  shift = false;
  
  //set global variables to passed in objects
  wind = window;
  inter = interact;
  
  //set the interaction style to ovUserStyle
  inter->SetInteractorStyle( this );
  
  //initialize timers
  UseTimersOn();
  SetTimerDuration( 1000 );
}  

//destructor for ovUserStyle
ovUserStyle::~ovUserStyle()
{

}  

//Turns toggle mode on
//If nw is true reset toggle and set toggle to true, else set toggle to true
void ovUserStyle::setToggle( bool nw )
{
  inter->Disable();

  //if nw true
  if( nw )
    {
      //reset toggle
      ////gra->toggleOn();
    }      
  //set toggle to true
  ////gra->setToggle( true );
  
  //toggle with no real coordinates
  ////gra->toggle( 0, 0 );
  
  //draw selected node
  ////gra->select( shift );

  //render window
  ////gra->renderWin();     

  //change bools
  toggle = true;
  highlight = false;
  act = false;
  path = false;

  inter->Enable();
}

//Turns on highlight mode
//If nw is true turn highlight on and highlight, else highlight
void ovUserStyle::setHighlight( bool nw )
{
  inter->Disable();

  //if nw true
  if( nw ) //&& !//gra->getLoad() )
    {
      //turn on highlight in graph
      ////gra->highlightOn();
    }      

  //highlight with no real coordinates
  ////gra->highlight( 0, 0 );
  ////gra->drawHighlighted();

  //draw selected node
  ////gra->select( false );

  //render window
  ////gra->renderWin();

  //change bools
  act = false;
  path = false;
  toggle = false;
  highlight = true;

  inter->Enable();
}

//Return true if highlight is on, false otherwise
bool ovUserStyle::getHighlight()
{
  return highlight;
}

//Return true if toggle is on, false otherwise
bool ovUserStyle::getToggle()
{
  return toggle;
}

//Return true if graph is on, false otherwise
bool ovUserStyle::getGraph()
{
  return act;
}

//Return true if path is on, false otherwise
bool ovUserStyle::getPath()
{
  return path;
}

//Return true if camera is on, false otherwise
bool ovUserStyle::getCamera()
{
  return ( !act&&!toggle );
}

//Turns on graph mode
void ovUserStyle::setGraph( bool nw )
{
  inter->Disable();

  if( nw )
    {
      //redraw the graph
      ////gra->redrawGraph();  
    }

  //display nd info with no real coordinates
  ////gra->displayNdInfo( 0, 0 );
 
  //set toggle to false
  ////gra->setToggle( false );

  //change bools
  toggle = false;
  highlight = false;
  act = true;
  path = false;

 //draw selected node
  ////gra->select( false );
   
  //if( //gra->getMode() != 'p' )
  //  {
      //render window
      ////gra->renderWin();
  //  }

  inter->Enable();
}

//Turn camera mode on
void ovUserStyle::setCamera()
{
  //change bools
  toggle = false;
  act = false;
}

//On mouse move event
void ovUserStyle::OnMouseMove()
{
  //disable user interaction
  inter->Disable();

  //ints to hold mouse coordinates
  int x;
  int y;
  //get mouse coordinates
  inter->GetEventPosition( x, y );

 //destroy name
  ////gra->destroyName();

  ////gra->highlightNode( x, y );

  //start the timer
  StartTimer(); 

  //enable user interaction
  inter->Enable();

  //do normal parent class function
  vtkInteractorStyleJoystickCamera::OnMouseMove();
}

void ovUserStyle::OnTimer()
{
  //disable user interaction
  inter->Disable();

  //ints to hold mouse coordinates
  int x;
  int y;
  //get mouse coordinates
  inter->GetEventPosition( x, y );

  //show name at coordinates
  ////gra->showName( x, y );

  //enable user interaction 
  inter->Enable();

  //do normal parent class function
  vtkInteractorStyleJoystickCamera::OnTimer();
}

//Function to do nothing when key is down
void ovUserStyle::OnKeyDown()
{
  
  inter->Disable();


  if( string( inter->GetKeySym() ) == "Shift_L" )
  {
    shift = true;
  }

  inter->Enable();

}

//Function to do nothing when key is up
void ovUserStyle::OnKeyUp()
{

  inter->Disable();

  if( string( inter->GetKeySym() ) == "Shift_L" )
  {
    shift = false;
  }

  inter->Enable();

}

//Function to do nothing when key is released
void ovUserStyle::OnKeyRelease()
{
}

//Function to do nothing when char on
void ovUserStyle::OnChar()
{
}

//when key pressed check which key is pressed and if necessary do commands required ( NOTHING )
void ovUserStyle::OnKeyPress()
{ 
}

//when left button is clicked
void ovUserStyle::OnLeftButtonDown()
{
  //turn off user interaction
  inter->Disable();
  
  //ints to hold mouse position
  int x;
  int y;

  //get mouse coordinates
  inter->GetEventPosition( x, y );

  
  //turn name on/off at position
  //gra->nameOnOff( true, x, y );

  if( shift )
  {
  //SELECTED is set in nodeatpos in graph.cpp
      //int* to hold window size
      int* coords;
      
      //get window size
      coords = wind->GetSize();

 // Need to temporary selected node like "nodeAtPos" to correctly add it to the selected list.
 //gra->nodeAtPosNoSelect( x, y );
 //gra->shiftSelect( shift );   
 
  }
  else
  {

  //if path is off
  if( !path )
    {
      //int* to hold window size
      int* coords;
      
      //get window size
      coords = wind->GetSize();

      //if toggle is on
      if( toggle )
  {
    //set toggle for mouse position
    //gra->toggle( x, y );
    //draw selected node
    //gra->select( shift );
  }
      //if graph mode is enabled select node and display node information
      else if( act )
  {      
    //display the node information at mouse coordinates
    //gra->displayNdInfo( x, y );
    //draw selected node
    //gra->select( shift );  
  }
      //if highlight is on
      else if( highlight )
  {
    //highlight at the mouse coordinates
    //gra->highlight( x, y );
    //draw selected node
    //gra->select( shift );
  }
    }
  }
  //enable user interaction
  inter->Enable();
}

//when middle button is clicked
void ovUserStyle::OnMiddleButtonDown()
{
  //disable user interaction
  inter->Disable();

 //if toggle  mode is on move the node
  if( toggle )
    {
      //ints to hold mouse coordinates
      int x;
      int y;
      //Get mouse coordinates
      inter->GetEventPosition( x, y );

      //move node at mouse coordinates
      //gra->moveNode( x, y );

      //toggle at the mouse coordinates
      //gra->toggle( x, y );

      //draw selected node
      //gra->select( false );
    }
  //if graph mode is on move the node
  else if( act )
    {
      //ints to hold mouse coordinates
      int x;
      int y;
      //Get mouse coordinates
      inter->GetEventPosition( x, y );

      //move node at mouse coordinates
      //gra->moveNode( x, y );

      //display node information at mouse coordinates
      //gra->displayNdInfo( x, y );

      //draw selected node
      //gra->select( false );
    }

  //turn path to false
  path = false;

  //enable user interaction
  inter->Enable();
}

//when middle button is lifted up
void ovUserStyle::OnMiddleButtonUp()
{
  //disable user interaction
  inter->Disable();

  //if toggle mode is enabled change the position of the node
  if( toggle )
    {
      if( path )
  {
    //gra->setMode( 't' );
  }

      //ints to hold mouse coordinates
      int x;
      int y;
      //get mouse position
      inter->GetEventPosition( x, y );

      //change node to move to new position
      //gra->changeToPos( x, y );

      //toggle position
      //gra->toggle( x, y );

      //draw selected node
      //gra->select( false );
    }
  //if graph mode is on change the position of the node
  else if( act )
    {
      if( path )
  {
    //gra->setMode( 'g' );
  }

      //ints to hold mouse coordinates
      int x;
      int y;
      //get mouse coordinates
      inter->GetEventPosition( x, y );

      //change node to move to the new position
      //gra->changeToPos( x, y );

      //display node information at mouse position
      //gra->displayNdInfo( x, y );

      //redraw the graph
      //gra->redrawGraph();

      //draw selected node
      //gra->select( false );
    }

  //enable user interaction
  inter->Enable();
}

//when right button is clicked
void ovUserStyle::OnRightButtonDown()
{
  //disable user interaction
  inter->Disable();

  //ints to hold mouse coordinates
  int x;
  int y;
  
  //get mouse coordinates
  inter->GetEventPosition( x, y );

  //gra->nameOnOff( false, x, y );

  //if graph mode is enabled find the path
  if( act )
    {
      //find path at coordinates
      ////gra->findPath( x, y ); 

      //draw selected node
      ////gra->select();

      //turn path to true
      //path = true;   
    }
  else if( toggle | highlight )
    {    
      //gra->deselect( x, y );
    }
  
  //enable user interaction
  inter->Enable();
}

//do nothing when mouse wheel is moved forward
void ovUserStyle::OnMouseWheelForward()
{
}

//do nothing when mouse wheel is moved backward
void ovUserStyle::OnMouseWheelBackward()
{
}
