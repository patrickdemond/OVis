#ifndef DATE_H
#define DATE_H

#include <list>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

class Date
{

public:
  //constructors
  Date(int yr, int mnth, int dy);
  Date(char* dt, bool start);
  //destructor
  ~Date();
  bool lessThan(Date* dt);
  bool greaterThan(Date* dt);
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
