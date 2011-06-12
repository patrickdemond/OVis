/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQTextSearchDialog.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQTextSearchDialog.h"

#include "ui_ovQTextSearchDialog.h"
#include "source/ovSearchPhrase.h"

#include <vtkstd/algorithm>

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQTextSearchDialog::ovQTextSearchDialog( QWidget* parent, bool stem )
  : QDialog( parent )
{
  this->ui = new Ui_ovQTextSearchDialog;
  this->ui->setupUi( this );
  this->useStemColumn = stem;
  
  if( this->useStemColumn )
    this->ui->termTableWidget->setColumnWidth(
      this->getColumnIndex( ovQTextSearchDialog::StemColumn ), 68 );
  // if columns move around the 0 needs to be changed
  else this->ui->termTableWidget->removeColumn( 0 );

  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQTextSearchDialog::AndColumn ), 68 );
  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQTextSearchDialog::NotColumn ), 68 );
  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQTextSearchDialog::TermColumn ), this->useStemColumn ? 425 : 493 );
  
  QObject::connect(
    this->ui->termTableWidget, SIGNAL( cellClicked ( int, int ) ),
    this, SLOT( slotTermTableWidgetCellClicked( int, int ) ) );
  QObject::connect(
    this->ui->addPushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotAddPushButton() ) );
  QObject::connect(
    this->ui->removePushButton, SIGNAL( clicked( bool ) ),
    this, SLOT( slotRemovePushButton() ) );
  QObject::connect(
    this->ui->buttonBox, SIGNAL( accepted() ),
    this, SLOT( accept() ) );
  QObject::connect(
    this->ui->buttonBox, SIGNAL( rejected() ),
    this, SLOT( reject() ) );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQTextSearchDialog::~ovQTextSearchDialog()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovQTextSearchDialog::getColumnIndex( int column )
{
  if( ovQTextSearchDialog::StemColumn == column ) return this->useStemColumn ? 0 : -1;
  else if( ovQTextSearchDialog::AndColumn == column ) return this->useStemColumn ? 1 : 0;
  else if( ovQTextSearchDialog::NotColumn == column ) return this->useStemColumn ? 2 : 1;
  else if( ovQTextSearchDialog::TermColumn == column ) return this->useStemColumn ? 3 : 2;
  else return -1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::getSearchPhrase( ovSearchPhrase *phrase )
{
  if( NULL == phrase ) return;
  phrase->Clear();

  QTableWidgetItem *item;
  int column, rows = this->ui->termTableWidget->rowCount();
  
  for( int row = 0; row < rows; row++ )
  {
    ovSearchTerm searchTerm;
    if( this->useStemColumn )
    {
      column = this->getColumnIndex( ovQTextSearchDialog::StemColumn );
      searchTerm.stemming = "YES" == this->ui->termTableWidget->item( row, column )->text();
    }
    else
    {
      searchTerm.stemming = false;
    }
    column = this->getColumnIndex( ovQTextSearchDialog::AndColumn );
    searchTerm.andLogic = "AND" == this->ui->termTableWidget->item( row, column )->text();
    column = this->getColumnIndex( ovQTextSearchDialog::NotColumn );
    searchTerm.notLogic = "NOT" == this->ui->termTableWidget->item( row, column )->text();
    column = this->getColumnIndex( ovQTextSearchDialog::TermColumn );
    searchTerm.term = this->ui->termTableWidget->item( row, column )->text().toStdString();
    
    phrase->Add( searchTerm );
  }

  return;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::setSearchPhrase( ovSearchPhrase *search )
{
  // clean out the table widget
  this->ui->termTableWidget->clearContents();
  
  if( NULL == search ) return;

  ovSearchTermVector *terms = search->GetSearchTerms();
  this->ui->termTableWidget->setRowCount( terms->size() );
  
  int row = 0;
  QTableWidgetItem *item;
  ovSearchTerm *term;
  ovSearchTermVector::iterator it;
  for( it = terms->begin(); it != terms->end(); it++ )
  {
    term = *it;
  
    if( this->useStemColumn )
    {
      item = new QTableWidgetItem( term->stemming ? "YES" : "NO" );
      item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
      item->setFlags( Qt::ItemIsEnabled );
      this->ui->termTableWidget->setItem(
        row, this->getColumnIndex( ovQTextSearchDialog::StemColumn ), item );
    }

    item = new QTableWidgetItem( 0 == row ? "" : term->andLogic ? "AND" : "OR" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQTextSearchDialog::AndColumn ), item );

    item = new QTableWidgetItem( term->notLogic ? "NOT" : "" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQTextSearchDialog::NotColumn ), item );
    
    item = new QTableWidgetItem( term->term.c_str() );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQTextSearchDialog::TermColumn ), item );
  
    row++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::slotTermTableWidgetCellClicked( int row, int column )
{
  if( this->getColumnIndex( ovQTextSearchDialog::TermColumn ) == column ) return;
  
  QTableWidgetItem *item = this->ui->termTableWidget->item( row, column );
  
  if( this->useStemColumn && this->getColumnIndex( ovQTextSearchDialog::StemColumn ) == column )
  {
    item->setText( "YES" == item->text() ? "NO" : "YES" );
  }
  else if( this->getColumnIndex( ovQTextSearchDialog::AndColumn ) == column && 0 != row )
  {
    item->setText( "AND" == item->text() ? "OR" : "AND" );
  }
  else if( this->getColumnIndex( ovQTextSearchDialog::NotColumn ) == column )
  {
    item->setText( "NOT" == item->text() ? "" : "NOT" );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::slotAddPushButton()
{
  int row = this->ui->termTableWidget->rowCount();
  this->ui->termTableWidget->insertRow( row );

  QTableWidgetItem *item;
  
  if( this->useStemColumn )
  {
    item = new QTableWidgetItem( "YES" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQTextSearchDialog::StemColumn ), item );
  }

  item = new QTableWidgetItem( 0 == row ? "" : "AND" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQTextSearchDialog::AndColumn ), item );

  item = new QTableWidgetItem( "" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQTextSearchDialog::NotColumn ), item );
  
  item = new QTableWidgetItem( "" );
  item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQTextSearchDialog::TermColumn ), item );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::slotRemovePushButton()
{
  this->ui->termTableWidget->removeRow( this->ui->termTableWidget->currentRow() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
bool ovQTextSearchDialog::isNarrow()
{
  return this->ui->narrowRadioButton->isChecked();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQTextSearchDialog::setNarrow( bool narrow )
{
  return this->ui->narrowRadioButton->setChecked( narrow );
}
