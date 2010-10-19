/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovXMLReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovXMLReader.h"

#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovXMLReader, "Revision: 1" );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLReader::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "FileName: \"" << this->FileName << "\"" << endl;
  os << indent << "Reader: " << this->Reader << endl;
  os << indent << "CurrentNode: " << endl;
  this->CurrentNode->PrintSelf( os, indent.GetNextIndent() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLReader::Open( ovString fileName )
{
  // do not re-open a file that has already been opened
  if( fileName != this->FileName )
  {
    this->FileName = fileName;
    this->Reader = xmlReaderForFile( this->FileName.c_str(), NULL, 0 );\
    if( 1 != xmlTextReaderIsValid( this->Reader ) )
    {
      throw vtkstd::runtime_error( "Unable to open file." );
    }
    this->CurrentNode = NULL;

  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovXMLReader::ParseNode()
{
  if( 1 != xmlTextReaderIsValid( this->Reader ) )
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
    this->CurrentNode = NULL;
  }
  else
  { // successful read
    this->CurrentNode->Name = xmlTextReaderConstName( this->Reader );
    if( this->CurrentNode->Name == NULL ) this->CurrentNode->Name = BAD_CAST "--";
    this->CurrentNode->Value = xmlTextReaderConstValue( this->Reader );
    this->CurrentNode->Id = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "id" );
    this->CurrentNode->Standard = xmlTextReaderGetAttribute( this->Reader, BAD_CAST "STANDARD" );
    this->CurrentNode->Depth = xmlTextReaderDepth( this->Reader );
    this->CurrentNode->NodeType = xmlTextReaderNodeType( this->Reader );
    this->CurrentNode->IsEmptyElement = xmlTextReaderIsEmptyElement( this->Reader );
    this->CurrentNode->HasValue = xmlTextReaderHasValue( this->Reader );
  }

  return result;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLReader::Rewind()
{
  if( 1 != xmlTextReaderIsValid( this->Reader ) )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }
  
  // close and reopen the current file
  ovString fileName = this->FileName;
  this->Close();
  this->Open( fileName );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovXMLReader::Close()
{
  if( 1 != xmlTextReaderIsValid( this->Reader ) )
  {
    throw vtkstd::runtime_error( "No file opened." );
  }

  xmlFreeTextReader( this->Reader );
  this->Reader = NULL;
}
