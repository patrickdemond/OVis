#ifndef __ovQMainWindow_h
#define __ovQMainWindow_h

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkGlyphSource2D.h" // for the glyph types
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;

class ovQMainWindowProgressCommand;
class ovOrlandoReader;
class ovRestrictGraphFilter;
class QActionGroup;
class QTreeWidgetItem;
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
  
public slots:
  //event functions
  virtual void slotFileOpen();
  virtual void slotReCenterGraph();
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

  virtual void slotAuthorCheckBoxStateChanged( int );
  virtual void slotGenderComboBoxCurrentIndexChanged( const QString& );
  virtual void slotWriterComboBoxCurrentIndexChanged( const QString& );
  virtual void slotVertexSizeSliderValueChanged( int );
  virtual void slotEdgeSizeSliderValueChanged( int );
  virtual void slotAuthorVertexColorPushButtonClicked();
  virtual void slotAssociationVertexColorPushButtonClicked();
  virtual void slotStartLineEditTextChanged( const QString& );
  virtual void slotStartSetPushButtonClicked();
  virtual void slotEndLineEditTextChanged( const QString& );
  virtual void slotEndSetPushButtonClicked();
  virtual void slotTagTreeCheckButtonClicked();
  virtual void slotTagTreeUnCheckButtonClicked();
  virtual void slotTagTreeItemChanged( QTreeWidgetItem*, int );
  virtual void slotTagTreeItemDoubleClicked( QTreeWidgetItem*, int );

protected:
  virtual void readSettings();
  virtual void writeSettings();
  virtual bool maybeSave();
  virtual void closeEvent( QCloseEvent *event );

  virtual void RenderGraph( bool resetCamera = false );
  virtual void UpdateIncludeTags();
  virtual void SetVertexStyle( int );
  virtual void SetLayoutStrategy( const char* );

  vtkSmartPointer< vtkGraphLayoutView > GraphLayoutView;
  vtkSmartPointer< vtkViewTheme > GraphLayoutViewTheme;
  vtkSmartPointer< ovOrlandoReader > OrlandoReader;
  vtkSmartPointer< ovRestrictGraphFilter > RestrictGraphFilter;

protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  // Action groups for radio-based menu items
  QActionGroup *vertexStyleActionGroup;
  QActionGroup *layoutStrategyActionGroup;
  
  vtkSmartPointer< ovQMainWindowProgressCommand > ProgressObserver;

  // Set to false to disable automatic updating of graph
  bool AutoUpdateIncludeTags;
  bool SkipRenderGraph;
};

#endif
