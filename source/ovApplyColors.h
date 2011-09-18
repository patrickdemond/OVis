/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovApplyColors.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovApplyColors - Custom vtkApplyColors
//
// .SECTION Description
// ovApplyColors overrides vtkApplyColors in order to implement custom graph
// selection functionality.
//

#ifndef __ovApplyColors_h
#define __ovApplyColors_h

#include "vtkApplyColors.h"

class vtkScalarsToColors;
class vtkUnsignedCharArray;

class ovApplyColors : public vtkApplyColors 
{
public:
  static ovApplyColors *New();
  vtkTypeMacro( ovApplyColors, vtkApplyColors );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // The default fading factor for all unselected elements.
  vtkSetMacro( FadingFactor, double );
  vtkGetMacro( FadingFactor, double );
  
  // Description:
  // The background color (needed for fading to work correctly)
  vtkSetVector3Macro( BackgroundColor, double );
  vtkGetVector3Macro( BackgroundColor, double );

protected:
  ovApplyColors();
  ~ovApplyColors() {};
  
  // Description:
  // Convert the vtkGraph into vtkPolyData.
  int RequestData(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );
  
  double FadingFactor;
  double BackgroundColor[3];

private:
  ovApplyColors( const ovApplyColors& );  // Not implemented.
  void operator=( const ovApplyColors& );  // Not implemented.
};

#endif
