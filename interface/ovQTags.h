#ifndef TAGS_H
#define TAGS_H

#include "ui_ovQTags.h"
#include "QVTKWidget.h"

#include "source/ovGraph.h"

//dialog for tag selection
class ovQTags : public QDialog, private Ui_Dialog
{
  Q_OBJECT

public:
  
  //constructor
  ovQTags( QWidget* parent = 0 );
  //destructor
  ~ovQTags(){};

  //functions
  QListWidget* getTagList();
  bool getContinue();
  void setGraph( ovGraph* g );
  

public slots:
  
  virtual void changeSelPal( QListWidgetItem* tagToChange );
  virtual void checkUncheckAll();
  virtual void loadCols();
  virtual void saveCols();
  virtual void changeTagCol( QListWidgetItem* tagToChange );
  //slots for ok and cancel
  virtual void tagCancel();
  virtual void tagOk();

protected:

protected slots:

private:

  //global variables
  bool continueTrue;
  ovGraph* graph;

};

#endif
