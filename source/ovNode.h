#ifndef NODE_H
#define NODE_H

#include <list>
#include <stdio.h>
#include "vtkActor.h"
#include "ovEdge.h"

using namespace std;

class ovNode
{

public:
  //constructor
  ovNode();
  //destructor
  ~ovNode();
  //constructor with int k as key
  ovNode( int k );
  void SetKey( int i );
  bool operator==( const ovNode &a );
  void setCenter( int i, int j, int k );
  int getX() const;
  int getY() const;
  int getZ() const;
  void addChild( ovEdge e );
  int numOfChildren();
  int getKey() const;
  bool visited();
  void setVisited( bool b );
  list<ovEdge> getChildren();
  void resetChildren();
  int connectivity();
  void resetConnectivity();
  vtkActor* getSphereActor();
  list<vtkActor*> getEdgeActors();
  void setSphereActor( vtkActor* vtkA );
  void resetEdgeActors();
  void addEdgeActor( vtkActor* vtkA );
  void removeEdgeActor( vtkActor* vtkA );

private:
  //global variables
  list<ovEdge> children;
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
