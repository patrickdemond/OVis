/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovOrlandoReader - Reads Orlando XML files into memory
//
// .SECTION Description
// This class parses Orlando's XML files and puts them into memory in an STL
// map, indexing all entries by a hash based on the entry's name.
//
// .SECTION See Also
// 

#ifndef __ovOrlandoReader_h
#define __ovOrlandoReader_h

#include "ovXMLReader.h"
#include "../ovUtilities.h"

class ovOrlandoReader : public ovXMLReader 
{
public:
  static ovOrlandoReader* New();
  vtkTypeRevisionMacro( ovOrlandoReader, ovXMLReader );
  void PrintSelf( ostream &os, vtkIndent indent );

protected:
  // Description:
  // Constructor and destructor
  ovOrlandoReader() {};
  ~ovOrlandoReader() {};

  // Description:
  // Parses all the entries in the XML file by gathering all authors and
  // their associations with other authors.
  // Throws an exception if the opened file is invalid or if there is a parse error.
  virtual void Parse();
  
private:
  ovOrlandoReader( const ovOrlandoReader& );  // Not implemented.
  void operator=( const ovOrlandoReader& );  // Not implemented.
};

#endif
