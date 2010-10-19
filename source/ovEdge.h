/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovEdge.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovEdge - A mathematical node
//
// .SECTION Description
// This class represents a node in a mathematical graph.  It is connected to
// other nodes by edges (ovEdge).
//
// .SECTION See Also ovEdge
// 

#ifndef __ovEdge_h
#define __ovEdge_h

#include "vtkObject.h"
#include "../ovUtilities.h"

class ovEdge : public vtkObject
{
public:
  vtkTypeRevisionMacro(ovEdge, vtkObject);
  void PrintSelf(ostream &os, vtkIndent indent);

protected:
  // Description:
  // Constructor and destructor
  ovEdge() {};
  ~ovEdge() {};
  
private:
  ovEdge( const ovEdge& );  // Not implemented.
  void operator=( const ovEdge& );  // Not implemented.
};

#endif
