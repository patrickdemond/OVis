/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSessionReader.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovSessionReader - Reads Session XML files
//
// .SECTION Description
// This is a source object that reads Session XML files.  The output of
// this reader is a single ovSession data object.
//
// .SECTION See Also
// ovSession ovXMLReader
// 

#ifndef __ovSessionReader_h
#define __ovSessionReader_h

#include "ovXMLReader.h"

#include "ovUtilities.h"

class ovSession;
class vtkCamera;

class ovSessionReader : public ovXMLReader
{
public:
  static ovSessionReader *New();
  vtkTypeRevisionMacro( ovSessionReader, ovXMLReader );
  
  // Description:
  // Get the output of this reader.
  ovSession *GetOutput() { return this->GetOutput( 0 ); }
  ovSession *GetOutput( int idx );
  void SetOutput( ovSession *output );

protected:
  ovSessionReader() {}
  ~ovSessionReader() {}

  virtual int ProcessRequest(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int RequestDataObject(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int FillOutputPortInformation( int, vtkInformation* );

  virtual void ReadInt( int& );
  virtual void ReadDouble( double& );
  virtual void ReadString( ovString& );
  virtual void ReadColor( double[4] );
  virtual void ReadDate( ovDate& );
  virtual void ReadIntList( ovIntVector* );
  virtual void ReadTagList( ovTagVector* );
  virtual void ReadCoordinates( double[3] );
  virtual void ReadDistanceRange( double[2] );
  virtual void ReadCamera( vtkCamera* );

private:
  ovSessionReader( const ovSessionReader& );  // Not implemented.
  void operator=( const ovSessionReader& );  // Not implemented.
};

#endif
