#include "ovNode.h"

//using namespace std;

//constructor
ovNode::ovNode()
{
}

//destructor
ovNode::~ovNode()
{
}

//constructor with int k for the key
ovNode::ovNode( int k )
{
  key = k;
  con = true;
  visit = false;
  
}

void ovNode::SetKey( int i )
{
  key = i;
}

//returns whether the node has been visited
bool ovNode::visited()
{
  return visit;
}

//set visited to assigned value b
void ovNode::setVisited( bool b )
{
  visit = b;
}

//check to see if the node is equal
//if equal return true
//else return false
bool ovNode::operator==( const ovNode &a )
{
  if( a.getKey() == key )
    {
      if( a.getX() == x )
  {
    if( a.getY() == y )
      {
        if( a.getZ() == z )
    {
      return true;
    }
      }
  }
    }
  return false;
}

//set the center of the node to i, j, k
void ovNode::setCenter( int i, int j, int k )
{
  x = i;
  y = j;
  z = k;
}
    
//return the x coordinate of the center
int ovNode::getX() const
{
  return x;
}
    
//return the y coordinate of the center
int ovNode::getY() const
{
  return y;
}

//return the z coordinate of the center
int ovNode::getZ() const
{
  return z;
}

//add the child ch to node's children 
void ovNode::addChild( ovEdge ch )
{
  con = true;
  children.push_back( ch );
}

//return the number of children
int ovNode::numOfChildren()
{
  return children.size();
}

//return the key
int ovNode::getKey() const
    {
  return key;
    }

//return the list of children
list<ovEdge> ovNode::getChildren()
{
  return children;
}

//reset the children
void ovNode::resetChildren()
{
  children.clear();
}

//return the number of nodes connected to the node 
int ovNode::connectivity()
{
  if( con )
    {
      return numOfChildren();
    }
  
  return -1;
}

//reset connectivity
void ovNode::resetConnectivity()
{
  con = false;
}

vtkActor* ovNode::getSphereActor()
{
  return vtkAct;
}

list<vtkActor*> ovNode::getEdgeActors()
{
  return vtkActorEdges;
}

void ovNode::setSphereActor( vtkActor* vtkA )
{
  vtkAct = vtkA;
}

void ovNode::resetEdgeActors()
{
  vtkActorEdges.clear();
}

void ovNode::addEdgeActor( vtkActor* vtkA )
{
  vtkActorEdges.push_back( vtkA );
}

void ovNode::removeEdgeActor( vtkActor* vtkA )
{
  vtkActorEdges.remove( vtkA );
}


