/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoTagInfo.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoTagInfo.h"

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
ovOrlandoTagInfo::~ovOrlandoTagInfo()
{
  // delete all tags
  ovTagVector::iterator it;
  for( it = this->TagVector.begin(); it != this->TagVector.end(); it++ )
  {
    delete (*it);
  }
  this->TagVector.empty();
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
        this->Add( line, currentRank, 1 == currentRank ? true : false );
      }
    }
    fileStream.close();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::Add( ovString name, int rank, bool active )
{
  if( this->Final )
  {
    vtkWarningMacro( "Attempting to add tag to finalized tag info." );
  }
  else
  {
    ovTag *tag = new ovTag( name, rank, active );
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
      ovTagSort );
    vtkstd::unique(
      this->TagVector.begin(),
      this->TagVector.end(),
      ovTagUnRankedCompare );
    this->Final = true;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::GetNumberOfRanks()
{
  // make sure to finalize before searching
  this->Finalize();
  
  int highestRank = 0;

  // search through the tags and count the number or ranks
  ovTagVector::iterator it;
  for( it = this->TagVector.begin(); it != this->TagVector.end(); it++ )
    if( highestRank < (*it)->rank ) highestRank = (*it)->rank;

  return highestRank;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::GetNumberOfTags( int rank )
{
  // make sure to finalize before searching
  this->Finalize();
  
  int total = 0;
  if( 0 == rank )
  {
    total = this->TagVector.size();
  }
  else
  {
    ovTagVector::iterator it;
    for( it = this->TagVector.begin(); it != this->TagVector.end(); it++ )
      if( rank == (*it)->rank ) total++;
  }

  return total;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovTag* ovOrlandoTagInfo::FindTag( ovString name, int rank )
{
  int index = this->FindTagIndex( name, rank );
  return -1 == index ? NULL : this->TagVector.at( index );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::FindTagIndex( ovString name, int rank )
{
  int index = -1; // assume the tag isn't present
  ovTag *tag;
  
  // make sure to finalize before searching
  this->Finalize();

  tag = new ovTag( name, rank );
  ovTagVectorRange range = 
    vtkstd::equal_range(
      this->TagVector.begin(),
      this->TagVector.end(), tag,
      ovTagSort );
  delete tag;

  // if the range is 0 (ends are equal) then the tag wasn't found
  if( range.first != range.second )
  {
    // make sure rank matches, if necessary
    tag = *( range.first );
    if( 0 == rank || rank == tag->rank )
    {
      index = static_cast< int >( range.first - this->TagVector.begin() );
    }
  }

  return index;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::GetTags( ovTagVector &array, int rank )
{
  // make sure to finalize before searching
  this->Finalize();
  
  array.clear();
  ovTagVector::iterator it;
  for( it = this->TagVector.begin(); it != this->TagVector.end(); it++ )
  {
    if( 0 == rank || rank >= (*it)->rank )
    {
      array.push_back( (*it) );
    }
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Final: " << ( this->Final ? "yes" : "no" ) << endl;
  os << indent << "TagVector: " << &this->TagVector << endl;
  os << indent << "Number of tags: " << this->TagVector.size() << endl;
}

