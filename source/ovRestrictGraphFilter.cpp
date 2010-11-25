/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovRestrictGraphFilter.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovRestrictGraphFilter.h"

#include "ovOrlandoReader.h"
#include "ovOrlandoTagInfo.h"
#include "ovUtilities.h"

#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkEdgeListIterator.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkIntArray.h"
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
// ovRestrictGraphFilter

vtkCxxRevisionMacro( ovRestrictGraphFilter, "$Revision: 1.2 $" );
vtkStandardNewMacro( ovRestrictGraphFilter );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRestrictGraphFilter::ovRestrictGraphFilter()
{
  this->AuthorsOnly = false;
  this->GenderTypeRestriction = ovRestrictGraphFilter::GenderTypeRestrictionAny;
  this->WriterTypeRestriction = ovRestrictGraphFilter::WriterTypeRestrictionAny;
  this->IncludeTags = NULL;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRestrictGraphFilter::~ovRestrictGraphFilter()
{
  this->SetIncludeTags( NULL );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovRestrictGraphFilter::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovRestrictGraphFilter::SetIncludeTags( vtkStringArray* newTagArray )
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
int ovRestrictGraphFilter::RequestData(
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

  // Make sure we have all the necessary graph arrays
  vtkStringArray *tagBitArray =
    vtkStringArray::SafeDownCast( input->GetEdgeData()->GetAbstractArray( "tags" ) );
  if( NULL == tagBitArray )
  {
    vtkErrorMacro( << "Input graph edge data does not have a 'tags' array." );
    output->ShallowCopy( input ); 
    return 0;
  }
  
  vtkIntArray *genderArray =
    vtkIntArray::SafeDownCast( input->GetVertexData()->GetAbstractArray( "gender" ) );
  if( NULL == genderArray )
  {
    vtkErrorMacro( << "Input graph vertex data does not have a 'gender' array." );
    output->ShallowCopy( input ); 
    return 0;
  }
  
  vtkIntArray *writerTypeArray =
    vtkIntArray::SafeDownCast( input->GetVertexData()->GetAbstractArray( "writerTypes" ) );
  if( NULL == writerTypeArray )
  {
    vtkErrorMacro( << "Input graph vertex data does not have a 'writerTypes' array." );
    output->ShallowCopy( input ); 
    return 0;
  }
  
  // Create a vector of tag bit indices which are to be searched for in every edge
  ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
  int numTags = tagInfo->GetNumberOfTags();
  vtkstd::vector< vtkIdType > includeIndices;
  for( vtkIdType i = 0; i < this->IncludeTags->GetNumberOfValues(); ++i )
  {
    int tagIndex = tagInfo->FindTagIndex( this->IncludeTags->GetValue( i ) );
    if( 0 <= tagIndex && tagIndex < numTags ) includeIndices.push_back( tagIndex );
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

  vtkVariantArray *colorArray = vtkVariantArray::SafeDownCast(
    builderEdgeData->GetAbstractArray( "color" ) );
  if( NULL == colorArray )
  {
    vtkErrorMacro( << "Input graph edge data does not have a 'color' array." );
    output->ShallowCopy( input ); 
    return 0;
  }

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
    
    // STEP #1
    // make sure that the edge's two vertices are both to be included
    bool sourceIsAuthor = ovOrlandoReader::WriterTypeNone != writerTypeArray->GetValue( e.Source );
    bool targetIsAuthor = ovOrlandoReader::WriterTypeNone != writerTypeArray->GetValue( e.Target );
    if( this->AuthorsOnly && ( !sourceIsAuthor || !targetIsAuthor ) ) continue;

    int sourceGender = genderArray->GetValue( e.Source );
    int targetGender = genderArray->GetValue( e.Target );
    
    if( ovRestrictGraphFilter::GenderTypeRestrictionMale == this->GenderTypeRestriction )
    {
      if( ovOrlandoReader::GenderTypeFemale == sourceGender ||
          ovOrlandoReader::GenderTypeFemale == targetGender ) continue;
    }
    else if( ovRestrictGraphFilter::GenderTypeRestrictionFemale == this->GenderTypeRestriction )
    {
      if( ovOrlandoReader::GenderTypeMale == sourceGender ||
          ovOrlandoReader::GenderTypeMale == targetGender ) continue;
    }
    
    int sourceWriterType = writerTypeArray->GetValue( e.Source );
    int targetWriterType = writerTypeArray->GetValue( e.Target );
    if( ovRestrictGraphFilter::WriterTypeRestrictionWriter == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeBRW == sourceWriterType ||
          ovOrlandoReader::WriterTypeIBR == sourceWriterType ||
          ovOrlandoReader::WriterTypeBRW == targetWriterType ||
          ovOrlandoReader::WriterTypeIBR == targetWriterType ) continue;
    }
    else if( ovRestrictGraphFilter::WriterTypeRestrictionBRW == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeWriter == sourceWriterType ||
          ovOrlandoReader::WriterTypeIBR == sourceWriterType ||
          ovOrlandoReader::WriterTypeWriter == targetWriterType ||
          ovOrlandoReader::WriterTypeIBR == targetWriterType ) continue;
    }
    else if( ovRestrictGraphFilter::WriterTypeRestrictionIBR == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeWriter == sourceWriterType ||
          ovOrlandoReader::WriterTypeBRW == sourceWriterType ||
          ovOrlandoReader::WriterTypeWriter == targetWriterType ||
          ovOrlandoReader::WriterTypeBRW == targetWriterType ) continue;
    }
    else if( ovRestrictGraphFilter::WriterTypeRestrictionWriterOrBRW == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeIBR == sourceWriterType ||
          ovOrlandoReader::WriterTypeIBR == targetWriterType ) continue;
    }
    else if( ovRestrictGraphFilter:: WriterTypeRestrictionWriterOrIBR == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeBRW == sourceWriterType ||
          ovOrlandoReader::WriterTypeBRW == targetWriterType ) continue;
    }
    else if( ovRestrictGraphFilter::WriterTypeRestrictionBRWOrIBR == this->WriterTypeRestriction )
    {
      if( ovOrlandoReader::WriterTypeWriter == sourceWriterType ||
          ovOrlandoReader::WriterTypeWriter == targetWriterType ) continue;
    }

    // STEP #2
    // make sure that the edge itself is to be included
    vtkIdType matchIndex = -1;
    vtkstd::vector< vtkIdType >::iterator it;
    for( it = includeIndices.begin(); it != includeIndices.end(); ++it )
    {
      if( '1' == tagBitArray->GetValue( e.Id ).at( *it ) )
      {
        matchIndex = *it;
        break;
      }
    }

    if( 0 <= matchIndex )
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
      colorArray->SetValue( outputEdge.Id, vtkVariant( matchIndex ) );
    }
  }

  // Pass constructed graph to output.
  output->ShallowCopy( builder->GetGraph() );
  output->GetFieldData()->PassData( input->GetFieldData() );

  // Clean up
  output->Squeeze();

  return 1;
}