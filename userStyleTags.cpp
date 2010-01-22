#include "userStyleTags.h"

#define nodeFile "/home/jberberi/Desktop/ORLANDO/test.nodes"

//constructor for userStyleTags
userStyleTags::userStyleTags(QVTKInteractor* interact, vtkRenderWindow* window, Graph* grap)
{
  //set the global variables to those passed in
  gra = grap;
  wind = window;
  inter = interact;

  //set the interactor style of the QVTKInteractor to userStyleTags
  inter->SetInteractorStyle(this);
}  

//destructor for userStyleTags
userStyleTags::~userStyleTags()
{
  //delete global variables
  inter->Delete();
}  

//when the mouse moves make sure the 3D mouse moves if enabled
void userStyleTags::OnMouseMove()
{
}

void userStyleTags::OnKeyDown()
{
}

void userStyleTags::OnKeyUp()
{
}

void userStyleTags::OnKeyRelease()
{
}

void userStyleTags::OnChar()
{
}

//when key pressed check which key is pressed and if necessary do commands required
void userStyleTags::OnKeyPress()
{ 
}

//when left button is clicked check which tag is clicked and turn it on/off
void userStyleTags::OnLeftButtonDown()
{
  //disable user input
  inter->Disable();

  //get mouse position
  int x;
  int y;
  inter->GetEventPosition(x, y);

  //get window size
  int* coords;
  coords = wind->GetSize();

  int a = x;
  int b = y;
  int c = coords[0];
  int d = coords[1];

  //get the coordinate keeping in mind the window size
  a = ((float)a/c)*800.0;
  b = ((float)b/d)*600.0;

  //call the tag touched function in the Graph class to turn on/off tag
  gra->tagTouched((int)a,(int)b);

  //rerender the window
  wind->Render();

  //enable user input
  inter->Enable();
}

//when middle button is clicked
void userStyleTags::OnMiddleButtonDown()
{
}

//when middle button is lifted up
void userStyleTags::OnMiddleButtonUp()
{ 
}

//when middle button is clicked
void userStyleTags::OnRightButtonDown()
{ 
}

void userStyleTags::OnMouseWheelForward()
{ 
}

void userStyleTags::OnMouseWheelBackward()
{ 
}
