#include "name.h"

//constructor
Name::Name()
{
}

//destructor
Name::~Name()
{
}

//set key number to i
void Name::setKeyNum(int i)
{
  keyNum = i;
}  

//set key to k
void Name::setKey(char* k)
{
  key = strdup(k);
  clearFileSpots();
}

//add file spot
void Name::addFileSpot(int fs)
{
  fileSpots.push_front(fs);  
}

//check if the name is between ln1 and ln2 (between the tag)
void Name::checkTag(int ln1, int ln2, int tagNum)
{
  list<int>::iterator it;
  
  for(it=fileSpots.begin(); it!=fileSpots.end(); it++)
    {
      if(ln1 >= *it && ln2 <= *it)
	{
	  tags[tagNum] = true;
	}
    }
    
}

//return key num
int Name::getKeyNum()
{
  return keyNum;
}

//return key
char* Name::getKey()
{
  return key;
}

//return filespots
list<int> Name::getFileSpots()
{
  return fileSpots;
}

//return tags
bool* Name::getTags()
{
  return tags;
}

//clear filespots
void Name::clearFileSpots()
{
  fileSpots.clear();
  int i = 0;
  for(int i=0; i<NUM_OF_TAGS_C; i++)
    {
      tags[i] = false;
    }
}
