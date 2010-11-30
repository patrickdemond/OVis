/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSession.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovSession - Session information object
//
// .SECTION Description
//
// .SECTION See Also
// ovSessionReader ovSessionWriter
// 

#ifndef __ovSession_h
#define __ovSession_h

#include "vtkDataObject.h"

#include "ovUtilities.h"

class vtkStringArray;

class ovSession : public vtkDataObject
{
public:
  static ovSession *New();
  vtkTypeRevisionMacro( ovSession, vtkDataObject );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  bool operator == ( const ovSession& ) const;
  bool operator != ( const ovSession &rhs ) const { return !( *this == rhs ); }
  
  // Description:
  // Set/get the path of the data file currently loaded
  virtual ovString GetDataFile() { return this->DataFile; }
  virtual void SetDataFile( const ovString& );

  // Description:
  // Set/get the first background color
  vtkGetVector3Macro( BackgroundColor1, double );
  vtkSetVector3Macro( BackgroundColor1, double );

  // Description:
  // Set/get the second background color
  vtkGetVector3Macro( BackgroundColor2, double );
  vtkSetVector3Macro( BackgroundColor2, double );
  
  // Description:
  // Set/get the vertex style
  vtkGetMacro( VertexStyle, int );
  vtkSetMacro( VertexStyle, int );
  
  // Description:
  // Set/get the path of the layout strategy
  virtual ovString GetLayoutStrategy() { return this->LayoutStrategy; }
  virtual void SetLayoutStrategy( const ovString& );

  // Description:
  // Set/get the vertex style
  vtkGetMacro( AuthorsOnly, int );
  vtkSetMacro( AuthorsOnly, int );
  vtkBooleanMacro( AuthorsOnly, int );
  
  // Description:
  // Set/get the gender type restriction
  vtkGetMacro( GenderTypeRestriction, int );
  vtkSetMacro( GenderTypeRestriction, int );

  // Description:
  // Set/get the writer type restriction
  vtkGetMacro( WriterTypeRestriction, int );
  vtkSetMacro( WriterTypeRestriction, int );
  
  // Description:
  // Set/get the vertex size
  vtkGetMacro( VertexSize, int );
  vtkSetMacro( VertexSize, int );
  
  // Description:
  // Set/get the edge size
  vtkGetMacro( EdgeSize, int );
  vtkSetMacro( EdgeSize, int );
  
  // Description:
  // Set/get the author vertices color
  vtkGetVector3Macro( AuthorVertexColor, double );
  vtkSetVector3Macro( AuthorVertexColor, double );

  // Description:
  // Set/get the association vertices color
  vtkGetVector3Macro( AssociationVertexColor, double );
  vtkSetVector3Macro( AssociationVertexColor, double );

  // Description:
  // Set/get the start date
  virtual ovDate GetStartDate() { return this->StartDate; }
  virtual void SetStartDate( const ovDate& );

  // Description:
  // Set/get the end date
  virtual ovDate GetEndDate() { return this->EndDate; }
  virtual void SetEndDate( const ovDate& );

  // Description:
  // Set/get the active tags
  virtual vtkStringArray* GetActiveTags() { return this->ActiveTags; }
  virtual void SetActiveTags( vtkStringArray* );
  
  // Description:
  // Set/get the camera's position
  vtkGetVector3Macro( CameraPosition, double );
  vtkSetVector3Macro( CameraPosition, double );

  // Description:
  // Set/get the camera's focal point
  vtkGetVector3Macro( CameraFocalPoint, double );
  vtkSetVector3Macro( CameraFocalPoint, double );

  // Description:
  // Set/get the camera's view up vector
  vtkGetVector3Macro( CameraViewUp, double );
  vtkSetVector3Macro( CameraViewUp, double );

  // Description:
  // Set/get the camera's clipping range
  vtkGetVector2Macro( CameraClippingRange, double );
  vtkSetVector2Macro( CameraClippingRange, double );

  // Description:
  // Set/get the camera's parallel scale
  vtkGetMacro( CameraParallelScale, double );
  vtkSetMacro( CameraParallelScale, double );
  
  // Description:
  // Set/get the camera's compute view plane normal
  vtkGetVector3Macro( CameraComputeViewPlaneNormal, double );
  vtkSetVector3Macro( CameraComputeViewPlaneNormal, double );
  
protected:
  ovSession();
  ~ovSession();
  
  ovString DataFile;
  double BackgroundColor1[3];
  double BackgroundColor2[3];
  int VertexStyle;
  ovString LayoutStrategy;
  bool AuthorsOnly;
  int GenderTypeRestriction;
  int WriterTypeRestriction;
  int VertexSize;
  int EdgeSize;
  double AuthorVertexColor[3];
  double AssociationVertexColor[3];
  ovDate StartDate;
  ovDate EndDate;
  vtkStringArray *ActiveTags;
  double CameraPosition[3];
  double CameraFocalPoint[3];
  double CameraViewUp[3];
  double CameraClippingRange[2];
  double CameraParallelScale;
  double CameraComputeViewPlaneNormal[3];

private:
  ovSession( const ovSession& );  // Not implemented.
  void operator=( const ovSession& );  // Not implemented.
};

#endif
