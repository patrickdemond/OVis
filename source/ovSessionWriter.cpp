/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSessionWriter.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSessionWriter.h"

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

#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovSessionWriter, "$Revision: $" );
vtkStandardNewMacro( ovSessionWriter );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::WriteData()
{
  try
  {
    this->CreateWriter();
    
    double *rgba;
    ovSession *input = ovSession::SafeDownCast( this->GetInput() );

    vtkstd::runtime_error e( "Error writing to ovis session file." );
    
    // set up the indent and root element
    if( 0 > xmlTextWriterSetIndent( this->Writer, 1 ) ) throw( e );
    if( 0 > xmlTextWriterSetIndentString( this->Writer, BAD_CAST "  " ) ) throw( e );
    if( 0 > xmlTextWriterStartElement( this->Writer, BAD_CAST "OvisSession" ) ) throw( e );
    if( 0 > xmlTextWriterWriteAttribute( this->Writer, BAD_CAST "Version", BAD_CAST "1.0.0" ) ) throw( e );

    this->Write( "DataFile", input->GetDataFile() );
    rgba = input->GetBackgroundColor1();
    this->WriteColor( "BackgroundColor1", rgba );
    rgba = input->GetBackgroundColor2();
    this->WriteColor( "BackgroundColor2", rgba );
    this->Write( "VertexStyle", input->GetVertexStyle() );
    this->Write( "LayoutStrategy", input->GetLayoutStrategy() );
    this->Write( "AuthorsOnly", input->GetAuthorsOnly() );
    this->Write( "GenderTypeRestriction", input->GetGenderTypeRestriction() );
    this->Write( "WriterTypeRestriction", input->GetWriterTypeRestriction() );
    this->Write( "VertexSize", input->GetVertexSize() );
    this->Write( "EdgeSize", input->GetEdgeSize() );
    rgba = input->GetAuthorVertexColor();
    this->WriteColor( "AuthorVertexColor", rgba );
    rgba = input->GetAssociationVertexColor();
    this->WriteColor( "AssociationVertexColor", rgba );
    this->Write( "TextSearchPhrase", input->GetTextSearchPhrase() );
    this->Write( "AuthorSearchPhrase", input->GetAuthorSearchPhrase() );
    this->Write( "StartDateRestriction", input->GetStartDateRestriction() );
    this->Write( "EndDateRestriction", input->GetEndDateRestriction() );
    this->Write( "SelectedVertexList", input->GetSelectedVertexList() );
    this->Write( "SelectedEdgeList", input->GetSelectedEdgeList() );
    this->Write( input->GetTagList() );
    this->Write( input->GetCamera() );

    // close the OvisSession element
    if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e );
    
    this->FreeWriter();
  }
  catch( vtkstd::exception &e )
  {
    if( this->Writer ) this->FreeWriter();
    vtkErrorMacro( << e.what() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovString name, int value )
{
  vtkstd::runtime_error e( "Error writing integer to ovis session file." );

  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "int" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatString(
    this->Writer, "%d", value ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovString name, double value )
{
  vtkstd::runtime_error e( "Error writing double to ovis session file." );

  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "double" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatString(
    this->Writer, "%f", value ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovString name, ovString value )
{
  vtkstd::runtime_error e( "Error writing string to ovis session file." );

  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "string" ) ) throw( e );
  if( 0 > xmlTextWriterWriteString(
    this->Writer, BAD_CAST value.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::WriteColor( ovString name, double rgba[4] )
{
  vtkstd::runtime_error e( "Error writing color to ovis session file." );
  
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "Color" ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "double" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Red", "%f", rgba[0] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Green", "%f", rgba[1] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Blue", "%f", rgba[2] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Alpha", "%f", rgba[3] ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Color element
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovString name, ovDate date )
{
  vtkstd::runtime_error e( "Error writing date to ovis session file." );
  
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "Date" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Year", "%d", date.year ) ) throw( e );
  if( date.month ) // only write the month if we have to
    if( 0 > xmlTextWriterWriteFormatElement(
      this->Writer, BAD_CAST "Month", "%d", date.month ) ) throw( e );
  if( date.day ) // only write the day if we have to
    if( 0 > xmlTextWriterWriteFormatElement(
      this->Writer, BAD_CAST "Day", "%d", date.day ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Date element
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovString name, ovIntVector *numbers )
{
  if( NULL == numbers ) return;
  vtkstd::runtime_error e( "Error writing int array to ovis session file." );
  
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "Array" ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "int" ) ) throw( e );
  for( ovIntVector::iterator it = numbers->begin(); it != numbers->end(); ++it )
    if( 0 > xmlTextWriterWriteFormatElement(
      this->Writer, BAD_CAST "Number", "%d", *it ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Array element
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( ovTagVector *tags )
{
  if( NULL == tags ) return;
  vtkstd::runtime_error e( "Error writing tag array to ovis session file." );

  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "TagList" ) ) throw( e );
  
  for( ovTagVector::iterator it = tags->begin(); it != tags->end(); ++it )
  {
    ovTag *tag = *it;
    if( tag )
    {
      if( 0 > xmlTextWriterStartElement(
        this->Writer, BAD_CAST "Tag" ) ) throw( e );
      this->Write( "Parent", tag->parent );
      this->Write( "Name", tag->name );
      this->Write( "Active", tag->active );
      this->Write( "Expanded", tag->expanded );
      this->WriteColor( "TagColor", tag->color );
      if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Tag element
    }
  }
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the TagList element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::WriteCoordinates( ovString name, double coordinates[3] )
{
  vtkstd::runtime_error e( "Error writing coordinates to ovis session file." );
  
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "Coordinates" ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "double" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "X", "%f", coordinates[0] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Y", "%f", coordinates[1] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Z", "%f", coordinates[2] ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Coordinates element
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::WriteDistanceRange( ovString name, double range[2] )
{
  vtkstd::runtime_error e( "Error writing distance range to ovis session file." );
  
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST name.c_str() ) ) throw( e );
  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "DistanceRange" ) ) throw( e );
  if( 0 > xmlTextWriterWriteAttribute(
    this->Writer, BAD_CAST "type", BAD_CAST "double" ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Near", "%f", range[0] ) ) throw( e );
  if( 0 > xmlTextWriterWriteFormatElement(
    this->Writer, BAD_CAST "Far", "%f", range[1] ) ) throw( e );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the DistanceRange element
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the named element
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSessionWriter::Write( vtkCamera* camera )
{
  if( NULL == camera ) return;
  vtkstd::runtime_error e( "Error writing camera to ovis session file." );

  if( 0 > xmlTextWriterStartElement(
    this->Writer, BAD_CAST "Camera" ) ) throw( e );
  this->WriteCoordinates( "Position", camera->GetPosition() );
  this->WriteCoordinates( "FocalPoint", camera->GetFocalPoint() );
  this->WriteCoordinates( "ViewUp", camera->GetViewUp() );
  this->WriteDistanceRange( "ClippingRange", camera->GetClippingRange() );
  this->Write( "ParallelScale", camera->GetParallelScale() );
  if( 0 > xmlTextWriterEndElement( this->Writer ) ) throw( e ); // close the Camera element
}
