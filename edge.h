#ifndef EDGE_H
#define EDGE_H

#include <list>
#include <stdio.h>

#define NUM_OF_TAGS 25

using namespace std;

class Edge
{

public:
  //constructor
  Edge(int nd1, int nd2, list<int> tgs);
  //destructor
  ~Edge();
  int GetNode1();
  int GetNode2();
  bool HasTag(int i);

private:
  //global variables
  int node1;
  int node2;
  bool tags[NUM_OF_TAGS+1];

};

#endif
