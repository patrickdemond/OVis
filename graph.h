#ifndef GRAPH_H
#define GRAPH_H

#define NUM_OF_NAMES_C 30000
#define NUM_OF_TAGS 25
#define ARIAL 0
#define COURIER 1
#define TIMES 2
#define PNG 0
#define JPG 1
#define BMP 2
#define TIF 3

#include <qapplication.h>
#include "ui_uiOrlando4.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <list>
#include "vtkWindowToImageFilter.h"
#include "vtkImageWriter.h"
#include "vtkPNGWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkBMPWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkMapper2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"
#include "vtkImagePlaneWidget.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkLineWidget.h"
#include "vtkTestUtilities.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"
#include "vtkLineSource.h"
#include "vtkConeSource.h"
#include "vtkCubeSource.h"
#include "vtkRegularPolygonSource.h"
#include "vtkInteractorStyleUser.h"
#include "vtkWorldPointPicker.h"
#include "vtkTextSource.h"
#include "vtkTextMapper.h"
#include "vtkTextActor.h"
#include "vtkCursor3D.h"
#include "vtkImageMapper.h"
#include "vtkImageActor.h"
#include "vtkTextActor3D.h"
#include "vtkFollower.h"
#include "vtkPolyDataMapper.h"
#include "vtkVectorText.h"
#include "vtkMatrix4x4.h"
#include "vtkCaptionActor2D.h"
#include "vtkLinearTransform.h"
#include "node.h"
#include "name.h"
#include "edge.h"
#include "QVTKWidget.h"
#include <QAbstractItemModel>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtGui>
#include <QProgressBar>

using namespace std;

class Graph 
{

public:
  Graph(vtkRenderWindow* wind, QVTKInteractor* interact, QListWidget* lst, QLabel* label, QLineEdit* line, QWidget* widge, QPushButton* button, QLabel* lab1, QLabel* lab2, vtkRenderWindow* wind2, QVTKInteractor* interact2, QProgressBar* pBar, QListWidget* tagLst, QWidget* mainWind, QWidget* tagWind);
  void saveFile(char* filename);
  void loadFile(char* filename);
  void loadXML(char* filename);
  void renderWin();
  void drawGraph();
  void saveInc(char* filename);
  void loadInc(char* filename);
  void saveExc(char* filename);
  void loadExc(char* filename);
  void saveNamesOn(char* filename);
  void loadNamesOn(char* filename);
  void saveTagsUsed(char* filename);
  void loadTagsUsed(char* filename);
  void saveTogOn(char* filename);
  void loadTogOn(char* filename);
  void saveTogCon(char* filename);
  void loadTogCon(char* filename);
  void saveTagsOn(char* filename);
  void loadTagsOn(char* filename);
  void SaveNodePos(char* filename);
  void GetNodePos(char* filename);
  int nodeAtPos(int a, int b, bool setSel);
  void moveNode(int a, int b);
  void changeToPos(int a, int b);
  bool findAllPaths(int startNode, int nodeToFind, list<int> pth, int cutoff, bool display);
  bool findIfPath(int startNode, int nodeToFind, int cutoff);
  void findPath(int a, int b);
  void findPathBtw(char* x, char* y);
  int findNodes(int a, list<int> path, int b, int graph2[]);
  void highlightOn();
  void highlight(int a, int b);
  void drawHighlighted();
  void toggleOn();
  void toggle(int a, int b);
  void drawToggled();
  void redrawGraph();
  void displayPathInfo(int a, int b, list<int> path);
  void displayNdInfo(int a, int b);
  bool empty(int graph2[]);
  void GetEntry(char* filename);
  void includeOnlyEntries();
  int isEntry(int n);
  void printPath(list<int> path);
  void windowSetup();
  void drawEdges();
  void drawKeys();
  void drawKey(float a, float b, float c, float d, double* col, char* nam, bool on);
  void tagTouched(int x, int y);
  void drawText(float dt, char* nam, int offset, bool col);
  void textSetUp();
  void rendSetUp();
  void select();
  void turnOnOffTag(int tagNum);
  void display();
  void resetSearch();
  void search(char* key);
  void done();
  double *getColor(int dt);
  char* stristr(char* strToSearch, char* searchStr);
  vtkRenderWindow* getWindow();
  vtkRenderWindowInteractor* getInteractor();
  void setSelected(char* sel);
  void setToggle(bool tog);
  void showName(int x, int y);
  void destroyName();
  void highlightNode(int x, int y);
  void allNamesOn(bool all);
  void allNamesOff(bool nw);
  void nameOnOff(bool on, int a, int b);
  void nameOnOff(bool on, char* nm);
  void initNames();
  void changeInteractorToGraph();
  void changeInteractorToCamera();
  bool isInteractGraph();
  void tagChoices();
  void setTags(list<char*> lst);
  void popUpPath();
  void initialize(char* filename);
  void setMode(char a);
  char getMode();
  bool getLoad();
  void tagWindowOn();
  void textWindowOn();
  void namesFromEntry(char* en, QComboBox* cb);
  void tagsFromNameEntry(char* en, char* nm, QComboBox* cb);
  void showXMLEntry(char* en, char* nm, char* tg, QTextBrowser* tb);
  int findIndexFromName(char* nm);
  bool includeLine(char* ln);
  void include(char* s);
  void exclude(char* s);
  void setEntriesOnly(bool b);
  list<char*> wordCases(char* s);
  void drawEdge(int stInd, int endInd, bool alpha, int tag);
  vtkActor* drawNode(int ind, bool alpha, vtkActor* actor, double colR, double colG, double colB);
  void drawEdgesForNode(int ind, bool alpha);
  void drawFadedEdges();
  void resetGraphCons();
  void removeEdgesForNode(int nd);
  void removeNode(int nd);
  bool hasEdgeBetween(int nd1, int nd2, int tag);
  void includeDatesBtw(int bY, int bM, int bD, int dY, int dM, int dD);
  bool includeByTime(char* stdName);
  void saveTagColors(char* filename);
  void setDefaultColors(char* filename);
  void setNewTagCol(int ind, int r, int g, int b);
  void mergeEdges();
  void setCaptionItalic(bool b);
  void setCaptionBold(bool b);
  void setCaptionSize(int sz);
  void setCaptionFont(int fnt);
  void setCaptionColour(int r, int g, int b);
  bool getCaptionItalic();
  bool getCaptionBold();
  int getCaptionSize();
  int getCaptionFont();
  int getCaptionRed();
  int getCaptionGreen();
  int getCaptionBlue(); 
  void redrawNameTags();
  void deselect(int x, int y);
  void setTitleText();
  void saveScreenshot(char* filename, int filetype);

private:  
  int lineNum[NUM_OF_NAMES_C];
  QWidget* tagWindow;  
  int deathYear;
  int deathMonth;
  int deathDay;
  int birthYear;
  int birthMonth;
  int birthDay;
  bool load;
  int pathS;
  int pathF;
  char mode;
  char prevMode;
  char* orlandoDataName;
  int NUM_OF_NAMES;
  bool found;
  bool donePth[NUM_OF_NAMES_C];
  list<int> tagsUsed;
  int numOfTags;
  list<int> inPath;
  bool pathConnected[NUM_OF_NAMES_C];
  bool interactGraph;
  bool namesAllowedOn;
  bool nameOn[NUM_OF_NAMES_C];
  vtkCaptionActor2D* nameText[NUM_OF_NAMES_C];
  bool pathTrue;
  QProgressBar* progBar;
  vtkCaptionActor2D* txtAct;
  bool toggleTrue;
  vtkRenderer* rend2;
  list<char*> searchStrings;
  int length;
  bool con[NUM_OF_NAMES_C][NUM_OF_NAMES_C];
  bool ndCon[NUM_OF_NAMES_C];
  char* tags [NUM_OF_TAGS+1];
  bool toggleConnected[NUM_OF_NAMES_C];
  list<int> toggledOn;
  bool tagOn[NUM_OF_TAGS+1];
  int ndToMove;
  char* names[NUM_OF_NAMES_C];
  char searchString[1000];
  list<int> entries;
  int distPairs;
  int numPairs;
  int searchCnt;
  double cutOff;
  float cutoff;
  int VAR;
  int x;
  int y;
  int z;
  int selected;
  int oldSelected;
  Node* graph1[NUM_OF_NAMES_C];
  int* stInd;  int* endInd;
  int* nameInd;
  QPushButton* searchButton;
  QWidget* orland;
  QLabel* label1;
  QLabel* label2;
  QLabel* labelSelected;
  QLineEdit* searchLine;
  QListWidget* listWidge;
  QListWidget* tagList;
  vtkRenderer *rend1;
  vtkRenderer *rend; 
  vtkRenderer *rendText;
  vtkRenderWindow* window2;
  vtkRenderWindow *window;
  QWidget* mainWin;
  QVTKInteractor* inter;
  QVTKInteractor* inter2;
  vtkActor *selActor;
  vtkCamera* cam; 
  vtkActor* mouseActor;
  vtkActor* highlightActor;
  bool highlighted;
  bool toggedOn[NUM_OF_NAMES_C];
  int lastHighlighted;
  vtkSphereSource* sphere1;
  vtkPolyDataMapper* mapper1;
  vtkLineSource* line1;  
  vtkPolyDataMapper* mapper2;
  double tagCols[NUM_OF_TAGS+1][3];
  bool onlyEntries;
  bool captionBold;
  bool captionItalic;
  int captionFont; //0-ARIAL, 1-COURIER, 2-TIMES
  int captionSize; 
  int captionRed;
  int captionGreen;
  int captionBlue;
  char inclWords[1000];
  char exclWords[1000];
  int edgeCount;
};

#endif
