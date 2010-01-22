#ifndef PERIOD_H
#define PERIOD_H

#include <list>
#include <stdio.h>
#include "date.h"

using namespace std;

class Period
{

public:
  //constructor
  Period(char* p);
  //destructor
  ~Period();
  Date* getStartDate();
  Date* getEndDate();
  char* getTitle();

private:
  //global variables
  Date* startDate;
  Date* endDate;
  char* title;
};

#endif
