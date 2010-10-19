#include <qapplication.h>
#include <qfiledialog.h>
#include "ovQFont.h"

#include <QColorDialog>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"

#include "vtkProperty2D.h"

//tag constructor
ovQFont::ovQFont( QWidget* parent )
  : QDialog( parent )
{
  //set up the user interface
  setupUi( this );

  fonts[0] = "Arial";
  fonts[1] = "Courier";
  fonts[2] = "Times";

  fontStyles[0] = "Normal";
  fontStyles[1] = "Bold";
  fontStyles[2] = "Italic";
  fontStyles[3] = "Bold Italic";

  int sz = 4;
  int index = -1;
  int capSize = 10; //graph->getCaptionSize();
  size = capSize;

  for( int i=0; i<NUM_SIZES; i++ )
    {
      sizes[i] = sz;

      if( sz == capSize )
  {
    index = i;
  }
      
      sz+=2;      
    }

  setupComboBoxes();

  comboBox_3->setCurrentIndex( index );

  col = new QColor( 1, 1, 1 ); //graph->getCaptionRed(), graph->getCaptionGreen(), graph->getCaptionBlue() );

  inter = qvtkWidget->GetInteractor();
  //inter->Disable();

  fontWin = qvtkWidget->GetRenderWindow();

  rend = vtkRenderer::New();
  rend->SetBackground( 0.2, 0.2, 0.2 );
  rend->SetViewport( 0.0, 0.0, 1.0, 1.0 );
  
  caption = vtkCaptionActor2D::New();
  caption->SetAttachmentPoint( -0.8, -0.1, 0.0 );
  caption->SetPadding( 0 );
  caption->SetCaption( "AaBbCc 123" );
  caption->BorderOff();
  
  caption->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );

  prop = caption->GetTextActor()->GetTextProperty();
  
  if( false ) // graph->getCaptionBold() )
    {
      prop->BoldOn();
      bold = true;
    }
  else
    {
      prop->BoldOff();
      bold = false;
    }

  if( false ) // graph->getCaptionItalic() )
    {
      prop->ItalicOn();
      italic = true;
    }
  else
    {
      prop->ItalicOff();
      italic = false;
    }

  if( bold )
    {
      if( italic )
  {
    comboBox_2->setCurrentIndex( 3 );
  }
      else
  {
    comboBox_2->setCurrentIndex( 1 );
  }
    }
  else if( italic )
    {
      comboBox_2->setCurrentIndex( 2 );
    }
  else
    {
      comboBox_2->setCurrentIndex( 0 );
    }

  int x = col->red();
  int y = col->green();
  int z = col->blue();

  //prop->SetFontSize( graph->getCaptionSize() );

  prop->ShadowOff();
  prop->SetColor( x/255.0, y/255.0, z/255.0 );
/*
  switch( graph->getCaptionFont() )
    {
    case ARIAL:
      prop->SetFontFamilyToArial();
      font = ARIAL;
      comboBox->setCurrentIndex( ARIAL );
      break;
    case COURIER:
      prop->SetFontFamilyToCourier();
      font = COURIER;
      comboBox->setCurrentIndex( COURIER );
      break;
    case TIMES:
      prop->SetFontFamilyToTimes();
      font = TIMES;
      comboBox->setCurrentIndex( TIMES );
      break;
    }
*/  
  rend->AddActor( caption );

  fontWin->AddRenderer( rend );

  fontWin->Render();

  pushButton->setStyleSheet( "* { background-color: rgb( " + QString::number( x ) + ", " + QString::number( y ) + ", " + QString::number( z ) + " )}" );

  //connect the signals to the new slots
  connect( comboBox, SIGNAL( currentIndexChanged( int )), this, SLOT( changeFont( int )) );
  connect( comboBox_2, SIGNAL( currentIndexChanged( int )), this, SLOT( changeFontStyle( int )) );
  connect( comboBox_3, SIGNAL( currentIndexChanged( int )), this, SLOT( changeFontSize( int )) );
  connect( pushButton, SIGNAL( pressed() ), this, SLOT( changeFontColour() ));
  connect( buttonBox, SIGNAL( accepted() ), this, SLOT( okToGo() ));
  connect( buttonBox, SIGNAL( rejected() ), this, SLOT( cancel() ));
}

//changes the font colour
void ovQFont::changeFontColour()
{
   QColor col2 = QColorDialog::getColor( *col, this );

   int x = col2.red();
   int y = col2.green();
   int z = col2.blue();
  
   pushButton->setStyleSheet( "* { background-color: rgb( " + QString::number( x ) + ", " + QString::number( y ) + ", " + QString::number( z ) + " )}" );

   col->setRed( col2.red() );
   col->setGreen( col2.green() );
   col->setBlue( col2.blue() );
   
   prop->SetColor( col->red()/255.0, col->green()/255.0, col->blue()/255.0 );
   
   fontWin->Render();
}

//changes the font to the index
//0-ARIAL, 1-COURIER, 2-TIMES
void ovQFont::changeFont( int index )
{
  switch( index )
    {
      //arial
      case 0:
  prop->SetFontFamilyToArial();
  break;
    
      //courier
      case 1:
  prop->SetFontFamilyToCourier();
  break;
    
      //times;
      case 2:
  prop->SetFontFamilyToTimes();
  break;
    }
  
  font = index;

  fontWin->Render();
}

//changes the font style to the index
//0-normal 1-bold 2-italic 3-bold & italic
void ovQFont::changeFontStyle( int index )
{
  switch( index )
    {
      //normal
      case 0:
  prop->BoldOff();
  prop->ItalicOff();
  bold = false;
  italic = false;
  break;
    
      //bold
      case 1:
  prop->BoldOn();
  prop->ItalicOff();
  bold = true;
  italic = false;
  break;
    
      //italic
      case 2:
  prop->ItalicOn();
  prop->BoldOff();
  bold = false;
  italic = true;
  break;
  
      //bold italic
      case 3:
  prop->BoldOn();
  prop->ItalicOn();
  bold = true;
  italic = true;
  break;
    }

  fontWin->Render();
}

//change the font size to the sz passed in 
void ovQFont::changeFontSize( int sz )
{
  int i = sz*2 + 4;
  prop->SetFontSize( i );

  size = i;

  fontWin->Render();
}

//set up the combo boxes
void ovQFont::setupComboBoxes()
{
  for( int i=0; i<NUM_FONTS; i++ )
    {
      comboBox->addItem( fonts[i] );
    }

  for( int i=0; i<NUM_FONT_STYLES; i++ )
    {
      comboBox_2->addItem( fontStyles[i] );
    }

  for( int i=0; i<NUM_SIZES; i++ )
    {
      char* tmp = ( char* ) calloc( 10, sizeof( char ));
      sprintf( tmp, "%i", sizes[i] );
      comboBox_3->addItem( tmp );
    }
}

//
void ovQFont::okToGo()
{
  printf( "DONE" );
  fflush( stdout );

  //graph->setCaptionColour( col->red(), col->green(), col->blue() );
  //graph->setCaptionBold( bold );
  //graph->setCaptionItalic( italic );
  //graph->setCaptionSize( size );
  //graph->setCaptionFont( font );

  //graph->redrawNameTags();

  this->done( 1 );
}
 
void ovQFont::cancel()
{  
  this->done( 1 );
}
 
