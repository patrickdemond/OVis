#include <qapplication.h>
#include <qfiledialog.h>
#include "Path.h"
#include "graph.h"

//tag constructor
Path::Path(QWidget* parent)
  : QDialog(parent)
{
  //set up the user interface
  setupUi(this);

  //connect the signals to the new slots
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(pathOk()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(pathCancel()));  
  connect(comboBox, SIGNAL(activated(int)), this, SLOT(setIndex1(int)));
  connect(comboBox_2, SIGNAL(activated(int)), this, SLOT(setIndex2(int)));

  par = this;
  
  index1 = 0;
  index2 = 0;
};

QWidget* Path::getParent()
{
  return this;
}

//return the first combo box
QComboBox* Path::getCombo1()
{
  return comboBox;
}
 
//return the second combo box
QComboBox* Path::getCombo2()
{
  return comboBox_2;
}

//set the graph
void Path::setGraph(Graph* g)
{
  graph = g;
}

//if cancel is pressed
void Path::pathCancel()
{
  //hide this widget
  this->hide();
}

//if ok is pressed
void Path::pathOk()
{  
  //get selected items from the combo boxes
  char* str1 = (char*) calloc(1000, sizeof(char));
  sprintf(str1, comboBox->itemText(index1));

  char* str2 = (char*) calloc(1000, sizeof(char));
  sprintf(str2, comboBox_2->itemText(index2));

  if(str1 != NULL && str2 != NULL)
    {
      graph->findPathBtw(str1, str2);
    }

  free(str1);
  free(str2);

  //hide this widget
  this->hide();
}

void Path::setIndex1(int ind1)
{
  index1 = ind1;
}

void Path::setIndex2(int ind2)
{
  index2 = ind2;
}
