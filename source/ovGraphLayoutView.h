/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovGraphLayoutView.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovGraphLayoutView - Custom GraphLayoutView
//
// .SECTION Description
// ovGraphLayoutView overrides vtkGraphLayoutView in order to implement custom
// graph selection functionality.
//

#ifndef __ovGraphLayoutView_h
#define __ovGraphLayoutView_h

#include "vtkGraphLayoutView.h"

#include "ovUtilities.h"
#include "vtksys/SystemTools.hxx"

class vtkAnnotationLink;
class vtkIdTypeArray;

class ovGraphLayoutView : public vtkGraphLayoutView
{
public:
  static ovGraphLayoutView* New();
  vtkTypeRevisionMacro( ovGraphLayoutView, vtkGraphLayoutView );
  
  static vtkIdTypeArray* GetSelectedVertexList( vtkAnnotationLink* );
  static void GetSelectedVertexList( vtkAnnotationLink*, ovIntVector* );
  static void SetSelectedVertexList( vtkAnnotationLink*, ovIntVector* );
  static vtkIdTypeArray* GetSelectedEdgeList( vtkAnnotationLink* );
  static void GetSelectedEdgeList( vtkAnnotationLink*, ovIntVector* );
  static void SetSelectedEdgeList( vtkAnnotationLink*, ovIntVector* );

protected:
  ovGraphLayoutView() { this->Processing = false; }
  ~ovGraphLayoutView() {}

  // Called to process events.  Overrides behavior in vtkGraphLayoutView.
  virtual void ProcessEvents(vtkObject* caller, unsigned long eventId, void* callData);
  
  ovIntVector SelectedVertexCache;
  ovIntVector SelectedEdgeCache;
  bool Processing;

private:
  ovGraphLayoutView( const ovGraphLayoutView& ); // Not implemented
  void operator=( const ovGraphLayoutView& );   // Not implemented
};

#endif
