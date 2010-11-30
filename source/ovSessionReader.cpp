/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSessionReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSessionReader.h"

#include "ovOrlandoTagInfo.h"
#include "ovSession.h"

#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"

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
      ovString currentTagName, openTagName;

      this->CreateReader();
      
      while( this->ParseNode() )
      {
        // ignore node type 14 (#text stuff that we don't need) and non-opening nodes
        if( 14 == this->CurrentNode.NodeType ) continue;
        if( !( this->CurrentNode.IsOpeningElement() ) ) continue;
        
        // --------------------------------------------------------------------------
        if( 0 == xmlStrcmp( BAD_CAST "DataFile", this->CurrentNode.Name ) )
        {
          output->SetDataFile( ( char* )( xmlTextReaderReadString( this->Reader ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "BackgroundColor1", this->CurrentNode.Name ) )
        {
          double rgb[] = { 0, 0, 0 };
          this->ReadColor( rgb );
          output->SetBackgroundColor1( rgb );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "BackgroundColor2", this->CurrentNode.Name ) )
        {
          double rgb[] = { 0, 0, 0 };
          this->ReadColor( rgb );
          output->SetBackgroundColor2( rgb );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "VertexStyle", this->CurrentNode.Name ) )
        {
          output->SetVertexStyle( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "LayoutStrategy", this->CurrentNode.Name ) )
        {
          output->SetLayoutStrategy( ( char* )( xmlTextReaderReadString( this->Reader ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AuthorsOnly", this->CurrentNode.Name ) )
        {
          output->SetAuthorsOnly( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "GenderTypeRestriction", this->CurrentNode.Name ) )
        {
          output->SetGenderTypeRestriction( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "WriterTypeRestriction", this->CurrentNode.Name ) )
        {
          output->SetWriterTypeRestriction( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "VertexSize", this->CurrentNode.Name ) )
        {
          output->SetVertexSize( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "EdgeSize", this->CurrentNode.Name ) )
        {
          output->SetEdgeSize( atoi( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AuthorVertexColor", this->CurrentNode.Name ) )
        {
          double rgb[] = { 0, 0, 0 };
          this->ReadColor( rgb );
          output->SetAuthorVertexColor( rgb );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "AssociationVertexColor", this->CurrentNode.Name ) )
        {
          double rgb[] = { 0, 0, 0 };
          this->ReadColor( rgb );
          output->SetAssociationVertexColor( rgb );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "StartDate", this->CurrentNode.Name ) )
        {
          ovDate date;
          this->ReadDate( date );
          output->SetStartDate( date );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "EndDate", this->CurrentNode.Name ) )
        {
          ovDate date;
          this->ReadDate( date );
          output->SetEndDate( date );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "ActiveTags", this->CurrentNode.Name ) )
        {
          vtkSmartPointer< vtkStringArray > array = vtkSmartPointer< vtkStringArray >::New();
          this->ReadTagArray( array );
          output->SetActiveTags( array );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraPosition", this->CurrentNode.Name ) )
        {
          double xyz[] = { 0, 0, 0 };
          this->ReadPosition( xyz );
          output->SetCameraPosition( xyz );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraFocalPoint", this->CurrentNode.Name ) )
        {
          double xyz[] = { 0, 0, 0 };
          this->ReadPosition( xyz );
          output->SetCameraFocalPoint( xyz );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraViewUp", this->CurrentNode.Name ) )
        {
          double xyz[] = { 0, 0, 0 };
          this->ReadPosition( xyz );
          output->SetCameraViewUp( xyz );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraClippingRange", this->CurrentNode.Name ) )
        {
          double range[] = { 0, 0 };
          this->ReadClippingRange( range );
          output->SetCameraClippingRange( range );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraParallelScale", this->CurrentNode.Name ) )
        {
          output->SetCameraParallelScale( atof( ( char* )( xmlTextReaderReadString( this->Reader ) ) ) );
        }
        // --------------------------------------------------------------------------
        else if( 0 == xmlStrcmp( BAD_CAST "CameraComputeViewPlaneNormal", this->CurrentNode.Name ) )
        {
          double xyz[] = { 0, 0, 0 };
          this->ReadPosition( xyz );
          output->SetCameraComputeViewPlaneNormal( xyz );
        }
      } // end while

      this->FreeReader();
    }
    catch( vtkstd::exception &e )
    {
      vtkErrorMacro( << e.what() );
      return 0;
    }
    cout << *output << endl;
  }

  return this->Superclass::ProcessRequest( request, inputVector, outputVector );
}

//----------------------------------------------------------------------------
int ovSessionReader::FillOutputPortInformation( int, vtkInformation* info )
{
  info->Set( vtkDataObject::DATA_TYPE_NAME(), "ovSession" );
  return 1;
}

//----------------------------------------------------------------------------
void ovSessionReader::ReadColor( double rgb[3] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  node = node->children;
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Red", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgb[0] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Green", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgb[1] = atof( ( char* )( node->children->content ) );
    }
    else if( 0 == xmlStrcmp( BAD_CAST "Blue", node->name ) &&
        node->children &&
        node->children->content )
    {
      rgb[2] = atof( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//----------------------------------------------------------------------------
void ovSessionReader::ReadDate( ovDate &date )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  node = node->children;
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

//----------------------------------------------------------------------------
void ovSessionReader::ReadTagArray( vtkStringArray* array )
{
  if( NULL == array ) return;

  xmlNode *node = xmlTextReaderExpand( this->Reader );
  node = node->children;
  while( node )
  {
    if( 0 == xmlStrcmp( BAD_CAST "Tag", node->name ) &&
        node->children &&
        node->children->content )
    {
      array->InsertNextValue( ( char* )( node->children->content ) );
    }
    node = node->next;
  }
}

//----------------------------------------------------------------------------
void ovSessionReader::ReadPosition( double position[3] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  node = node->children;
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

//----------------------------------------------------------------------------
void ovSessionReader::ReadClippingRange( double range[2] )
{
  xmlNode *node = xmlTextReaderExpand( this->Reader );
  node = node->children;
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
