/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQSearchDialog.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQSearchDialog.h"

#include "ui_ovQSearchDialog.h"
#include "source/ovSearchPhrase.h"

#include <vtkstd/algorithm>

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQSearchDialog::ovQSearchDialog( QWidget* parent, bool stem )
  : QDialog( parent )
{
  this->ui = new Ui_ovQSearchDialog;
  this->ui->setupUi( this );
  this->useStemColumn = stem;
  
  if( this->useStemColumn )
    this->ui->termTableWidget->setColumnWidth(
      this->getColumnIndex( ovQSearchDialog::StemColumn ), 68 );
  // if columns move around the 0 needs to be changed
  else this->ui->termTableWidget->removeColumn( 0 );

  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQSearchDialog::AndColumn ), 68 );
  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQSearchDialog::NotColumn ), 68 );
  this->ui->termTableWidget->setColumnWidth(
    this->getColumnIndex( ovQSearchDialog::TermColumn ), this->useStemColumn ? 425 : 493 );
  
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
ovQSearchDialog::~ovQSearchDialog()
{
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovQSearchDialog::getColumnIndex( int column )
{
  if( ovQSearchDialog::StemColumn == column ) return this->useStemColumn ? 0 : -1;
  else if( ovQSearchDialog::AndColumn == column ) return this->useStemColumn ? 1 : 0;
  else if( ovQSearchDialog::NotColumn == column ) return this->useStemColumn ? 2 : 1;
  else if( ovQSearchDialog::TermColumn == column ) return this->useStemColumn ? 3 : 2;
  else return -1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::getSearchPhrase( ovSearchPhrase *phrase )
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
      column = this->getColumnIndex( ovQSearchDialog::StemColumn );
      searchTerm.stemming = "YES" == this->ui->termTableWidget->item( row, column )->text();
    }
    else
    {
      searchTerm.stemming = false;
    }
    column = this->getColumnIndex( ovQSearchDialog::AndColumn );
    searchTerm.andLogic = "AND" == this->ui->termTableWidget->item( row, column )->text();
    column = this->getColumnIndex( ovQSearchDialog::NotColumn );
    searchTerm.notLogic = "NOT" == this->ui->termTableWidget->item( row, column )->text();
    column = this->getColumnIndex( ovQSearchDialog::TermColumn );
    searchTerm.term = this->ui->termTableWidget->item( row, column )->text().toStdString();
    
    phrase->Add( searchTerm );
  }

  return;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::setSearchPhrase( ovSearchPhrase *search )
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
        row, this->getColumnIndex( ovQSearchDialog::StemColumn ), item );
    }

    item = new QTableWidgetItem( 0 == row ? "" : term->andLogic ? "AND" : "OR" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQSearchDialog::AndColumn ), item );

    item = new QTableWidgetItem( term->notLogic ? "NOT" : "" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQSearchDialog::NotColumn ), item );
    
    item = new QTableWidgetItem( term->term.c_str() );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
    this->ui->termTableWidget->setItem(
      row, this->getColumnIndex( ovQSearchDialog::TermColumn ), item );
  
    row++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotTermTableWidgetCellClicked( int row, int column )
{
  if( this->getColumnIndex( ovQSearchDialog::TermColumn ) == column ) return;
  
  QTableWidgetItem *item = this->ui->termTableWidget->item( row, column );
  
  if( this->useStemColumn && this->getColumnIndex( ovQSearchDialog::StemColumn ) == column )
  {
    item->setText( "YES" == item->text() ? "NO" : "YES" );
  }
  else if( this->getColumnIndex( ovQSearchDialog::AndColumn ) == column && 0 != row )
  {
    item->setText( "AND" == item->text() ? "OR" : "AND" );
  }
  else if( this->getColumnIndex( ovQSearchDialog::NotColumn ) == column )
  {
    item->setText( "NOT" == item->text() ? "" : "NOT" );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotAddPushButton()
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
      row, this->getColumnIndex( ovQSearchDialog::StemColumn ), item );
  }

  item = new QTableWidgetItem( 0 == row ? "" : "AND" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQSearchDialog::AndColumn ), item );

  item = new QTableWidgetItem( "" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQSearchDialog::NotColumn ), item );
  
  item = new QTableWidgetItem( "" );
  item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
  this->ui->termTableWidget->setItem(
    row, this->getColumnIndex( ovQSearchDialog::TermColumn ), item );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotRemovePushButton()
{
  this->ui->termTableWidget->removeRow( this->ui->termTableWidget->currentRow() );
}
