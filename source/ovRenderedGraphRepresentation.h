/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovRenderedGraphRepresentation.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovRenderedGraphRepresentation - Custom ovRenderedGraphRepresentation
//
// .SECTION Description
// ovRenderedGraphRepresentation overrides vtkRenderedGraphRepresentation in order
// to implement custom graph selection functionality.
//

#ifndef __ovRenderedGraphRepresentation_h
#define __ovRenderedGraphRepresentation_h

#include "vtkRenderedGraphRepresentation.h"

class ovApplyColors;

class ovRenderedGraphRepresentation : public vtkRenderedGraphRepresentation
{
public:
  static ovRenderedGraphRepresentation* New();
  vtkTypeMacro( ovRenderedGraphRepresentation, vtkRenderedGraphRepresentation );

  ovApplyColors* GetApplyColors();
  
  // Description:
  // Apply a theme to this representation.
  virtual void ApplyViewTheme(vtkViewTheme* theme);

protected:
  ovRenderedGraphRepresentation();
  ~ovRenderedGraphRepresentation() {};
  
private:
  ovRenderedGraphRepresentation( const ovRenderedGraphRepresentation& ); // Not implemented
  void operator=( const ovRenderedGraphRepresentation& );   // Not implemented
};

#endif

