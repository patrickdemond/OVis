/*=========================================================================

  Program:  ovis (XMLVision)
  Module:   ovXMLReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovXMLReader - Generic XML file reader
//
// .SECTION Description
// This abstract class is to be extended by any class which reads XML files.
// The parent of this class, vtkAlgorithm, provides many methods for
// controlling the reading of the data file, see vtkAlgorithm for more
// information.
//
// .SECTION See Also
// vtkAlgorithm
// 

#ifndef __ovXMLReader_h
#define __ovXMLReader_h

#include "vtkAlgorithm.h"

#include "ovUtilities.h"
#include <libxml/xmlreader.h>
#include "vtksys/SystemTools.hxx"

class vtkGraph;

class ovXMLReader : public vtkAlgorithm
{
public:
  vtkTypeRevisionMacro( ovXMLReader, vtkAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set/get the file name
  virtual ovString GetFileName() { return this->FileName; }
  virtual void SetFileName( const ovString &fileName );
  
protected:
  ovXMLReader();
  ~ovXMLReader();

  virtual int ProcessRequest(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  // Description:
  // Opens and parses the current XML file.
  // An exception is thrown if the file cannot be opened.
  void CreateReader();

  // Description:
  // Closes the current file.
  void FreeReader();

  // Description:
  // Parses the next node in the XML file.  Make sure to use Open() before calling this method.
  // Returns 1 if a new node has been parsed or 0 if the end of the file has been reached.
  // An exception is thrown if there is a parsing error.
  int ParseNode();

  // Description:
  // Points the current node to the beginning of the file so that the first node in the file
  // will be parsed next time ParseNode() is called.
  void RewindReader();

  // Description:
  // Internal struct for managing nodes
  struct _ovXMLNode
  {
    const xmlChar* Name;
    const xmlChar* Content;
    int Depth;
    int NodeType;
    int IsEmptyElement;
    int HasContent;

    _ovXMLNode() { this->Clear(); }
    void Clear()
    {
      this->Name = NULL;
      this->Content = NULL;
      this->Depth = 0;
      this->NodeType = 0;
      this->IsEmptyElement = 0;
      this->HasContent = 0;
    }
    bool IsOpeningElement() { return 1 == this->NodeType; }
    bool IsClosingElement() { return 15 == this->NodeType; }
    void PrintSelf( ostream &os, vtkIndent indent )
    {
      os << indent << "Name: "
         << ( NULL == this->Name ? "(null)" : ( char* )( this->Name ) ) << endl;
      os << indent << "Content: "
         << ( NULL == this->Content ? "(null)" : ( char* )( this->Content ) ) << endl;
      os << indent << "Depth: " << this->Depth << endl;
      os << indent << "NodeType: " << this->NodeType << endl;
      os << indent << "IsEmptyElement: " << this->IsEmptyElement << endl;
      os << indent << "HasContent: " << this->HasContent << endl;
    }
  };
  
  ovString FileName;
  xmlTextReader *Reader;
  _ovXMLNode CurrentNode;

private:
  ovXMLReader( const ovXMLReader& );  // Not implemented.
  void operator=( const ovXMLReader& );  // Not implemented.
};

#endif
