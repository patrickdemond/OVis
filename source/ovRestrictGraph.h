/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovRestrictGraph.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovRestrictGraph - Restricts edges to the given tags
//
// .SECTION Description
// ovRestrictGraph restricts the graph by removing any edges which do not
// have any of the tags listed in the given tag list.  Vertices which are
// left with no edges are also removed.
//
// Input port 0: graph

#ifndef __ovRestrictGraph_h
#define __ovRestrictGraph_h

#include "vtkGraphAlgorithm.h"

class vtkStringArray;

class ovRestrictGraph : public vtkGraphAlgorithm
{
public:
  static ovRestrictGraph* New();
  vtkTypeRevisionMacro( ovRestrictGraph, vtkGraphAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );

  vtkGetObjectMacro( IncludeTags, vtkStringArray );
  virtual void SetIncludeTags( vtkStringArray* );

protected:
  ovRestrictGraph();
  ~ovRestrictGraph();

  int FillInputPortInformation( int port, vtkInformation* info );
  
  int RequestData(
    vtkInformation*, 
    vtkInformationVector**, 
    vtkInformationVector* );
 
  vtkStringArray *IncludeTags;
  
private:
  ovRestrictGraph( const ovRestrictGraph& ); // Not implemented
  void operator=( const ovRestrictGraph& );   // Not implemented
};

#endif

