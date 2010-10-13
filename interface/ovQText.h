#ifndef TEXT_H
#define TEXT_H

#include "ui_text.h"
#include "QVTKWidget.h"
#include <QComboBox>
#include "graph.h"

//dialog for tag selection
class Text: public QDialog, private Ui_TextDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  Text(QWidget* parent = 0);
  //destructor
  ~Text(){};

  //functions
  void setGraph(Graph* g);
  QComboBox* getCombo1();
  QComboBox* getCombo2();
  QComboBox* getCombo3();

public slots:

  //slots for ok and cancel
  virtual void showText();
  virtual void done();
  virtual void entrySel(QString str);
  virtual void nameSel(QString str);
  virtual void tagSel(QString str);

protected:

protected slots:

private:

  //global variables
  Graph* graph;
  char* entry;
  char* name;
  char* tag;
};

#endif
