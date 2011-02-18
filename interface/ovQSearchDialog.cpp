/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQSearchDialog.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovQSearchDialog.h"

#include "ui_ovQSearchDialog.h"

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
ovString ovQSearchDialog::getSearch()
{
  ovString search = "";
  QTableWidgetItem *item;
  int rows = this->ui->termTableWidget->rowCount();
  
  for( int row = 0; row < rows; row++ )
  {
    if( 0 != row )
    { // add the AND/OR and NOT terms
      item = this->ui->termTableWidget->item( row, 2 );
      search += " ";
      search += item->text().toStdString();
    }
    
    // add the NOT term, if needed
    item = this->ui->termTableWidget->item( row, 1 );
    if( "NOT" == item->text() ) search += " NOT";
    
    // add the search term, stripped of double-quotes
    ovString term = this->ui->termTableWidget->item( row, 0 )->text().toStdString();
    term.erase( vtkstd::remove( term.begin(), term.end(), '"' ), term.end() );
    search += " \"";
    search += term;
    search += '"';
  }

  // get rid of the space at the beginning of the string
  if( 1 < search.length() ) search = search.substr( 1 );

  return search;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovQSearchDialog::setSearch( const ovString &search )
{
  // TODO: implement
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
