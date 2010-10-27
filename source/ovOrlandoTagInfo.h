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
  // Add association types to track when loading the file.
  virtual void Add( ovString tag, int rank = 0 );

  // Description:
  // Call this once all tags have been read, this sorts and makes unique the list
  // and prevents any further changes.
  void Finalize();

  // Description:
  // Returns the total number of tags
  virtual int GetNumberOfTags() { return this->TagVector.size(); }
  
  // Description:
  // Returns the index of a particular tag, or -1 if the tag is not found.
  // By calling this method the tag info object will be finalized, meaning it
  // cannot be changed any further.
  virtual int FindTag( ovString );

protected:
  ovOrlandoTagInfo();
  ~ovOrlandoTagInfo() {};
  
  // Description:
  // Reads the tag list from the resources directory
  virtual void ReadDefaultTags();

  static ovOrlandoTagInfo *New();
  static vtkSmartPointer< ovOrlandoTagInfo > Instance;
  
  bool Final;
  ovStringVector TagVector;

private:
  ovOrlandoTagInfo( const ovOrlandoTagInfo& );  // Not implemented.
  void operator=( const ovOrlandoTagInfo& );  // Not implemented.
};

#endif
