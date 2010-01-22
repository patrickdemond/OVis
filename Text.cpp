#include <qapplication.h>
#include <qfiledialog.h>
#include "Text.h"

//tag constructor
Text::Text(QWidget* parent)
  : QDialog(parent)
{
  //set up the user interface
  setupUi(this);

  //connect the signals to the new slots
  connect(pushButton, SIGNAL(pressed()), this, SLOT(showText()));
  connect(pushButton_2, SIGNAL(pressed()), this, SLOT(done()));
  connect(comboBox, SIGNAL(activated(QString)), this, SLOT(entrySel(QString)));
  connect(comboBox_2, SIGNAL(activated(QString)), this, SLOT(nameSel(QString)));
  connect(comboBox_3, SIGNAL(activated(QString)), this, SLOT(tagSel(QString)));
};

//set the graph
void Text::setGraph(Graph* g)
{
  graph = g;
}

QComboBox* Text::getCombo1()
{
  return comboBox;
}

QComboBox* Text::getCombo2()
{
  return comboBox_2;
}

QComboBox* Text::getCombo3()
{
  return comboBox_3;
}

void Text::showText()
{
  QString str = comboBox->currentText();
  
  char* st = (char*) calloc(1000,sizeof(char));
  sprintf(st, str);
  
  if(st != NULL)
    {
      entry = (char*) calloc(1000,sizeof(char));
      entry = st;
    }

  QString str1 = comboBox_2->currentText();
  
  char* st1 = (char*) calloc(1000,sizeof(char));
  sprintf(st1, str1);
  
  if(st1 != NULL)
    {
      name = (char*) calloc(1000,sizeof(char));
      name = st1;
    }

  QString str2 = comboBox_3->currentText();
  
  char* st2 = (char*) calloc(1000,sizeof(char));
  sprintf(st1, str1);
  
  if(st2 != NULL)
    {
      tag = (char*) calloc(1000,sizeof(char));
      tag = st2;
    }

  graph->showXMLEntry(entry, name, tag, textBrowser);
}

void Text::entrySel(QString str)
{
  char* st = (char*) calloc(1000,sizeof(char));
  sprintf(st, str); 

  if(st != NULL)
    {
      graph->namesFromEntry(st, comboBox_2);
      
      entry = (char*) calloc(1000,sizeof(char));
      entry = st;
    }
}

void Text::nameSel(QString str)
{
  char* st = (char*) calloc(1000,sizeof(char));
  sprintf(st, str);
  
  if(st != NULL)
    {
      graph->tagsFromNameEntry(entry,st,comboBox_3);
      
      name = (char*) calloc(1000,sizeof(char));
      name = st;
    }
}

void Text::tagSel(QString str)
{ 
  char* st = (char*) calloc(1000,sizeof(char));
  sprintf(st, str);

  if(st != NULL)
    {
      tag = (char*) calloc(1000,sizeof(char));
      tag = st;
    }
}

void Text::done()
{
  this->hide();
}
 
