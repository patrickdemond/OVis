#include "ovEdge.h"

//using namespace std;

//empty constructor
ovEdge::ovEdge()
{
  node1 = -1;
  node2 = -1;

  for( int i=0; i<NUM_OF_TAGS_C+1; i++ )
    {
      tags[i] = false;
    }
  
}

//constructor
ovEdge::ovEdge( int nd1, int nd2, list<int> tgs )
{
  node1 = nd1;
  node2 = nd2;
  
  tagList=tgs;

  for( int i=0; i<NUM_OF_TAGS_C+1; i++ )
    {
      tags[i] = false;
    }
  
  list<int>::iterator it;
  for( it=tgs.begin(); it!=tgs.end(); it++ )
    {
      //printf( "Tag on: %i.    ", *it );
      //fflush( stdout );
      tags[*it] = true;
    }
}

//destructor
ovEdge::~ovEdge()
{
  
}

//set the node of one of the vertices
void ovEdge::SetNode1( int n1 )
{
  node1 = n1;
}

//set the node of the other vertex
void ovEdge::SetNode2( int n2 )
{
  node2 = n2;
}

//add the list of tags for the edge
void ovEdge::AddTags( list<int> tgs )
{
  list<int>::iterator it;
  for( it=tgs.begin(); it!=tgs.end(); it++ )
    {
      AddTag( *it );
    }
}

//add the tag
void ovEdge::AddTag( int i )
{
  tagList.push_back( i );
  tagList.unique();
  tags[i] = true;
}

//get one of the vertices
int ovEdge::GetNode1()
{
  return node1;
}

//get the other vertex
int ovEdge::GetNode2()
{
  return node2;
}

//return true if the edge has the tag i turned on
bool ovEdge::HasTag( int i )
{
  return tags[i];
}

//gets the list of all the tags turned on in the edge
list<int> ovEdge::GetTags()
{
  return tagList;
}



