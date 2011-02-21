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
ovQSearchDialog::ovQSearchDialog( QWidget* parent )
  : QDialog( parent )
{
  this->ui = new Ui_ovQSearchDialog;
  this->ui->setupUi( this );
  
  this->ui->termTableWidget->setColumnWidth( 0, 400 );
  
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
    ovString term = this->ui->termTableWidget->item( row, 0 )->text().toStdString();
    bool notLogic = "NOT" == this->ui->termTableWidget->item( row, 1 )->text();
    bool andLogic = "AND" == this->ui->termTableWidget->item( row, 2 )->text();
    
    phrase->Add( term, notLogic, andLogic );
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
    int column = 0;
  
    item = new QTableWidgetItem( term->term.c_str() );
    item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
    this->ui->termTableWidget->setItem( row, column++, item );
  
    item = new QTableWidgetItem( term->notLogic ? "NOT" : "" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem( row, column++, item );
    
    item = new QTableWidgetItem( 0 == row ? "" : term->andLogic ? "AND" : "OR" );
    item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    item->setFlags( Qt::ItemIsEnabled );
    this->ui->termTableWidget->setItem( row, column++, item );

    row++;
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotTermTableWidgetCellClicked( int row, int column )
{
  if( 0 == column ) return;
  
  QTableWidgetItem *item = this->ui->termTableWidget->item( row, column );
  
  if( 1 == column )
  {
    item->setText( "NOT" == item->text() ? "" : "NOT" );
  }
  else if( 2 == column && 0 != row )
  {
    item->setText( "AND" == item->text() ? "OR" : "AND" );
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotAddPushButton()
{
  int column = 0;
  int row = this->ui->termTableWidget->rowCount();
  this->ui->termTableWidget->insertRow( row );

  QTableWidgetItem *item;
  
  item = new QTableWidgetItem( "" );
  item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsEditable );
  this->ui->termTableWidget->setItem( row, column++, item );

  item = new QTableWidgetItem( "" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem( row, column++, item );
  
  item = new QTableWidgetItem( 0 == row ? "" : "AND" );
  item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
  item->setFlags( Qt::ItemIsEnabled );
  this->ui->termTableWidget->setItem( row, column++, item );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::slotRemovePushButton()
{
  this->ui->termTableWidget->removeRow( this->ui->termTableWidget->currentRow() );
}
