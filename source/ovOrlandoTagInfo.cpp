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

vtkCxxRevisionMacro( ovOrlandoTagInfo, "$Revision: $" );

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
  vtkstd::for_each( this->TagVector.begin(), this->TagVector.end(), safe_delete() );
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
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PADVERTISING", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PANTHOLOGIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PARCHIVALLOCATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PATTITUDES", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PAUTHORSHIP", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PCIRCULATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PCONTRACT", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PCOPYRIGHT", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PDEDICATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PEARNINGS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PEDITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PFIRSTLITERARYACTIVITY", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PINFLUENCESHER", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PLACE", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PLASTLITERARYACTIVITY", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PLITERARYSCHOOLS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PMANUSCRIPTHISTORY", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PMATERIALCONDITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PMODEOFPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PMOTIVES", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PNONBOOKMEDIA", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PNONSURVIVAL", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "POLITICALAFFILIATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PPERFORMANCE", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PPERIODICALPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PPLACEOFPUBLICATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PPRESSRUN", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PPRICE", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PRARITIESFEATURESDECORATIONS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PRELATIONSWITHPUBLISHER", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PSERIALIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PSUBMISSIONSREJECTIONS", false );
    ovOrlandoTagInfo::Instance->Add( "PRODUCTION", "PTYPEOFPRESS", false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", "RECEPTION", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RBESTKNOWNWORK", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RDESTRUCTIONOFWORK", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RFICTIONALIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RLANDMARKTEXT", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RPENALTIES", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RRECOGNITIONS", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RRECOGNITIONVALUE", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RRESPONSES", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RSELFDESCRIPTION", false );
    ovOrlandoTagInfo::Instance->Add( "RECEPTION", "RSHEINFLUENCED", false );
    ovOrlandoTagInfo::Instance->Add( "WRITING", "TEXTUALFEATURES", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TCHARACTERIZATION", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TCHARACTERNAME", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TCHARACTERTYPEROLE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TEXT", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TGENRE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TGENREISSUE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TINTERTEXTUALITY", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TITLE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TMOTIF", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TPLOT", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TSETTINGDATE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TSETTINGPLACE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TTECHNIQUES", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TTHEMETOPIC", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TTONESTYLE", false );
    ovOrlandoTagInfo::Instance->Add( "TEXTUALFEATURES", "TVOICENARRATION", false );
    
    // add in the unordered tags
    ovOrlandoTagInfo::Instance->Add( "", "General", false );
    ovOrlandoTagInfo::Instance->Add( "General", "COMPANION", false );
    ovOrlandoTagInfo::Instance->Add( "General", "DATE", false );
    ovOrlandoTagInfo::Instance->Add( "General", "DEATH", false );
    ovOrlandoTagInfo::Instance->Add( "General", "GEOG", false );
    ovOrlandoTagInfo::Instance->Add( "General", "LIVESWITH", false );
    ovOrlandoTagInfo::Instance->Add( "General", "NAME", false );
    ovOrlandoTagInfo::Instance->Add( "General", "ORGNAME", false );
    ovOrlandoTagInfo::Instance->Add( "General", "QUOTE", false );
    ovOrlandoTagInfo::Instance->Add( "General", "SCHOLARNOTE", false );
    ovOrlandoTagInfo::Instance->Add( "General", "SIGNIFICANTACTIVITY", false );
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
  
  for( ovTagVector::iterator it = this->TagVector.begin(); it != this->TagVector.end(); ++it )
  {
    if( (*it)->name == name )
    {
      index = static_cast< int >( it - this->TagVector.begin() );
      break;
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

