/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoReader.h"

#include "ovOrlandoTagInfo.h"

#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovOrlandoReader, "$Revision: 1.4 $" );
vtkStandardNewMacro( ovOrlandoReader );

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

    // get the tag info and finalize it since we are about to use it
    ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
    tagInfo->Finalize();
    
    // create and set up a graph with pedigree and tag arrays
    // (the pedigree array is used to track vertices by name)
    vtkSmartPointer< vtkMutableDirectedGraph > graph =
      vtkSmartPointer< vtkMutableDirectedGraph >::New();
    vtkSmartPointer< vtkStringArray > pedigreeArray = vtkSmartPointer< vtkStringArray >::New();
    pedigreeArray->SetName( "pedigrees" );
    graph->GetVertexData()->AddArray( pedigreeArray );
    graph->GetVertexData()->SetPedigreeIds( pedigreeArray );

    // the sizes array is used to determine the size of each vertex
    vtkSmartPointer< vtkDoubleArray > sizeArray = vtkSmartPointer< vtkDoubleArray >::New();
    sizeArray->SetName( "sizes" );
    graph->GetVertexData()->AddArray( sizeArray );
    graph->GetVertexData()->SetActiveScalars( sizeArray->GetName() );

    // the births and deaths arrays track author birth and death dates
    vtkSmartPointer< vtkIntArray > birthArray = vtkSmartPointer< vtkIntArray >::New();
    birthArray->SetName( "births" );
    graph->GetVertexData()->AddArray( birthArray );
    graph->GetVertexData()->SetActiveScalars( birthArray->GetName() );
    vtkSmartPointer< vtkIntArray > deathArray = vtkSmartPointer< vtkIntArray >::New();
    deathArray->SetName( "deaths" );
    graph->GetVertexData()->AddArray( deathArray );
    graph->GetVertexData()->SetActiveScalars( deathArray->GetName() );

    // the tag array is used to keep a list of tags which associates the two vertices
    ovTagVector tags;
    ovTagVector::iterator it;
    tagInfo->GetTags( tags );
    vtkSmartPointer< vtkStringArray > tagArray = vtkSmartPointer< vtkStringArray >::New();
    tagArray->SetName( "tags" );
    graph->GetEdgeData()->AddArray( tagArray );

    // the colors array is used for edge coloring, this will be manipulated internally by the
    // restrict graph algorithm
    vtkSmartPointer< vtkVariantArray > colorArray = vtkSmartPointer< vtkVariantArray >::New();
    colorArray->SetName( "colors" );
    graph->GetEdgeData()->AddArray( colorArray );

    // A string to store the current active tags and mark them all as '0' chars to begin with.
    // Every time we open a tag in the association types vector we will set the tag to '1',
    // and every time the tag closes we will set it back to '0'.  This string will then be
    // copied every time we add a new edge.
    ovString currentTagArray( tagInfo->GetNumberOfTags(), '0' );
    
    try
    {
      vtkIdType currentVertexId;
      vtkStdString currentVertexPedigree;
      bool inBirthTag = false, inDeathTag = false;
      this->CreateReader();
      
      // count how many entries are in the file (for progress reporting)
      double totalEntries = 0;
      ovString line;
      vtkstd::ifstream fileStream( this->FileName.c_str() );
      if( !fileStream.is_open() ) throw vtkstd::runtime_error( "Unable to stream file." );
      // this count may be inaccurate since it doesn't account for spacing (ie: "< ENTRY"),
      // however, it is nice and fast and if the count is incorrect then the only side effect
      // is incorrect progress reporting which really isn't so bad (so worth the uncertainty)
      while( getline( fileStream, line ) ) if( line.find( "<ENTRY" ) ) totalEntries++;
      fileStream.close();

      // process each node, one at a time
      double numEntries = 0, progress;
      while( this->ParseNode() )
      {
        // This node is an entry (vertex)
        if( 1 == this->CurrentNode.Depth &&
            0 == xmlStrcmp( BAD_CAST "ENTRY", this->CurrentNode.Name ) )
        {
          if( this->CurrentNode.IsOpeningElement() )
          {
            progress = numEntries / totalEntries;
            this->InvokeEvent( vtkCommand::ProgressEvent, &( progress ) );

            // create a new vertex using the Id (author name) as the pedigree
            vtkSmartPointer< vtkVariantArray > array = vtkSmartPointer< vtkVariantArray >::New();
            array->SetNumberOfComponents( 4 );
            array->SetNumberOfTuples( 1 );
            array->SetValue( 0, vtkVariant( ( char* )( this->CurrentNode.Id ) ) ); // pedigrees
            array->SetValue( 1, 1 ); // sizes
            array->SetValue( 2, 0 ); // birth date
            array->SetValue( 3, 0 ); // death date
            currentVertexId = graph->AddVertex( array );
            currentVertexPedigree = pedigreeArray->GetValue( currentVertexId );
          }
          else if( this->CurrentNode.IsClosingElement() )
          {
            // no more child vertices for this vertex
            currentVertexId = -1;
            numEntries++;
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
            vtkSmartPointer< vtkVariantArray > array = vtkSmartPointer< vtkVariantArray >::New();
            array->SetNumberOfComponents( 2 );
            array->SetNumberOfTuples( 1 );
            array->SetValue( 0, vtkVariant( currentTagArray ) ); // tags
            array->SetValue( 1, vtkVariant( ovHash( currentTagArray ) ) ); // colors
            graph->AddEdge( currentVertexPedigree, ( char* )( this->CurrentNode.Standard ), array );
          }
          // Maybe this is a birth tag?
          else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "BIRTH" ) )
          {
            if( this->CurrentNode.IsOpeningElement() )
            {
              inBirthTag = true;
            }
            else if( this->CurrentNode.IsClosingElement() )
            {
              inBirthTag = false;
            }
          }
          // Maybe this is a death tag?
          else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DEATH" ) )
          {
            if( this->CurrentNode.IsOpeningElement() )
            {
              inDeathTag = true;
            }
            else if( this->CurrentNode.IsClosingElement() )
            {
              inDeathTag = false;
            }
          }
          // This node describes an association type (edge tag)
          else
          {
            int index = tagInfo->FindTagIndex( ( char* )( this->CurrentNode.Name ) );
            if( 0 <= index ) // we found a match
            {
              if( this->CurrentNode.IsOpeningElement() )
              { // opening element, mark the tag as true
                currentTagArray[index] = '1';
              }
              else if( this->CurrentNode.IsClosingElement() )
              { // closing element, mark the tag as false
                currentTagArray[index] = '0';
              }
            }

            // record birth date if we are inside a birth tag
            if( inBirthTag && 
                this->CurrentNode.IsOpeningElement() &&
                0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) )
            {
              ovDate newDate( ( char* )( this->CurrentNode.Value ) );
              ovDate curDate( birthArray->GetValue( currentVertexId ) );

              // there might be more than one date in the birth tag, so use the earliest date
              if( curDate > newDate )
              {
                // make sure the earlier date has as much information as the current or more
                if( 0 < newDate.day ||
                    ( 0 < newDate.month && 0 == curDate.day ) ||
                    0 == curDate.month )
                {
                  birthArray->SetValue( currentVertexId, newDate.AsInt() );
                }
              }
            }

            // record death date if we are inside a death tag
            if( inDeathTag &&
                this->CurrentNode.IsOpeningElement() &&
                0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) )
            {
              ovDate newDate( ( char* )( this->CurrentNode.Value ) );
              ovDate curDate( deathArray->GetValue( currentVertexId ) );

              // there might be more than one date in the death tag, so use the latest date
              if( curDate < newDate )
              {
                // make sure the earlier date has as much information as the current or more
                if( 0 < newDate.day ||
                    ( 0 < newDate.month && 0 == curDate.day ) ||
                    0 == curDate.month )
                {
                  deathArray->SetValue( currentVertexId, newDate.AsInt() );
                }
              }
            }
          }
        }
      } // end while

      this->FreeReader();
    }
    catch( vtkstd::exception &e )
    {
      vtkErrorMacro( << e.what() );
      return 0;
    }
   
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
    this->CurrentNode.Content = xmlTextReaderConstValue( this->Reader );
    this->CurrentNode.Id = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "id" );
    this->CurrentNode.Value = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "VALUE" );
    this->CurrentNode.Standard = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "STANDARD" );
    this->CurrentNode.Depth = xmlTextReaderDepth( this->Reader );
    this->CurrentNode.NodeType = xmlTextReaderNodeType( this->Reader );
    this->CurrentNode.IsEmptyElement = xmlTextReaderIsEmptyElement( this->Reader );
    this->CurrentNode.HasContent = xmlTextReaderHasValue( this->Reader );
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
