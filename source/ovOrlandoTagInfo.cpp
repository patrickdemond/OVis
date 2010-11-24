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

    // add in the ordered tags
    ovOrlandoTagInfo::Instance->Add( "", "BIOGRAPHY", false ); // no parent
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "BIRTH", false );
    ovOrlandoTagInfo::Instance->Add( "BIRTH", "BIRTHPOSITION", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "CULTURALFORMATION", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "CLASS", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "CLASSISSUE", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "DENOMINATION", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "ETHNICITY", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "GEOGHERITAGE", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "LANGUAGE", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "NATIONALHERITAGE", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "NATIONALITY", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "NATIONALITYISSUE", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "RACEANDETHNICITY", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "RACECOLOUR", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "RELIGION", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "SEXUALIDENTITY", false );
    ovOrlandoTagInfo::Instance->Add( "CULTURALFORMATION", "SEXUALITY", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "EDUCATION", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "AWARD", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "CONTESTEDBEHAVIOUR", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "DEGREE", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "INSTRUCTOR", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "SCHOOL", false );
    ovOrlandoTagInfo::Instance->Add( "EDUCATION", "SUBJECT", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "FAMILY", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "MEMBER", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "CHILDLESSNESS", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "CHILDREN", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "DIVORCE", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "MARRIAGE", false );
    ovOrlandoTagInfo::Instance->Add( "FAMILY", "SEPARATION", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "FRIENDSASSOCIATES", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "HEALTH", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "INTIMATERELATIONSHIPS", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "LEISUREANDSOCIETY", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "LOCATION", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "OCCUPATION", false );
    ovOrlandoTagInfo::Instance->Add( "OCCUPATION", "EMPLOYER", false );
    ovOrlandoTagInfo::Instance->Add( "OCCUPATION", "JOB", false );
    ovOrlandoTagInfo::Instance->Add( "OCCUPATION", "REMUNERATION", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "OTHERLIFEEVENT", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "PERSONNAME", false );
    ovOrlandoTagInfo::Instance->Add( "PERSONNAME", "PSEUDONYM", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "POLITICS", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "VIOLENCE", false );
    ovOrlandoTagInfo::Instance->Add( "BIOGRAPHY", "WEALTH", false );
    ovOrlandoTagInfo::Instance->Add( "", "WRITING", false ); // no parent
    ovOrlandoTagInfo::Instance->Add( "WRITING", "AUTHORSUMMARY", false );
    ovOrlandoTagInfo::Instance->Add( "AUTHORSUMMARY", "EXTENTOFOEUVRE", false );
    ovOrlandoTagInfo::Instance->Add( "AUTHORSUMMARY", "GENERICRANGE", false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", "PRODUCTION", false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", "RECEPTION", false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", "TEXTUALFEATURES", false );
    
    // add in the unordered tags
    ovOrlandoTagInfo::Instance->Add( "", "Unordered", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "COMPANION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "DATE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "DEATH", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "GEOG", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "LIVESWITH", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "NAME", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "ORGNAME", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PADVERTISING", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PANTHOLOGIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PARCHIVALLOCATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PATTITUDES", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PAUTHORSHIP", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PCIRCULATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PCONTRACT", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PCOPYRIGHT", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PDEDICATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PEARNINGS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PEDITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PFIRSTLITERARYACTIVITY", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PINFLUENCESHER", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PLACE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PLASTLITERARYACTIVITY", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PLITERARYSCHOOLS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PMANUSCRIPTHISTORY", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PMATERIALCONDITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PMODEOFPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PMOTIVES", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PNONBOOKMEDIA", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PNONSURVIVAL", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "POLITICALAFFILIATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PPERFORMANCE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PPERIODICALPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PPLACEOFPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PPRESSRUN", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PPRICE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PRARITIESFEATURESDECORATIONS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PRELATIONSWITHPUBLISHER", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PSERIALIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PSUBMISSIONSREJECTIONS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "PTYPEOFPRESS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "QUOTE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RBESTKNOWNWORK", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RDESTRUCTIONOFWORK", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RFICTIONALIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RLANDMARKTEXT", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RPENALTIES", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RRECOGNITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RRECOGNITIONVALUE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RRESPONSES", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RSELFDESCRIPTION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "RSHEINFLUENCED", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "SCHOLARNOTE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "SIGNIFICANTACTIVITY", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TCHARACTERIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TCHARACTERNAME", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TCHARACTERTYPEROLE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TEXT", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TGENRE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TGENREISSUE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TINTERTEXTUALITY", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TITLE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TMOTIF", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TPLOT", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TSETTINGDATE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TSETTINGPLACE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TTECHNIQUES", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TTHEMETOPIC", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TTONESTYLE", false );
    ovOrlandoTagInfo::Instance->Add( "Unordered", "TVOICENARRATION", false );
  }

  return ovOrlandoTagInfo::Instance;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::Add( ovString parent, ovString name, bool active )
{
  ovTag *tag = new ovTag( parent, name, active );
  this->TagVector.push_back( tag );
  this->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::GetNumberOfTags()
{
  return this->TagVector.size();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovTag* ovOrlandoTagInfo::FindTag( ovString name )
{
  int index = this->FindTagIndex( name );
  return -1 == index ? NULL : this->TagVector.at( index );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::FindTagIndex( ovString name )
{
  int index = -1; // assume the tag isn't present
  ovTag *tag;
  
  tag = new ovTag( name );
  ovTagVectorRange range = 
    vtkstd::equal_range(
      this->TagVector.begin(),
      this->TagVector.end(), tag,
      ovTagSort );
  delete tag;

  // if the range is 0 (ends are equal) then the tag wasn't found
  if( range.first != range.second )
  {
    tag = *( range.first );
    index = static_cast< int >( range.first - this->TagVector.begin() );
  }

  return index;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::GetTags( ovTagVector &array )
{
  array.clear();
  ovTagVector::iterator it;
  for( it = this->TagVector.begin(); it != this->TagVector.end(); it++ )
  {
    array.push_back( (*it) );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "TagVector: " << &this->TagVector << endl;
  os << indent << "Number of tags: " << this->TagVector.size() << endl;
}

