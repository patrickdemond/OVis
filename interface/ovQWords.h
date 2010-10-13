#ifndef PATH_H
#define PATH_H

#include "ui_ovQWords.h"
#include <iostream>
#include "QVTKWidget.h"
#include "ovQWordHelp.h"

#include "source/ovGraph.h"
#include "source/ovPeriod.h"
#include "source/ovDate.h"

//dialog for tag selection
class ovQWords : public QDialog, private Ui_WordsDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  ovQWords( ovGraph* g, QWidget* parent = 0 );
  //destructor
  ~ovQWords(){};

  //functions
  QWidget* getParent();
  void checkDates( ovDate* dt1, ovDate* dt2 );
  void setUpCombo1();
  void setUpCombo2();
  void getHistorical( char* filename );
  void getMonarch( char* filename );

public slots:

  //slots for ok and cancel
  virtual void wordsCancel();
  virtual void wordsOk();
  virtual void wordHelp();
  virtual void setIndex1( int i );
  virtual void setIndex2( int i );

protected:

protected slots:

private:

  //global variables
  ovGraph* graph;
  int index1;
  int index2;
  list<ovPeriod*> historical;
  list<ovPeriod*> monarch;
};

#endif
