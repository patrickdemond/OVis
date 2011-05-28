/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoTagInfo.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovOrlandoTagInfo - Contains information about Orlando tags
//
// .SECTION Description
// This class is a singleton which describes details about orlando tags.
//
// .SECTION See Also
// 

#ifndef __ovOrlandoTagInfo_h
#define __ovOrlandoTagInfo_h

#include "vtkObject.h"

#include "ovUtilities.h"
#include "vtkSmartPointer.h"

class ovOrlandoTagInfo : public vtkObject
{
// we need to friend the smart pointer class so that it has access to ::New()
friend class vtkSmartPointer< ovOrlandoTagInfo >;
public:
  // Description:
  // This method returns the one and only instantiation of the class, use this
  // method to get an object from this class
  static ovOrlandoTagInfo *GetInfo();
  vtkTypeRevisionMacro( ovOrlandoTagInfo, vtkObject );
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Add association types (tags) to track when loading the file.  The parent-child
  // hierarchy is used by the UI for easier management of tags
  virtual void Add( ovString parent, ovString name, ovString title, bool active = true );

  // Description:
  // Returns the a particular tag given its name, or NULL if the tag is not found.
  virtual ovTag* FindTagFromName( ovString name )
  { return this->FindTag( name, false ); }

  // Description:
  // Returns the index of a particular tag given its name, or -1 if the tag is not found.
  virtual int FindTagIndexFromName( ovString name )
  { return this->FindTagIndex( name, false ); }

  // Description:
  // Returns the a particular tag given its title, or NULL if the tag is not found.
  virtual ovTag* FindTagFromTitle( ovString title )
  { return this->FindTag( title, true ); }

  // Description:
  // Returns the index of a particular tag given its title, or -1 if the tag is not found.
  virtual int FindTagIndexFromTitle( ovString title )
  { return this->FindTagIndex( title, true ); }

  // Description:
  // Returns the total number of tags.
  virtual int GetNumberOfTags();
  
  // Description:
  // Populates a string array with the tags.
  virtual ovTagVector* GetTags() { return &( this->TagVector ); }

protected:
  ovOrlandoTagInfo();
  ~ovOrlandoTagInfo();
  
  static ovOrlandoTagInfo *New();
  static vtkSmartPointer< ovOrlandoTagInfo > Instance;
  
  // Description:
  // Returns the a particular tag given its name or title, or NULL if the tag is not found.
  virtual ovTag* FindTag( ovString, bool );

  // Description:
  // Returns the index of a particular tag given its name or title, or -1 if the tag is not found.
  virtual int FindTagIndex( ovString, bool );

  ovTagVector TagVector;

private:
  ovOrlandoTagInfo( const ovOrlandoTagInfo& );  // Not implemented.
  void operator=( const ovOrlandoTagInfo& );  // Not implemented.
};

#endif
