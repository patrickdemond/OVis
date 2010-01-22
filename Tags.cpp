#include <qapplication.h>
#include <qfiledialog.h>
#include "Tags.h"
#include "graph.h"

//tag constructor
Tags::Tags(QWidget* parent)
  : QDialog(parent)
{
  //set up the user interface
  setupUi(this);

  //connect the signals to the new slots
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(tagOk()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(tagCancel()));

  //turn on extended selection for the list
  listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  
};

//return tag list widget
QListWidget* Tags::getTagList()
{
  return listWidget;
}

//set the graph
void Tags::setGraph(Graph* g)
{
  graph = g;
}

//return true if continue is on, false otherwise
bool Tags::getContinue()
{
  return continueTrue;
}

//if cancel is pressed
void Tags::tagCancel()
{
  //set continue to false
  continueTrue = false;

  //hide this widget
  this->hide();
}

//if ok is pressed
void Tags::tagOk()
{  
  //get selected items from the list
  QList<QListWidgetItem*> tgs = listWidget->selectedItems();
  
  //list<char*> to hold the tags that are turned on
  list<char*> strs;

  //iterator to go through list
  QList<QListWidgetItem*>::iterator it;

  //for the selected items in the list
  for(it=tgs.begin(); it!=tgs.end(); it++)
    {
      //char* to hold item
      char* str = (char*) calloc(1000, sizeof(char));
      //print text of item to string
      sprintf(str, (*it)->text());
      //push string into list
      strs.push_back(str);   
    }

  //set tags to the strings
  graph->setTags(strs);

  //set continue to true
  continueTrue = true;
  
  //hide this widget
  this->hide();
}
