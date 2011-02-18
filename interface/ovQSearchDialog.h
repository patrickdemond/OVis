#ifndef __ovQSearchDialog_h
#define __ovQSearchDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQSearchDialog;

// TODO: implement this dialog
class ovQSearchDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQSearchDialog( QWidget* parent = 0 );
  //destructor
  ~ovQSearchDialog();
  
  // get/set date
  ovString getSearch();
  void setSearch( const ovString &search );

public slots:
  //event functions
  void slotTermTableWidgetCellClicked( int row, int column );
  void slotAddPushButton();
  void slotRemovePushButton();

protected:

protected slots:

private:
  // Designer form
  Ui_ovQSearchDialog *ui;
};

#endif
