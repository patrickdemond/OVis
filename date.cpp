#include "date.h"

using namespace std;

//constructor
Date::Date(int yr, int mnth, int dy)
{
  year = yr;
  month = mnth;
  day = dy;
}

//constructor
Date::Date(char* dt, bool start)
{
  /*printf("starting date set up");
  fflush(stdout);

  printf(dt);
  fflush(stdout);
  */
  char* s1 = strtok(dt, "-");
  if(s1 != NULL)
    {
      int tempInt[strlen(s1)];

      for(int i=0; i<strlen(s1); i++)
	{
	  tempInt[i] = atoi(&(s1[i]));
	}

      year = *tempInt;

      //printf("year: %i ", year);
      //fflush(stdout);
    }
  
  char* s2 = strtok(NULL, "-");     

  if(s2 != NULL)
    {
      if(strlen(s2) < 5)
	{

	  int tempInt[strlen(s2)];
	  
	  for(int i=0; i<strlen(s2); i++)
	    {
	      tempInt[i] = atoi(&(s2[i]));
	    }
	  
	  month = *tempInt;
	  //printf("month: %i ", month);
	  //fflush(stdout);
	}
      else
	{
	  if(start)
	    {
	      month = 0;
	    } 
	  else
	    {
	      month = 12;
	    }
	}
    }

  char* s3 = strtok(NULL, "\0");
  if(s3 != NULL)
    {
      int tempInt[strlen(s3)];

      for(int i=0; i<strlen(s3); i++)
	{
	  tempInt[i] = atoi(&(s3[i]));
	}

      day = *tempInt;
      //printf("day: %i ", day);
      //fflush(stdout);
    }
  else
    {
      if(start)
	{
	  day = 0;
	} 
      else
	{
	  day = 31;
	}
    }

  //printf("done date setup");
  //fflush(stdout);
}

//destructor
Date::~Date()
{
  
}

//check if the date is less than the date passed in
bool Date::lessThan(Date* dt)
{
  if(year<dt->GetYear())
    {
      return true;
    }
  else if(year==dt->GetYear())
    {
      if(month<dt->GetMonth())
	{
	  return true;
	}
	else if(month==dt->GetMonth())
	  {
	    if(day<=dt->GetDay())
	      {
		return true;
	      }
	  }
    }
	
  return false;
}

//check if the date is greater than the date passed in
bool Date::greaterThan(Date* dt)
{
  if(year>dt->GetYear())
    {
      return true;
    }
  else if(year==dt->GetYear())
    {
      if(month>dt->GetMonth())
	{
	  return true;
	}
	else if(month==dt->GetMonth())
	  {
	    if(day>=dt->GetDay())
	      {
		return true;
	      }
	  }
    }
	
  return false;
}

//get the year
int Date::GetYear()
{
  return year;
}

//get the month
int Date::GetMonth()
{
  return month;
}

//get the day
int Date::GetDay()
{
  return day;
}



