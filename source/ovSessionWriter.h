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
class vtkStringArray;

class ovSessionWriter : public ovXMLWriter
{
public:
  static ovSessionWriter *New();
  vtkTypeRevisionMacro( ovSessionWriter, ovXMLWriter );
  
  virtual void WriteData();

protected:
  ovSessionWriter() {}
  ~ovSessionWriter() {}

  virtual void WriteColor( double[3] );
  virtual void WriteDate( ovDate& );
  virtual void WriteTagArray( vtkStringArray* );
  virtual void WritePosition( double[3] );
  virtual void WriteClippingRange( double[2] );

private:
  ovSessionWriter( const ovSessionWriter& );  // Not implemented.
  void operator=( const ovSessionWriter& );  // Not implemented.
};

#endif
