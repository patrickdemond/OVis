/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovXMLWriter.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovSessionReader - writes XML files
//
// .SECTION Description
//
// .SECTION See Also
// 

#ifndef __ovXMLWriter_h
#define __ovXMLWriter_h

#include "vtkWriter.h"

#include "ovUtilities.h"
#include <libxml/xmlwriter.h>

class ovXMLWriter : public vtkWriter
{
public:
  vtkTypeRevisionMacro( ovXMLWriter, vtkWriter );

  // Description:
  // Set/get the file name
  virtual ovString GetFileName() { return this->FileName; }
  virtual void SetFileName( const ovString &fileName );

protected:
  ovXMLWriter();
  ~ovXMLWriter();

  void WriteData() = 0;
  virtual int FillInputPortInformation( int port, vtkInformation *info );
  
  // Description:
  // Opens a XML file for writing (truncating it)
  void CreateWriter();

  // Description:
  // Closes the current file.
  void FreeWriter();

  ovString FileName;
  xmlTextWriter *Writer;

private:
  ovXMLWriter( const ovXMLWriter& );  // Not implemented.
  void operator=( const ovXMLWriter& );  // Not implemented.
};

#endif
