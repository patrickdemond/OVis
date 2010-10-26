#ifndef ORLANDO_H
#define ORLANDO_H

#include <QMainWindow>

#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;
class vtkGraphLayoutView;

class ovQMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  //constructor
  ovQMainWindow( QWidget* parent = 0 );
  //destructor
  ~ovQMainWindow(){};

public slots:
  //event functions
  virtual void slotFileOpen();
  virtual void slotFileExit();

protected:
  vtkSmartPointer< vtkGraphLayoutView > GraphLayoutView;

protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  //file menu variables
  QAction *ActionFileOpen;
  QAction *ActionFileExit;
};

#endif
