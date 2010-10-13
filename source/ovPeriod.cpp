#include "ovPeriod.h"

//using namespace std;

//constructor
ovPeriod::ovPeriod( char* p )
{
  printf( "\n\n\n" );
  fflush( stdout );

  strtok( p, "\"" );
  char* str1 = strtok( NULL, "\"" ); 
  strtok( NULL, "\"" );

  char* str2 = strtok( NULL, "\">" );

  char* str3 = strtok( NULL, "><" );

  title = strdup( str3 );

  /*printf( "title: " );
  printf( str3 );
  printf( title );
  fflush( stdout );*/

  startDate = new ovDate( str1, true );
  endDate = new ovDate( str2, false );
}

//destructor
ovPeriod::~ovPeriod()
{
  free( title );
}

ovDate* ovPeriod::getStartDate()
{
  return startDate;
}

ovDate* ovPeriod::getEndDate()
{
  return endDate;
}

char* ovPeriod::getTitle()
{
  return title;
}
