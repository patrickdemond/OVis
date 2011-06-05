/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovRestrictGraphFilter.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovRestrictGraphFilter - Restricts edges to the given tags
//
// .SECTION Description
// ovRestrictGraphFilter restricts the graph by removing any edges which do not
// have any of the tags listed in the given tag list.  Vertices which are
// left with no edges are also removed.
//
// Input port 0: graph

#ifndef __ovRestrictGraphFilter_h
#define __ovRestrictGraphFilter_h

#include "vtkGraphAlgorithm.h"

#include "ovUtilities.h"
#include "vtksys/SystemTools.hxx"

class ovSearchPhrase;
class vtkStringArray;

class ovRestrictGraphFilter : public vtkGraphAlgorithm
{
public:
  static ovRestrictGraphFilter* New();
  vtkTypeRevisionMacro( ovRestrictGraphFilter, vtkGraphAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  vtkSetMacro( IncludeWriters, int );
  vtkGetMacro( IncludeWriters, int );
  vtkBooleanMacro( IncludeWriters, int );

  vtkSetMacro( IncludeOthers, int );
  vtkGetMacro( IncludeOthers, int );
  vtkBooleanMacro( IncludeOthers, int );

  vtkSetMacro( IncludeFemale, int );
  vtkGetMacro( IncludeFemale, int );
  vtkBooleanMacro( IncludeFemale, int );

  vtkSetMacro( IncludeMale, int );
  vtkGetMacro( IncludeMale, int );
  vtkBooleanMacro( IncludeMale, int );

  vtkSetMacro( IncludeBRWType, int );
  vtkGetMacro( IncludeBRWType, int );
  vtkBooleanMacro( IncludeBRWType, int );

  vtkSetMacro( IncludeWriterType, int );
  vtkGetMacro( IncludeWriterType, int );
  vtkBooleanMacro( IncludeWriterType, int );

  vtkSetMacro( IncludeIBRType, int );
  vtkGetMacro( IncludeIBRType, int );
  vtkBooleanMacro( IncludeIBRType, int );

  // Description:
  // Set/get the Pedigree array name
  virtual ovString GetPedigreeArrayName() { return this->PedigreeArrayName; }
  virtual void SetPedigreeArrayName( const ovString &name );
  
  // Description:
  // Set/get the Tags array name
  virtual ovString GetTagsArrayName() { return this->TagsArrayName; }
  virtual void SetTagsArrayName( const ovString &name );
  
  // Description:
  // Set/get the Content array name
  virtual ovString GetContentArrayName() { return this->ContentArrayName; }
  virtual void SetContentArrayName( const ovString &name );
  
  // Description:
  // Set/get the Content array name
  virtual ovString GetStemmedContentArrayName() { return this->StemmedContentArrayName; }
  virtual void SetStemmedContentArrayName( const ovString &name );
  
  // Description:
  // Set/get the Gender array name
  virtual ovString GetGenderArrayName() { return this->GenderArrayName; }
  virtual void SetGenderArrayName( const ovString &name );
  
  // Description:
  // Set/get the Birth array name
  virtual ovString GetBirthArrayName() { return this->BirthArrayName; }
  virtual void SetBirthArrayName( const ovString &name );
  
  // Description:
  // Set/get the Death array name
  virtual ovString GetDeathArrayName() { return this->DeathArrayName; }
  virtual void SetDeathArrayName( const ovString &name );
  
  // Description:
  // Set/get the WriterType array name
  virtual ovString GetWriterTypeArrayName() { return this->WriterTypeArrayName; }
  virtual void SetWriterTypeArrayName( const ovString &name );
  
  // Description:
  // Set/get the EdgeColor array name
  virtual ovString GetEdgeColorArrayName() { return this->EdgeColorArrayName; }
  virtual void SetEdgeColorArrayName( const ovString &name );
  
  vtkGetObjectMacro( ActiveTags, vtkStringArray );
  virtual void SetActiveTags( vtkStringArray* );
  
  vtkGetObjectMacro( TextSearchPhrase, ovSearchPhrase );
  virtual void SetTextSearchPhrase( ovSearchPhrase* );

  vtkGetObjectMacro( AuthorSearchPhrase, ovSearchPhrase );
  virtual void SetAuthorSearchPhrase( ovSearchPhrase* );

  virtual ovDate* GetStartDate() { return &( this->StartDate ); }
  virtual void SetStartDate( const ovDate& );

  virtual ovDate* GetEndDate() { return &( this->EndDate ); }
  virtual void SetEndDate( const ovDate& );

protected:
  ovRestrictGraphFilter();
  ~ovRestrictGraphFilter();

  int RequestData(
    vtkInformation*, 
    vtkInformationVector**, 
    vtkInformationVector* );
  
  // array names
  ovString PedigreeArrayName;
  ovString TagsArrayName;
  ovString ContentArrayName;
  ovString StemmedContentArrayName;
  ovString GenderArrayName;
  ovString BirthArrayName;
  ovString DeathArrayName;
  ovString WriterTypeArrayName;
  ovString EdgeColorArrayName;

  bool IncludeWriters;
  bool IncludeOthers;
  bool IncludeFemale;
  bool IncludeMale;
  bool IncludeBRWType;
  bool IncludeWriterType;
  bool IncludeIBRType;
  vtkStringArray *ActiveTags;
  ovSearchPhrase *TextSearchPhrase;
  ovSearchPhrase *AuthorSearchPhrase;
  ovDate StartDate;
  ovDate EndDate;
  
private:
  ovRestrictGraphFilter( const ovRestrictGraphFilter& ); // Not implemented
  void operator=( const ovRestrictGraphFilter& );   // Not implemented
};

#endif

