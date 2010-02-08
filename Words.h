#ifndef PATH_H
#define PATH_H

#include "ui_words.h"
#include <iostream>
#include "QVTKWidget.h"
#include "graph.h"
#include "WordHelp.h"
#include "period.h"
#include "date.h"

//dialog for tag selection
class Words : public QDialog, private Ui_WordsDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  Words(Graph* g,QWidget* parent = 0);
  //destructor
  ~Words(){};

  //functions
  QWidget* getParent();
  void checkDates(Date* dt1, Date* dt2);
  void setUpCombo1();
  void setUpCombo2();
  void getHistorical(char* filename);
  void getMonarch(char* filename);

public slots:

  //slots for ok and cancel
  virtual void wordsCancel();
  virtual void wordsOk();
  virtual void wordHelp();
  virtual void setIndex1(int i);
  virtual void setIndex2(int i);

protected:

protected slots:

private:

  //global variables
  Graph* graph;
  int index1;
  int index2;
  list<Period*> historical;
  list<Period*> monarch;
};

#endif
