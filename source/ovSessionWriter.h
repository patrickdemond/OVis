/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSessionWriter.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovSessionWriter - Writes Session XML files
//
// .SECTION Description
// This is a source object that reads Session XML files.  The output of
// this reader is a single ovSession data object.
//
// .SECTION See Also
// ovSession ovXMLWriter
// 

#ifndef __ovSessionWriter_h
#define __ovSessionWriter_h

#include "ovXMLWriter.h"

#include "ovUtilities.h"

class ovSession;
class vtkCamera;

class ovSessionWriter : public ovXMLWriter
{
public:
  static ovSessionWriter *New();
  vtkTypeRevisionMacro( ovSessionWriter, ovXMLWriter );
  
  virtual void WriteData();

protected:
  ovSessionWriter() {}
  ~ovSessionWriter() {}

  // Description:
  // Write an integer element
  virtual void Write( ovString name, int );

  // Description:
  // Write a double element
  virtual void Write( ovString name, double );
  
  // Description:
  // Write a string element
  virtual void Write( ovString name, ovString );
  
  // Description:
  // Write an rgba color element
  virtual void WriteColor( ovString name, double[4] );
  
  // Description:
  // Write a date element
  virtual void Write( ovString name, ovDate );
  
  // Description:
  // Write a tag list element
  virtual void Write( ovTagVector* );
  
  // Description:
  // Write a coordinates element
  virtual void WriteCoordinates( ovString name, double[3] );
  
  // Description:
  // Write a distance range element
  virtual void WriteDistanceRange( ovString name, double[2] );

  // Description:
  // Write a camera element
  virtual void Write( vtkCamera* );

private:
  ovSessionWriter( const ovSessionWriter& );  // Not implemented.
  void operator=( const ovSessionWriter& );  // Not implemented.
};

#endif
