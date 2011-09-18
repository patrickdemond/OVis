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
  this->TagVector.clear();
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
    ovOrlandoTagInfo::Instance->Add(
      "", "BIOGRAPHY", "Biography", false ); // no parent
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "BIRTH", "Birth", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIRTH", "BIRTHPOSITION", "Birth Position", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "COMPANION", "Companion (at school)", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "CULTURALFORMATION", "Cultural Formation", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "CLASS", "Class", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "CLASSISSUE", "Class Issue", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "DENOMINATION", "Denomination", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "ETHNICITY", "Ethnicity", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "GEOGHERITAGE", "Geographic Heritage", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "LANGUAGE", "Language", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "NATIONALHERITAGE", "National Heritage", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "NATIONALITY", "Nationality", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "NATIONALITYISSUE", "Nationality Issue", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "RACEANDETHNICITY", "Race and Ethnicity", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "RACECOLOUR", "Race Colour", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "RELIGION", "Religion", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "SEXUALIDENTITY", "Sexual Identity", false );
    ovOrlandoTagInfo::Instance->Add(
      "CULTURALFORMATION", "SEXUALITY", "Sexuality", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "DEATH", "Death", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "EDUCATION", "Education", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "AWARD", "Award", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "CONTESTEDBEHAVIOUR", "Contested Behaviour", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "DEGREE", "Degree", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "INSTRUCTOR", "Instructor", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "SCHOOL", "School", false );
    ovOrlandoTagInfo::Instance->Add(
      "EDUCATION", "SUBJECT", "Subject", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "FAMILY", "Family", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "MEMBER", "Member", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "CHILDLESSNESS", "Childlessness", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "CHILDREN", "Children", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "DIVORCE", "Divorce", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "MARRIAGE", "Marriage", false );
    ovOrlandoTagInfo::Instance->Add(
      "FAMILY", "SEPARATION", "Separation", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "FRIENDSASSOCIATES", "Friends or Associates", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "HEALTH", "Health", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "INTIMATERELATIONSHIPS", "Intimate Relationships", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "LEISUREANDSOCIETY", "Leisure and Society", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "LIVESWITH", "Lives with (intimately)", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "LOCATION", "Location", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "OCCUPATION", "Occupation", false );
    ovOrlandoTagInfo::Instance->Add(
      "OCCUPATION", "EMPLOYER", "Employer", false );
    ovOrlandoTagInfo::Instance->Add(
      "OCCUPATION", "JOB", "Job", false );
    ovOrlandoTagInfo::Instance->Add(
      "OCCUPATION", "REMUNERATION", "Remuneration", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "OTHERLIFEEVENT", "Other Life Event", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "PERSONNAME", "Person Name", false );
    ovOrlandoTagInfo::Instance->Add(
      "PERSONNAME", "PSEUDONYM", "Pseudonym", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "POLITICS", "Politics", false );
    ovOrlandoTagInfo::Instance->Add(
      "POLITICS", "POLITICALAFFILIATION", "Political Affiliation", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "SIGNIFICANTACTIVITY", "Significant Activity (see also occupation)", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "VIOLENCE", "Violence", false );
    ovOrlandoTagInfo::Instance->Add(
      "BIOGRAPHY", "WEALTH", "Wealth", false );
    ovOrlandoTagInfo::Instance->Add(
      "", "WRITING", "Writing", false ); // no parent
    ovOrlandoTagInfo::Instance->Add(
      "WRITING", "AUTHORSUMMARY", "Author Summary", false );
    ovOrlandoTagInfo::Instance->Add(
      "AUTHORSUMMARY", "EXTENTOFOEUVRE", "Extent of Oeuvre", false );
    ovOrlandoTagInfo::Instance->Add(
      "AUTHORSUMMARY", "GENERICRANGE", "Generic Range", false );
    ovOrlandoTagInfo::Instance->Add(
      "WRITING", "PRODUCTION", "Production", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PADVERTISING", "Advertising", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PANTHOLOGIZATION", "Anthologization", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PARCHIVALLOCATION", "Archival Location", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PATTITUDES", "Attitudes", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PAUTHORSHIP", "Authorship", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PCIRCULATION", "Circulation", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PCONTRACT", "Contract", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PCOPYRIGHT", "Copyright", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PDEDICATION", "Dedication", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PEARNINGS", "Earnings", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PEDITIONS", "Editions", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PFIRSTLITERARYACTIVITY", "First Literary Activity", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PINFLUENCESHER", "Influences Her", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PLACE", "Place", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PLASTLITERARYACTIVITY", "Last Literary Activity", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PLITERARYSCHOOLS", "Literary Schools", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PMANUSCRIPTHISTORY", "Manuscript History", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PMATERIALCONDITIONS", "Material Conditions", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PMODEOFPUBLICATION", "Mode of Publication", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PMOTIVES", "Motives", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PNONBOOKMEDIA", "Non-book Media", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PNONSURVIVAL", "Non-Survival", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PPERFORMANCE", "Performance", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PPERIODICALPUBLICATION", "Periodical Publication", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PPLACEOFPUBLICATION", "Place of Publication", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PPRESSRUN", "Press Run", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PPRICE", "Price", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PRARITIESFEATURESDECORATIONS", "Rarities, Features orDecorations", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PRELATIONSWITHPUBLISHER", "Relations with Publisher", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PSERIALIZATION", "Serialization", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PSUBMISSIONSREJECTIONS", "Submissions or Rejections", false );
    ovOrlandoTagInfo::Instance->Add(
      "PRODUCTION", "PTYPEOFPRESS", "Type of Press", false );
    ovOrlandoTagInfo::Instance->Add(
      "WRITING", "RECEPTION", "Reception", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RBESTKNOWNWORK", "Best Known Work", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RDESTRUCTIONOFWORK", "Destruction of Work", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RFICTIONALIZATION", "Fictionalization", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RLANDMARKTEXT", "Landmark Text", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RPENALTIES", "Penalties", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RRECOGNITIONS", "Recognitions", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RRECOGNITIONVALUE", "Recognition Value", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RRESPONSES", "Responses", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RSELFDESCRIPTION", "Self Description", false );
    ovOrlandoTagInfo::Instance->Add(
      "RECEPTION", "RSHEINFLUENCED", "She Influenced", false );
    ovOrlandoTagInfo::Instance->Add(
      "WRITING", "TEXTUALFEATURES", "Textual Features", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TCHARACTERIZATION", "Characterization", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TCHARACTERNAME", "Character Name", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TCHARACTERTYPEROLE", "Character, Type or Role", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TEXT", "Text", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TGENRE", "Genre", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TGENREISSUE", "Genre Issue", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TINTERTEXTUALITY", "Intertextuality", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TMOTIF", "Motif", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TPLOT", "Plot", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TSETTINGDATE", "Setting Date", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TSETTINGPLACE", "Setting Place", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TTECHNIQUES", "Techniques", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TTHEMETOPIC", "Theme or Topic", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TTONESTYLE", "Tone or Style", false );
    ovOrlandoTagInfo::Instance->Add(
      "TEXTUALFEATURES", "TVOICENARRATION", "Voice Narration", false );
    
    // add in the unordered tags
    ovOrlandoTagInfo::Instance->Add(
      "", "General", "General", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "DATE", "Date", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "GEOG", "Geography", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "NAME", "Name", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "ORGNAME", "Organization Name", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "QUOTE", "Quote", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "SCHOLARNOTE", "Scholar Note", false );
    ovOrlandoTagInfo::Instance->Add(
      "General", "TITLE", "Title", false );
  }

  return ovOrlandoTagInfo::Instance;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovOrlandoTagInfo::Add( ovString parent, ovString name, ovString title, bool active )
{
  ovTag *tag = new ovTag( parent, name, title, active );
  this->TagVector.push_back( tag );
  this->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::GetNumberOfTags()
{
  return this->TagVector.size();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovTag* ovOrlandoTagInfo::FindTag( ovString search, bool title )
{
  int index = this->FindTagIndex( search, title );
  return -1 == index ? NULL : this->TagVector.at( index );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoTagInfo::FindTagIndex( ovString search, bool title )
{
  int index = -1; // assume the tag isn't present
  ovTag *tag;
  
  for( ovTagVector::iterator it = this->TagVector.begin(); it != this->TagVector.end(); ++it )
  {
    if( ( title && (*it)->title == search ) || ( !title && (*it)->name == search ) )
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

