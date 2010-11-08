#ifndef ORLANDO_H
#define ORLANDO_H

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkGlyphSource2D.h" // for the glyph types
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;

class ovQMainWindowProgressCommand;
class ovOrlandoReader;
class ovRestrictGraph;
class QActionGroup;
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
  virtual void SetLayoutStrategy( const char* );

public slots:
  //event functions
  virtual void slotFileOpen();
  virtual void slotFileExit();

  virtual void slotSetBackgroundSolid();
  virtual void slotSetBackgroundTop();
  virtual void slotSetBackgroundBottom();

  virtual void slotSetVertexStyleToNone() { this->SetVertexStyle( VTK_NO_GLYPH ); }
  virtual void slotSetVertexStyleToVertex() { this->SetVertexStyle( VTK_VERTEX_GLYPH ); }
  virtual void slotSetVertexStyleToDash() { this->SetVertexStyle( VTK_DASH_GLYPH ); }
  virtual void slotSetVertexStyleToCross() { this->SetVertexStyle( VTK_CROSS_GLYPH ); }
  virtual void slotSetVertexStyleToThickcross() { this->SetVertexStyle( VTK_THICKCROSS_GLYPH ); }
  virtual void slotSetVertexStyleToTriangle() { this->SetVertexStyle( VTK_TRIANGLE_GLYPH ); }
  virtual void slotSetVertexStyleToSquare() { this->SetVertexStyle( VTK_SQUARE_GLYPH ); }
  virtual void slotSetVertexStyleToCircle() { this->SetVertexStyle( VTK_CIRCLE_GLYPH ); }
  virtual void slotSetVertexStyleToDiamond() { this->SetVertexStyle( VTK_DIAMOND_GLYPH ); }
  virtual void slotSetVertexStyleToArrow() { this->SetVertexStyle( VTK_ARROW_GLYPH ); }
  virtual void slotSetVertexStyleToThickarrow() { this->SetVertexStyle( VTK_THICKARROW_GLYPH ); }
  virtual void slotSetVertexStyleToHookedarrow() { this->SetVertexStyle( VTK_HOOKEDARROW_GLYPH ); }
  virtual void slotSetVertexStyleToEdgearrow() { this->SetVertexStyle( VTK_EDGEARROW_GLYPH ); }
  
  virtual void slotSetLayoutStrategyToRandom() { this->SetLayoutStrategy( "Random" ); }
  virtual void slotSetLayoutStrategyToForceDirected() { this->SetLayoutStrategy( "Force Directed" ); }
  virtual void slotSetLayoutStrategyToSimple2D() { this->SetLayoutStrategy( "Simple 2D" ); }
  virtual void slotSetLayoutStrategyToClustering2D() { this->SetLayoutStrategy( "Clustering 2D" ); }
  virtual void slotSetLayoutStrategyToCommunity2D() { this->SetLayoutStrategy( "Community 2D" ); }
  virtual void slotSetLayoutStrategyToFast2D() { this->SetLayoutStrategy( "Fast 2D" ); }
  virtual void slotSetLayoutStrategyToCircular() { this->SetLayoutStrategy( "Circular" ); }
  virtual void slotSetLayoutStrategyToTree() { this->SetLayoutStrategy( "Tree" ); }
  virtual void slotSetLayoutStrategyToCosmicTree() { this->SetLayoutStrategy( "Cosmic Tree" ); }
  virtual void slotSetLayoutStrategyToCone() { this->SetLayoutStrategy( "Cone" ); }
  virtual void slotSetLayoutStrategyToSpanTree() { this->SetLayoutStrategy( "Span Tree" ); }

  virtual void slotVertexSizeSliderValueChanged( int change );
  virtual void slotEdgeSizeSliderValueChanged( int change );
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

  // Action groups for radio-based menu items
  QActionGroup *vertexStyleActionGroup;
  QActionGroup *layoutStrategyActionGroup;
  
  vtkSmartPointer< ovQMainWindowProgressCommand > ProgressObserver;

  // Set to false to disable automatic updating of graph
  bool AutoUpdateGraphView;
};

#endif
