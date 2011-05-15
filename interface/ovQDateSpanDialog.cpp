/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQDateSpanDialog.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQDateSpanDialog.h"

#include "ui_ovQDateSpanDialog.h"

#include <stdio.h>

#include <QComboBox>
#include <QLineEdit>

#include <vtkstd/algorithm>

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQDateSpanDialog::ovQDateSpanDialog( QWidget* parent )
  : QDialog( parent )
{
  this->ui = new Ui_ovQDateSpanDialog;
  this->ui->setupUi( this );
  
  this->isValidatingStartYear = false;
  this->isValidatingEndYear = false;

  QObject::connect(
    this->ui->startYearLineEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( slotYearLineEditTextChanged( const QString& ) ) );
  QObject::connect(
    this->ui->startMonthComboBox, SIGNAL( currentIndexChanged( int ) ),
    this, SLOT( slotMonthComboBoxCurrentIndexChanged( int ) ) );
  QObject::connect(
    this->ui->endYearLineEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( slotYearLineEditTextChanged( const QString& ) ) );
  QObject::connect(
    this->ui->endMonthComboBox, SIGNAL( currentIndexChanged( int ) ),
    this, SLOT( slotMonthComboBoxCurrentIndexChanged( int ) ) );
  QObject::connect(
    this->ui->historicDateComboBox, SIGNAL( currentIndexChanged( int ) ),
    this, SLOT( slotHistoricComboBoxCurrentIndexChanged( int ) ) );
  QObject::connect(
    this->ui->monarchDateComboBox, SIGNAL( currentIndexChanged( int ) ),
    this, SLOT( slotMonarchComboBoxCurrentIndexChanged( int ) ) );
  
  QObject::connect(
    this->ui->buttonBox, SIGNAL( accepted() ),
    this, SLOT( accept() ) );
  QObject::connect(
    this->ui->buttonBox, SIGNAL( rejected() ),
    this, SLOT( reject() ) );

  // add in historical periods
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "English Middle Ages (1066 - 1501)",
      ovDate( "1066-12-25" ),
      ovDate( "1501--" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Early Modern Period (1485 - 1752)",
      ovDate( "1485-08-22" ),
      ovDate( "1752-11-23" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "English Renaissance (1501 - 1639)",
      ovDate( "1501--" ),
      ovDate( "1639-12-31" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "17th Century (1600 - 1699)",
      ovDate( "1600--" ),
      ovDate( "1699-12-31" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "English Civil War (1639 - 1647)",
      ovDate( "1639-03-27" ),
      ovDate( "1647-06-03" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Interregnum (1647 - 1660)",
      ovDate( "1647-06-03" ),
      ovDate( "1660-05-08" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Restoration (1660 - 1700)",
      ovDate( "1660-05-08" ),
      ovDate( "1700-09-26" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Restoration and Early 18th (1660 - 1730)",
      ovDate( "1660-05-08" ),
      ovDate( "1730-01-14" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Restoration and 18th Century (1660 - 1800)",
      ovDate( "1660-05-08" ),
      ovDate( "1800-12-26" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Long 18th Century (1660 - 1821)",
      ovDate( "1660-05-08" ),
      ovDate( "1821-01-" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Cartesian Protofeminism (1689 - 1706)",
      ovDate( "1689-04-22" ),
      ovDate( "1706-07-" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "18th Century (1700 - 1799)",
      ovDate( "1700--" ),
      ovDate( "1799-12-31" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Revolutionary Period (1775 - 1800)",
      ovDate( "1775-01-" ),
      ovDate( "1800-12-25" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Romantic Movement (1784 - 1821)",
      ovDate( "1784-06-" ),
      ovDate( "1821-01-" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "French Revolution (1789 - 1800)",
      ovDate( "1789-06-17" ),
      ovDate( "1800-12-25" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Long 19th Century (1789 - 1914)",
      ovDate( "1789-06-17" ),
      ovDate( "1914-08-04" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Enlightenment Protofeminism (1790 - 1799)",
      ovDate( "1790-09-" ),
      ovDate( "1799-03-" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Napoleonic Wars (1793 - 1815)",
      ovDate( "1793-02-01" ),
      ovDate( "1815-06-18" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "19th Century (1800-1899)",
      ovDate( "1800--" ),
      ovDate( "1899-12-31" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Early Victorian (1832 - 1867)",
      ovDate( "1832-06-07" ),
      ovDate( "1867-08-14" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Victorian (1832 - 1901)",
      ovDate( "1832-06-07" ),
      ovDate( "1901-01-22" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "First-Wave Feminism (1855 - 1918)",
      ovDate( "1855--" ),
      ovDate( "1918-02-06" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Suffrage Struggle (1866 - 1928)",
      ovDate( "1866-06-07" ),
      ovDate( "1928-07-02" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Mid-Victorian (1867 - 1884)",
      ovDate( "1867-08-15" ),
      ovDate( "1884-12-09" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Late Victorian (1884 - 1901)",
      ovDate( "1884-12-10" ),
      ovDate( "1901-01-22" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Fin de Siecle (1885 - 1900)",
      ovDate( "1885-03-04" ),
      ovDate( "1900--" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Modernist Period (1899 - 1941)",
      ovDate( "1899-02-" ),
      ovDate( "1941-07-17" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "20th Century (1900 - 1999)",
      ovDate( "1900--" ),
      ovDate( "1999-12-31" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Edwardian (1901 - 1910)",
      ovDate( "1901-01-22" ),
      ovDate( "1910-05-06" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "World War I (1914 - 1918)",
      ovDate( "1914-08-04" ),
      ovDate( "1918-11-11" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Postwar Period (World War I) (1918 - 1920)",
      ovDate( "1918-11-11" ),
      ovDate( "1920-11-29" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Between the Wars (1918 - 1939)",
      ovDate( "1918-11-11" ),
      ovDate( "1939-09-03" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "World War II (1939 - 1945)",
      ovDate( "1939--" ),
      ovDate( "1945-06-21" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Postwar Period (World War II) (1945 - 1951)",
      ovDate( "1945-05-08" ),
      ovDate( "1951-10-26" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Cold War (1947 - 1989)",
      ovDate( "1947-03-12" ),
      ovDate( "1989-11-09" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Postmodern Period (1963 to present)",
      ovDate( "1963-11-22" ),
      ovDate( "9999-01-01" ) ) );
  this->HistoricPeriodVector.push_back(
    new ovDatePeriod(
      "Second-Wave Feminism (1963-1990)",
      ovDate( "1963-02-" ),
      ovDate( "1990-12-31" ) ) );
  
  // add in monarchy periods
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Henry VII  (1485 - 1509)",
      ovDate( "1485-08-22" ),
      ovDate( "1509-04-21" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Henry VIII  (1509 - 1547)",
      ovDate( "1509-04-22" ),
      ovDate( "1547-01-28" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Edward VI  (1547 - 1553)",
      ovDate( "1547-01-28" ),
      ovDate( "1553-07-06" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Lady Jane Grey  (1553 - 1553)",
      ovDate( "1553-07-06" ),
      ovDate( "1553-07-19" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Mary I  (1553 - 1558)",
      ovDate( "1553-11-17" ),
      ovDate( "1558-11-17" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Elizabeth I  (1558 - 1603)",
      ovDate( "1558-11-17" ),
      ovDate( "1603-03-24" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "James I  (1603 - 1625)",
      ovDate( "1603-03-24" ),
      ovDate( "1625-03-27" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Charles I  (1625 - 1649)",
      ovDate( "1625-03-27" ),
      ovDate( "1649-01-30" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Oliver Cromwell  (1653 - 1658)",
      ovDate( "1653-12-16" ),
      ovDate( "1658-09-03" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Richard Cromwell  (1658 - 1659)",
      ovDate( "1658-09-03" ),
      ovDate( "1659-05-25" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Charles II  (1660 - 1685)",
      ovDate( "1660-05-08" ),
      ovDate( "1685-02-06" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "James II  (1685 - 1689)",
      ovDate( "1685-02-06" ),
      ovDate( "1689-02-13" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "William and Mary  (1689 - 1694)",
      ovDate( "1689-02-13" ),
      ovDate( "1694-12-28" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "William III  (1694 - 1702)",
      ovDate( "1694-12-28" ),
      ovDate( "1702-02-08" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Anne  (1702 - 1714)",
      ovDate( "1702-03-08" ),
      ovDate( "1714-08-01" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George I  (1714 - 1727)",
      ovDate( "1714-08-01" ),
      ovDate( "1727-06-11" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George II  (1727 - 1760)",
      ovDate( "1727-06-11" ),
      ovDate( "1760-10-25" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George III  (1760 - 1820)",
      ovDate( "1760-10-25" ),
      ovDate( "1820-01-29" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George IV  (1820 - 1830)",
      ovDate( "1820-01-29" ),
      ovDate( "1830-06-26" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "William IV  (1830 - 1837)",
      ovDate( "1830-06-26" ),
      ovDate( "1837-06-20" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Victoria  (1837 - 1901)",
      ovDate( "1837-06-20" ),
      ovDate( "1901-01-22" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Edward VII  (1901 - 1910)",
      ovDate( "1901-01-22" ),
      ovDate( "1910-05-06" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George V  (1910 - 1936)",
      ovDate( "1910-05-06" ),
      ovDate( "1936-01-20" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Edward VIII  (1936 - 1936)",
      ovDate( "1936-01-20" ),
      ovDate( "1936-12-10" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "George VI  (1936 - 1952)",
      ovDate( "1936-12-10" ),
      ovDate( "1952-02-06" ) ) );
  this->MonarchPeriodVector.push_back(
    new ovDatePeriod(
      "Elizabeth II 1952 - ",
      ovDate( "1952-02-06" ),
      ovDate( "9999-01-01" ) ) );

  // now add the periods to the combo boxes
  for( ovDatePeriodVector::iterator it = this->HistoricPeriodVector.begin();
       it != this->HistoricPeriodVector.end();
       ++it )
  {
    this->ui->historicDateComboBox->addItem( (*it)->name.c_str() );
  }
  
  for( ovDatePeriodVector::iterator it = this->MonarchPeriodVector.begin();
       it != this->MonarchPeriodVector.end();
       ++it )
  {
    this->ui->monarchDateComboBox->addItem( (*it)->name.c_str() );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQDateSpanDialog::~ovQDateSpanDialog()
{
  // delete all periods
  vtkstd::for_each(
    this->HistoricPeriodVector.begin(), this->HistoricPeriodVector.end(), safe_delete() );
  this->HistoricPeriodVector.clear();

  vtkstd::for_each(
    this->MonarchPeriodVector.begin(), this->MonarchPeriodVector.end(), safe_delete() );
  this->MonarchPeriodVector.clear();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovDate ovQDateSpanDialog::getStartDate()
{
  QVariant year( this->ui->startYearLineEdit->text() );
  this->startDate.year = year.toInt();
  this->startDate.month = this->ui->startMonthComboBox->currentIndex();
  this->startDate.day = this->ui->startDayComboBox->currentIndex();
  this->startDate.Validate();
  return this->startDate;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovDate ovQDateSpanDialog::getEndDate()
{
  QVariant year( this->ui->endYearLineEdit->text() );
  this->endDate.year = year.toInt();
  this->endDate.month = this->ui->endMonthComboBox->currentIndex();
  this->endDate.day = this->ui->endDayComboBox->currentIndex();
  this->endDate.Validate();
  return this->endDate;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::setDateSpan( const ovDate &start, const ovDate &end )
{
  char buffer[64];
  
  sprintf( buffer, start.year ? "%d" : "", start.year );
  this->ui->startYearLineEdit->setText( buffer );
  this->ui->startMonthComboBox->setCurrentIndex( start.month );
  this->ui->startDayComboBox->setCurrentIndex( start.day );

  sprintf( buffer, end.year ? "%d" : "", end.year );
  this->ui->endYearLineEdit->setText( buffer );
  this->ui->endMonthComboBox->setCurrentIndex( end.month );
  this->ui->endDayComboBox->setCurrentIndex( end.day );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::SetDateSpanByHistoricDate()
{
  int index = this->ui->historicDateComboBox->currentIndex();

  if( 0 < index )
  {
    // change the date based on the historic date and period
    ovDatePeriod *datePeriod = this->HistoricPeriodVector.at( index - 1 );
    this->setDateSpan( datePeriod->start, datePeriod->end );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::SetDateSpanByMonarchDate()
{
  int index = this->ui->monarchDateComboBox->currentIndex();

  if( 0 < index )
  {
    // change the date based on the monarch date and period
    ovDatePeriod *datePeriod = this->MonarchPeriodVector.at( index - 1 );
    this->setDateSpan( datePeriod->start, datePeriod->end );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotStartYearLineEditTextChanged( const QString &constText )
{
  if( this->isValidatingStartYear ) return;

  this->isValidatingStartYear = true;

  // validate the input
  int pos;
  QValidator *v = new QIntValidator( 1, 9999, NULL );
  QString text = constText;
  
  this->ui->startYearLineEdit->setText(
    QValidator::Invalid == v->validate( text, pos ) ? "" : text );
  
  // enable/disable month drop down depending on if we have a year
  this->ui->startMonthComboBox->setEnabled( 0 < this->ui->startYearLineEdit->text().length() );
  
  // We may be changing the year with the month and day combo boxes already active,
  // so make sure to update the number of days per month (in case the month selector
  // is on February and we change whether we're in a leap year or not)
  if( 2 == this->ui->startMonthComboBox->currentIndex() )
  {
    // this is cheating a bit, but currently speed (of coding) is of the essence
    this->ovQDateSpanDialog::slotStartMonthComboBoxCurrentIndexChanged( 2 );
  }

  this->isValidatingStartYear = false;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotStartMonthComboBoxCurrentIndexChanged( int index )
{
  QVariant y( this->ui->startYearLineEdit->text() );
  int year = y.toInt();
  int month = index;
  
  if( 0 == month )
  {
    // set the day to none and disable it
    this->ui->startDayComboBox->setCurrentIndex( 0 );
    this->ui->startDayComboBox->setEnabled( false );
  }
  else
  {
    // determine how many days should be in the day drop down
    char buffer[64];
    int maxDays = ovDate::DaysInMonth( year, month );
    this->ui->startDayComboBox->clear();
    this->ui->startDayComboBox->addItem( "none" );
    for( int i = 1; i <= maxDays; i++ )
    {
      sprintf( buffer, "%02d", i );
      this->ui->startDayComboBox->addItem( buffer );
    }
    this->ui->startDayComboBox->setCurrentIndex( 0 );
    this->ui->startDayComboBox->setEnabled( true );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotEndYearLineEditTextChanged( const QString &constText )
{
  if( this->isValidatingEndYear ) return;

  this->isValidatingEndYear = true;

  // validate the input
  int pos;
  QValidator *v = new QIntValidator( 1, 9999, NULL );
  QString text = constText;
  
  this->ui->endYearLineEdit->setText(
    QValidator::Invalid == v->validate( text, pos ) ? "" : text );
  
  // enable/disable month drop down depending on if we have a year
  this->ui->endMonthComboBox->setEnabled( 0 < this->ui->endYearLineEdit->text().length() );
  
  // We may be changing the year with the month and day combo boxes already active,
  // so make sure to update the number of days per month (in case the month selector
  // is on February and we change whether we're in a leap year or not)
  if( 2 == this->ui->endMonthComboBox->currentIndex() )
  {
    // this is cheating a bit, but currently speed (of coding) is of the essence
    this->ovQDateSpanDialog::slotEndMonthComboBoxCurrentIndexChanged( 2 );
  }

  this->isValidatingEndYear = false;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotEndMonthComboBoxCurrentIndexChanged( int index )
{
  QVariant y( this->ui->endYearLineEdit->text() );
  int year = y.toInt();
  int month = index;
  
  if( 0 == month )
  {
    // set the day to none and disable it
    this->ui->endDayComboBox->setCurrentIndex( 0 );
    this->ui->endDayComboBox->setEnabled( false );
  }
  else
  {
    // determine how many days should be in the day drop down
    char buffer[64];
    int maxDays = ovDate::DaysInMonth( year, month );
    this->ui->endDayComboBox->clear();
    this->ui->endDayComboBox->addItem( "none" );
    for( int i = 1; i <= maxDays; i++ )
    {
      sprintf( buffer, "%02d", i );
      this->ui->endDayComboBox->addItem( buffer );
    }
    this->ui->endDayComboBox->setCurrentIndex( 0 );
    this->ui->endDayComboBox->setEnabled( true );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotHistoricComboBoxCurrentIndexChanged( int index )
{
  if( 0 != index ) this->SetDateSpanByHistoricDate();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQDateSpanDialog::slotMonarchComboBoxCurrentIndexChanged( int index )
{
  if( 0 != index ) this->SetDateSpanByMonarchDate();
}
