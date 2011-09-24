/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQAuthorSearchDialog.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/

#ifndef __ovQAuthorSearchDialog_h
#define __ovQAuthorSearchDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQAuthorSearchDialog;
class ovSearchPhrase;

class ovQAuthorSearchDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQAuthorSearchDialog( QWidget* parent = 0, bool stem = false );
  //destructor
  ~ovQAuthorSearchDialog();
  
  // get/set search phrase
  void getSearchPhrase( ovSearchPhrase *search );
  void setSearchPhrase( ovSearchPhrase *search );

public slots:
  //event functions
  void slotTermTableWidgetCellClicked( int row, int column );
  void slotAddPushButton();
  void slotRemovePushButton();

protected:
  bool useStemColumn;
  int getColumnIndex( int column );

  enum ColumnIndex
  {
    StemColumn,
    AndColumn,
    NotColumn,
    TermColumn
  };

protected slots:

private:
  // Designer form
  Ui_ovQAuthorSearchDialog *ui;
};

#endif
