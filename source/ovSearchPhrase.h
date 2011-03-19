/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSearchPhrase.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .NAME ovSearchPhrase - Contains compound search terms.
//
// .SECTION Description
//
// .SECTION See Also
// 

#ifndef __ovSearchPhrase_h
#define __ovSearchPhrase_h

#include "vtkObject.h"

#include "ovUtilities.h"
#include "vtkSmartPointer.h"

class ovSearchPhrase : public vtkObject
{
// we need to friend the smart pointer class so that it has access to ::New()
friend class vtkSmartPointer< ovSearchPhrase >;
public:
  // Description:
  // This method returns the one and only instantiation of the class, use this
  // method to get an object from this class
  static ovSearchPhrase *New();
  vtkTypeRevisionMacro( ovSearchPhrase, vtkObject );
  void PrintSelf( ostream& os, vtkIndent indent );
  
  // Description:
  // Build search phrase based on the string
  virtual void Parse( ovString phrase );
  
  // Description:
  // Gets the string representation of the search phrase
  virtual ovString ToString() const;

  // Description:
  // Empties the phrase of all search terms
  virtual void Clear();

  // Description:
  // Add a search term.
  virtual void Add( const ovSearchTerm &term );

  // Description:
  // Returns a reference to the vector of search terms
  virtual ovSearchTermVector* GetSearchTerms() { return &( this->SearchTermVector ); }
  
  // Description:
  // Returns a reference to the vector of search terms
  virtual int GetNumberOfSearchTerms() { return this->SearchTermVector.size(); }
  
  // Description:
  // Returns whether the search term is a match for the string
  // The first string is the raw content, the second is the stemmed content
  virtual bool Find( ovString content, ovString stemmedContent ) const;

protected:
  ovSearchPhrase();
  ~ovSearchPhrase();
  
  ovSearchTermVector SearchTermVector;

private:
  ovSearchPhrase( const ovSearchPhrase& );  // Not implemented.
  void operator=( const ovSearchPhrase& );  // Not implemented.
};

#endif
