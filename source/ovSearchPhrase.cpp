/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSearchPhrase.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSearchPhrase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkVariant.h"

#include <vtkstd/algorithm>
#include <vtkstd/stdexcept>
#include <vtksys/RegularExpression.hxx>
#include <vtksys/SystemTools.hxx>

vtkCxxRevisionMacro( ovSearchPhrase, "$Revision: $" );
vtkStandardNewMacro( ovSearchPhrase );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSearchPhrase::ovSearchPhrase()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSearchPhrase::~ovSearchPhrase()
{
  this->Clear();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSearchPhrase::Parse( ovString phrase )
{
  vtksys::RegularExpression re( "\"?[^\" ]+\"?" );
  
  ovSearchTerm term;
  while( re.find( phrase ) )
  {
    ovString match = re.match( 0 );
    if( "NOT" == match )
    {
      term.notLogic = true;
    }
    else if( "AND" == match )
    {
      term.andLogic = true;
    }
    else if( "OR" == match )
    {
      term.andLogic = false;
    }
    else if( "STEM" == match )
    {
      term.stemming = true;
    }
    else if( "\"" == match.substr( 0, 1 ) && "\"" == match.substr( match.length() - 1 ) )
    {
      // this is the search itself, so time to add it to the phrase
      term.term = match.substr( 1, match.length() - 2 );
      this->Add( term );
      
      // reset the term
      term.Clear();
    }
    
    // cut out the matched string from the phrase so we can find the next
    phrase = phrase.substr( re.end() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovString ovSearchPhrase::ToString( const bool natural ) const
{
  ovString phrase;
  ovSearchTerm *term;
  ovSearchTermVector::const_iterator it;
  for( it = this->SearchTermVector.begin(); it != this->SearchTermVector.end(); ++it )
  {
    term = *it;
    if( it != this->SearchTermVector.begin() )
    { // add the AND/OR connector to terms after the first
      phrase += natural
              ? term->andLogic ? " and" : " or"
              : term->andLogic ? " AND" : " OR";
    }

    // add the NOT term, if needed
    if( term->notLogic ) phrase += natural ? " not" : " NOT";

    // add the STEM term, if needed
    if( !natural && term->stemming ) phrase += " STEM";

    // add the search term, stripped of double-quotes
    ovString search = term->term;
    search.erase( vtkstd::remove( search.begin(), search.end(), '"' ), search.end() );
    phrase += " \"";
    phrase += search;
    phrase += '"';

    if( natural && term->stemming ) phrase += " (stemmed)";
  }

  // get rid of the space at the beginning of the string
  if( 1 < phrase.length() ) phrase = phrase.substr( 1 );
  return phrase;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSearchPhrase::Clear()
{
  // delete all terms
  ovSearchTermVector::iterator it;
  vtkstd::for_each( this->SearchTermVector.begin(), this->SearchTermVector.end(), safe_delete() );
  this->SearchTermVector.clear();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSearchPhrase::Add( const ovSearchTerm &term )
{
  ovSearchTerm *newTerm = new ovSearchTerm();
  newTerm->copy( term );
  this->SearchTermVector.push_back( newTerm );
  this->Modified();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
bool ovSearchPhrase::Find( ovString content, ovString stemmedContent ) const
{
  if( 0 == content.length() ) return false;
  
  // make search case-insensitive
  content = vtksys::SystemTools::LowerCase( content );

  // loop through every search term until we find a match
  bool first = true, found = false, match = false;
  ovSearchTerm *term;
  ovSearchTermVector::const_iterator it;
  for( it = this->SearchTermVector.begin(); it != this->SearchTermVector.end(); ++it )
  {
    term = *it;

    // if the previous term is a match and we're OR-ing the next term then we're done
    if( true == match && !term->andLogic ) break;
    
    found = vtkstd::string::npos != content.find( vtksys::SystemTools::LowerCase( term->term ) );
    if( term->stemming && !found )
      found = vtkstd::string::npos != stemmedContent.find(
        vtksys::SystemTools::LowerCase( term->term ) );

    bool thisMatch = ( found && !term->notLogic ) || ( !found && term->notLogic );
    match = first // if this is the first term
          ? thisMatch // just set match to whatever this match is
          : term->andLogic // if not, then look for what type of logic we should use
          ? match && thisMatch // and the current and this match
          : match || thisMatch; // or the current and this match
    first = false;
  }

  return match;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSearchPhrase::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  os << indent << "SearchTermVector: " << &this->SearchTermVector << endl;
  os << indent << "Number of search terms: " << this->SearchTermVector.size() << endl;
  os << indent << "Search phrase: " << this->ToString() << endl;
}

