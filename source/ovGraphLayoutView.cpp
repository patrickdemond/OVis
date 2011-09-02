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
#include "vtkIdTypeArray.h"
#include "vtkInteractorStyleRubberBand2D.h"
#include "vtkObjectFactory.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"

///////////////////////////////////////////////////////////////////////////////////
// ovGraphLayoutView

vtkCxxRevisionMacro( ovGraphLayoutView, "$Revision: $" );
vtkStandardNewMacro( ovGraphLayoutView );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovGraphLayoutView::ProcessEvents(
  vtkObject* caller, unsigned long eventId, void* callData)
{
  this->Superclass::ProcessEvents( caller, eventId, callData );

  // we only need to process end selection changed events
  if( caller != this->GetInteractorStyle() ||
      eventId != vtkCommand::SelectionChangedEvent ) return;

  int numValues = 0;
  vtkIdTypeArray *edgeIds, *vertexIds;
  vtkAnnotationLink *link;
  ovIntVector nonUnionVertexArray, nonUnionEdgeArray;

  unsigned int* data = reinterpret_cast<unsigned int*>( callData );
  if( data[4] == vtkInteractorStyleRubberBand2D::SELECT_UNION )
  {
    // do a non-union select
    vtkSmartPointer< vtkSelection > selection = vtkSmartPointer<vtkSelection>::New();
    this->GenerateSelection( callData, selection );
    for( int i = 0; i < this->GetNumberOfRepresentations(); ++i )
      this->GetRepresentation(i)->Select( this, selection, false );
    
    link = this->GetRepresentation()->GetAnnotationLink();
    vertexIds = ovGraphLayoutView::GetSelectedVertexList( link );
    edgeIds = ovGraphLayoutView::GetSelectedVertexList( link );

    // and record the non-union arrays
    numValues = vertexIds->GetNumberOfTuples();
    for( int index = 0; index < numValues; index++ )
      nonUnionVertexArray.push_back( vertexIds->GetValue( index ) );
    numValues = edgeIds->GetNumberOfTuples();
    for( int index = 0; index < numValues; index++ )
      nonUnionEdgeArray.push_back( edgeIds->GetValue( index ) );
    
    // make an empty copy of the edge and vertex arrays
    vtkSmartPointer< vtkIdTypeArray > newVertexIds = vtkSmartPointer< vtkIdTypeArray >::New();
    vtkSmartPointer< vtkIdTypeArray > newEdgeIds = vtkSmartPointer< vtkIdTypeArray >::New();
    
    // add any ids in the selection which are not in the cache
    for( ovIntVector::iterator sIt = nonUnionVertexArray.begin();
         sIt != nonUnionVertexArray.end(); ++sIt )
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
      for( ovIntVector::iterator sIt = nonUnionVertexArray.begin();
           sIt != nonUnionVertexArray.end(); ++sIt )
      {
        if( *sIt == *cIt ) found = true;
        if( found ) break;
      }
  
      if( !found ) newVertexIds->InsertNextValue( *cIt );
    }
    
    vertexIds->DeepCopy( newVertexIds );
  
    // go through the non-union selected edge ids and invert whether they are in the cache
    for( ovIntVector::iterator sIt = nonUnionEdgeArray.begin();
         sIt != nonUnionEdgeArray.end(); ++sIt )
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
      for( ovIntVector::iterator sIt = nonUnionEdgeArray.begin();
           sIt != nonUnionEdgeArray.end(); ++sIt )
      {
        if( *sIt == *cIt ) found = true;
        if( found ) break;
      }
  
      if( !found ) newEdgeIds->InsertNextValue( *cIt );
    }
        
    edgeIds->DeepCopy( newEdgeIds );
    
    int tempData[] = { 1, 1, 1, 1, 1 };
    this->Superclass::ProcessEvents( caller, eventId, &tempData );

  }

  link = this->GetRepresentation()->GetAnnotationLink();
  vertexIds = ovGraphLayoutView::GetSelectedVertexList( link );
  edgeIds = ovGraphLayoutView::GetSelectedVertexList( link );

  // now cache the selected ids
  this->SelectedVertexCache.clear();
  numValues = vertexIds->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      this->SelectedVertexCache.push_back( vertexIds->GetValue( index ) );

  this->SelectedEdgeCache.clear();
  numValues = edgeIds->GetNumberOfTuples();
  if( 0 < numValues )
    for( int index = 0; index < numValues; index++ )
      this->SelectedEdgeCache.push_back( edgeIds->GetValue( index ) );
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
  return vtkSelectionNode::VERTEX == node->GetFieldType()
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
