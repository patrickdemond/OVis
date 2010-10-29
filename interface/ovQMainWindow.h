#ifndef ORLANDO_H
#define ORLANDO_H

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;
class vtkGraphLayoutView;
class QListWidget;
class QListWidgetItem;

class ovQMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  //constructor
  ovQMainWindow( QWidget* parent = 0 );
  //destructor
  ~ovQMainWindow();

public slots:
  //event functions
  virtual void slotFileOpen();
  virtual void slotFileExit();
  virtual void slotTagListCheckAllButtonClicked();
  virtual void slotTagListCheckNoneButtonClicked();
  virtual void slotTagListItemChanged( QListWidgetItem* );
  virtual void slotTagListPresetComboBoxIndexChanged( int );

protected:
  vtkSmartPointer< vtkGraphLayoutView > GraphLayoutView;

protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  // File menu variables
  QAction *actionFileOpen;
  QAction *actionFileExit;
};

#endif
