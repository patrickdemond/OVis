#ifndef NAME_H
#define NAME_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <list>

#define NUM_OF_TAGS_C 1000

using namespace std;

class Name
{
 private:
  //global variables
  char* key;

 public: 
  //constructor
  Name();
  
  //destructor
  ~Name();

  void setKeyNum(int i);
  void setKey(char* k);
  void addFileSpot(int fs);
  void checkTag(int ln1, int ln2, int tagNum);
  int getKeyNum();
  char* getKey();
  list<int> getFileSpots();
  bool* getTags();
  void clearFileSpots();
  int keyNum;
  list<int> fileSpots;
  bool tags[NUM_OF_TAGS_C]; 
};

#endif
