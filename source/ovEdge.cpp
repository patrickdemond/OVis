#include "edge.h"

//using namespace std;

//empty constructor
Edge::Edge()
{
  node1 = -1;
  node2 = -1;

  for(int i=0; i<NUM_OF_TAGS_C+1; i++)
    {
      tags[i] = false;
    }
  
}

//constructor
Edge::Edge(int nd1, int nd2, list<int> tgs)
{
  node1 = nd1;
  node2 = nd2;
  
  tagList=tgs;

  for(int i=0; i<NUM_OF_TAGS_C+1; i++)
    {
      tags[i] = false;
    }
  
  list<int>::iterator it;
  for(it=tgs.begin(); it!=tgs.end(); it++)
    {
      //printf("Tag on: %i.    ", *it);
      //fflush(stdout);
      tags[*it] = true;
    }
}

//destructor
Edge::~Edge()
{
  
}

//set the node of one of the vertices
void Edge::SetNode1(int n1)
{
  node1 = n1;
}

//set the node of the other vertex
void Edge::SetNode2(int n2)
{
  node2 = n2;
}

//add the list of tags for the edge
void Edge::AddTags(list<int> tgs)
{
  list<int>::iterator it;
  for(it=tgs.begin(); it!=tgs.end(); it++)
    {
      AddTag(*it);
    }
}

//add the tag
void Edge::AddTag(int i)
{
  tagList.push_back(i);
  tagList.unique();
  tags[i] = true;
}

//get one of the vertices
int Edge::GetNode1()
{
  return node1;
}

//get the other vertex
int Edge::GetNode2()
{
  return node2;
}

//return true if the edge has the tag i turned on
bool Edge::HasTag(int i)
{
  return tags[i];
}

//gets the list of all the tags turned on in the edge
list<int> Edge::GetTags()
{
  return tagList;
}



