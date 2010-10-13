#ifndef PATH_H
#define PATH_H

#include "ui_ovQPath.h"
#include "QVTKWidget.h"

#include "source/ovGraph.h"

//dialog for tag selection
class ovQPath : public QDialog, private Ui_PathDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  ovQPath( QWidget* parent = 0 );
  //destructor
  ~ovQPath(){};

  //functions
  QWidget* getParent();
  QComboBox* getCombo1();
  QComboBox* getCombo2();
  void setGraph( ovGraph* g );

public slots:

  //slots for ok and cancel
  virtual void pathCancel();
  virtual void pathOk();
  virtual void setIndex1( int ind1 );
  virtual void setIndex2( int ind2 );

protected:

protected slots:

private:

  //global variables
  ovGraph* graph;
  int index1;
  int index2;  
  QWidget* par;
};

#endif
