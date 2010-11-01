/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovRestrictGraph.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovRestrictGraph.h"

#include "ovOrlandoTagInfo.h"
#include "ovUtilities.h"
#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkEdgeListIterator.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkMutableUndirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkVariantArray.h"

#include <vtkstd/algorithm>

///////////////////////////////////////////////////////////////////////////////////
// BuildGraph

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
template< typename GraphT >
static void BuildGraph(
  ovRestrictGraph *filter,
  vtkGraph* inputGraph,
  vtkStringArray *includeTags,
  vtkGraph* outputGraph )
{
  // if we have no include tags then do nothing
  if( NULL == includeTags )
  {
    outputGraph->ShallowCopy( inputGraph );
    return;
  }

  vtkSmartPointer<GraphT> graph = vtkSmartPointer<GraphT>::New();
  graph->ShallowCopy( inputGraph );
  vtkDataSetAttributes* const inputEdgeData = inputGraph->GetEdgeData();
  vtkDataSetAttributes* const outputEdgeData = graph->GetEdgeData();
  outputEdgeData->CopyAllocate( inputEdgeData );
  
  // if there the array is empty then display an empty graph
  if( 0 == includeTags->GetNumberOfValues() )
  {
    graph->Initialize();
  }
  else
  {
    // Go through each edge in the graph and remove all those which do not have any
    // of the tags in the includeTags array.  Tag info is stored as the scalar in
    // the edge data in the form of a bit array (see ovOrlandoReader::ProcessRequest())
    ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
    vtkDataSetAttributes *data = graph->GetEdgeData();
    int numTags = tagInfo->GetNumberOfTags();
    
    // Take the include tags array and turn it into a bit map
    bool *includeBits = new bool[ numTags ];
    for( int i = 0; i < numTags; i++ ) includeBits[ i ] = false;
    for( vtkIdType i = 0; i < includeTags->GetNumberOfValues(); i++ )
    {
      int tagIndex = tagInfo->FindTagIndex( includeTags->GetValue( i ) );
      if( 0 <= tagIndex && tagIndex < numTags ) includeBits[ tagIndex ] = true;
    }
    
    double progress, numEdges = 0, totalEdges = inputGraph->GetNumberOfEdges();
  
    // read the edge's bits and compare them to the include bits
    vtkSmartPointer< vtkIdTypeArray > removeIds = vtkSmartPointer< vtkIdTypeArray >::New();
    vtkSmartPointer< vtkEdgeListIterator > it = vtkSmartPointer<vtkEdgeListIterator>::New();
    inputGraph->GetEdges( it );
    while( it->HasNext() )
    {
      // invoke progress update
      progress = numEdges++ / totalEdges;
      if( filter ) filter->InvokeEvent( vtkCommand::ProgressEvent, &( progress ) );
  
      bool match = false;
      vtkIdType edgeId = it->Next().Id;
      for( int i = 0; i < inputGraph->GetEdgeData()->GetNumberOfArrays(); i++ )
      {
        vtkAbstractArray *array = inputGraph->GetEdgeData()->GetAbstractArray( i );
        if( includeBits[ i ] && 0 != array->GetVariantValue( edgeId ).ToInt() )
        {
          match = true;
          break;
        }
      }
  
      // if we do not have a match then remove the edge from the graph
      if( !match ) removeIds->InsertNextValue( edgeId );
    }
    
    graph->RemoveEdges( removeIds );
    delete [] includeBits;
  }

  outputGraph->ShallowCopy( graph ); 
}

///////////////////////////////////////////////////////////////////////////////////
// ovRestrictGraph

vtkCxxRevisionMacro( ovRestrictGraph, "$Revision: 1.2 $" );
vtkStandardNewMacro( ovRestrictGraph );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRestrictGraph::ovRestrictGraph()
{
  this->SetNumberOfInputPorts( 1 );
  this->IncludeTags = NULL;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRestrictGraph::~ovRestrictGraph()
{
  this->SetIncludeTags( NULL );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovRestrictGraph::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovRestrictGraph::SetIncludeTags( vtkStringArray* newTagArray )
{
  // make sure the new array holds different values from the current
  // array before causing the filter to re-run itself
  bool isDifferent = false;
  
  if( newTagArray != this->IncludeTags )
  {
    if( NULL == newTagArray || NULL == this->IncludeTags )
    {
      isDifferent = true;
    }
    else
    {
      int numValues = newTagArray->GetNumberOfValues();
      if( numValues != this->IncludeTags->GetNumberOfValues() )
      {
        isDifferent = true;
      }
      else
      {
        ovStringVector oldStrings, newStrings;
        ovStringVector::iterator oldIt, newIt;
        for( int index = 0; index < numValues; index++ )
        {
          oldStrings.push_back( this->IncludeTags->GetValue( index ) );
          newStrings.push_back( newTagArray->GetValue( index ) );
          vtkstd::sort( oldStrings.begin(), oldStrings.end() );
          vtkstd::sort( newStrings.begin(), newStrings.end() );
          for( oldIt = oldStrings.begin(), newIt = newStrings.begin();
               oldIt != oldStrings.end() && newIt != newStrings.end();
               ++oldIt, ++newIt )
          {
            if( ( *oldIt ) != ( *newIt ) )
            {
              isDifferent = true;
              break;
            }
          }
        }
      }
    }
  }
  
  if( isDifferent )
  {
    vtkStringArray* oldTagArray = this->IncludeTags;
    this->IncludeTags = newTagArray;
    if( NULL != this->IncludeTags ) { this->IncludeTags->Register( this ); }
    if( NULL != oldTagArray ) { oldTagArray->UnRegister( this ); }
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovRestrictGraph::FillInputPortInformation( int port, vtkInformation* info )
{
  if( port == 0 )
  {
    info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkGraph" );
    return 1;
  }
    
  return 0;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovRestrictGraph::RequestData(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector, 
  vtkInformationVector* outputVector )
{
  // Ensure we have valid inputs ...
  vtkGraph* const inputGraph = vtkGraph::GetData( inputVector[0] );
  vtkGraph* const outputGraph = vtkGraph::GetData( outputVector );

  // Build the new output graph, based on the graph type ...
  if( vtkDirectedGraph::SafeDownCast( inputGraph ) )
  {
    BuildGraph<vtkMutableDirectedGraph>( this, inputGraph, this->IncludeTags, outputGraph );
  }
  else if( vtkUndirectedGraph::SafeDownCast( inputGraph ) )
  {
    BuildGraph<vtkMutableUndirectedGraph>( this, inputGraph, this->IncludeTags, outputGraph );
  }
  else
  {
    vtkErrorMacro( << "Unknown input graph type" );
    return 0;
  }
    
  return 1;
}

