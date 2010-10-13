#ifndef USERSTYLE_TAGS_H
#define USERSTYLE_TAGS_H

//include necessary files
#include "vtkInteractorStyleUser.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include <stdio.h>
#include "ovGraph.h"
#include "QVTKWidget.h"

using namespace std;

class ovUserStyleTags : public vtkInteractorStyleJoystickCamera
{

public:
  //constructor
  ovUserStyleTags( QVTKInteractor* interact, vtkRenderWindow* window, ovGraph* grap );
  //destructor
  ~ovUserStyleTags();

  //interaction events
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

private:
  //global variables
  ovGraph* gra;
  vtkInteractorStyleJoystickCamera *joyCam; 
  QVTKInteractor *inter;
  vtkRenderWindow *wind;
};

#endif
