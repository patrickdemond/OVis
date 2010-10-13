#ifndef DATE_H
#define DATE_H

#include <list>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

class ovDate
{

public:
  //constructors
  ovDate( int yr, int mnth, int dy );
  ovDate( char* dt, bool start );
  //destructor
  ~ovDate();
  bool lessThan( ovDate* dt );
  bool greaterThan( ovDate* dt );
  int GetYear();
  int GetMonth();
  int GetDay();

private:
  //global variables
  int year;
  int month;
  int day;

};

#endif
