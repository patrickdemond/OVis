#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include "ui_screenshot.h"
#include "QVTKWidget.h"
#include "graph.h"

//dialog for screenshot selection
class Screenshot : public QDialog, private Ui_screenshotDialog
{
  Q_OBJECT

 public:
  //constructor
  Screenshot(QWidget* parent = 0);
  //destructor
  ~Screenshot(){};  

  int getMagnification();

 public slots:
  virtual void okClicked();

 private:
  void populateComboBox();
  int magnification;

};

#endif
