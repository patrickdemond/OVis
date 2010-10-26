/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoReader.h"

#include "vtkDataSetAttributes.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>

vtkCxxRevisionMacro(ovOrlandoReader, "$Revision: 1.4 $");
vtkStandardNewMacro(ovOrlandoReader);

// this undef is required on the hp. vtkMutexLock ends up including
// /usr/inclue/dce/cma_ux.h which has the gall to #define read as cma_read
#ifdef read
#undef read
#endif

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovOrlandoReader::ovOrlandoReader()
{
  this->FileName = "";
  this->Reader = NULL;
  this->SetNumberOfInputPorts( 0 );
  this->SetNumberOfOutputPorts( 1 );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovOrlandoReader::~ovOrlandoReader()
{
  if( NULL != this->Reader )
  {
    this->FreeReader();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
vtkGraph* ovOrlandoReader::GetOutput(int idx)
{
  return vtkGraph::SafeDownCast( this->GetOutputDataObject( idx ) );
}

//----------------------------------------------------------------------------
void ovOrlandoReader::SetOutput( vtkGraph *output )
{
  this->GetExecutive()->SetOutputData( 0, output );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoReader::RequestDataObject(
  vtkInformation* request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkMutableDirectedGraph *output = vtkMutableDirectedGraph::New();
  this->SetOutput( output );
  // Releasing data for pipeline parallism.
  // Filters will know it is empty.
  output->ReleaseData();
  output->Delete();
  
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoReader::ProcessRequest(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject( 0 );

  if( request->Has( vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT() ) )
  {
    return this->RequestDataObject( request, inputVector, outputVector );
  }

  if( request->Has( vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT() ) )
  {
    return 1;
  }

  if( request->Has( vtkDemandDrivenPipeline::REQUEST_INFORMATION() ) )
  {
    return 1;
  }

  
  if( request->Has( vtkDemandDrivenPipeline::REQUEST_DATA() ) &&
      0 == outInfo->Get( vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER() ) )
  {
    vtkDebugMacro( << "Reading orlando data ...");
    
    // create and set up a graph
    vtkSmartPointer< vtkMutableDirectedGraph > graph =
      vtkSmartPointer< vtkMutableDirectedGraph >::New();
    vtkSmartPointer< vtkStringArray > pedigreeArray =
      vtkSmartPointer< vtkStringArray >::New();
    graph->GetVertexData()->AddArray( pedigreeArray );
    graph->GetVertexData()->SetPedigreeIds( pedigreeArray );
    
    // create a list of association types to track which apply to new edges
    // when they are added
    this->AssociationTypes.sort();
    this->AssociationTypes.unique();
    ovStringList currentTypes;

    try
    {
      vtkIdType currentVertexId;
      vtkStdString currentVertexPedigree;
      this->CreateReader();

      // process each node, one at a time
      while( this->ParseNode() )
      {
        // This node is an entry (vertex)
        if( 1 == this->CurrentNode.Depth &&
            0 == xmlStrcmp( BAD_CAST "ENTRY", this->CurrentNode.Name ) )
        {
          if( this->CurrentNode.IsOpeningElement() )
          {
            // create a new vertex using the Id (author name) as the pedigree
            currentVertexId = graph->AddVertex();
            pedigreeArray->InsertNextValue( ( char* )( this->CurrentNode.Id ) );
            currentVertexPedigree = pedigreeArray->GetValue( currentVertexId );
          }
          else if( this->CurrentNode.IsClosingElement() )
          {
            // no more child vertices for this vertex
            currentVertexId = -1;
          }
        }
        else if( 0 <= currentVertexId )
        {
          // This node describes an association (edge)
          if ( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "NAME" ) &&
               this->CurrentNode.IsOpeningElement() &&
               currentVertexPedigree != vtkStdString( ( char* )( this->CurrentNode.Standard ) ) )
          {
            // Create an edge from the current vertex to the (possibly) new vertex
            // using the standard (linked author name) to identify it
            graph->AddEdge( currentVertexPedigree, ( char* )( this->CurrentNode.Standard ) );
            
            // TODO: add the current types as properties to this edge
            currentTypes.sort();
            currentTypes.unique();
          }
          // This node describes an association type (edge data)
          else if(
            binary_search(
              this->AssociationTypes.begin(),
              this->AssociationTypes.end(),
              ( char* )( this->CurrentNode.Name ) ) )
          {
            vtkStdString name = ( char* )( this->CurrentNode.Name );
            if( this->CurrentNode.IsOpeningElement() )
            { // opening element, add the type to the current list
              currentTypes.push_back( name );
            }
            else if( this->CurrentNode.IsClosingElement() )
            { // closing element, remove the type from the current list
              currentTypes.remove( name );
            }
          }
        }
      }

      this->FreeReader();
    }
    catch( vtkstd::exception &e )
    {
      vtkErrorMacro( << e.what() );
      return 0;
    }
   
/*
    vtkFieldData* const field_data = this->ReadFieldData();
    graph->SetFieldData(field_data);
    field_data->Delete();

    int vertex_count = 0;
    this->Read(&vertex_count);
    this->ReadVertexData(graph, vertex_count);

    int edge_count = 0;
    this->Read(&edge_count);
    this->ReadEdgeData(graph, edge_count);
*/

    vtkDebugMacro(<< "Read " 
      << graph->GetNumberOfVertices() 
      << " vertices and "
      << graph->GetNumberOfEdges() 
      << " edges.\n");
  
    // Copy graph into output.
    vtkGraph* const output =
      vtkGraph::SafeDownCast( outInfo->Get( vtkDataObject::DATA_OBJECT() ) );
    if( !output->CheckedShallowCopy( graph ) )
    {
      vtkErrorMacro( << "Invalid graph structure, returning empty graph." );
      return 0;
    }
  }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}


//----------------------------------------------------------------------------
int ovOrlandoReader::FillOutputPortInformation( int, vtkInformation* info )
{
  info->Set( vtkDataObject::DATA_TYPE_NAME(), "vtkGraph" );
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "FileName: \"" << this->FileName << "\"" << endl;
  os << indent << "Reader: " << this->Reader << endl;
  os << indent << "CurrentNode: " << endl;
  this->CurrentNode.PrintSelf( os, indent.GetNextIndent() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::SetFileName( ovString fileName )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "FileName to " << fileName.c_str() );

  if( fileName != this->FileName )
  {
    this->FileName = fileName;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::AddAssociationType( ovString type )
{
  this->AssociationTypes.push_back( type );
  this->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::RemoveAssociationType( ovString type )
{
  this->AssociationTypes.remove( type );
  this->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::CreateReader()
{
  this->Reader = xmlReaderForFile( this->FileName.c_str(), NULL, 0 );
  if( NULL == this->Reader )
  {
    throw vtkstd::runtime_error( "Unable to open file." );
  }
  this->CurrentNode.Clear();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoReader::ParseNode()
{
  if( NULL == this->Reader )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }

  int result = xmlTextReaderRead( this->Reader );
  if( -1 == result )
  { // error
    throw vtkstd::runtime_error( "Parse error." );
  }
  else if( 0 == result )
  { // end of file
    this->CurrentNode.Clear();
  }
  else
  { // successful read
    this->CurrentNode.Name = xmlTextReaderConstName( this->Reader );
    if( this->CurrentNode.Name == NULL ) this->CurrentNode.Name = BAD_CAST "--";
    this->CurrentNode.Value = xmlTextReaderConstValue( this->Reader );
    this->CurrentNode.Id = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "id" );
    this->CurrentNode.Standard = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "STANDARD" );
    this->CurrentNode.Depth = xmlTextReaderDepth( this->Reader );
    this->CurrentNode.NodeType = xmlTextReaderNodeType( this->Reader );
    this->CurrentNode.IsEmptyElement = xmlTextReaderIsEmptyElement( this->Reader );
    this->CurrentNode.HasValue = xmlTextReaderHasValue( this->Reader );
  }

  return result;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::RewindReader()
{
  if( NULL == this->Reader )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }
  
  // close and reopen the current file
  this->FreeReader();
  this->CreateReader();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::FreeReader()
{
  if( NULL == this->Reader )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }

  xmlFreeTextReader( this->Reader );
  this->Reader = NULL;
}
