#ifndef TAGS_H
#define TAGS_H

#include "ui_tags.h"
#include "QVTKWidget.h"
#include "graph.h"

//dialog for tag selection
class Tags : public QDialog, private Ui_Dialog
{
  Q_OBJECT

public:
  
  //constructor
  Tags(QWidget* parent = 0);
  //destructor
  ~Tags(){};

  //functions
  QListWidget* getTagList();
  bool getContinue();
  void setGraph(Graph* g);
  

public slots:

  //slots for ok and cancel
  virtual void tagCancel();
  virtual void tagOk();

protected:

protected slots:

private:

  //global variables
  bool continueTrue;
  Graph* graph;
  
};

#endif
