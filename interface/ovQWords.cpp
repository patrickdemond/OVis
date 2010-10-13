#include <qapplication.h>
#include <qfiledialog.h>
#include "ovQWords.h"

#include "source/ovGraph.h"

//tag constructor
ovQWords::ovQWords( ovGraph* g, QWidget* parent )
  : QDialog( parent )
{
  //set up the user interface
  setupUi( this );

  //connect the signals to the new slots
  connect( buttonBox, SIGNAL( accepted() ), this, SLOT( wordsOk() ));
  connect( buttonBox, SIGNAL( rejected() ), this, SLOT( wordsCancel() ));
  connect( pushButton, SIGNAL( pressed() ), this, SLOT( wordHelp() ));

  connect( comboBox, SIGNAL( activated( int )), this, SLOT( setIndex1( int )) );
  connect( comboBox_2, SIGNAL( activated( int )), this, SLOT( setIndex2( int )) );

  index1 = -1;
  index2 = -1;

  setUpCombo1();
  setUpCombo2();

  graph = g;
};

//if cancel is pressed
void ovQWords::wordsCancel()
{
  //hide this widget
  this->hide();
}

//if ok is pressed
void ovQWords::wordsOk()
{ 
  if( radioButton->isChecked() )
    {
      char* str3 = ( char* ) calloc( 100, sizeof( char ));
      sprintf( str3, lineEdit_3->text() );

      char* str4 = ( char* ) calloc( 100, sizeof( char ));
      sprintf( str4, lineEdit_4->text() );

      strcat( str3, "--" );
      strcat( str4, "--" );

      ovDate* dt1 = new ovDate( str3, true );
      ovDate* dt2 = new ovDate( str4, false );

      checkDates( dt1, dt2 );

      free( str3 );
      free( str4 );
    }
  else if( radioButton_2->isChecked() )
    {
      char* str3 = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( str3, comboBox->itemText( index1 ));

      list<ovPeriod*>::iterator it;
      for( it=historical.begin(); it!=historical.end(); it++ )
  {
    if( strcmp( (*it )->getTitle(), str3 ) == 0 )
      {
        checkDates( (*it )->getStartDate(), ( *it )->getEndDate() );
      }
  }

      free( str3 );
    }
  else if( radioButton_3->isChecked() )
    {
      char* str3 = ( char* ) calloc( 1000, sizeof( char ));
      sprintf( str3, comboBox_2->itemText( index2 ));
      
      list<ovPeriod*>::iterator it;
      for( it=monarch.begin(); it!=monarch.end(); it++ )
  {
    if( strcmp( (*it )->getTitle(), str3 ) == 0 )
      {
        checkDates( (*it )->getStartDate(), ( *it )->getEndDate() );
      }
  }

      free( str3 );
    }

  //get text from the line edit boxes
  char* str1 = ( char* ) calloc( 1000, sizeof( char ));
  sprintf( str1, lineEdit->text() );

  char* str2 = ( char* ) calloc( 1000, sizeof( char ));
  sprintf( str2, lineEdit_2->text() );

  if( str1 != NULL || str2 != NULL )
    {
      graph->include( str1 );
      graph->exclude( str2 );
    }

  free( str1 );
  free( str2 );

  if( checkBox->isChecked() )
    {
      graph->setEntriesOnly( true );
    }

  //hide this widget
  this->hide();
}

void ovQWords::wordHelp()
{
  ovQWordHelp wHelp;

  wHelp.exec();
}

void ovQWords::setUpCombo1()
{
  getHistorical( "resources/historical_dates.txt" );

  list<ovPeriod*>::iterator it;
  for( it=historical.begin(); it!=historical.end(); it++ )
    {
      comboBox->addItem( (*it )->getTitle() );
    }
}

void ovQWords::setUpCombo2()
{
  getMonarch( "resources/monarchs_dates.txt" );
  
  list<ovPeriod*>::iterator it;
  for( it=monarch.begin(); it!=monarch.end(); it++ )
    {
      comboBox_2->addItem( (*it )->getTitle() );
    }
}

void ovQWords::checkDates( ovDate* dt1, ovDate* dt2 )
{
  //printf( "CHECKING DATES" );
  //fflush( stdout );
  
  graph->includeDatesBtw( dt1->GetYear(), dt1->GetMonth(), dt1->GetDay(), dt2->GetYear(), dt2->GetMonth(), dt2->GetDay() );
}

void ovQWords::getHistorical( char* filename )
{
  ifstream file;

  file.open( filename, ios::in );

  if( file.is_open() )
    {
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );
      while( !file.eof() && line!=NULL )
  {    
    historical.push_back( new ovPeriod( line ));

    free( line );
    line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );
  }
      file.close();
    }
}

void ovQWords::getMonarch( char* filename )
{
  ifstream file;

  file.open( filename, ios::in );

  if( file.is_open() )
    {
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );
      while( !file.eof() && line!=NULL )
  {    
    monarch.push_back( new ovPeriod( line ));

    free( line );
    line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );
  }
      file.close();
    }
}

void ovQWords::setIndex1( int i )
{
  index1 = i;
}

void ovQWords::setIndex2( int i )
{
  index2 = i;
}
