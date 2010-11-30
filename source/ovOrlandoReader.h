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
//
// .SECTION See Also
// vtkGraph ovXMLReader
// 

#ifndef __ovOrlandoReader_h
#define __ovOrlandoReader_h

#include "ovXMLReader.h"

#include "vtksys/SystemTools.hxx"

class vtkGraph;

class ovOrlandoReader : public ovXMLReader
{
public:
  static ovOrlandoReader *New();
  vtkTypeRevisionMacro( ovOrlandoReader, ovXMLReader );
  
  // Description:
  // Get the output of this reader.
  vtkGraph *GetOutput() { return this->GetOutput( 0 ); }
  vtkGraph *GetOutput( int idx );
  void SetOutput( vtkGraph *output );

  enum GenderType
  {
    GenderTypeUnknown,
    GenderTypeMale,
    GenderTypeFemale
  };

  static int GenderTypeFromString( const char *gender )
  {
    return 0 == vtksys::SystemTools::Strucmp( "male", gender )
         ? GenderTypeMale
         : 0 == vtksys::SystemTools::Strucmp( "female", gender )
         ? GenderTypeFemale
         : GenderTypeUnknown;
  }

  enum WriterType
  {
    WriterTypeNone,
    WriterTypeWriter,
    WriterTypeBRW,
    WriterTypeIBR
  };

  static int WriterTypeFromString( const char *writer )
  {
    return 0 == vtksys::SystemTools::Strucmp( "writer", writer )
         ? WriterTypeWriter
         : 0 == vtksys::SystemTools::Strucmp( "brw", writer ) ||
           0 == vtksys::SystemTools::Strucmp( "brwwriter", writer )
         ? WriterTypeBRW
         : 0 == vtksys::SystemTools::Strucmp( "ibr", writer ) ||
           0 == vtksys::SystemTools::Strucmp( "ibrwriter", writer )
         ? WriterTypeIBR
         : WriterTypeNone;
  }

protected:
  ovOrlandoReader() {}
  ~ovOrlandoReader() {}

  virtual int ProcessRequest(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int RequestDataObject(
    vtkInformation *, vtkInformationVector **, vtkInformationVector * );

  virtual int FillOutputPortInformation( int, vtkInformation* );

private:
  ovOrlandoReader( const ovOrlandoReader& );  // Not implemented.
  void operator=( const ovOrlandoReader& );  // Not implemented.
};

#endif
