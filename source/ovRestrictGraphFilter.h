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

class vtkStringArray;

class ovRestrictGraphFilter : public vtkGraphAlgorithm
{
public:
  static ovRestrictGraphFilter* New();
  vtkTypeRevisionMacro( ovRestrictGraphFilter, vtkGraphAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  vtkSetMacro( AuthorsOnly, int );
  vtkGetMacro( AuthorsOnly, int );
  vtkBooleanMacro( AuthorsOnly, int );

  // Description:
  // Set/get the Tags array name
  virtual ovString GetTagsArrayName() { return this->TagsArrayName; }
  virtual void SetTagsArrayName( const ovString &name );
  
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
  
  enum GenderTypeRestriction
  {
    GenderTypeRestrictionMale,
    GenderTypeRestrictionFemale,
    GenderTypeRestrictionAny
  };

  vtkSetClampMacro( GenderTypeRestriction, int, GenderTypeRestrictionMale, GenderTypeRestrictionAny );
  vtkGetMacro( GenderTypeRestriction, int );

  static int GenderTypeRestrictionFromString( const char *type )
  {
    return 0 == vtksys::SystemTools::Strucmp( "male", type )
         ? GenderTypeRestrictionMale
         : 0 == vtksys::SystemTools::Strucmp( "female", type )
         ? GenderTypeRestrictionFemale
         : GenderTypeRestrictionAny;
  }

  enum WriterTypeRestriction
  {
    WriterTypeRestrictionWriter,
    WriterTypeRestrictionBRW,
    WriterTypeRestrictionIBR,
    WriterTypeRestrictionWriterOrBRW,
    WriterTypeRestrictionWriterOrIBR,
    WriterTypeRestrictionBRWOrIBR,
    WriterTypeRestrictionAny
  };

  vtkSetClampMacro( WriterTypeRestriction, int, WriterTypeRestrictionWriter, WriterTypeRestrictionAny );
  vtkGetMacro( WriterTypeRestriction, int );

  static int WriterTypeRestrictionFromString( const char *type )
  {
    return 0 == vtksys::SystemTools::Strucmp( "writer", type )
         ? WriterTypeRestrictionWriter
         : 0 == vtksys::SystemTools::Strucmp( "brw", type ) ||
           0 == vtksys::SystemTools::Strucmp( "brw writer", type )
         ? WriterTypeRestrictionBRW
         : 0 == vtksys::SystemTools::Strucmp( "ibr", type ) || 
           0 == vtksys::SystemTools::Strucmp( "ibr writer", type )
         ? WriterTypeRestrictionIBR
         : 0 == vtksys::SystemTools::Strucmp( "writer or brw", type ) ||
           0 == vtksys::SystemTools::Strucmp( "writerorbrw", type )
         ? WriterTypeRestrictionWriterOrBRW
         : 0 == vtksys::SystemTools::Strucmp( "writer or ibr", type ) ||
           0 == vtksys::SystemTools::Strucmp( "writeroribr", type )
         ? WriterTypeRestrictionWriterOrIBR
         : 0 == vtksys::SystemTools::Strucmp( "brw or ibr", type ) ||
           0 == vtksys::SystemTools::Strucmp( "brworibr", type )
         ? WriterTypeRestrictionBRWOrIBR
         : WriterTypeRestrictionAny;
  }

  vtkGetObjectMacro( ActiveTags, vtkStringArray );
  virtual void SetActiveTags( vtkStringArray* );
  
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
  ovString TagsArrayName;
  ovString GenderArrayName;
  ovString BirthArrayName;
  ovString DeathArrayName;
  ovString WriterTypeArrayName;
  ovString EdgeColorArrayName;

  int AuthorsOnly;
  int GenderTypeRestriction;
  int WriterTypeRestriction;
  vtkStringArray *ActiveTags;
  ovDate StartDate;
  ovDate EndDate;
  
private:
  ovRestrictGraphFilter( const ovRestrictGraphFilter& ); // Not implemented
  void operator=( const ovRestrictGraphFilter& );   // Not implemented
};

#endif

