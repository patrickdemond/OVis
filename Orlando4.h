#ifndef ORLANDO_H
#define ORLANDO_H

#include "ui_uiOrlando4.h"
#include "vtkRenderWindow.h"
#include "QVTKWidget.h"
#include "graph.h"
#include "userStyle.h"
#include "Font.h"

class Orlando : public QMainWindow, private Ui_MainWindow
{
  Q_OBJECT

public:
  //constructor
  Orlando(QWidget* parent = 0);
  //destructor
  ~Orlando(){};

  //get widgets
  vtkRenderWindow* getWindow2();
  vtkRenderWindow* getWindow();
  QVTKInteractor* getInteractor();
  QVTKInteractor* getInteractor2();
  QListWidget* getList();
  QLabel* getLabel();
  QLineEdit* getLineEdit();
  QWidget* getWidget();
  QPushButton* getButton();
  QLabel* getLabel1();
  QLabel* getLabel2();
  QProgressBar* getProgressBar();

  //set variables
  void setUserStyle(userStyle* st);
  void setGraph(Graph* gra);
  void setVisualizationText(char* text);

public slots:

  //event functions
  virtual void fileOpen();
  virtual void fileLoad();
  virtual void fileSave();
  virtual void fileExit();
  virtual void saveScreenshot();
  virtual void popUpTags();
  virtual void popUpText();
  virtual void search();
  virtual void graphButton();
  virtual void toggleMode();
  virtual void graphMode(bool draw);
  virtual void cameraMode();
  virtual void highlightMode();
  virtual void doubleClick(QListWidgetItem* item);
  virtual void nameTagsOn();
  virtual void allNameTagsOn();
  virtual void nameTagsOff();
  virtual void nameTagsClear();
  virtual void findPath();
  virtual void setLabelProperties();

protected:

protected slots:

private:

  //global variables
  QAction* a_names;
  QAction* a_names2;
  Graph* graph;
  userStyle* style;  
};

#endif
