/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQDateDialog.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/

#ifndef __ovQDateDialog_h
#define __ovQDateDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQDateDialog;

class ovQDateDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQDateDialog( QWidget* parent = 0 );
  //destructor
  ~ovQDateDialog();
  
  // get/set date
  ovDate getDate();
  void setDate( const ovDate &date );

public slots:
  //event functions
  virtual void slotYearLineEditTextChanged( const QString& );
  virtual void slotMonthComboBoxCurrentIndexChanged( int );
  virtual void slotHistoricComboBoxCurrentIndexChanged( int );
  virtual void slotHistoricPeriodComboBoxCurrentIndexChanged( int );
  virtual void slotMonarchComboBoxCurrentIndexChanged( int );
  virtual void slotMonarchPeriodComboBoxCurrentIndexChanged( int );

protected:
  virtual void SetDateByHistoricDate();
  virtual void SetDateByMonarchDate();

  ovDate date;
  ovDatePeriodVector HistoricPeriodVector;
  ovDatePeriodVector MonarchPeriodVector;
  bool isValidatingYear;

protected slots:

private:
  // Designer form
  Ui_ovQDateDialog *ui;
};

#endif
