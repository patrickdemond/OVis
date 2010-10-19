/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovXMLReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovXMLReader - Class wrapper for reading XML files
//
// .SECTION Description
// This abstract class uses LibXML2 to parse XML files.  It should be
// extended by defining the Parse() method.
//
// .SECTION See Also
// 

#ifndef __ovXMLReader_h
#define __ovXMLReader_h

#include "vtkObject.h"
#include "../ovUtilities.h"

#include <libxml/xmlreader.h>

class ovXMLReader : public vtkObject
{
public:
  vtkTypeRevisionMacro( ovXMLReader, vtkObject );
  void PrintSelf( ostream &os, vtkIndent indent );

  // Description:
  // Opens and parses and XML file.
  // Do not call this method to reopen a file at the beginning, instead call Rewind()
  // An exception is thrown if the file cannot be opened.
  void Open( ovString fileName );

  // Description:
  // Closes the current file.
  void Close();

  // Description:
  // Pure abstract method that, when implemented, should read an entire XML file and store
  // relavant data into some external variables/objects.  Implementing this method should
  // make use of the ParseNode() method, remembering to handle any thrown exceptions.
  virtual void Parse() = 0;

protected:
  // Description:
  // Constructor and destructor
  ovXMLReader() {};
  ~ovXMLReader() {};
  
  // Description:
  // Parses the next node in the XML file.  Make sure to use Open() before calling this method.
  // Returns 1 if a new node has been parsed or 0 if the end of the file has been reached.
  // An exception is thrown if there is a parsing error.
  int ParseNode();

  // Description:
  // Points the current node to the beginning of the file so that the first node in the file
  // will be parsed next time ParseNode() is called.
  void Rewind();

  // Description:
  // Internal struct for managing nodes
  struct _ovXMLNode
  {
    const xmlChar* Name;
    const xmlChar* Value;
    const xmlChar* Id;
    const xmlChar* Standard;
    int Depth;
    int NodeType;
    int IsEmptyElement;
    int HasValue;

    _ovXMLNode()
    {
      this->Name = NULL;
      this->Value = NULL;
      this->Id = NULL;
      this->Standard = NULL;
      this->Depth = 0;
      this->NodeType = 0;
      this->IsEmptyElement = 0;
      this->HasValue = 0;
    }
    
    bool IsOpeningElement() { return 1 == this->NodeType; }
    bool IsClosingElement() { return 15 == this->NodeType; }
    void PrintSelf( ostream &os, vtkIndent indent )
    {
      os << indent << "Name: "
         << ( NULL == this->Name ? "(null)" : ( char* )( this->Name ) ) << endl;
      os << indent << "Value: "
         << ( NULL == this->Value ? "(null)" : ( char* )( this->Value ) ) << endl;
      os << indent << "Id: "
         << ( NULL == this->Id ? "(null)" : ( char* )( this->Id ) ) << endl;
      os << indent << "Standard: "
         << ( NULL == this->Standard ? "(null)" : ( char* )( this->Standard ) ) << endl;
      os << indent << "Depth: " << this->Depth << endl;
      os << indent << "NodeType: " << this->NodeType << endl;
      os << indent << "IsEmptyElement: " << this->IsEmptyElement << endl;
      os << indent << "HasValue: " << this->HasValue << endl;
    }
  };
  
  ovString FileName;
  xmlTextReader *Reader;
  _ovXMLNode *CurrentNode;

private:
  ovXMLReader( const ovXMLReader& );  // Not implemented.
  void operator=( const ovXMLReader& );  // Not implemented.
};

#endif
