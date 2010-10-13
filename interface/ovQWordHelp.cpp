#include <qapplication.h>
#include <qfiledialog.h>
#include "WordHelp.h"

//tag constructor
WordHelp::WordHelp(QWidget* parent)
  : QDialog(parent)
{
  //set up the user interface
  setupUi(this);

  //connect the signals to the new slots
  connect(pushButton, SIGNAL(pressed()), this, SLOT(wordHelpOk()));
};

//if ok is pressed
void WordHelp::wordHelpOk()
{ 
  //hide this widget
  this->hide();
}
