/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSessionReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSessionReader.h"

#include "ovOrlandoTagInfo.h"
#include "ovSession.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovSessionReader, "$Revision: $" );
vtkStandardNewMacro( ovSessionReader );

// this undef is required on the hp. vtkMutexLock ends up including
// /usr/inclue/dce/cma_ux.h which has the gall to #define read as cma_read
#ifdef read
#undef read
#endif

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession* ovSessionReader::GetOutput(int idx)
{
  return ovSession::SafeDownCast( this->GetOutputDataObject( idx ) );
} 

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::SetOutput( ovSession *output )
{
  this->GetExecutive()->SetOutputData( 0, output );
} 

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovSessionReader::RequestDataObject(
  vtkInformation* request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  ovSession *output = ovSession::New();
  this->SetOutput( output );
  output->Delete();
  
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovSessionReader::ProcessRequest(
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
    vtkDebugMacro( << "Reading ovis session ...");

    ovSession* output = ovSession::SafeDownCast( outInfo->Get( vtkDataObject::DATA_OBJECT() ) );

    try
    {
      int intVal;
      double doubleVal;
      ovString string;
      vtkstd::runtime_error e( "Error reading ovis session file." );

      this->CreateReader();
      
      while( this->ParseNode() )
      {
        // ignore node type 14 (#text stuff that we don't need) and non-opening nodes
        if( 14 == this->CurrentNode.NodeType ) continue;
        if( !( this->CurrentNode.IsOpeningElement() ) ) continue;
        
        // --------------------------------------------------------------------------
        if( 0 == xmlStrcmp( BAD_CAST "DataFile", this->CurrentNode.Name ) )
        {
          this->ReadString( string );
          output->SetDataFile( string );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "BackgroundColor1", this->CurrentNode.Name ) )
        {
          double rgba[] = { 0, 0, 0, 0 };
          this->ReadColor( rgba );
          output->SetBackgroundColor1( rgba );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "BackgroundColor2", this->CurrentNode.Name ) )
        {
          double rgba[] = { 0, 0, 0, 0 };
          this->ReadColor( rgba );
          output->SetBackgroundColor2( rgba );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CustomAnnotationText", this->CurrentNode.Name ) )
        {
          this->ReadString( string );
          output->SetCustomAnnotationText( string );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "ShowAnnotation", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetShowAnnotation( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "VertexStyle", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetVertexStyle( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "LayoutStrategy", this->CurrentNode.Name ) )
        {
          this->ReadString( string );
          output->SetLayoutStrategy( string );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeWriters", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeWriters( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeOthers", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeOthers( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeFemale", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeFemale( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeMale", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeMale( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeBRWType", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeBRWType( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeWriterType", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeWriterType( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "IncludeIBRType", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetIncludeIBRType( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "VertexSize", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetVertexSize( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "EdgeSize", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetEdgeSize( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "FadingFactor", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetFadingFactor( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AuthorVertexColor", this->CurrentNode.Name ) )
        {
          double rgba[] = { 0, 0, 0, 0 };
          this->ReadColor( rgba );
          output->SetAuthorVertexColor( rgba );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AssociationVertexColor", this->CurrentNode.Name ) )
        {
          double rgba[] = { 0, 0, 0, 0 };
          this->ReadColor( rgba );
          output->SetAssociationVertexColor( rgba );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "TextSearchPhrase", this->CurrentNode.Name ) )
        {
          this->ReadString( string );
          output->SetTextSearchPhrase( string );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "TextSearchNarrow", this->CurrentNode.Name ) )
        {
          this->ReadInt( intVal );
          output->SetTextSearchNarrow( intVal );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AuthorSearchPhrase", this->CurrentNode.Name ) )
        {
          this->ReadString( string );
          output->SetAuthorSearchPhrase( string );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "StartDateRestriction", this->CurrentNode.Name ) )
        {
          ovDate date;
          this->ReadDate( date );
          output->SetStartDateRestriction( date );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "EndDateRestriction", this->CurrentNode.Name ) )
        {
          ovDate date;
          this->ReadDate( date );
          output->SetEndDateRestriction( date );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "SelectedVertexList", this->CurrentNode.Name ) )
        {
          output->GetSelectedVertexList()->empty();
          if( !this->CurrentNode.IsEmptyElement )
            this->ReadIntList( output->GetSelectedVertexList() );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "SelectedEdgeList", this->CurrentNode.Name ) )
        {
          output->GetSelectedEdgeList()->empty();
          if( !this->CurrentNode.IsEmptyElement )
            this->ReadIntList( output->GetSelectedEdgeList() );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "TagList", this->CurrentNode.Name ) )
        {
          vtkstd::for_each(
            output->GetTagList()->begin(),
            output->GetTagList()->end(),
            safe_delete() );
          output->GetTagList()->empty();
          if( !this->CurrentNode.IsEmptyElement )
            this->ReadTagList( output->GetTagList() );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "Camera", this->CurrentNode.Name ) )
        {
          vtkSmartPointer< vtkCamera > camera = vtkSmartPointer< vtkCamera >::New();
          this->ReadCamera( camera );
          output->GetCamera()->DeepCopy( camera );
        }
      } // end while

      this->FreeReader();
    }
    catch( vtkstd::exception &e )
    {
      vtkErrorMacro( << e.what() );
      return 0;
    }
  }


  return this->Superclass::ProcessRequest( request, inputVector, outputVector );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovSessionReader::FillOutputPortInformation( int, vtkInformation* info )
{
  info->Set( vtkDataObject::DATA_TYPE_NAME(), "ovSession" );
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadInt( int &value )
{
  xmlChar *read = xmlTextReaderReadString( this->Reader );
  if( NULL == read ) throw( vtkstd::runtime_error( "Failed to read integer." ) );
  value = atoi( ( char* )( read ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadDouble( double &value )
{
  xmlChar *read = xmlTextReaderReadString( this->Reader );
  if( NULL == read ) throw( vtkstd::runtime_error( "Failed to read double." ) );
  value = atof( ( char* )( read ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadString( ovString &string )
{
  xmlChar *read = xmlTextReaderReadString( this->Reader );
  if( NULL == read ) throw( vtkstd::runtime_error( "Failed to read string." ) );
  string = ( char* )( read );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadColor( double rgba[4] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read color." ) );
  node = node->children;
  
  // loop until we find the Color element
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Color", node->name ) )
    {
      break;
    }
    node = node->next;
  }
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read color." ) );
  node = node->children;

  // loop until we find the red/green/blue elements
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Red", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgba[0] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Green", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgba[1] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Blue", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgba[2] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Alpha", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgba[3] = atof( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadDate( ovDate &date )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read date." ) );
  node = node->children;
  
  // loop until we find the Date element
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Date", node->name ) )
    {
      break;
    }
    node = node->next;
  }
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read date." ) );
  node = node->children;

  // loop until we find the day/month/year elements
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Year", node->name ) &&
        node->children &&
        node->children->content )
    {
      date.year = atoi( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Month", node->name ) &&
        node->children &&
        node->children->content )
    {
      date.month = atoi( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Day", node->name ) &&
        node->children &&
        node->children->content )
    {
      date.day = atoi( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadIntList( ovIntVector *numbers )
{
  if( NULL == numbers ) throw( vtkstd::runtime_error( "Failed to read int list." ) );
  
  while( this->ParseNode() )
  {
    // ignore node type 14 (#text stuff that we don't need) and non-opening nodes
    if( 14 == this->CurrentNode.NodeType ) continue;
    
    if( ( this->CurrentNode.IsClosingElement() ||
          this->CurrentNode.IsEmptyElement ) &&
        0 == xmlStrcmp( BAD_CAST "Array", this->CurrentNode.Name ) )
    {
      // we're done, break out of the loop
      break;
    }
    else if( this->CurrentNode.IsOpeningElement() &&
             0 == xmlStrcmp( BAD_CAST "Number", this->CurrentNode.Name ) )
    {
      int value;
      this->ReadInt( value );
      numbers->push_back( value );
    }
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadTagList( ovTagVector *tags )
{
  if( NULL == tags ) throw( vtkstd::runtime_error( "Failed to read tag list." ) );

  int intVal;
  ovString string;
  ovTag *tag = NULL;

  while( this->ParseNode() )
  {
    // ignore node type 14 (#text stuff that we don't need) and non-opening nodes
    if( 14 == this->CurrentNode.NodeType ) continue;
    
    if( this->CurrentNode.IsClosingElement() &&
        0 == xmlStrcmp( BAD_CAST "TagList", this->CurrentNode.Name ) )
    {
      // we're done, break out of the loop
      break;
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Tag", this->CurrentNode.Name ) )
    {  
      if( this->CurrentNode.IsOpeningElement() )
      {
        tag = new ovTag;
      }
      else if( this->CurrentNode.IsClosingElement() )
      {
        tags->push_back( tag );
        tag = NULL;
      }
    }
    else if( this->CurrentNode.IsOpeningElement() && tag )
    {
      if( 0 == xmlStrcmp( BAD_CAST "Parent", this->CurrentNode.Name ) )
      {
        this->ReadString( string );
        tag->parent = string;
      }
      else if( 0 == xmlStrcmp( BAD_CAST "Name", this->CurrentNode.Name ) )
      {
        this->ReadString( string );
        tag->name = string;
      }
      else if( 0 == xmlStrcmp( BAD_CAST "Active", this->CurrentNode.Name ) )
      {
        this->ReadInt( intVal );
        tag->active = 0 != intVal;
      }
      else if( 0 == xmlStrcmp( BAD_CAST "Expanded", this->CurrentNode.Name ) )
      {
        this->ReadInt( intVal );
        tag->expanded = 0 != intVal;
      }
      else if( 0 == xmlStrcmp( BAD_CAST "TagColor", this->CurrentNode.Name ) )
      {
        double rgba[] = { 0, 0, 0, 0 };
        this->ReadColor( rgba );
        for( int i = 0; i < 4; i++ ) tag->color[i] = rgba[i];
      }
    }
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadCoordinates( double position[3] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read coordinates." ) );
  node = node->children;
  
  // loop until we find the Coordinates element
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Coordinates", node->name ) )
    {
      break;
    }
    node = node->next;
  }
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read coordinates." ) );
  node = node->children;

  // loop until we find the x/y/z elements
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "X", node->name ) &&
        node->children &&
        node->children->content )
    {
      position[0] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Y", node->name ) &&
        node->children &&
        node->children->content )
    {
      position[1] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Z", node->name ) &&
        node->children &&
        node->children->content )
    {
      position[2] = atof( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadDistanceRange( double range[2] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read distance range." ) );
  node = node->children;
  
  // loop until we find the DistanceRange element
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "DistanceRange", node->name ) )
    {
      break;
    }
    node = node->next;
  }
  
  // make sure we have a node with children
  if( !node || !node->children ) throw( vtkstd::runtime_error( "Failed to read distance range." ) );
  node = node->children;

  // loop until we find the near/far elements
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Near", node->name ) &&
        node->children &&
        node->children->content )
    {
      range[0] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Far", node->name ) &&
        node->children &&
        node->children->content )
    {
      range[1] = atof( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionReader::ReadCamera( vtkCamera* camera )
{
  if( NULL == camera ) throw( vtkstd::runtime_error( "Failed to read camera." ) );

  while( this->ParseNode() )
  {
    // ignore node type 14 (#text stuff that we don't need) and non-opening nodes
    if( 14 == this->CurrentNode.NodeType ) continue;
    
    if( this->CurrentNode.IsClosingElement() &&
        0 == xmlStrcmp( BAD_CAST "Camera", this->CurrentNode.Name ) )
    {
      // we're done, break out of the loop
      break;
    }
    else if( this->CurrentNode.IsOpeningElement() )
    {
      if( 0 == xmlStrcmp( BAD_CAST "Position", this->CurrentNode.Name ) )
      {
        double xyz[] = { 0, 0, 0 };
        this->ReadCoordinates( xyz );
        camera->SetPosition( xyz );
      }
      else if( 0 == xmlStrcmp( BAD_CAST "FocalPoint", this->CurrentNode.Name ) )
      {
        double xyz[] = { 0, 0, 0 };
        this->ReadCoordinates( xyz );
        camera->SetFocalPoint( xyz );
      }
      else if( 0 == xmlStrcmp( BAD_CAST "ViewUp", this->CurrentNode.Name ) )
      {
        double xyz[] = { 0, 0, 0 };
        this->ReadCoordinates( xyz );
        camera->SetViewUp( xyz );
      }
      else if( 0 == xmlStrcmp( BAD_CAST "ClippingRange", this->CurrentNode.Name ) )
      {
        double range[] = { 0, 0 };
        this->ReadDistanceRange( range );
        camera->SetClippingRange( range );
      }
      else if( 0 == xmlStrcmp( BAD_CAST "ParallelScale", this->CurrentNode.Name ) )
      {
        double value;
        this->ReadDouble( value );
        camera->SetParallelScale( value );
      }
    }
  }
}
