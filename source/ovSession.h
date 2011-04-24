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

class vtkCamera;

class ovSession : public vtkDataObject
{
public:
  static ovSession *New();
  vtkTypeRevisionMacro( ovSession, vtkDataObject );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  bool operator == ( const ovSession& ) const;
  bool operator != ( const ovSession &rhs ) const { return !( *this == rhs ); }
  
  virtual void DeepCopy( ovSession* );

  // Description:
  // Set/get the path of the data file currently loaded
  virtual ovString GetDataFile() { return this->DataFile; }
  virtual void SetDataFile( const ovString& );

  // Description:
  // Set/get the first background color
  vtkGetVector4Macro( BackgroundColor1, double );
  vtkSetVector4Macro( BackgroundColor1, double );

  // Description:
  // Set/get the second background color
  vtkGetVector4Macro( BackgroundColor2, double );
  vtkSetVector4Macro( BackgroundColor2, double );
  
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
  vtkGetVector4Macro( AuthorVertexColor, double );
  vtkSetVector4Macro( AuthorVertexColor, double );

  // Description:
  // Set/get the association vertices color
  vtkGetVector4Macro( AssociationVertexColor, double );
  vtkSetVector4Macro( AssociationVertexColor, double );

  // Description:
  // Set/get the text search phrase
  virtual ovString GetTextSearchPhrase() { return this->TextSearchPhrase; }
  virtual void SetTextSearchPhrase( const ovString& );

  // Description:
  // Set/get the text search phrase
  virtual ovString GetAuthorSearchPhrase() { return this->AuthorSearchPhrase; }
  virtual void SetAuthorSearchPhrase( const ovString& );

  // Description:
  // Set/get the start date
  virtual ovDate GetStartDateRestriction() { return this->StartDateRestriction; }
  virtual void SetStartDateRestriction( const ovDate& );

  // Description:
  // Set/get the end date
  virtual ovDate GetEndDateRestriction() { return this->EndDateRestriction; }
  virtual void SetEndDateRestriction( const ovDate& );

  // Description:
  // Set/get the active tags
  virtual ovIntVector* GetSelectedVertexList() { return &( this->SelectedVertexList ); }

  // Description:
  // Set/get the active tags
  virtual ovIntVector* GetSelectedEdgeList() { return &( this->SelectedEdgeList ); }

  // Description:
  // Set/get the active tags
  virtual ovTagVector* GetTagList() { return &( this->TagList ); }

  // Description:
  // Set/get the camera
  virtual vtkCamera* GetCamera() { return this->Camera; }
  
protected:
  ovSession();
  ~ovSession();
  
  virtual void SetCamera( vtkCamera* );

  ovString DataFile;
  double BackgroundColor1[4];
  double BackgroundColor2[4];
  int VertexStyle;
  ovString LayoutStrategy;
  bool AuthorsOnly;
  int GenderTypeRestriction;
  int WriterTypeRestriction;
  int VertexSize;
  int EdgeSize;
  double AuthorVertexColor[4];
  double AssociationVertexColor[4];
  ovString TextSearchPhrase;
  ovString AuthorSearchPhrase;
  ovDate StartDateRestriction;
  ovDate EndDateRestriction;
  ovIntVector SelectedVertexList;
  ovIntVector SelectedEdgeList;
  ovTagVector TagList;
  vtkCamera *Camera;

private:
  ovSession( const ovSession& );  // Not implemented.
  void operator=( const ovSession& );  // Not implemented.
};

#endif
