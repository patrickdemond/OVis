/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovGraphLayoutView.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovGraphLayoutView.h"

#include "ovUtilities.h"

#include "vtkAnnotationLink.h"
#include "vtkCommand.h"
#include "vtkDataRepresentation.h"
#include "vtkGraph.h"
#include "vtkIdTypeArray.h"
#include "vtkInteractorStyleRubberBand2D.h"
#include "vtkObjectFactory.h"
#include "vtkInEdgeIterator.h"
#include "vtkOutEdgeIterator.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"

///////////////////////////////////////////////////////////////////////////////////
// ovGraphLayoutView

vtkCxxRevisionMacro( ovGraphLayoutView, "$Revision: $" );
vtkStandardNewMacro( ovGraphLayoutView );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovGraphLayoutView::ovGraphLayoutView()
{
  this->Processing = false;
  this->ShiftKeyDown = false;
  this->ControlKeyDown = false;
  
  // watch for key press and releases
  this->GetInteractorStyle()->AddObserver( vtkCommand::KeyPressEvent, this->GetObserver() );
  this->GetInteractorStyle()->AddObserver( vtkCommand::KeyReleaseEvent, this->GetObserver() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::ProcessEvents(
  vtkObject* caller, unsigned long eventId, void* callData)
{
  this->Superclass::ProcessEvents( caller, eventId, callData );
  
  // only process events from the interactor style
  if( caller != this->GetInteractorStyle() ) return;

  if( vtkCommand::KeyPressEvent == eventId ||
      vtkCommand::KeyReleaseEvent == eventId )
  {
    // track if the CTRL and SHIFT keys are pressed
    vtkInteractorObserver *observer = vtkInteractorObserver::SafeDownCast( caller );
    vtkRenderWindowInteractor *iact = observer->GetInteractor();
    this->ShiftKeyDown = 0 < iact->GetShiftKey();
    this->ControlKeyDown = 0 < iact->GetControlKey();
  }
  else if( eventId == vtkCommand::SelectionChangedEvent && !this->Processing )
  {
    int numValues = 0;
    vtkAnnotationLink *link;
    vtkIdTypeArray *edgeIds, *vertexIds;

    if( this->ShiftKeyDown || this->ControlKeyDown )
    {
      this->Processing = true;
  
      // make new arrays for selected vertex and edges for any changes to be stored in
      vtkSmartPointer< vtkIdTypeArray > newVertexIds = vtkSmartPointer< vtkIdTypeArray >::New();
      vtkSmartPointer< vtkIdTypeArray > newEdgeIds = vtkSmartPointer< vtkIdTypeArray >::New();
  
      ovIntVector newlySelectedVertexVector, newlySelectedEdgeVector;
  
      // first figure out what has been selected (may need to add nodes/edges if CTRL is down)
      if( this->ShiftKeyDown )
      {
        // do a non-union select
        vtkSmartPointer< vtkSelection > selection = vtkSmartPointer<vtkSelection>::New();
        this->GenerateSelection( callData, selection );
        for( int i = 0; i < this->GetNumberOfRepresentations(); ++i )
          this->GetRepresentation(i)->Select( this, selection, false );
      }
  
      // get the graph's vertex and edge arrays
      link = this->GetRepresentation()->GetAnnotationLink();
      
      vertexIds = ovGraphLayoutView::GetSelectedVertexList( link );
      numValues = vertexIds->GetNumberOfTuples();
      for( int index = 0; index < numValues; index++ )
        newlySelectedVertexVector.push_back( vertexIds->GetValue( index ) );
  
      edgeIds = ovGraphLayoutView::GetSelectedEdgeList( link );
      numValues = edgeIds->GetNumberOfTuples();
      for( int index = 0; index < numValues; index++ )
        newlySelectedEdgeVector.push_back( edgeIds->GetValue( index ) );
      
      // now add connected vertex/edges if the control key is down
      if( this->ControlKeyDown )
      {
        vtkGraph* graph = vtkGraph::SafeDownCast( this->GetRepresentation()->GetInput() );
        
        if( NULL != graph )
        {
          if( !newlySelectedVertexVector.empty() )
          { // add all edges connected to each vertex
            vtkSmartPointer< vtkOutEdgeIterator > outEdgeIt;
            vtkSmartPointer< vtkInEdgeIterator > inEdgeIt;
  
            for( ovIntVector::iterator sIt = newlySelectedVertexVector.begin();
                 sIt != newlySelectedVertexVector.end(); ++sIt )
            {
              outEdgeIt = vtkSmartPointer< vtkOutEdgeIterator >::New();
              graph->GetOutEdges( *sIt, outEdgeIt );
              while( outEdgeIt->HasNext() ) newlySelectedEdgeVector.push_back( outEdgeIt->Next().Id ); 
  
              inEdgeIt = vtkSmartPointer< vtkInEdgeIterator >::New();
              graph->GetInEdges( *sIt, inEdgeIt );
              while( inEdgeIt->HasNext() ) newlySelectedEdgeVector.push_back( inEdgeIt->Next().Id );
            }
          }
          else if( !newlySelectedEdgeVector.empty() )
          {
            for( ovIntVector::iterator sIt = newlySelectedEdgeVector.begin();
                 sIt != newlySelectedEdgeVector.end(); ++sIt )
            {
              newlySelectedVertexVector.push_back( graph->GetSourceVertex( *sIt ) );
              newlySelectedVertexVector.push_back( graph->GetTargetVertex( *sIt ) );
            }
          }
        }
      }
      
      //unsigned int* data = reinterpret_cast<unsigned int*>( callData );
      //if( data[4] == vtkInteractorStyleRubberBand2D::SELECT_UNION )
      
      // now add or replace the selection
      if( !this->ShiftKeyDown ) // replace old selection
      {
        // easy to do, just copy the newly selected vectors
        for( ovIntVector::iterator sIt = newlySelectedVertexVector.begin();
             sIt != newlySelectedVertexVector.end(); ++sIt )
          newVertexIds->InsertNextValue( *sIt );
        for( ovIntVector::iterator sIt = newlySelectedEdgeVector.begin();
             sIt != newlySelectedEdgeVector.end(); ++sIt )
          newEdgeIds->InsertNextValue( *sIt );
      }
      else // the shift key is down, add/remove to/from old selection
      {
        // add any ids in the selection which are not in the cache
        for( ovIntVector::iterator sIt = newlySelectedVertexVector.begin();
             sIt != newlySelectedVertexVector.end(); ++sIt )
        {
          bool found = false;
          for( ovIntVector::iterator cIt = this->SelectedVertexCache.begin();
               cIt != this->SelectedVertexCache.end(); ++cIt )
          {
            if( *sIt == *cIt ) found = true;
            if( found ) break;
          }
      
          if( !found ) newVertexIds->InsertNextValue( *sIt );
        }
      
        // now add any ids in the cache which are not in the selection
        for( ovIntVector::iterator cIt = this->SelectedVertexCache.begin();
             cIt != this->SelectedVertexCache.end(); ++cIt )
        {
          bool found = false;
          for( ovIntVector::iterator sIt = newlySelectedVertexVector.begin();
               sIt != newlySelectedVertexVector.end(); ++sIt )
          {
            if( *sIt == *cIt ) found = true;
            if( found ) break;
          }
      
          if( !found ) newVertexIds->InsertNextValue( *cIt );
        }
        
        // go through the non-union selected edge ids and invert whether they are in the cache
        for( ovIntVector::iterator sIt = newlySelectedEdgeVector.begin();
             sIt != newlySelectedEdgeVector.end(); ++sIt )
        {
          bool found = false;
          for( ovIntVector::iterator cIt = this->SelectedEdgeCache.begin();
               cIt != this->SelectedEdgeCache.end(); ++cIt )
          {
            if( *sIt == *cIt ) found = true;
            if( found ) break;
          }
      
          if( !found ) newEdgeIds->InsertNextValue( *sIt );
        }
      
        // now add any ids in the cache which are not in the selection
        for( ovIntVector::iterator cIt = this->SelectedEdgeCache.begin();
             cIt != this->SelectedEdgeCache.end(); ++cIt )
        {
          bool found = false;
          for( ovIntVector::iterator sIt = newlySelectedEdgeVector.begin();
               sIt != newlySelectedEdgeVector.end(); ++sIt )
          {
            if( *sIt == *cIt ) found = true;
            if( found ) break;
          }
          
          if( !found ) newEdgeIds->InsertNextValue( *cIt );
        }
      }
      
      // this is a hack to make sure everything gets updated
      vertexIds->DeepCopy( newVertexIds );
      edgeIds->DeepCopy( newEdgeIds );
      int tempData[] = { 1, 1, 1, 1, 1 };
      this->Superclass::ProcessEvents( caller, eventId, &tempData );

      this->Processing = false;
    }

    // now cache the selected ids
    link = this->GetRepresentation()->GetAnnotationLink();

    vertexIds = ovGraphLayoutView::GetSelectedVertexList( link );
    this->SelectedVertexCache.clear();
    numValues = vertexIds->GetNumberOfTuples();
    if( 0 < numValues )
      for( int index = 0; index < numValues; index++ )
        this->SelectedVertexCache.push_back( vertexIds->GetValue( index ) );
  
    edgeIds = ovGraphLayoutView::GetSelectedEdgeList( link );
    this->SelectedEdgeCache.clear();
    numValues = edgeIds->GetNumberOfTuples();
    if( 0 < numValues )
      for( int index = 0; index < numValues; index++ )
        this->SelectedEdgeCache.push_back( edgeIds->GetValue( index ) );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
vtkIdTypeArray* ovGraphLayoutView::GetSelectedVertexList( vtkAnnotationLink *link )
{
  vtkSelection *selection = link->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return NULL;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  return vtkSelectionNode::VERTEX == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::GetSelectedVertexList( vtkAnnotationLink *link, ovIntVector *ids )
{
  if( NULL == ids ) return;
  ids->clear();
  
  vtkIdTypeArray *array = ovGraphLayoutView::GetSelectedVertexList( link );
  if( NULL == array ) return;

  int numValues = array->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      ids->push_back( array->GetValue( index ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::SetSelectedVertexList( vtkAnnotationLink* link, ovIntVector *ids )
{
  if( NULL == ids ) return;

  vtkIdTypeArray *array = ovGraphLayoutView::GetSelectedVertexList( link );
  if( NULL == array ) return;

  array->Initialize();
  for( ovIntVector::iterator it = ids->begin(); it != ids->end(); ++it )
    array->InsertNextValue( *it );

  link->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
vtkIdTypeArray* ovGraphLayoutView::GetSelectedEdgeList( vtkAnnotationLink *link )
{
  vtkSelection *selection = link->GetCurrentSelection();
  
  if( 2 != selection->GetNumberOfNodes() ) return NULL;

  // there will be two nodes, one are vertices and the other are edges
  vtkSelectionNode *node = selection->GetNode( 0 );
  return vtkSelectionNode::EDGE == node->GetFieldType()
        ? vtkIdTypeArray::SafeDownCast( node->GetSelectionList() )
        : vtkIdTypeArray::SafeDownCast( selection->GetNode( 1 )->GetSelectionList() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::GetSelectedEdgeList( vtkAnnotationLink *link, ovIntVector *ids )
{
  if( NULL == ids ) return;
  ids->clear();
  
  vtkIdTypeArray *array = ovGraphLayoutView::GetSelectedEdgeList( link );
  if( NULL == array ) return;

  int numValues = array->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      ids->push_back( array->GetValue( index ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::SetSelectedEdgeList( vtkAnnotationLink* link, ovIntVector *ids )
{
  if( NULL == ids ) return;

  vtkIdTypeArray *array = ovGraphLayoutView::GetSelectedEdgeList( link );
  if( NULL == array ) return;

  array->Initialize();
  for( ovIntVector::iterator it = ids->begin(); it != ids->end(); ++it )
    array->InsertNextValue( *it );

  link->Modified();
}
