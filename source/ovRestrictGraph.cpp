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
#include "vtkMutableGraphHelper.h"
#include "vtkMutableUndirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkVariantArray.h"

#include <vtkstd/algorithm>

///////////////////////////////////////////////////////////////////////////////////
// ovRestrictGraph

vtkCxxRevisionMacro( ovRestrictGraph, "$Revision: 1.2 $" );
vtkStandardNewMacro( ovRestrictGraph );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRestrictGraph::ovRestrictGraph()
{
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
int ovRestrictGraph::RequestData(
  vtkInformation* vtkNotUsed( request ),
  vtkInformationVector** inputVector, 
  vtkInformationVector* outputVector )
{
  vtkGraph* input = vtkGraph::GetData( inputVector[0] );
  vtkGraph* output = vtkGraph::GetData( outputVector );

  // If we have no include tags then do nothing
  if( NULL == this->IncludeTags )
  {
    output->ShallowCopy( input );
    return 1;
  }
  
  // If the include tags array is empty then display an empty graph
  if( 0 == this->IncludeTags->GetNumberOfValues() )
  {
    return 1;
  }

  // Make sure the graph's edge data contains a tags arrays
  vtkStringArray *tagBitArray =
    vtkStringArray::SafeDownCast( input->GetEdgeData()->GetAbstractArray( "tags" ) );
  if( NULL == tagBitArray )
  {
    vtkErrorMacro( << "Input graph edge data does not have a 'tags' array." );
    output->ShallowCopy( input ); 
    return 0;
  }
  
  // Create a vector of booleans which correspond to the list of all tags and
  // whether or not to include them
  ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
  int numTags = tagInfo->GetNumberOfTags();
  int numIncludeTags = 0;
  vtkstd::vector< bool > includeBits( numTags, false );
  for( vtkIdType i = 0; i < this->IncludeTags->GetNumberOfValues(); i++ )
  {
    int tagIndex = tagInfo->FindTagIndex( this->IncludeTags->GetValue( i ) );
    if( 0 <= tagIndex && tagIndex < numTags )
    {
      includeBits[ tagIndex ] = true;
      numIncludeTags++;
    }
  }
  
  // Set up our mutable graph helper
  vtkSmartPointer<vtkMutableGraphHelper> builder = 
    vtkSmartPointer<vtkMutableGraphHelper>::New();
  if (vtkDirectedGraph::SafeDownCast(input))
  {
    vtkSmartPointer<vtkMutableDirectedGraph> dir = 
      vtkSmartPointer<vtkMutableDirectedGraph>::New();
    builder->SetGraph(dir);
  }
  else
  {
    vtkSmartPointer<vtkMutableUndirectedGraph> undir = 
      vtkSmartPointer<vtkMutableUndirectedGraph>::New();
    builder->SetGraph(undir);
  }

  // Initialize edge data, vertex data, and points
  vtkDataSetAttributes *inputEdgeData = input->GetEdgeData();
  vtkDataSetAttributes *builderEdgeData = builder->GetGraph()->GetEdgeData();
  builderEdgeData->CopyAllocate( inputEdgeData );

  vtkDataSetAttributes *inputVertData = input->GetVertexData();
  vtkDataSetAttributes *builderVertData = builder->GetGraph()->GetVertexData();
  builderVertData->CopyAllocate( inputVertData );

  vtkPoints* inputPoints = input->GetPoints();
  vtkSmartPointer<vtkPoints> builderPoints = vtkSmartPointer<vtkPoints>::New();
  builder->GetGraph()->SetPoints( builderPoints );

  // Vector keeps track of mapping of input vertex ids to output vertex ids
  vtkIdType numVert = input->GetNumberOfVertices();
  vtkstd::vector<int> outputVertex( numVert, -1 );

  vtkSmartPointer<vtkEdgeListIterator> edgeIter =
    vtkSmartPointer<vtkEdgeListIterator>::New();
  input->GetEdges( edgeIter );
  
  double progress, numEdges = 0, totalEdges = input->GetNumberOfEdges();
  while( edgeIter->HasNext() )
  {
    // invoke progress update
    progress = numEdges++ / totalEdges;
    this->InvokeEvent( vtkCommand::ProgressEvent, &( progress ) );

    vtkEdgeType e = edgeIter->Next();

    // check to see if this edge should be included and set the color array value
    // based on these results
    bool match = false;
    int includeTagIndex = 0;
    ovString includeTagMatches( numIncludeTags, '0' );
    for( int tagId = 0; tagId < numTags; tagId++ )
    {
      ovString tagBits = tagBitArray->GetValue( e.Id );
      if( includeBits[ tagId ] )
      {
        if( '1' == tagBitArray->GetValue( e.Id ).at( tagId ) )
        {
          includeTagMatches[includeTagIndex] = '1';
          match = true;
        }
        includeTagIndex++;
      }
    }
    
    if( match )
    {
      vtkIdType source = outputVertex[e.Source];
      if( source < 0 )
      {
        source = builder->AddVertex();
        outputVertex[e.Source] = source;
        builderVertData->CopyData( inputVertData, e.Source, source );
        builderPoints->InsertNextPoint( inputPoints->GetPoint( e.Source ) );
      }
      vtkIdType target = outputVertex[e.Target];
      if( target < 0 )
      {
        target = builder->AddVertex();
        outputVertex[e.Target] = target;
        builderVertData->CopyData( inputVertData, e.Target, target );
        builderPoints->InsertNextPoint( inputPoints->GetPoint( e.Target ) );
      }
      vtkEdgeType outputEdge = builder->AddEdge( source, target );
      builderEdgeData->CopyData( inputEdgeData, e.Id, outputEdge.Id );
      vtkVariantArray *colors = vtkVariantArray::SafeDownCast(
        builderEdgeData->GetAbstractArray( "colors" ) );
      if( colors ) colors->SetValue( outputEdge.Id, vtkVariant( ovHash( includeTagMatches ) ) );
    }
  }

  // Pass constructed graph to output.
  output->ShallowCopy( builder->GetGraph() );
  output->GetFieldData()->PassData( input->GetFieldData() );

  // Clean up
  output->Squeeze();

  return 1;
}
