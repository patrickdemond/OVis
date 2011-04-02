#ifndef __ovQSearchDialog_h
#define __ovQSearchDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQSearchDialog;
class ovSearchPhrase;

class ovQSearchDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQSearchDialog( QWidget* parent = 0, bool stem = false );
  //destructor
  ~ovQSearchDialog();
  
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
  Ui_ovQSearchDialog *ui;
};

#endif
