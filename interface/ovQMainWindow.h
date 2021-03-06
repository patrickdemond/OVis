/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovQMainWindow.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/

#ifndef __ovQMainWindow_h
#define __ovQMainWindow_h

#include <QMainWindow>

#include "ovUtilities.h"
#include "vtkGlyphSource2D.h" // for the glyph types
#include "vtkSmartPointer.h"

class Ui_ovQMainWindow;

class ovQMainWindowProgressCommand;
class ovQMainWindowSelectionCommand;
class ovOrlandoReader;
class ovRestrictGraphFilter;
class ovSearchPhrase;
class ovSession;

class QActionGroup;
class QTreeWidgetItem;

class vtkAnnotationLink;
class vtkCornerAnnotation;
class ovGraphLayoutView;
class vtkStringArray;
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
  virtual void slotOpenData();
  virtual void slotTakeScreenshot();
  virtual void slotReCenterGraph();
  virtual void slotSetBackgroundSolid();
  virtual void slotSetAnnotation();
  virtual void slotShowAnnotation();

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
  
  virtual void slotSetSnapshotMagnificationTo1() { this->SnapshotMagnification = 1; }
  virtual void slotSetSnapshotMagnificationTo2() { this->SnapshotMagnification = 2; }
  virtual void slotSetSnapshotMagnificationTo3() { this->SnapshotMagnification = 3; }
  virtual void slotSetSnapshotMagnificationTo4() { this->SnapshotMagnification = 4; }
  virtual void slotSetSnapshotMagnificationTo5() { this->SnapshotMagnification = 5; }
  virtual void slotSetSnapshotMagnificationTo6() { this->SnapshotMagnification = 6; }
  virtual void slotSetSnapshotMagnificationTo7() { this->SnapshotMagnification = 7; }
  virtual void slotSetSnapshotMagnificationTo8() { this->SnapshotMagnification = 8; }
  virtual void slotSetSnapshotMagnificationTo9() { this->SnapshotMagnification = 9; }
  virtual void slotSetSnapshotMagnificationTo10() { this->SnapshotMagnification = 10; }
  
  virtual void slotOpenSession();
  virtual void slotReloadSession();
  virtual void slotSaveSession();
  virtual void slotSaveSessionAs();

  virtual void slotAbout();

  virtual void slotWritersCheckBoxStateChanged( int );
  virtual void slotOthersCheckBoxStateChanged( int );
  virtual void slotFemaleCheckBoxStateChanged( int );
  virtual void slotMaleCheckBoxStateChanged( int );
  virtual void slotBRWTypeCheckBoxStateChanged( int );
  virtual void slotWriterTypeCheckBoxStateChanged( int );
  virtual void slotIBRTypeCheckBoxStateChanged( int );
  virtual void slotVertexSizeSliderValueChanged( int );
  virtual void slotEdgeSizeSliderValueChanged( int );
  virtual void slotFadingFactorSliderValueChanged( int );
  virtual void slotAuthorVertexColorPushButtonClicked();
  virtual void slotAssociationVertexColorPushButtonClicked();
  virtual void slotSetTextSearchPushButtonClicked();
  virtual void slotClearTextSearchPushButtonClicked();
  virtual void slotSetAuthorSearchPushButtonClicked();
  virtual void slotClearAuthorSearchPushButtonClicked();
  virtual void slotSetDateSpanPushButtonClicked();
  virtual void slotClearDateSpanPushButtonClicked();
  virtual void slotTagTreeCheckAllButtonClicked();
  virtual void slotTagTreeUnCheckAllButtonClicked();
  /* NOTE: Functionality removed by request
  virtual void slotTagTreeCheckButtonClicked();
  virtual void slotTagTreeUnCheckButtonClicked();
  virtual void slotTagTreeExpandButtonClicked();
  virtual void slotTagTreeCollapseButtonClicked();
  */
  virtual void slotTagTreeItemChanged( QTreeWidgetItem*, int );
  virtual void slotTagTreeItemDoubleClicked( QTreeWidgetItem*, int );

protected:
  virtual void closeEvent( QCloseEvent *event );

  virtual void ReadSettings();
  virtual void WriteSettings();
  virtual bool MaybeSave();
  virtual void LoadData();
  virtual void ApplyStateToSession();
  virtual void ApplySessionToState();

  virtual void RenderGraph( bool resetCamera = false );
  virtual void GetTagList( ovTagVector* );
  virtual void SetTagList( ovTagVector* );
  virtual void GetActiveTags( vtkStringArray* );
  virtual void UpdateActiveTags();
  virtual void SetBackgroundColor( double rgba[4] );
  virtual void SetVertexStyle( int );
  virtual void SetLayoutStrategy( const ovString& );
  virtual void SetVertexSize( int );
  virtual void SetEdgeSize( int );
  virtual void SetFadingFactor( int );
  virtual void SetAuthorVertexColor( double rgba[4] );
  virtual void SetAssociationVertexColor( double rgba[4] );
  virtual void SetTextSearchPhrase( ovSearchPhrase*, bool narrow = false );
  virtual void SetAuthorSearchPhrase( ovSearchPhrase* );
  virtual void SetStartDate( const ovDate& );
  virtual void SetEndDate( const ovDate& );

  virtual ovString GetAnnotationText();

  vtkSmartPointer< ovSession > Session;
  vtkSmartPointer< ovGraphLayoutView > GraphLayoutView;
  vtkSmartPointer< vtkViewTheme > GraphLayoutViewTheme;
  vtkSmartPointer< vtkCornerAnnotation > TopAnnotation;
  vtkSmartPointer< vtkCornerAnnotation > BottomAnnotation;
  vtkSmartPointer< ovOrlandoReader > OrlandoReader;
  vtkSmartPointer< ovRestrictGraphFilter > RestrictGraphFilter;
  vtkSmartPointer< ovRestrictGraphFilter > SelectGraphFilter;
  vtkSmartPointer< ovQMainWindowProgressCommand > ProgressObserver;
  vtkSmartPointer< ovQMainWindowSelectionCommand > SelectionObserver;

  ovString CurrentDataFileName;
  ovString CurrentSessionFileName;
  ovString CurrentLayoutStrategy;
  ovString CustomAnnotationText;
  int SnapshotMagnification;
  bool IsLoadingData;
  bool IsLoadingSession;
  bool IsCheckingMultipleTags;
  bool SkipRenderGraph;
protected slots:

private:
  // Designer form
  Ui_ovQMainWindow *ui;

  // Action groups for radio-based menu items
  QActionGroup *vertexStyleActionGroup;
  QActionGroup *layoutStrategyActionGroup;
  QActionGroup *snapshotMagnificationActionGroup;
};

#endif
