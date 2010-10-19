#include <qapplication.h>
#include <qfiledialog.h>
#include <QColorDialog>
#include "ovQTags.h"

//tag constructor
ovQTags::ovQTags( QWidget* parent )
  : QDialog( parent )
{
  //set up the user interface
  setupUi( this );

  //connect the signals to the new slots
  connect( buttonBox, SIGNAL( accepted() ), this, SLOT( tagOk() ));
  connect( buttonBox, SIGNAL( rejected() ), this, SLOT( tagCancel() ));
  connect( listWidget, SIGNAL( itemDoubleClicked( QListWidgetItem* )), this, SLOT( changeTagCol( QListWidgetItem* )) );
  connect( toolButton, SIGNAL( pressed() ), this, SLOT( loadCols() ));
  connect( toolButton_2, SIGNAL( pressed() ), this, SLOT( saveCols() ));
  connect( listWidget, SIGNAL( itemClicked( QListWidgetItem* )), this, SLOT( changeSelPal( QListWidgetItem* )) );
  connect( checkBox, SIGNAL( clicked() ), this, SLOT( checkUncheckAll() ));

  //turn on extended selection for the list
  //listWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );

  /*  QPalette pal = listWidget->palette();
  pal.setBrush( QPalette::Highlight, Qt::gray );
  //pal.setAlpha( 10 );
  listWidget->setPalette( pal );*/

};

void ovQTags::checkUncheckAll()
{
  bool checked = checkBox->isChecked();

  if( checked )
    {
      for( int i=0; i<listWidget->count(); i++ )
  {
    ( listWidget->item( i ))->setCheckState( Qt::Checked );
  }
    }
  else 
    {
      for( int i=0; i<listWidget->count(); i++ )
  {
    ( listWidget->item( i ))->setCheckState( Qt::Unchecked );
  }
    }
}

void ovQTags::changeSelPal( QListWidgetItem* tagToChange )
{  
  /*//QPalette pal = listWidget->palette();
  
  int index = listWidget->row( tagToChange );
            
  double* oldCol = graph->getColor( index );
  
  //QColor* col = new QColor();
  //col->setRed( (int )( oldCol[0]*255 ));
  //col->setGreen( (int )( oldCol[1]*255 ));
  //col->setBlue( (int )( oldCol[2]*255 ));
  //col->setAlpha();
  //pal.setBrush( QPalette::HighlightedText, *col );
  //listWidget->setPalette( pal );
  
  listWidget->setStyleSheet( "*{selection-background-color: rgb( " + QString::number( oldCol[0]*255 ) + ", " + QString::number( oldCol[1]*255 ) + ", " + QString::number( oldCol[2]*255 ) + " )}" );
  */

  if( tagToChange->checkState() )
    {
      tagToChange->setCheckState( Qt::Unchecked );
    }
  else
    {
      tagToChange->setCheckState( Qt::Checked );
    }
}

void ovQTags::changeTagCol( QListWidgetItem* tagToChange )
{
  int index = listWidget->row( tagToChange );

  //double* oldCol = graph->getColor( index );
  
  QColor* tagCol = new QColor();
  tagCol->setRed( (int )( 255 ) ); //oldCol[0]*255 ));
  tagCol->setGreen( (int )( 255 ) ); //oldCol[1]*255 ));
  tagCol->setBlue( (int )( 255 ) ); //oldCol[2]*255 ));
  
  QColor col = QColorDialog::getColor( *tagCol, this );

  if( col.red() != col.blue() != col.green() != 0 )
    {
      //graph->setNewTagCol( index, col.red(), col.green(), col.blue() );
      tagToChange->setBackgroundColor( col );
    }
}

void ovQTags::loadCols()
{
  QString str = QFileDialog::getOpenFileName( this, tr( "Open File" ), "resources", tr( "Tag Colour Files ( *.tagCols )" ));

  if( str != "" )
    {
      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );
      //graph->setDefaultColors( fname );
    }

  for( int i=0; i<listWidget->count(); i++ )
    {
      //double* col = graph->getColor( i );
  
      QColor* tagCol = new QColor();
      tagCol->setRed( (int )( 255 ) ); // col[0]*255 ));
      tagCol->setGreen( (int )( 255 ) ); // col[1]*255 ));
      tagCol->setBlue( (int )( 255 ) ); // col[2]*255 ));

      ( listWidget->item( i ))->setBackground( *tagCol );
    }
}

void ovQTags::saveCols()
{
  QString str = QFileDialog::getSaveFileName( this, tr( "Save File" ), "resources", tr( "Tag Colour Files ( *.tagCols )" ));

  if( str != "" )
    {
      char* fname = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( fname, str );
      if( strstr( fname, ".tagCols" ) == NULL )
  {
    strcat( fname, ".tagCols" );
  }

      //graph->saveTagColors( fname );
    }
}

//return tag list widget
QListWidget* ovQTags::getTagList()
{
  return listWidget;
}

//return true if continue is on, false otherwise
bool ovQTags::getContinue()
{
  return continueTrue;
}

//if cancel is pressed
void ovQTags::tagCancel()
{
  //set continue to false
  continueTrue = false;

  //hide this widget
  this->hide();
}

//if ok is pressed
void ovQTags::tagOk()
{    
  //list<char*> to hold the tags that are turned on
  //list<char*> strs;


  QList<QListWidgetItem*> tgs;

  for( int i=0; i<listWidget->count(); i++ )
    {
      if( listWidget->item( i )->checkState() )
  {
    tgs.push_back( listWidget->item( i ));
  }
    }

  //iterator to go through list
  QList<QListWidgetItem*>::iterator it;

  //for the selected items in the list
  for( it=tgs.begin(); it!=tgs.end(); it++ )
    {
      //char* to hold item
      char* str = ( char* ) calloc( 1000, sizeof( char ));
      //print text of item to string
      sprintf( str, ( *it )->text() );
      //push string into list
      //strs.push_back( str );   
      }

  //set tags to the strings
  //graph->setTags( strs );

  //set continue to true
  continueTrue = true;
  
  //hide this widget
  this->hide();
}
