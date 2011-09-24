/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQAboutDialog.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/

#ifndef __ovQAboutDialog_h
#define __ovQAboutDialog_h

#include <QDialog>

#include "ovUtilities.h"

class Ui_ovQAboutDialog;

class ovQAboutDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQAboutDialog( QWidget* parent = 0 );
  //destructor
  ~ovQAboutDialog();
  
public slots:

protected:

protected slots:

private:
  // Designer form
  Ui_ovQAboutDialog *ui;
};

#endif
