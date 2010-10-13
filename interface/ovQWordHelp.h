#ifndef WORDHELP_H
#define WORDHELP_H

#include "ui_ovQWordHelp.h"
#include "QVTKWidget.h"

//dialog for tag selection
class ovQWordHelp : public QDialog, private Ui_WordHelpDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  ovQWordHelp( QWidget* parent = 0 );
  //destructor
  ~ovQWordHelp(){};

public slots:

  //slots for ok
  virtual void wordHelpOk();

protected:

protected slots:

private:

};

#endif
