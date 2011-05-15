#ifndef __ovQDateSpanDialog_h
#define __ovQDateSpanDialog_h

#include <QDialog>

#include "ovUtilities.h"

#include <QtCore/qstring.h>

class Ui_ovQDateSpanDialog;

class ovQDateSpanDialog : public QDialog
{
  Q_OBJECT

public:
  //constructor
  ovQDateSpanDialog( QWidget* parent = 0 );
  //destructor
  ~ovQDateSpanDialog();
  
  // get/set date span
  ovDate getStartDate();
  ovDate getEndDate();
  void setDateSpan( const ovDate &startDate, const ovDate &endDate );

public slots:
  //event functions
  virtual void slotStartYearLineEditTextChanged( const QString& );
  virtual void slotStartMonthComboBoxCurrentIndexChanged( int );
  virtual void slotEndYearLineEditTextChanged( const QString& );
  virtual void slotEndMonthComboBoxCurrentIndexChanged( int );
  virtual void slotHistoricComboBoxCurrentIndexChanged( int );
  virtual void slotMonarchComboBoxCurrentIndexChanged( int );

protected:
  virtual void SetDateSpanByHistoricDate();
  virtual void SetDateSpanByMonarchDate();

  ovDate startDate;
  ovDate endDate;
  ovDatePeriodVector HistoricPeriodVector;
  ovDatePeriodVector MonarchPeriodVector;
  bool isValidatingStartYear;
  bool isValidatingEndYear;

protected slots:

private:
  // Designer form
  Ui_ovQDateSpanDialog *ui;
};

#endif
