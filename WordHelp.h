#ifndef WORDHELP_H
#define WORDHELP_H

#include "ui_wordHelp.h"
#include "QVTKWidget.h"

//dialog for tag selection
class WordHelp : public QDialog, private Ui_WordHelpDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  WordHelp(QWidget* parent = 0);
  //destructor
  ~WordHelp(){};

public slots:

  //slots for ok
  virtual void wordHelpOk();

protected:

protected slots:

private:

};

#endif
