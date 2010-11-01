/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovRemoveIsolatedVertices.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovRemoveIsolatedVertices - vtkRemoveIsolatedVertices with progress
//
// .SECTION Description
// Identical copy of vtkRemoveIsolatedVertices from VTK 5.6.0 but with
// progress events added in.
//
// Input port 0: graph

#ifndef __ovRemoveIsolatedVertices_h
#define __ovRemoveIsolatedVertices_h

#include "vtkGraphAlgorithm.h"

class vtkDataSet;

class ovRemoveIsolatedVertices : public vtkGraphAlgorithm
{
public:
  static ovRemoveIsolatedVertices* New();
  vtkTypeRevisionMacro(ovRemoveIsolatedVertices,vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  ovRemoveIsolatedVertices();
  ~ovRemoveIsolatedVertices();

  int RequestData(
    vtkInformation*, 
    vtkInformationVector**, 
    vtkInformationVector*);
  
private:
  ovRemoveIsolatedVertices(const ovRemoveIsolatedVertices&); // Not implemented
  void operator=(const ovRemoveIsolatedVertices&);   // Not implemented
};

#endif

