#ifndef ORLANDO_H
#define ORLANDO_H

#include "ui_ovQMainWindow.h"
#include "vtkRenderWindow.h"
#include "QVTKWidget.h"
#include "ovQFont.h"

class ovQMainWindow : public QMainWindow, private Ui_MainWindow
{
  Q_OBJECT

public:
  //constructor
  ovQMainWindow( QWidget* parent = 0 );
  //destructor
  ~ovQMainWindow(){};

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
  void setVisualizationText( char* text );
  void enableMenuItems( char mode );

public slots:

  //event functions
  virtual void fileOpen();
  virtual void fileLoad();
  virtual void fileSave();
  virtual void fileExit();
  virtual void saveScreenshot();
  virtual void clearGraph();
  virtual void popUpTags();
  virtual void popUpText();
  virtual void search();
  virtual void graphButton();
  virtual void toggleMode();
  virtual void graphMode( bool draw );
  virtual void cameraMode();
  virtual void highlightMode();
  virtual void doubleClick( QListWidgetItem* item );
  virtual void nameTagsOn();
  virtual void allNameTagsOn();
  virtual void nameTagsOff();
  virtual void nameTagsClear();
  virtual void findPath();
  virtual void setLabelProperties();

protected:

protected slots:

private:

//file menu variables
  QAction* a_fileOpen;
  QAction* a_fileLoad;
  QAction* a_fileSave;
  QAction* a_fileExit;
  QAction* a_editScreenshot;
  QAction* a_editClearGraph; 
  QAction* a_names;
  QAction* a_names2;
  QAction* a_names3;
  QAction* a_names4;
  QAction* a_names5;
  QAction* a_path;
  QAction* a_tags;
  QAction* a_text;
};

#endif
