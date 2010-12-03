/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovXMLWriter.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovXMLWriter.h"

#include "ovSession.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"

#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovXMLWriter, "$Revision: $" );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovXMLWriter::ovXMLWriter()
{
  this->FileName = "";
  this->Writer = NULL;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovXMLWriter::~ovXMLWriter()
{
  if( NULL != this->Writer )
  {
    this->FreeWriter();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLWriter::SetFileName( const ovString &fileName )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "FileName to " << fileName.c_str() );

  if( fileName != this->FileName )
  {
    this->FileName = fileName;
    this->Modified();
  }
}

int ovXMLWriter::FillInputPortInformation( int, vtkInformation *info )
{
  info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "ovSession" );
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLWriter::CreateWriter()
{
  this->Writer = xmlNewTextWriterFilename( this->FileName.c_str(), 0 );
  if( NULL == this->Writer )
  {
    throw vtkstd::runtime_error( "Unable to open file." );
  }

  // Start the document with the xml default for the version, encoding UTF-8
  // and the default for the standalone declaration.
  if( xmlTextWriterStartDocument( this->Writer, NULL, "UTF-8", NULL ) < 0 )
  {
    throw vtkstd::runtime_error( "Unable to start document." );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLWriter::FreeWriter()
{
  if( NULL == this->Writer )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }
  \
  xmlFreeTextWriter( this->Writer );
  this->Writer = NULL;
}
