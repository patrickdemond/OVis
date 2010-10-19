#ifndef TEXT_H
#define TEXT_H

#include "ui_ovQText.h"
#include "QVTKWidget.h"
#include <QComboBox>

//dialog for tag selection
class ovQText: public QDialog, private Ui_TextDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  ovQText( QWidget* parent = 0 );
  //destructor
  ~ovQText(){};

  //functions
  QComboBox* getCombo1();
  QComboBox* getCombo2();
  QComboBox* getCombo3();

public slots:

  //slots for ok and cancel
  virtual void showText();
  virtual void done();
  virtual void entrySel( QString str );
  virtual void nameSel( QString str );
  virtual void tagSel( QString str );

protected:

protected slots:

private:

  //global variables
  char* entry;
  char* name;
  char* tag;
};

#endif
