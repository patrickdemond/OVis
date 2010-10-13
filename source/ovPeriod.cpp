#include "period.h"

//using namespace std;

//constructor
Period::Period(char* p)
{
  printf("\n\n\n");
  fflush(stdout);

  strtok(p, "\"");
  char* str1 = strtok(NULL, "\""); 
  strtok(NULL, "\"");

  char* str2 = strtok(NULL, "\">");

  char* str3 = strtok(NULL, "><");

  title = strdup(str3);

  /*printf("title: ");
  printf(str3);
  printf(title);
  fflush(stdout);*/

  startDate = new Date(str1, true);
  endDate = new Date(str2, false);
}

//destructor
Period::~Period()
{
  free(title);
}

Date* Period::getStartDate()
{
  return startDate;
}

Date* Period::getEndDate()
{
  return endDate;
}

char* Period::getTitle()
{
  return title;
}
