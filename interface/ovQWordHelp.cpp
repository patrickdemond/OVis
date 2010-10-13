#include <qapplication.h>
#include <qfiledialog.h>
#include "ovQWordHelp.h"

//tag constructor
ovQWordHelp::ovQWordHelp( QWidget* parent )
  : QDialog( parent )
{
  //set up the user interface
  setupUi( this );

  //connect the signals to the new slots
  connect( pushButton, SIGNAL( pressed() ), this, SLOT( wordHelpOk() ));
};

//if ok is pressed
void ovQWordHelp::wordHelpOk()
{ 
  //hide this widget
  this->hide();
}
