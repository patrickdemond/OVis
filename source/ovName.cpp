#include "ovName.h"

//constructor
ovName::ovName()
{
}

//destructor
ovName::~ovName()
{
}

//set key number to i
void ovName::setKeyNum( int i )
{
  keyNum = i;
}  

//set key to k
void ovName::setKey( char* k )
{
  key = strdup( k );
  clearFileSpots();
}

//add file spot
void ovName::addFileSpot( int fs )
{
  fileSpots.push_front( fs );  
}

//check if the name is between ln1 and ln2 ( between the tag )
void ovName::checkTag( int ln1, int ln2, int tagNum )
{
  list<int>::iterator it;
  
  for( it=fileSpots.begin(); it!=fileSpots.end(); it++ )
    {
      if( ln1 >= *it && ln2 <= *it )
  {
    tags[tagNum] = true;
  }
    }
    
}

//return key num
int ovName::getKeyNum()
{
  return keyNum;
}

//return key
char* ovName::getKey()
{
  return key;
}

//return filespots
list<int> ovName::getFileSpots()
{
  return fileSpots;
}

//return tags
bool* ovName::getTags()
{
  return tags;
}

//clear filespots
void ovName::clearFileSpots()
{
  fileSpots.clear();
  int i = 0;
  for( int i=0; i<NUM_OF_TAGS_C; i++ )
    {
      tags[i] = false;
    }
}
