/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovNode.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovNode - A mathematical node
//
// .SECTION Description
// This class represents a node in a mathematical graph.  It is connected to
// other nodes by edges (ovEdge).
//
// .SECTION See Also ovEdge
// 

#ifndef __ovNode_h
#define __ovNode_h

#include "vtkObject.h"
#include "../ovUtilities.h"

class ovNode : public vtkObject
{
public:
  vtkTypeRevisionMacro(ovNode, vtkObject);
  void PrintSelf(ostream &os, vtkIndent indent);

protected:
  // Description:
  // Constructor and destructor
  ovNode() {};
  ~ovNode() {};
  
private:
  ovNode( const ovNode& );  // Not implemented.
  void operator=( const ovNode& );  // Not implemented.
};

#endif
