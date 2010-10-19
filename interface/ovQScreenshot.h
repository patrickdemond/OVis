#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include "ui_ovQScreenshot.h"
#include "QVTKWidget.h"

//dialog for screenshot selection
class ovQScreenshot : public QDialog, private Ui_screenshotDialog
{
  Q_OBJECT

 public:
  //constructor
  ovQScreenshot( QWidget* parent = 0 );
  //destructor
  ~ovQScreenshot(){};  

  int getMagnification();

 public slots:
  virtual void okClicked();

 private:
  void populateComboBox();
  int magnification;

};

#endif
