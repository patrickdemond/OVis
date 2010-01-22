#include "node.h"

//using namespace std;

//constructor
Node::Node()
{
}

//destructor
Node::~Node()
{
}

//constructor with int k for the key
Node::Node(int k)
{
  key = k;
  con = true;
  visit = false;
  
}

void Node::SetKey(int i)
{
  key = i;
}

//returns whether the node has been visited
bool Node::visited()
{
  return visit;
}

//set visited to assigned value b
void Node::setVisited(bool b)
{
  visit = b;
}

//check to see if the node is equal
//if equal return true
//else return false
bool Node::operator==(const Node &a)
{
  if(a.getKey() == key)
    {
      if( a.getX() == x)
	{
	  if(a.getY() == y)
	    {
	      if(a.getZ() == z)
		{
		  return true;
		}
	    }
	}
    }
  return false;
}

//set the center of the node to i,j,k
void Node::setCenter( int i, int j, int k)
{
  x = i;
  y = j;
  z = k;
}
    
//return the x coordinate of the center
int Node::getX() const
{
  return x;
}
    
//return the y coordinate of the center
int Node::getY() const
{
  return y;
}

//return the z coordinate of the center
int Node::getZ() const
{
  return z;
}

//add the child ch to node's children 
void Node::addChild(Edge ch)
{
  con = true;
  children.push_back(ch);
}

//return the number of children
int Node::numOfChildren()
{
  return children.size();
}

//return the key
int Node::getKey() const
    {
	return key;
    }

//return the list of children
list<Edge> Node::getChildren()
{
  return children;
}


//return the number of nodes connected to the node 
int Node::connectivity()
{
  if(con)
    {
      return numOfChildren();
    }
  
  return -1;
}

//reset connectivity
void Node::resetConnectivity()
{
  con = false;
}

vtkActor* Node::getSphereActor()
{
  return vtkAct;
}

list<vtkActor*> Node::getEdgeActors()
{
  return vtkActorEdges;
}

void Node::setSphereActor(vtkActor* vtkA)
{
  vtkAct = vtkA;
}

void Node::resetEdgeActors()
{
  vtkActorEdges.clear();
}

void Node::addEdgeActor(vtkActor* vtkA)
{
  vtkActorEdges.push_back(vtkA);
}

void Node::removeEdgeActor(vtkActor* vtkA)
{
  vtkActorEdges.remove(vtkA);
}


