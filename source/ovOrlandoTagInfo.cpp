/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoTagInfo.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoTagInfo.h"

#include "ovUtilities.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkVariant.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovOrlandoTagInfo, "$Revision: 1.4 $" );

vtkSmartPointer< ovOrlandoTagInfo > ovOrlandoTagInfo::Instance = NULL; // set the initial singleton

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovOrlandoTagInfo::ovOrlandoTagInfo()
{
  this->Final = false;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
// This must be implemented directly instead of using the vtkStandardNewMacro
// since this method is protected (ie: we do not want an instantiator for it)
ovOrlandoTagInfo* ovOrlandoTagInfo::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance( "ovOrlandoTagInfo" );
  if( ret )
  {
    return static_cast< ovOrlandoTagInfo* >( ret );
  }
  return new ovOrlandoTagInfo;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovOrlandoTagInfo* ovOrlandoTagInfo::GetInfo()
{
  // create the one and only orlando tag info object
  if( NULL == ovOrlandoTagInfo::Instance )
  {
    ovOrlandoTagInfo::Instance = vtkSmartPointer< ovOrlandoTagInfo >::New();
    ovOrlandoTagInfo::Instance->ReadDefaultTags();
  }

  return ovOrlandoTagInfo::Instance;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::ReadDefaultTags()
{
  // open the tag list from the resources directory
  vtkstd::ifstream fileStream( "resources/tag_list.txt" );
  if( fileStream.is_open() )
  {
    bool valid;
    int currentRank = 0, rank = 0;
    ovString line;
    while( getline( fileStream, line ) )
    {
      // The line is either a number of text: a number meaning the rank of the
      // following tags has changed, text means a new tag is to be added to the
      // list
      rank = vtkVariant( line ).ToInt( &valid );
      if( valid )
      {
        // update the current rank that new tags will be added under
        currentRank = rank;
      }
      else
      {
        // assume the line is a tag
        this->Add( line, currentRank );
      }
    }
    fileStream.close();
  }

}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::Add( ovString name, int rank )
{
  if( this->Final )
  {
    vtkWarningMacro( "Attempting to add tag to finalized tag info." );
  }
  else
  {
    ovStringIntPair tag( name, rank );
    this->TagVector.push_back( tag );
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::Finalize()
{
  if( !this->Final )
  {
    vtkstd::sort(
      this->TagVector.begin(),
      this->TagVector.end(),
      ovOrlandoTagInfo::SortTags );
    vtkstd::unique(
      this->TagVector.begin(),
      this->TagVector.end(),
      ovOrlandoTagInfo::UnRankedCompareTags );
    this->Final = true;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::FindTag( ovString name, int rank )
{
  int index = -1; // assume the tag isn't present
  
  // make sure to finalize before searching
  this->Finalize();
  
  ovStringIntPair tag( name, rank );
  ovStringIntPairVectorRange range = 
    vtkstd::equal_range(
      this->TagVector.begin(), this->TagVector.end(), tag,
      ovOrlandoTagInfo::SortTags );
  
  // if the range is 0 (ends are equal) then the tag wasn't found
  if( range.first != range.second )
  {
    // make sure rank matches, if necessary
    if( 0 == rank || rank == range.first->second )
    {
      index = static_cast< int >( range.first - this->TagVector.begin() );
    }
  }

  return index;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "TagVector: " << &this->TagVector << endl;
  os << indent << "Number of tags: " << this->TagVector.size() << endl;
}

