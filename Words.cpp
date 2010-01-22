#include <qapplication.h>
#include <qfiledialog.h>
#include "Words.h"
#include "graph.h"

//tag constructor
Words::Words(QWidget* parent)
  : QDialog(parent)
{
  //set up the user interface
  setupUi(this);

  //connect the signals to the new slots
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(wordsOk()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(wordsCancel()));
  connect(pushButton, SIGNAL(pressed()), this, SLOT(wordHelp()));

  connect(comboBox, SIGNAL(activated(int)), this, SLOT(setIndex1(int)));
  connect(comboBox_2, SIGNAL(activated(int)), this, SLOT(setIndex2(int)));

  index1 = -1;
  index2 = -1;

  setUpCombo1();
  setUpCombo2();
};

//set the graph
void Words::setGraph(Graph* g)
{
  graph = g;
}

//if cancel is pressed
void Words::wordsCancel()
{
  //hide this widget
  this->hide();
}

//if ok is pressed
void Words::wordsOk()
{ 
  if(radioButton->isChecked())
    {
      char* str3 = (char*) calloc(100, sizeof(char));
      sprintf(str3, lineEdit_3->text());

      char* str4 = (char*) calloc(100, sizeof(char));
      sprintf(str4, lineEdit_4->text());

      strcat(str3, "--");
      strcat(str4, "--");

      Date* dt1 = new Date(str3, true);
      Date* dt2 = new Date(str4, false);

      checkDates(dt1, dt2);

      free(str3);
      free(str4);
    }
  else if(radioButton_2->isChecked())
    {
      char* str3 = (char*) calloc(1000,sizeof(char));
      sprintf(str3, comboBox->itemText(index1));

      list<Period*>::iterator it;
      for(it=historical.begin(); it!=historical.end(); it++)
	{
	  if(strcmp((*it)->getTitle(), str3) == 0 )
	    {
	      checkDates((*it)->getStartDate(),(*it)->getEndDate());
	    }
	}

      free(str3);
    }
  else if(radioButton_3->isChecked())
    {
      char* str3 = (char*) calloc(1000,sizeof(char));
      sprintf(str3, comboBox_2->itemText(index2));
      
      list<Period*>::iterator it;
      for(it=monarch.begin(); it!=monarch.end(); it++)
	{
	  if(strcmp((*it)->getTitle(), str3) == 0 )
	    {
	      checkDates((*it)->getStartDate(),(*it)->getEndDate());
	    }
	}

      free(str3);
    }

  //get text from the line edit boxes
  char* str1 = (char*) calloc(1000, sizeof(char));
  sprintf(str1, lineEdit->text());

  char* str2 = (char*) calloc(1000, sizeof(char));
  sprintf(str2, lineEdit_2->text());

  if(str1 != NULL || str2 != NULL)
    {
      graph->include(str1);
      graph->exclude(str2);
    }

  free(str1);
  free(str2);

  //hide this widget
  this->hide();
}

void Words::wordHelp()
{
  WordHelp wHelp;

  wHelp.exec();
}

void Words::setUpCombo1()
{
  getHistorical("Resources/historical_dates.txt");

  list<Period*>::iterator it;
  for(it=historical.begin(); it!=historical.end(); it++)
    {
      comboBox->addItem((*it)->getTitle());
    }
}

void Words::setUpCombo2()
{
  getMonarch("Resources/monarchs_dates.txt");
  
  list<Period*>::iterator it;
  for(it=monarch.begin(); it!=monarch.end(); it++)
    {
      comboBox_2->addItem((*it)->getTitle());
    }
}

void Words::checkDates(Date* dt1, Date* dt2)
{

}

void Words::getHistorical(char* filename)
{
  ifstream file;

  file.open(filename, ios::in);

  if(file.is_open())
    {
      char* line = (char*) calloc(1000,sizeof(char));
      file.getline(line, 1000);
      while(!file.eof() && line!=NULL)
	{	  
	  historical.push_back(new Period(line));

	  free(line);
	  line = (char*) calloc(1000,sizeof(char));
	  file.getline(line, 1000);
	}
    }
}

void Words::getMonarch(char* filename)
{
  ifstream file;

  file.open(filename, ios::in);

  if(file.is_open())
    {
      char* line = (char*) calloc(1000,sizeof(char));
      file.getline(line, 1000);
      while(!file.eof() && line!=NULL)
	{	  
	  monarch.push_back(new Period(line));

	  free(line);
	  line = (char*) calloc(1000,sizeof(char));
	  file.getline(line, 1000);
	}
    }
}

void Words::setIndex1(int i)
{
  index1 = i;
}

void Words::setIndex2(int i)
{
  index2 = i;
}
