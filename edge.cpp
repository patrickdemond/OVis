#include "edge.h"

//using namespace std;

//empty constructor
Edge::Edge()
{
  node1 = -1;
  node2 = -1;
}

//constructor
Edge::Edge(int nd1, int nd2, list<int> tgs)
{
  node1 = nd1;
  node2 = nd2;
  
  tagList=tgs;

  for(int i=0; i<NUM_OF_TAGS+1; i++)
    {
      tags[i] = false;
    }
  
  list<int>::iterator it;
  for(it=tgs.begin(); it!=tgs.end(); it++)
    {
      printf("Tag on: %i.    ", *it);
      fflush(stdout);
      tags[*it] = true;
    }
}

//destructor
Edge::~Edge()
{
  
}

void Edge::SetNode1(int n1)
{
  node1 = n1;
}

void Edge::SetNode2(int n2)
{
  node2 = n2;
}

void Edge::AddTags(list<int> tgs)
{
  list<int>::iterator it;
  for(it=tgs.begin(); it!=tgs.end(); it++)
    {
      AddTag(*it);
    }
}

void Edge::AddTag(int i)
{
  tagList.push_back(i);
  tagList.unique();
  tags[i] = true;
}

int Edge::GetNode1()
{
  return node1;
}

int Edge::GetNode2()
{
  return node2;
}

bool Edge::HasTag(int i)
{
  return tags[i];
}

list<int> Edge::GetTags()
{
  return tagList;
}



