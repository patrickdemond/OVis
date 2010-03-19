#ifndef FONT_H
#define FONT_H

#include "ui_font.h"
#include "QVTKWidget.h"
#include <QComboBox>
#include "graph.h"

#define NUM_FONTS 3
#define NUM_FONT_STYLES 4
#define NUM_SIZES 20 

//dialog for tag selection
class Font: public QDialog, private Ui_FontDialog
{
  Q_OBJECT

public:
  
  //constructor
  
  Font(QWidget* parent = 0, Graph* grap = 0);
  //destructor
  ~Font(){};

  //functions
  void setupComboBoxes();

public slots:

  //slots for ok and cancel
  virtual void okToGo();
  virtual void cancel();
  virtual void changeFont(int index);
  virtual void changeFontStyle(int index);
  virtual void changeFontSize(int index);
  virtual void changeFontColour();

protected:

protected slots:

private:

  //global variables
  Graph* graph;
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
