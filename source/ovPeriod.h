#ifndef PERIOD_H
#define PERIOD_H

#include <list>
#include <stdio.h>
#include "ovDate.h"

using namespace std;

class ovPeriod
{

public:
  //constructor
  ovPeriod( char* p );
  //destructor
  ~ovPeriod();
  ovDate* getStartDate();
  ovDate* getEndDate();
  char* getTitle();

private:
  ovDate* startDate;
  ovDate* endDate;
  char* title;
};

#endif
