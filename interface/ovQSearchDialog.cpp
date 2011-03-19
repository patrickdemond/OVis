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

#define STEM_COLUMN_INDEX 0
#define AND_COLUMN_INDEX  1
#define NOT_COLUMN_INDEX  2
#define TERM_COLUMN_INDEX 3

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovQSearchDialog::ovQSearchDialog( QWidget* parent )
  : QDialog( parent )
{
  this->ui = new Ui_ovQSearchDialog;
  this->ui->setupUi( this );
  
  this->ui->termTableWidget->setColumnWidth( STEM_COLUMN_INDEX, 68 );
  this->ui->termTableWidget->setColumnWidth( AND_COLUMN_INDEX, 68 );
  this->ui->termTableWidget->setColumnWidth( NOT_COLUMN_INDEX, 68 );
  this->ui->termTableWidget->setColumnWidth( TERM_COLUMN_INDEX, 425 );
  
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
void ovQSearchDialog::getSearchPhrase( ovSearchPhrase *phrase )
{
  if( NULL == phrase ) return;
  phrase->Clear();

  QTableWidgetItem *item;
  int rows = this->ui->termTableWidget->rowCount();
  
  for( int row = 0; row < rows; row++ )
  {
    ovSearchTerm searchTerm;
    searchTerm.stemming = "YES" == this->ui->termTableWidget->item( row, STEM_COLUMN_INDEX )->text();
    searchTerm.andLogic = "AND" == this->ui->termTableWidget->item( row, AND_COLUMN_INDEX )->text();
    searchTerm.notLogic = "NOT" == this->ui->termTableWidget->item( row, NOT_COLUMN_INDEX )->text();
    searchTerm.term = this->ui->termTableWidget->item( row, TERM_COLUMN_INDEX )->text().toStdString();
    
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
  
    item = new QTableWidgetItem( term->stemming ? "YES" : "NO" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem( row, STEM_COLUMN_INDEX, item );

    item = new QTableWidgetItem( 0 == row ? "" : term->andLogic ? "AND" : "OR" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem( row, AND_COLUMN_INDEX, item );

    item = new QTableWidgetItem( term->notLogic ? "NOT" : "" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem( row, NOT_COLUMN_INDEX, item );
    
    item = new QTableWidgetItem( term->term.c_str() );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
    this->ui->termTableWidget->setItem( row, TERM_COLUMN_INDEX, item );
  
    row++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotTermTableWidgetCellClicked( int row, int column )
{
  if( TERM_COLUMN_INDEX == column ) return;
  
  QTableWidgetItem *item = this->ui->termTableWidget->item( row, column );
  
  if( STEM_COLUMN_INDEX == column )
  {
    item->setText( "YES" == item->text() ? "NO" : "YES" );
  }
  else if( AND_COLUMN_INDEX == column && 0 != row )
  {
    item->setText( "AND" == item->text() ? "OR" : "AND" );
  }
  else if( NOT_COLUMN_INDEX == column )
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
  
  item = new QTableWidgetItem( "YES" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem( row, STEM_COLUMN_INDEX, item );

  item = new QTableWidgetItem( 0 == row ? "" : "AND" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem( row, AND_COLUMN_INDEX, item );

  item = new QTableWidgetItem( "" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem( row, NOT_COLUMN_INDEX, item );
  
  item = new QTableWidgetItem( "" );
  item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
  this->ui->termTableWidget->setItem( row, TERM_COLUMN_INDEX, item );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotRemovePushButton()
{
  this->ui->termTableWidget->removeRow( this->ui->termTableWidget->currentRow() );
}
