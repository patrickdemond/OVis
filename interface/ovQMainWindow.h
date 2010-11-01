#ifndef ORLANDO_H
#define ORLANDO_H

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;
class QListWidget;
class QListWidgetItem;
class ovOrlandoReader;
class ovRestrictGraph;
class ovRemoveIsolatedVertices;
class vtkGraphLayoutView;

class ovQMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  //constructor
  ovQMainWindow( QWidget* parent = 0 );
  //destructor
  ~ovQMainWindow();
  
  // Update the graph view based on the current state of the UI
  virtual void UpdateGraphView( bool resetCamera = false );

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
  vtkSmartPointer< ovOrlandoReader > OrlandoReader;
  vtkSmartPointer< ovRestrictGraph > RestrictFilter;
  vtkSmartPointer< ovRemoveIsolatedVertices > RemoveVerticesFilter;

protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  // File menu variables
  QAction *actionFileOpen;
  QAction *actionFileExit;

  // Set to false to disable automatic updating of graph
  bool AutoUpdateGraphView;
};

#endif
