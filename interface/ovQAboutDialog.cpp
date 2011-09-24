/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQAboutDialog.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQAboutDialog.h"

#include <QFile>
#include <QTextStream>

#include "ui_ovQAboutDialog.h"

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQAboutDialog::ovQAboutDialog( QWidget* parent )
  : QDialog( parent )
{
  this->ui = new Ui_ovQAboutDialog;
  this->ui->setupUi( this );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQAboutDialog::~ovQAboutDialog()
{
}
