#ifndef FONT_H
#define FONT_H

#include "ui_ovQFont.h"
#include "QVTKWidget.h"
#include <QComboBox>

class QVTKInteractor;
class vtkRenderWindow;
class vtkRenderer;
class vtkCaptionActor2D;
class vtkTextProperty;
class QColor;

#define NUM_FONTS 3
#define NUM_FONT_STYLES 4
#define NUM_SIZES 20 

//dialog for tag selection
class ovQFont: public QDialog, private Ui_FontDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  ovQFont( QWidget* parent = 0 );
  //destructor
  ~ovQFont(){};

  //functions
  void setupComboBoxes();

public slots:

  //slots for ok and cancel
  virtual void okToGo();
  virtual void cancel();
  virtual void changeFont( int index );
  virtual void changeFontStyle( int index );
  virtual void changeFontSize( int sz );
  virtual void changeFontColour();

protected:

protected slots:

private:

  //global variables
  char* fonts[NUM_FONTS];
  char* fontStyles[NUM_FONT_STYLES];
  int sizes[NUM_SIZES];
  QVTKInteractor* inter;
  vtkRenderWindow* fontWin;
  vtkRenderer* rend;
  vtkCaptionActor2D* caption;
  vtkTextProperty* prop;
  QColor* col;
  int size;
  int font;
  bool bold;
  bool italic;
};

#endif
