#include "edge.h"

//using namespace std;

//constructor
Edge::Edge(int nd1, int nd2, list<int> tgs)
{
  node1 = nd1;
  node2 = nd2;

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




