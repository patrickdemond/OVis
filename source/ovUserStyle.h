#ifndef USERSTYLE_H
#define USERSTYLE_H

//include files necessary
#include "vtkInteractorStyleUser.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include <stdio.h>
#include "ovGraph.h"

using namespace std;

class ovUserStyle : public vtkInteractorStyleJoystickCamera
{

public:
  //constructor
  ovUserStyle( QVTKInteractor* interact, vtkRenderWindow* window, ovGraph* grap );

  //destructor
  ~ovUserStyle();

  //event methods
  void OnTimer();
  void OnMouseMove();
  void OnKeyPress();
  void OnKeyDown();
  void OnKeyUp();
  void OnKeyRelease();
  void OnChar();
  void OnLeftButtonDown();
  void OnMiddleButtonDown();
  void OnMiddleButtonUp();
  void OnRightButtonDown();
  void OnMouseWheelForward();
  void OnMouseWheelBackward();

  //set bools
  void setGraph( bool nw );
  void setCamera();
  void setHighlight( bool nw );
  void setToggle( bool nw );

  //get bools
  bool getGraph();
  bool getCamera();
  bool getHighlight();
  bool getToggle();
  bool getPath();
  
private:
  //global variables
  ovGraph* gra;
  bool highlight; //bool for highlight on or off
  bool path;      //bool for path on or off
  bool toggle;    //bool for Toggle on or off
  bool search;    //bool for Search on or off
  bool act;       //bool for Graph  on or off
  bool dim;       //bool for 3D mouse on or off
  bool shift;     //bool for indicating whether the shift key is currently pressed
  vtkInteractorStyleJoystickCamera *joyCam; 
  QVTKInteractor *inter;
  vtkRenderWindow *wind;
};

#endif
