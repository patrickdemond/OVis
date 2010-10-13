#include <qapplication.h>
#include <qfiledialog.h>
#include "ovQScreenshot.h"

//screenshot constructor
ovQScreenshot::ovQScreenshot( QWidget* parent )
  : QDialog( parent )
{
  //set up the user interface
  setupUi( this );

  //connect the signals to the new slots
  connect( pushButton, SIGNAL( clicked() ), this, SLOT( okClicked() ));

  populateComboBox();
};

void ovQScreenshot::populateComboBox()
{
  comboBox->insertItem( 0, "1x, 100%" );
  comboBox->insertItem( 1, "2x, 200%" );
  comboBox->insertItem( 2, "3x, 300%" );
  comboBox->insertItem( 3, "4x, 400%" );
  comboBox->insertItem( 4, "5x, 500%" );
  comboBox->insertItem( 5, "6x, 600%" );
  comboBox->insertItem( 6, "7x, 700%" );
  comboBox->insertItem( 7, "8x, 800%" );
  comboBox->insertItem( 8, "9x, 900%" );
  comboBox->insertItem( 9, "10x, 1000%" );
  comboBox->insertItem( 10, "11x, 1100%" );
  comboBox->insertItem( 11, "12x, 1200%" );
  comboBox->insertItem( 12, "13x, 1300%" );
  comboBox->insertItem( 13, "14x, 1400%" );
  comboBox->insertItem( 14, "15x, 1500%" );

  comboBox->setCurrentIndex( 0 );
}

void ovQScreenshot::okClicked()
{
  magnification = comboBox->currentIndex()+1;
  this->hide();
}

int ovQScreenshot::getMagnification()
{
  return magnification;
}

