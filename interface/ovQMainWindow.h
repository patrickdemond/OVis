#ifndef ORLANDO_H
#define ORLANDO_H

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkGlyphSource2D.h" // for the glyph types
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;

class ovOrlandoReader;
class ovRestrictGraph;
class QListWidgetItem;
class vtkGraphLayoutView;
class vtkViewTheme;

class ovQMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  //constructor
  ovQMainWindow( QWidget* parent = 0 );
  //destructor
  ~ovQMainWindow();
  
  // Update the graph view based on the current state of the UI
  virtual void UpdateGraphView( bool resetCamera = false );
  virtual void SetVertexStyle( int );

public slots:
  //event functions
  virtual void slotFileOpen();
  virtual void slotFileExit();
  virtual void slotViewSetBackgroundSolid();
  virtual void slotViewSetBackgroundTop();
  virtual void slotViewSetBackgroundBottom();
  virtual void slotViewSetVertexStyleNone() { this->SetVertexStyle( VTK_NO_GLYPH ); }
  virtual void slotViewSetVertexStyleVertex() { this->SetVertexStyle( VTK_VERTEX_GLYPH ); }
  virtual void slotViewSetVertexStyleDash() { this->SetVertexStyle( VTK_DASH_GLYPH ); }
  virtual void slotViewSetVertexStyleCross() { this->SetVertexStyle( VTK_CROSS_GLYPH ); }
  virtual void slotViewSetVertexStyleThickcross() { this->SetVertexStyle( VTK_THICKCROSS_GLYPH ); }
  virtual void slotViewSetVertexStyleTriangle() { this->SetVertexStyle( VTK_TRIANGLE_GLYPH ); }
  virtual void slotViewSetVertexStyleSquare() { this->SetVertexStyle( VTK_SQUARE_GLYPH ); }
  virtual void slotViewSetVertexStyleCircle() { this->SetVertexStyle( VTK_CIRCLE_GLYPH ); }
  virtual void slotViewSetVertexStyleDiamond() { this->SetVertexStyle( VTK_DIAMOND_GLYPH ); }
  virtual void slotViewSetVertexStyleArrow() { this->SetVertexStyle( VTK_ARROW_GLYPH ); }
  virtual void slotViewSetVertexStyleThickarrow() { this->SetVertexStyle( VTK_THICKARROW_GLYPH ); }
  virtual void slotViewSetVertexStyleHookedarrow() { this->SetVertexStyle( VTK_HOOKEDARROW_GLYPH ); }
  virtual void slotViewSetVertexStyleEdgearrow() { this->SetVertexStyle( VTK_EDGEARROW_GLYPH ); }

  virtual void slotTagListCheckAllButtonClicked();
  virtual void slotTagListCheckNoneButtonClicked();
  virtual void slotTagListItemChanged( QListWidgetItem* );
  virtual void slotTagListPresetComboBoxIndexChanged( int );

protected:
  vtkSmartPointer< vtkGraphLayoutView > GraphLayoutView;
  vtkSmartPointer< vtkViewTheme > GraphLayoutViewTheme;
  vtkSmartPointer< ovOrlandoReader > OrlandoReader;
  vtkSmartPointer< ovRestrictGraph > RestrictFilter;

protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  // Set to false to disable automatic updating of graph
  bool AutoUpdateGraphView;
};

#endif
