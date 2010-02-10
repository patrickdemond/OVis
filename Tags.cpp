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
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changeTagCol(QListWidgetItem*)));
  connect(toolButton, SIGNAL(pressed()), this, SLOT(loadCols()));
  connect(toolButton_2, SIGNAL(pressed()), this, SLOT(saveCols()));

  //turn on extended selection for the list
  listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
};

void Tags::changeTagCol(QListWidgetItem* tagToChange)
{
  int index = listWidget->row(tagToChange);

  double* oldCol = graph->getColor(index);
  
  QColor* tagCol = new QColor();
  tagCol->setRed((int)(oldCol[0]*255));
  tagCol->setGreen((int)(oldCol[1]*255));
  tagCol->setBlue((int)(oldCol[2]*255));
  
  QColor col = QColorDialog::getColor(*tagCol, this);

  if(col.red() != col.blue() != col.green() != 0)
    {
      graph->setNewTagCol(index, col.red(), col.green(), col.blue());
      tagToChange->setBackground(col);
    }
}

void Tags::loadCols()
{
  QString str = QFileDialog::getOpenFileName(this, tr("Open File"), "Resources", tr("Tag Colour Files (*.tagCols)"));

  if(str != "")
    {
      char* fname = (char*) calloc(1000, sizeof(char));
      sprintf(fname, str);
      graph->setDefaultColors(fname);
    }

  for(int i=0; i<listWidget->count(); i++)
    {
      double* col = graph->getColor(i);
  
      QColor* tagCol = new QColor();
      tagCol->setRed((int)(col[0]*255));
      tagCol->setGreen((int)(col[1]*255));
      tagCol->setBlue((int)(col[2]*255));

      (listWidget->item(i))->setBackground(*tagCol);
    }
}

void Tags::saveCols()
{
  QString str = QFileDialog::getSaveFileName(this, tr("Save File"), "Resources", tr("Tag Colour Files (*.tagCols)"));

  if(str != "")
    {
      char* fname = (char*) calloc(1000, sizeof(char));
      sprintf(fname, str);
      if(strstr(fname, ".tagCols") == NULL)
	{
	  strcat(fname, ".tagCols");
	}

      graph->saveTagColors(fname);
    }
}

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
