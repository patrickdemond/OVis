/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovOrlandoReader - Reads Orlando XML files
//
// .SECTION Description
// This is a source object that reads Orlando XML files.  The output of
// this reader is a single vtkGraph data object.
// The parent of this class, vtkAlgorithm, provides many methods for
// controlling the reading of the data file, see vtkAlgorithm for more
// information.
//
// .SECTION See Also
// vtkGraph vtkAlgorithm
// 

#ifndef __ovOrlandoReader_h
#define __ovOrlandoReader_h

#include "vtkAlgorithm.h"

#include "ovUtilities.h"
#include <libxml/xmlreader.h>

class vtkGraph;

class ovOrlandoReader : public vtkAlgorithm
{
public:
  static ovOrlandoReader *New();
  vtkTypeRevisionMacro( ovOrlandoReader, vtkAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set/get the file name
  virtual ovString GetFileName() { return this->FileName; }
  virtual void SetFileName( ovString name );
  
  // Description:
  // Get the output of this reader.
  vtkGraph *GetOutput() { return this->GetOutput( 0 ); }
  vtkGraph *GetOutput( int idx );
  void SetOutput( vtkGraph *output );
  
protected:
  ovOrlandoReader();
  ~ovOrlandoReader();

  virtual int ProcessRequest(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int RequestDataObject(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int FillOutputPortInformation( int, vtkInformation* );

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
    const xmlChar* Value;
    const xmlChar* Id;
    const xmlChar* Standard;
    const xmlChar* Content;
    int Depth;
    int NodeType;
    int IsEmptyElement;
    int HasContent;

    _ovXMLNode() { this->Clear(); }
    void Clear()
    {
      this->Name = NULL;
      this->Value = NULL;
      this->Id = NULL;
      this->Standard = NULL;
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
      os << indent << "Value: "
         << ( NULL == this->Value ? "(null)" : ( char* )( this->Value ) ) << endl;
      os << indent << "Id: "
         << ( NULL == this->Id ? "(null)" : ( char* )( this->Id ) ) << endl;
      os << indent << "Standard: "
         << ( NULL == this->Standard ? "(null)" : ( char* )( this->Standard ) ) << endl;
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
  ovOrlandoReader( const ovOrlandoReader& );  // Not implemented.
  void operator=( const ovOrlandoReader& );  // Not implemented.
};

#endif
