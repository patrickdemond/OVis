/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQTextSearchDialog.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/

#ifndef __ovQTextSearchDialog_h
#define __ovQTextSearchDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQTextSearchDialog;
class ovSearchPhrase;

class ovQTextSearchDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQTextSearchDialog( QWidget* parent = 0, bool stem = false );
  //destructor
  ~ovQTextSearchDialog();
  
  // get/set search phrase
  void getSearchPhrase( ovSearchPhrase *search );
  void setSearchPhrase( ovSearchPhrase *search );

  bool isNarrow();
  void setNarrow( bool );

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
  Ui_ovQTextSearchDialog *ui;
};

#endif
