/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovXMLReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovXMLReader - Reads Orlando XML files into memory
//
// .SECTION Description
// This class uses LibXML2 to parse Orlando XML files into STL data structures.
//
// .SECTION See Also
// 

#ifndef __ovXMLReader_h
#define __ovXMLReader_h

#include "vtkObjectBase.h"

class ovXMLReader : public vtkObjectBase 
{
public:
protected:
  // Description:
  // Constructor and destructor
  ovXMLReader() {};
  ~ovXMLReader() {};

private:
  ovXMLReader( const ovXMLReader& );  // Not implemented.
  void operator=( const ovXMLReader& );  // Not implemented.
};

#endif
