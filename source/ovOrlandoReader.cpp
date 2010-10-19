/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoReader.h"

#include <libxml/xmlreader.h>
#include <vtkstd/stdexcept>

#include "vtkObjectFactory.h"

vtkStandardNewMacro( ovOrlandoReader );
vtkCxxRevisionMacro( ovOrlandoReader, "Revision: 1" );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoReader::Parse()
{
  while( this->ParseNode() )
  {
    // process the current node
    if( 1 == this->CurrentNode->Depth &&
        0 == xmlStrcmp( BAD_CAST "ENTRY", this->CurrentNode->Name ) )
    {
      
    }
//    else if( currentAuthor )
//    {
//    }
  }
}
