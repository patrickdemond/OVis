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
  // Add association types to track when loading the file.  The rank parameter
  // will place the tag into a particular rank, or none if it is 0.
  virtual void Add( ovString name, int rank = 0, bool active = true );

  // Description:
  // Call this once all tags have been read, this sorts and makes unique the list
  // and prevents any further changes.
  void Finalize();

  // Description:
  // Returns the number of ranks.
  virtual int GetNumberOfRanks();
  
  // Description:
  // Returns the total number of tags.  If rank is not 0 then the number of tags
  // at the given rank will be returned, otherwise the total count is returned.
  virtual int GetNumberOfTags( int rank = 0 );
  
  // Description:
  // Returns the a particular tag, or NULL if the tag is not found.
  // By calling this method the tag info object will be finalized, meaning it
  // cannot be changed any further.
  // The rank parameter will restrict the search to a particular rank, or if
  // it is 0 then no rank restriction will be performed.
  virtual ovTag* FindTag( ovString, int rank = 0 );

  // Description:
  // Returns the index of a particular tag, or -1 if the tag is not found.
  // By calling this method the tag info object will be finalized, meaning it
  // cannot be changed any further.
  // The rank parameter will restrict the search to a particular rank, or if
  // it is 0 then no rank restriction will be performed.
  virtual int FindTagIndex( ovString, int rank = 0 );

  // Description:
  // Populates a string array with the tags.  If rank is not 0 then only the tags
  // of the given rank or smaller are included, of it is 0 then all tags are included.
  virtual void GetTags( ovTagVector &array, int rank = 0 );

protected:
  ovOrlandoTagInfo();
  ~ovOrlandoTagInfo();
  
  static ovOrlandoTagInfo *New();
  static vtkSmartPointer< ovOrlandoTagInfo > Instance;
  
  bool Final;
  ovTagVector TagVector;

private:
  ovOrlandoTagInfo( const ovOrlandoTagInfo& );  // Not implemented.
  void operator=( const ovOrlandoTagInfo& );  // Not implemented.
};

#endif
