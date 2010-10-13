#ifndef EDGE_H
#define EDGE_H

#include <list>
#include <stdio.h>

#define NUM_OF_TAGS_C 1000

using namespace std;

class Edge
{

public:
  //constructor
  Edge();
  Edge(int nd1, int nd2, list<int> tgs);
  //destructor
  ~Edge();
  void SetNode1(int n1);
  void SetNode2(int n2);
  int GetNode1();
  int GetNode2();
  bool HasTag(int i);
  list<int> GetTags();
  void AddTags(list<int> tgs);
  void AddTag(int i);

private:
  //global variables
  int node1;
  int node2;
  bool tags[NUM_OF_TAGS_C+1];
  list<int> tagList;

};

#endif
