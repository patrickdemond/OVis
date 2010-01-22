#ifndef PATH_H
#define PATH_H

#include "ui_path.h"
#include "QVTKWidget.h"
#include "graph.h"

//dialog for tag selection
class Path : public QDialog, private Ui_PathDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  Path(QWidget* parent = 0);
  //destructor
  ~Path(){};

  //functions
  QWidget* getParent();
  QComboBox* getCombo1();
  QComboBox* getCombo2();
  void setGraph(Graph* g);

public slots:

  //slots for ok and cancel
  virtual void pathCancel();
  virtual void pathOk();
  virtual void setIndex1(int ind1);
  virtual void setIndex2(int ind2);

protected:

protected slots:

private:

  //global variables
  Graph* graph;
  int index1;
  int index2;  
  QWidget* par;
};

#endif
