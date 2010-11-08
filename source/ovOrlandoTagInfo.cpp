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

    // add in the default Orlando tags
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", 1, false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", 1, false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", 1, false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", 1, false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", 2, false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", 2, false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", 2, false );
    ovOrlandoTagInfo::Instance->Add( "FRIENDSASSOCIATES", 2, false );
    ovOrlandoTagInfo::Instance->Add( "INTIMATERELATIONSHIPS", 2, false );
    ovOrlandoTagInfo::Instance->Add( "LEISUREANDSOCIETY", 2, false );
    ovOrlandoTagInfo::Instance->Add( "LOCATION", 2, false );
    ovOrlandoTagInfo::Instance->Add( "OCCUPATION", 2, false );
    ovOrlandoTagInfo::Instance->Add( "POLITICS", 2, false );
    ovOrlandoTagInfo::Instance->Add( "SIGNIFICANTACTIVITY", 2, false );
    ovOrlandoTagInfo::Instance->Add( "VIOLENCE", 2, false );
    ovOrlandoTagInfo::Instance->Add( "WEALTH", 2, false );
    ovOrlandoTagInfo::Instance->Add( "AUTHORSUMMARY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "AWARD", 3, false );
    ovOrlandoTagInfo::Instance->Add( "CHILDLESSNESS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "CLASS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "CLASSISSUE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "COMPANION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "CONTESTEDBEHAVIOUR", 3, false );
    ovOrlandoTagInfo::Instance->Add( "DEGREE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "DIVORCE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "EMPLOYER", 3, false );
    ovOrlandoTagInfo::Instance->Add( "ETHNICITY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "GEOGHERITAGE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "HEALTH", 3, false );
    ovOrlandoTagInfo::Instance->Add( "JOB", 3, false );
    ovOrlandoTagInfo::Instance->Add( "LANGUAGE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "LIVESWITH", 3, false );
    ovOrlandoTagInfo::Instance->Add( "MARRIAGE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "MEMBER", 3, false );
    ovOrlandoTagInfo::Instance->Add( "NAME", 3, false );
    ovOrlandoTagInfo::Instance->Add( "NATIONALITYISSUE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "ORGNAME", 3, false );
    ovOrlandoTagInfo::Instance->Add( "OTHERLIFEEVENT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PADVERTISING", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PANTHOLOGIZATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PATTITUDES", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PAUTHORSHIP", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PCIRCULATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PCONTRACT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PCOPYRIGHT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PDEDICATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PEARNINGS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PFIRSTLITERARYACTIVITY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PINFLUENCESHER", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PLACE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PLITERARYSCHOOLS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PMATERIALCONDITIONS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PMODEOFPUBLICATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PMOTIVES", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PNONSURVIVAL", 3, false );
    ovOrlandoTagInfo::Instance->Add( "POLITICALAFFILIATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PPERFORMANCE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PPERIODICALPUBLICATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PPLACEOFPUBLICATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PPRESSRUN", 3, false );
    ovOrlandoTagInfo::Instance->Add( "PRELATIONSWITHPUBLISHER", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RACEANDETHNICITY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RACECOLOUR", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RBESTKNOWNWORK", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RDESTRUCTIONOFWORK", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RELIGION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "REMUNERATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RFICTIONALIZATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RLANDMARKTEXT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RPENALTIES", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RRECOGNITIONS", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RRECOGNITIONVALUE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RRESPONSES", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RSELFDESCRIPTION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "RSHEINFLUENCED", 3, false );
    ovOrlandoTagInfo::Instance->Add( "SEPARATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "SEXUALITY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "SUBJECT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TCHARACTERIZATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TCHARACTERNAME", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TCHARACTERTYPEROLE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TEXT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TGENRE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TGENREISSUE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TINTERTEXTUALITY", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TITLE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TMOTIF", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TPLOT", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TSETTINGDATE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TSETTINGPLACE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TTECHNIQUES", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TTHEMETOPIC", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TTONESTYLE", 3, false );
    ovOrlandoTagInfo::Instance->Add( "TVOICENARRATION", 3, false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", 3, false );
    ovOrlandoTagInfo::Instance->Add( "BIRTH", 3, false );
    ovOrlandoTagInfo::Instance->Add( "BIRTHPOSITION", 4, false );
    ovOrlandoTagInfo::Instance->Add( "CHILDREN", 4, false );
    ovOrlandoTagInfo::Instance->Add( "DATE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "DEATH", 4, false );
    ovOrlandoTagInfo::Instance->Add( "DENOMINATION", 4, false );
    ovOrlandoTagInfo::Instance->Add( "EXTENTOFOEUVRE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "GENERICRANGE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "GEOG", 4, false );
    ovOrlandoTagInfo::Instance->Add( "INSTRUCTOR", 4, false );
    ovOrlandoTagInfo::Instance->Add( "NATIONALHERITAGE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "NATIONALITY", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PARCHIVALLOCATION", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PEDITIONS", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PERSONNAME", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PLASTLITERARYACTIVITY", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PMANUSCRIPTHISTORY", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PNONBOOKMEDIA", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PPRICE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PRARITIESFEATURESDECORATIONS", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PSERIALIZATION", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PSEUDONYM", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PSUBMISSIONSREJECTIONS", 4, false );
    ovOrlandoTagInfo::Instance->Add( "PTYPEOFPRESS", 4, false );
    ovOrlandoTagInfo::Instance->Add( "QUOTE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "SCHOLARNOTE", 4, false );
    ovOrlandoTagInfo::Instance->Add( "SCHOOL", 4, false );
    ovOrlandoTagInfo::Instance->Add( "SEXUALIDENTITY", 4, false );
  }

  return ovOrlandoTagInfo::Instance;
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

