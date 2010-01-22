#ifndef NODE_H
#define NODE_H

#include <list>
#include <stdio.h>
#include "vtkActor.h"
#include "edge.h"

using namespace std;

class Node
{

public:
  //constructor
  Node();
  //destructor
  ~Node();
  //constructor with int k as key
  Node(int k);
  void SetKey(int i);
  bool operator==(const Node &a);
  void setCenter(int i,int j, int k);
  int getX() const;
  int getY() const;
  int getZ() const;
  void addChild(Edge e);
  int numOfChildren();
  int getKey() const;
  bool visited();
  void setVisited(bool b);
  list<Edge> getChildren();
  int connectivity();
  void resetConnectivity();
  vtkActor* getSphereActor();
  list<vtkActor*> getEdgeActors();
  void setSphereActor(vtkActor* vtkA);
  void resetEdgeActors();
  void addEdgeActor(vtkActor* vtkA);
  void removeEdgeActor(vtkActor* vtkA);

private:
  //global variables
  list<Edge> children;
  list<vtkActor*> vtkActorEdges;
  vtkActor* vtkAct;
  int x;
  int y;
  int z;
  bool visit;
  int key;
  bool con;
};

#endif
