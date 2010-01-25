//Created by Jennifer Berberich for the ORLANDO Project
//If you have any questions you can contact me... Jenn_b19@hotmail.com

//#define RESOLUTION_X 6
//#define RESOLUTION_Y 6

#include "graph.h"
#include "vtkCommand.h"
#include "Orlando4.h"
#include "Path.h"
#include "Tags.h"
#include "userStyle.h"
#include "userStyleTags.h"
#include "Text.h"

//constructor for the graph
//passed in all objects necessary from GUI
Graph::Graph(vtkRenderWindow* wind, QVTKInteractor* interact, QListWidget* lst, QLabel* label,QLineEdit* line, QWidget* widge, QPushButton* button, QLabel* lab1, QLabel* lab2, vtkRenderWindow* wind2, QVTKInteractor* interact2, QProgressBar* pBar, QListWidget* tagLst, QWidget* mainWind, QWidget* tagWind)
{
  //initialize global variables
  length = 0;
  cutoff = 0;
  cutOff = 0.0;
  x = 0;
  y = 0;
  z = 0;
  progBar = pBar;
  inter2 = interact2;
  window2 = wind2;
  label1 = lab1;
  label2 = lab2;
  searchButton = button;
  orland = widge;
  searchLine = line;
  labelSelected = label;
  listWidge = lst;
  rend1 = vtkRenderer::New();
  rend = vtkRenderer::New();
  window = wind;
  window->AddRenderer(rend1);
  window->AddRenderer(rend);
  inter = interact;
  cam = vtkCamera::New();  
  rendText = vtkRenderer::New();
  mouseActor = vtkActor::New();
  rend->AddActor(mouseActor);
  selActor = vtkActor::New();
  selected = -1;
  tagList = tagLst;
  mainWin = mainWind;
  tagWindow = tagWind;
  lastHighlighted = -1;
  highlighted = false;

  //create a sphere
  sphere1 = vtkSphereSource::New();
  sphere1->SetRadius(2.5);
  //vtkSphere->SetCenter(x,y,z);
  
  //create the mapper
  mapper1 = vtkPolyDataMapper::New();
  mapper1->SetInput(sphere1->GetOutput());
  //initialize the tag names

  sphere1->Delete();

  //initialize the tag names
  tags[0] = "PRODUCTION";
  tags[1] = "RECEPTION";
  tags[2] = "TEXTUALFEATURES";
  tags[3] = "MEMBER";
  tags[4] = "FAMILY";
  tags[5] = "FRIENDSANDASSOCIATES";
  tags[6] = "INTIMATERELATIONSHIPS";
  tags[7] = "LEISUREANDSOCIETY";
  tags[8] = "RELATIONSWITHPUBLISHERS";
  tags[9] = "OCCUPATION";
  tags[10] = "POLITICS";
  tags[11] = "TEXT";
  tags[12] = "PANTHOLOGIZATION";
  tags[13] = "PAUTHORSHIP";
  tags[14] = "PATTITUDES";
  tags[15] = "PDEDICATION";
  tags[16] = "PINFLUENCESHER";
  tags[17] = "RPENALTIES";
  tags[18] = "RRECOGNITIONS";
  tags[19] = "TINTERTEXTUALITY";
  tags[20] = "PLITERARYSCHOOLS";
  tags[21] = "TTHEMEORTOPIC";
  tags[22] = "RRESPONSES";
  tags[23] = "RFICTIONALIZATION";
  tags[24] = "RSHEINFLUENCED";
  tags[25] = "WHOLE ENTRY";

  //set sorting enabled for the list to get it to be alphabetized
  listWidge->setSortingEnabled(true);
  
  //for the number of tags 
  for(int i=0; i<NUM_OF_TAGS+1; i++)
    {
      //set tagOn to false
      tagOn[i] = false;
    }

  namesAllowedOn = false;

  rendSetUp();

//create new renderer and add it to tag legend window
  rend2 = vtkRenderer::New();
  rend2->SetViewport(0.0/600,0.0/800.0,600.0/600.0,800.0/800.0);
  rend2->SetBackground(0.2,0.2,0.2);
  window2->AddRenderer(rend2);

  renderWin();
  window2->Render();

  Orlando* orl = (Orlando*) orland;
  orl->setGraph(this);
  
  //create new user style
  userStyle* style = new userStyle(inter,window,this);

  orl->setUserStyle(style);
  orl->graphMode(false);

  mode = 'g'; 

  progBar->setValue(100);
}

//saves the graph in its current state for next time
void Graph::saveFile(char* filename)
{
  char* str = strtok(filename, " .");

  if(str != NULL)
    {
      strcat(str, ".orlando");
    }
  else
    {
      str = filename;
    }

  ofstream oFile(str);

  oFile << orlandoDataName << "\n";

  oFile << selected << "\n";
  
  oFile << namesAllowedOn << "\n";

  oFile << mode << "\n";

  if(mode == 'p')
    {
      oFile << names[pathS] << "\n";

      oFile << names[pathF] << "\n";
    }
  
  oFile.close();

  char* strT = strtok(str, ". ");
  mkdir(strT,0777);
  
  char* pch;
  pch=strrchr(filename,'/');

  char* pch2 = strdup(pch);

  char* str1 = strtok(filename, " .");
  strcat(str1, pch2);
  strcat(str1, ".nodes");
  SaveNodePos(str1);

  char* str2 = strtok(str1, ".");
  strcat(str2, ".tagsU");
  saveTagsUsed(str2);

  char* str3 = strtok(str2, ".");
  strcat(str3, ".tagsO");
  saveTagsOn(str3);

  char* str7 = strtok(str3, ".");
  strcat(str7, ".inc");
  saveInc(str7);

  char* str8 = strtok(str3, ".");
  strcat(str8, ".exc");
  saveExc(str8);

  if(namesAllowedOn)
    {
      char* str6 = strtok(str3, ".");
      strcat(str6, ".namesO");
      saveNamesOn(str6);
    }

  if(mode == 'h' || mode == 't')
    {
      char* str4 = strtok(str3, ".");
      strcat(str4, ".togO");
      saveTogOn(str4);

      char* str5 = strtok(str4, ".");
      strcat(str5, ".togC");
      saveTogCon(str5);
    }

  free(str);
}

//loads the graph in from the state found in the file
void Graph::loadFile(char* filename)
{
  load = true;

  //reset names
  allNamesOn(false);
  
  windowSetup();

  //create a file
  fstream file;

  //open the file
  file.open(filename);
  
  //if the file opens properly
  if(file.is_open())
    {      
      //get the filename of the data file from the file
      char* fileN = (char*) calloc(1000, sizeof(char));
      file.getline(fileN, 1000);
	
      //get the selected node from the file
      char* sel = (char*) calloc(1000, sizeof(char));
      file.getline(sel, 1000);

      selected = atoi(sel);

      //find if names are allowed
      char* nameA = (char*) calloc(1000, sizeof(char));
      file.getline(nameA, 1000);

      if(strcmp(nameA, "1") == 0)
	{
	  allNamesOn(false);
	  namesAllowedOn = true;
	}
      else
	{
	  allNamesOff(true);
	}      
 
      //get the mode from the file
      char* md = (char*) calloc(1000, sizeof(char));
      file.getline(md, 1000);

      mode = md[0];

      char* pch = strtok(filename, ". ");
      pch=strrchr(filename,'/');

      char* pch2 = strdup(pch);

      char* fileC = strtok(filename, ". ");
      strcat(fileC, pch2);
      strcat(fileC, ".inc");
      loadInc(fileC);

      fileC = strtok(fileC, ". ");
      strcat(fileC, ".exc");
      loadExc(fileC);    

      initialize(fileN);
   
      fileC = strtok(fileC, ". ");
      strcat(fileC, ".nodes");     
      GetNodePos(fileC);
  
      fileC = strtok(fileC, ". ");
      strcat(fileC, ".tagsU");  
      loadTagsUsed(fileC);

      fileC = strtok(fileC, ". ");
      strcat(fileC, ".tagsO");
      loadTagsOn(fileC);

      fileC = strtok(fileC, ". ");
      strcat(fileC, ".inc");
      loadInc(fileC);

      fileC = strtok(fileC, ". ");
      strcat(fileC, ".exc");
      loadExc(fileC);    

      Orlando* orl = (Orlando*) orland;
      orl->setGraph(this);

      if(namesAllowedOn)
	{   
	  orl->nameTagsOn();

	  fileC = strtok(fileC, ". ");
	  strcat(fileC, ".namesO");
	  loadNamesOn(fileC);
	}
      else
	{
	  orl->nameTagsOff();
	}
  
      if(mode == 'h' || mode == 't')
	{
	  fileC = strtok(fileC, ". ");
	  strcat(fileC, ".togO");
	  loadTogOn(fileC);

	  fileC = strtok(fileC, ". ");
	  strcat(fileC, ".togC");
	  loadTogCon(fileC);
	}

      //create new user style
      userStyle* style = new userStyle(inter,window,this);

      //if the mode is graph then redraw the graph
      if(mode == 'g')
	{
	  redrawGraph();
	  displayNdInfo(0,0);
	  orl->setUserStyle(style);
	  orl->graphMode(false);
	}
      //else if the mode is camera redraw the graph and go into camera mode
      else if(mode == 'c')
	{
	  redrawGraph();
	  displayNdInfo(0,0);
	  orl->cameraMode();
	}
      //else if the mode is toggle, draw the toggled graph
      else if(mode == 't')
	{
	  toggle(0,0);
	  drawToggled();
	  orl->setUserStyle(style);
	  orl->toggleMode();
	}
      //else if the mode is highlight, draw the highlighted graph
      else if(mode == 'h')
	{
	  highlight(0,0);
	  drawHighlighted();    
	  orl->setUserStyle(style);
	  orl->highlightMode();
	}
      //else the mode is path, find the strings of the path ends and display the paths between them
      else
	{
	  //get path beginning
	  char* nd1 = (char*) calloc(1000, sizeof(char));
	  file.getline(nd1, 1000);

	  //get path end
	  char* nd2 = (char*) calloc(1000, sizeof(char));
	  file.getline(nd2, 1000);

	  orl->setUserStyle(style);
	  orl->graphMode(false);
	  findPathBtw(nd1, nd2);
	}
      
      if(namesAllowedOn)
	{  

	  for(int i=0; i<NUM_OF_NAMES; i++)
	    {
	      if(nameOn[i])
		{
		  nameText[i] = vtkCaptionActor2D::New();

		  nameText[i]->SetCaption(names[i]);  
		  nameText[i]->SetPadding(0);
		  nameText[i]->SetHeight(0.015);
		  nameText[i]->GetCaptionTextProperty()->BoldOff();		 
		  nameText[i]->GetCaptionTextProperty()->ShadowOff();
		  nameText[i]->SetAttachmentPoint(graph1[i]->getX(),graph1[i]->getY(),graph1[i]->getZ());
		  nameText[i]->BorderOff();

		  rend->AddActor(nameText[i]);
		}	      
	    }
	}
      
      drawKeys(); 
      load = false;

      //rend->ResetCamera();
      renderWin();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

//loads an xml file to work with -- must open and add ^ as the last line and take out first 2 lines
void Graph::loadXML(char* filename)
{  
  Orlando* orl = (Orlando*) orland;
  orl->setGraph(this);

  //set the progress bar to 0
  progBar->setValue(0);

  //create new user style
  userStyle* style = new userStyle(inter,window,this);
  orl->setUserStyle(style);
 
  printf("before initialize");
  fflush(stdout);
//set the progress bar to 0
  progBar->setValue(0);
  initialize(filename);

  drawGraph(); 
  display();
  orl->graphMode(true); 
}

void Graph::renderWin()
{
  progBar->setValue(80);

  window->Render();

  //set the progress bar to 100
  progBar->setValue(100);
}

void Graph::saveInc(char* filename)
{  
  //create file
  ofstream file(filename);
  fstream fl;

  fl.open("/var/tmp/orlando.inc");

  do
    {   
      char* line = (char*) calloc(1000, sizeof(char));
      fl.getline(line, 1000);
      
      file << line << "\n";

    }while(!fl.eof());

  file.close();
  fl.close();
}

void Graph::loadInc(char* filename)
{  
  //create file
  ofstream file("/var/tmp/orlando.inc");
  fstream fl;

  fl.open(filename);

   do
    {   
      char* line = (char*) calloc(1000, sizeof(char));
      fl.getline(line, 1000);
      
      file << line << "\n";

    }while(!fl.eof());

  file.close();
  fl.close();
}

void Graph::saveExc(char* filename)
{  
  //create file
  ofstream file(filename);
  fstream fl;

  fl.open("/var/tmp/orlando.exc");

  do
    {   
      char* line = (char*) calloc(1000, sizeof(char));
      fl.getline(line, 1000);
      
      file << line << "\n";
      
    }while(!fl.eof());

  file.close();
  fl.close();
}

void Graph::loadExc(char* filename)
{  
  //create file
  ofstream file("/var/tmp/orlando.exc");
  fstream fl;

  fl.open(filename);
  
  do
    {   
      char* line = (char*) calloc(1000, sizeof(char));
      fl.getline(line, 1000);
      
      file << line << "\n";

    }while(!fl.eof());

  file.close();
  fl.close();
}

void Graph::saveNamesOn(char* filename)
{  
  //create file
  ofstream file(filename);

  //for all the tags save the on bool in a file
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      file << nameOn[i] << "\n";
    }

  file.close();
}

void Graph::loadNamesOn(char* filename)
{
  fstream file;

  file.open(filename);

  if(file.is_open())
    {
      for(int i=0; i<NUM_OF_NAMES; i++)
	{
	  char* line = (char*) calloc(1000, sizeof(char));
	  file.getline(line, 1000);

	  nameOn[i] = atoi(line);
	}
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

//save tagsUsed to a file
void Graph::saveTagsUsed(char* filename)
{
  //create file
  ofstream file(filename);

  list<int>::iterator it;
  for(it=tagsUsed.begin(); it!=tagsUsed.end(); it++)
    {
      file << *it << "\n";
    }

  file.close();
}

void Graph::loadTagsUsed(char* filename)
{
  fstream file;

  file.open(filename);

  if(file.is_open())
    {
      tagsUsed.clear();

      char* line = (char*) calloc(1000, sizeof(char));
      file.getline(line, 1000);

      while(!file.eof())
	{
	  tagsUsed.push_back(atoi(line));
	  file.getline(line, 1000);
	}
      numOfTags = tagsUsed.size();
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file" << filename << endl;
}

void Graph::saveTogOn(char* filename)
{
  //create file
  ofstream file(filename);

  list<int>::iterator it;
  for(it=toggledOn.begin(); it!=toggledOn.end(); it++)
    {
      file << *it << "\n";
    }

  file.close();
}

void Graph::loadTogOn(char* filename)
{
  fstream file;

  file.open(filename);

  if(file.is_open())
    {
      toggledOn.clear();

      char* line = (char*) calloc(1000, sizeof(char));
      file.getline(line, 1000);

      while(!file.eof())
	{
	  toggledOn.push_back(atoi(line));
	  file.getline(line, 1000);
	}
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

void Graph::saveTogCon(char* filename)
{
  //create file
  ofstream file(filename);

  //for all the tags save the on bool in a file
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      file << toggleConnected[i] << "\n";
    }

  file.close();
}

void Graph::loadTogCon(char* filename)
{
  fstream file;

  file.open(filename);

  if(file.is_open())
    {
      for(int i=0; i<NUM_OF_NAMES; i++)
	{
	  char* line = (char*) calloc(1000, sizeof(char));
	  file.getline(line, 1000);

	  toggleConnected[i] = atoi(line);
	}
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

void Graph::saveTagsOn(char* filename)
{
  //create file
  ofstream file(filename);

  //for all the tags save the on bool in a file
  for(int i=0; i<NUM_OF_TAGS+1; i++)
    {
      file << tagOn[i] << "\n";
    }

  file.close();
}

void Graph::loadTagsOn(char* filename)
{
  fstream file;

  file.open(filename);

  if(file.is_open())
    {
      for(int i=0; i<NUM_OF_TAGS+1; i++)
	{
	  char* line = (char*) calloc(1000, sizeof(char));
	  file.getline(line, 1000);

	  tagOn[i] = atoi(line);
	}
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

//save node positions to file
//filename is the name of the file to save to
void Graph::SaveNodePos(char* filename)
{
  //create variables
  ofstream file(filename);
  Node* nd;

  //for the number of names
  for(int i=0; i <NUM_OF_NAMES; i++)
    {
      //get the node
      nd=graph1[i];
      //get the coordinates
      x=nd->getX();
      y=nd->getY();
      z=nd->getZ();
      //print the coordinates to the file
      file << x << " " << y << " " << z << "\n";
    }

  file.close();
}

//load node positions from file
//filename is the name of the file to open
void Graph::GetNodePos(char* filename)
{
  //create variables
  char* line;
  int x, y, z;  
  fstream file;
  Node* nd;
  
  //open file
  file.open(filename);

  //if the file is open
  if(file.is_open())
    {
      //for the number of names
      for(int i=0; i<NUM_OF_NAMES; i++)
	{
	  //get the line from the file
	  line = (char*) calloc(1000, sizeof(char));
	  file.getline(line, 1000);
	  //get the coordinates from the line
	  x=atoi(strtok(line, " "));
	  y=atoi(strtok(NULL, " "));
	  z=atoi(strtok(NULL, " \n"));
	  //set the center to the read in coordinates
	  graph1[i]->setCenter(x,y,z);
      	}
      //close the file
      file.close();
    }
  //else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

//draw the key for a certain colour
//a and b are the first and second coordinates for the start and end of the y axis
//x,y,z are the values for the colour of the background
//nam is the text that is going inside the key
//on is true if the tag is on, else the tag is off
void Graph::drawKey(float a, float b, float c, float d, double* col, char* nam, bool on)
{
  //create new renderer
  rend1 = vtkRenderer::New();
  rend1->SetViewport(c/800.0,a/600.0,d/800.0,b/600.0);

  //if the tag is on
  if(on)
    {
      //set the background colour to the one passed in
      rend1->SetBackground(col);
    }
  else
    {
      //set the colour to the regular background
      rend1->SetBackground(0.2,0.2,0.2);
    }
  //add the renderer to the window
  window2->AddRenderer(rend1);

  //create a new text actor
  vtkTextActor *txtAct = vtkTextActor::New();
  txtAct->SetInput(nam);
  txtAct->GetTextProperty()->SetLineOffset(10);

  //if the tag is on
  if(on)
    {
      //set the text colour to black
      txtAct->GetTextProperty()->SetColor(0.0,0.0,0.0);
    }
  else
    {
      //set the text colour to white
      txtAct->GetTextProperty()->SetColor(1.0,1.0,1.0);
    }
  
  //add the actor to the renderer
  rend1->AddActor(txtAct);

  //delete the actor
  txtAct->Delete();
}

//draw all the keys (legend)
void Graph::drawKeys()
{
  //initialize variables
  float width = 800.0;
  float height = 600.0;

  //create new renderer and add it to tag legend window
  rend2 = vtkRenderer::New();
  rend2->SetViewport(0.0/width,0.0/height,width/width,height/height);
  rend2->SetBackground(0.2,0.2,0.2);
  window2->AddRenderer(rend2);

  //initialize row and column variables
  int numRows = 0;
  int numCols = 3;

  //find the number of rows that are needed from the number of tags
  if(numOfTags%numCols == 0)
    {
      numRows = numOfTags/numCols + 1;
    }
  else
    {
      numRows = numOfTags/numCols + 2;
    }

  //get the line width and height allowed for each tag
  float lineWidth = width/numCols;
  float lineHeight = height/numRows;

  //y coordinates
  float a = 0.0;
  float b = lineHeight;

  //y coordinates
  float c = 0.0;
  float d = lineWidth;

  //create a new renderer
  rend1 = vtkRenderer::New();
  rend1->SetViewport(0.0/800.0,lineHeight*(numRows-1)/600.0,800.0/800.0,600.0/600.0);
  rend1->SetBackground(0.2,0.2,0.2);

  //add the renderer to the window
  window2->AddRenderer(rend1);

  //create text actor for the title of the window
  vtkTextActor *txtAct = vtkTextActor::New();
  txtAct->SetInput("TAG LEGEND");
  txtAct->GetTextProperty()->BoldOn();
  txtAct->GetTextProperty()->SetColor(1.0,1.0,1.0);
  txtAct->GetTextProperty()->SetLineOffset(10);
  txtAct->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  txtAct->GetPosition2Coordinate()->SetValue(0.6,0.1);

  //add the text actor to the renderer
  rend1->AddActor(txtAct);

  //delete the text actor
  txtAct->Delete();

  //initialize variables
  list<int>::iterator it;
  int i=0;
  it=tagsUsed.begin();

  //while the tags have not all been gone through
  while(it!=tagsUsed.end())
    {
      //for the number of columns
      for(int j=0; j<numCols; j++)
	{
	  //draw the first key
	  drawKey(a,b,c,d,getColor(*it),tags[*it],tagOn[*it]);
	  
	  //increment the column
	  c = d;
	  d += lineWidth;
	  
	  //if the number of columns have not been reached
	  if(j<numCols)
	    {
	      //move on to next tag
	      it++;
	      //increment i
	      i++;
	    }
	  
	  //if all the tags have been gone through
	  if(i==numOfTags)
	    {
	      //break out of the loop
	      j = numCols;
	    }
	}
      
      //if there are still tags left to go through
      if(i<numOfTags)
	{
	  //increment the row
	  a = b;
	  b += lineHeight;

	  //reset the column
	  c = 0.0;
	  d = lineWidth;
	}
    }  

  //render the tags window
  window2->Render();
}

void Graph::resetGraphCons()
{
  //for the number of names reset ndCon values to false
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      ndCon[i] = false;
    }
}

void Graph::removeHighlightAct()
{
  //if the highlight actor is not NULL
  if(highlightActor != NULL)
    {  
      //remove the actor from the renderer and delete it
      rend->RemoveActor(highlightActor);
      highlightActor->Delete();
      highlightActor = NULL;
      //render the window
      renderWin();
    }
  
  //reset highlighted variable
  highlighted = false; 
}

void Graph::removeEdgesForNode(int nd)
{
  //get the list of edge actors
  list<vtkActor*> edgs = graph1[nd]->getEdgeActors();
  
  //for all of the edge actors for the node remove the actor
  list<vtkActor*>::iterator it;
  for(it=edgs.begin(); it!=edgs.end(); it++)
    {
      rend->RemoveActor(*it);
    }

  //reset the edge actors for the node
  graph1[nd]->resetEdgeActors();
}

void Graph::removeNode(int nd)
{
  vtkActor* act = graph1[nd]->getSphereActor();
  rend->RemoveActor(act);
}

vtkActor* Graph::drawNode(int ind, bool alpha, vtkActor* actor, double colR, double colG, double colB)
{
  //get the selected node
  Node* nd = graph1[ind];
  
  //get the coordinates of the node
  x = nd->getX();
  y = nd->getY();
  z = nd->getZ();
      
  //create a sphere
  vtkSphereSource* vtkSphere = vtkSphereSource::New();
  vtkSphere->SetRadius(2.5);
  vtkSphere->SetCenter(x,y,z);
  
  //create the mapper
  vtkPolyDataMapper* vtkMapper = vtkPolyDataMapper::New();
  vtkMapper->SetInput(vtkSphere->GetOutput());

  if(actor != highlightActor && !alpha)
    {
      printf("not highlighted");
      fflush(stdout);
      ndCon[ind] = true;
    }

  //create a new actor
  actor = vtkActor::New();
  actor->SetMapper(vtkMapper);
  actor->GetProperty()->SetColor(colR,colG,colB);
  
  if(alpha)
    {
      //set the opacity low so the edge is faded
      actor->GetProperty()->SetOpacity(0.1);
    }
  
  //if the index is not the selected node
  if(ind != selected)
    {
      //remove the node and set the actor to the new one
      removeNode(ind); 
      nd->setSphereActor(actor);
    }

  //add the actor to the renderer
  rend->AddActor(actor);
  
  //return the actor
  return actor;
}

void Graph::drawEdge(int stInd, int endInd, bool alpha, int tag)
{
  //set connected to true
  con[stInd][endInd] = true;
  con[endInd][stInd] = true;
  
  //initialize variables
  int a = stInd;
  int b = endInd;
			  			  
  //get the x coordinates
  int x1 = graph1[a]->getX();
  int x2 = graph1[b]->getX();
				  
  //get the y coordinates
  int y1 = graph1[a]->getY();
  int y2 = graph1[b]->getY();
				  
  //get the z coordinates
  int z1 = graph1[a]->getZ();
  int z2 = graph1[b]->getZ();
				  
  //create line
  vtkLineSource *line = vtkLineSource::New();			  
  line->SetPoint1(x1,y1,z1);
  line->SetPoint2(x2,y2,z2);
			      
  //create mapper for line
  vtkPolyDataMapper *mapp = vtkPolyDataMapper::New();
  mapp->SetInput(line->GetOutput());
  
  //create actor for line
  vtkActor *linAct = vtkActor::New();
  linAct->SetMapper(mapp);
  
  //set the colour for the actors
  linAct->GetProperty()->SetColor(getColor(tag));

  if(alpha)
    {
      //set the opacity low so the edge is faded
      linAct->GetProperty()->SetOpacity(0.1);
    }  		
	  
  //add the actors to the renderers
  rend->AddActor(linAct);

  //add the edge actor to the nodes
  graph1[a]->addEdgeActor(linAct);
  graph1[b]->addEdgeActor(linAct);
  
   
  //delete vtk objects	  
  line->Delete();
  mapp->Delete();
  linAct->Delete();
}

void Graph::drawEdgesForNode(int ind, bool alpha)
{
  //get the children
  list<Edge> ch = graph1[ind]->getChildren();

  //create iterators
  list<Edge>::iterator j;
  list<int>::iterator k;

  bool nodeConnected = false;

  //for the number of children
  for(j=ch.begin(); j!=ch.end(); j++)
    {       
      bool connected = false;

      //for the number of tags
      for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	{
	  //if the tag is on and the edge has not already beeen connected
	  if(tagOn[*k] && !con[j->GetNode1()][j->GetNode2()] && !con[j->GetNode2()][j->GetNode1()]  && !connected)
	    {
	      //if the names are connected with this tag
	      if(j->HasTag(*k))
		{
		  //draw the edge
		  drawEdge(j->GetNode1(), j->GetNode2(), alpha, (*k));
		  connected = true;
		  
		  if(!nodeConnected)
		    {
		      //draw the node if it has not already been drawn
		      drawNode(ind, alpha, vtkActor::New(),0.0, 95.0/255.0, 1.0);
		      nodeConnected = true;
		    }
		  
		  //get the other vertex of the edge
		  int q = j->GetNode1();

		  if(q == ind)
		    {
		      q = j->GetNode2();
		    }

		  //draw the node if it has not already been drawn
		  drawNode(q, alpha, vtkActor::New(),0.0, 95.0/255.0, 1.0);
		}	 
	    }
	}
    }      
}

//tag is touched, find which tag and turn on/off
void Graph::tagTouched(int x, int y)
{
  //initialize variables
  int height = 600.0;
  int width = 800.0;
  int numRows = 0;
  int numCols = 3;

  //find the number of rows based on the number of tags
  if(numOfTags%numCols == 0)
    {
      numRows = numOfTags/numCols +1;
    }
  else
    {
      numRows = numOfTags/numCols + 2;
    }

  //initialize variables
  float lineWidth = width/numCols;
  float lineHeight = height/numRows;

  //initialize x coordinates
  float a = 0.0;
  float b = lineHeight;

  //initialize y coordinates
  float c = 0.0;
  float d = lineWidth;

  //initialize variables
  list<int>::iterator it;
  int i=0;
  it=tagsUsed.begin();

  //while there are still tags to search through
  while(it!=tagsUsed.end())
    {
      //for the number of columns
      for(int j=0; j<numCols; j++)
	{	
	  //if the coordinates passed in are within the tag's area
	  if(y > a && y < b && x > c && x < d)
	    {
	      //turn on or off the tag
	      turnOnOffTag(*it);
	    }
	  
	  //increment the column
	  c = d;
	  d += lineWidth;

	  //get the next tag
	  it++;

	  //increment the number of tags done
	  i++;

	  //if all the tags have been gone through
	  if(i==numOfTags)
	    {
	      //break out of the loop
	      j = numCols;
	    }
	}
      
      //if there are still more tags to go through
      if(i<numOfTags)
	{
	  //increment the row
	  a = b;
	  b += lineHeight;

	  //reset the column
	  c = 0.0;
	  d = lineWidth;
	}
    }

  //draw the keys
  drawKeys();

  //render the tag window
  window2->Render();
      
  //set the mode to graph mode to redraw the graph with the tag off/on
  Orlando* orlan = (Orlando*) orland;
  orlan->graphMode(true);

  //render the main window
  renderWin();
}

//set toggle to tog
void Graph::setToggle(bool tog)
{
  if(tog)
    {
      //set mode to toggle
      mode = 't';
    }

  //set toggle to variable passed in
  toggleTrue = tog;

  //turn off path
  pathTrue = false;
}

//set up the renderer for rendText
void Graph::textSetUp()
{
  listWidge->clear();
}

//set up the renderer for rend
void Graph::rendSetUp()
{ 
  //create an actor collection
  vtkActorCollection* actors;
  
  //if the renderer is not NULL
  if(rend !=NULL)
    { 
      
      //remove the renderer from the window
      window->RemoveRenderer(rend);
      //get the actors from the renderers
      actors = rend->GetActors();
      // get the next actor
      vtkActor* actor = actors->GetNextActor();
      //while the actor is not NULL
      while(actor != NULL)
	{
	  //remove the actor and delete
	  rend->RemoveActor(actor);
	  actor->Delete();
	  //get next actor
	  actor = actors->GetNextActor();
	}
    }
  
  //delete the renderer and create a new one
  rend->Delete();
  rend = vtkRenderer::New();

  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //add the text actors
      rend->AddActor(nameText[i]);
    }

  //set up the renderer
  rend->SetBackground(0.2,0.2,0.2);
  rend->SetActiveCamera(cam);

  //add renderer to window
   window->AddRenderer(rend);
}

//display the path information
//a and b are start and end of the path
//path is the list of nodes in the path
void Graph::displayPathInfo(int a, int b, list<int> path)
{ 
  //nam to hold the path title
  char* nam = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
  sprintf(nam,"%s  &  %s",names[a], names[b]);
      
  //set the labels
  label1->setText("Paths Between:");
  label2->setText("Results:");
  labelSelected->setText(nam);
  free(nam);

  //for the nodes in the path
  list<int>::iterator j;
  for(j=path.begin(); j!=path.end(); j++)
    {
      //nam1 to hold the name of the node
      char* nam1 = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
      sprintf(nam1,"%s", names[*j]);
	  
      //draw the node in the path results
      drawText(0,nam1,0,false);
      free(nam1);
    }

  //draw a blank line to show end of path
  drawText(0,"",0,false);
}

//draw text
//dt -> the data to decide the colour
//nam -> the char* to print out
//offset -> the offset on the screen
//col -> changes to the color based on dt if true else colour is white
void Graph::drawText(float dt, char* nam, int offset, bool col)
{
  new QListWidgetItem(nam, listWidge);
}

//recursive algorithm to find all paths
//cuts off at the cutOff value so it will only do the shortest paths
bool Graph::findAllPaths(int startNode, int nodeToFind, list<int> pth, int cutOff, bool display)
{
  //if the size cutOff has been reached or the path is empty return
  if(pth.size() > cutOff || pth.size() <= 0)
    {
      return found;
    }

  //if the node to found has been found
  if(startNode == nodeToFind)
    {
      //show and print the path information
      printPath(pth);
      displayPathInfo(startNode, nodeToFind, pth);
	
      //set found to true and return
      found = true;
      return true;
    }

  //get the children
  list<Edge> ch = graph1[startNode]->getChildren();

  //for the adjacent nodes
  list<Edge>::iterator it;  
  for(it=ch.begin(); it!=ch.end(); it++)
    {
      int q = (*it).GetNode1();

      if(q == startNode)
	{
	  q = (*it).GetNode2();
	}

      //put the node in the path
      pth.push_back(q);
      //recall the recursive function with this node 
      found = findAllPaths(q, nodeToFind, pth, cutOff, display);
      //delete the node from the path
      pth.pop_back();       
    }

  //return when finished
  return found;
}

//find out if there is a path between the start node and the node to find
bool Graph::findIfPath(int startNode, int nodeToFind, int cutOff)
{
  bool found = false;
  list<int> q;
  q.push_back(startNode);

  int parent[NUM_OF_NAMES];
  bool done[NUM_OF_NAMES];

  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      parent[i] = -1;
      done[i] = false;
    }

  while(!q.empty())
    {
      int currentVertex = q.front();
      q.pop_front();

      list<Edge> ch = graph1[currentVertex]->getChildren();

      list<Edge>::iterator it;
      for(it=ch.begin(); it!=ch.end(); it++)
	{
	  int a = (*it).GetNode1();
	  if(a == currentVertex)
	    {
	      a = (*it).GetNode2();
	    }

	  if(a == nodeToFind)
	    {	      
	      return true;
	    }
	  
	  if(!done[a])
	    {
	      q.push_front(a);
	      parent[a] = currentVertex;
	    }
	}
      done[currentVertex] = true;
    }

  if(found)
    printf("FOUND!");
  
  fflush(stdout);
  return found;
}

//finds the path between nodes passed in 
void Graph::findPathBtw(char* x, char* y)
{	
  
  int c = -1;
  int d = -1;

  printf("in findPathBTW");
  fflush(stdout);
    
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {	  
      if(strcmp(x,names[i]) == 0)
	{
	  c = i;
	  pathS = i;
	}
          
      if(strcmp(y,names[i]) == 0)
	{
	  pathF = i;
	  d = i;
	}
    }

  //set up the renderers
  rendSetUp();
  textSetUp();

  printf("after set up");
  fflush(stdout);

  //clear the inPath list
  inPath.clear();
  
  //set sorting enabled to false for the list so the paths show up in order
  listWidge->setSortingEnabled(false);

  //set pathTrue to true
  pathTrue = true;
  
  //turn toggle on
  toggleOn();

  //put the first node in the path list
  list<int> path;
  path.push_back(c);
  
  oldSelected = selected;
  selected = d;

  //create graph2 and initialize it
  int graph2[NUM_OF_NAMES];
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      graph2[i] = i;
    }
  
  //initialize found to false
  found = false;  


  //check to make sure that there is a path between the nodes
  if(findIfPath(c, d, NUM_OF_NAMES))
    {	          
      //find the shortest path between the nodes
       int sz = findNodes(c, path, d, graph2);
      
      //if there is an indirect path found
      if(sz >= 0)
	{
	  //for the number of names initialize path connected
	  for(int i=0; i<NUM_OF_NAMES; i++)
	    {
	      pathConnected[i] = false;
	    }  
	  
	  //create new path to search with
	  path.clear();		  
	  path.push_back(c);
	  
	  //initialize found to false
	  found = false;
	  
	  //find all paths with the cutoff of the shortest path's size
	  findAllPaths(c, d, path, sz, true);
	}
    }
  
    printf("before print path");
    fflush(stdout);

  //show the path information for the two nodes
  list<int> empty;	  
  displayPathInfo(c,d,empty);
  empty.push_back(c);
  empty.push_back(d);

  printf("right before");
  fflush(stdout);

  printPath(empty);

  printf("after print path");
  fflush(stdout);

  //set mode to path
  mode = 'p';
}

//finds the path from the old selected node to the node at position passed in
void Graph::findPath(int a, int b)
{
  
  //set the mode to path
  mode = 'p';

  //set up the renderers
  rendSetUp();
  textSetUp();

  //clear the inPath list
  inPath.clear();
  
  //set sorting enabled to false for the list so the paths show up in order
  listWidge->setSortingEnabled(false);

  //set pathTrue to true
  pathTrue = true;

  //get the selected node
  int c = selected;
  
  //turn toggle on
  toggleOn();
 
  //if a node is selected
  if(selected >= 0)  
    {
      pathS = selected;

      //if there is a node at the position passed in
      if(nodeAtPos(a,b,true)>=0)
	{	  
	  pathF = selected;
	  
	  //put the old selected node in the path list
	  list<int> path;
	  path.push_back(c);

	  //create graph2 and initialize it
	  int graph2[NUM_OF_NAMES];
	  for(int i=0; i<NUM_OF_NAMES; i++)
	    {
	      graph2[i] = i;
	    }

	  //initialize found to false
	  found = false;
	  
	  //check to make sure that there is a path between the nodes
	  if(findIfPath(c, selected, NUM_OF_NAMES))
	    {	      
	      //find the shortest path between the nodes
	      int sz = findNodes(c, path, selected, graph2);
 
	      //if there is an indirect path found
	      if(sz >= 0)
		{
		  //for the number of names initialize path connected
		  for(int i=0; i<NUM_OF_NAMES; i++)
		    {
		      pathConnected[i] = false;
		    }  

		  //create new path to search with
		  path.clear();		  
		  path.push_back(c);

		  //initialize found to false
		  found = false;

		  //find all paths with the cutoff of the shortest path's size
		  findAllPaths(c, selected, path, sz, true);
		}
	    }
	 
	  //show the path information for the two nodes
	  list<int> empty;	  
	  displayPathInfo(c,selected,empty);
	  empty.push_back(c);
	  empty.push_back(selected);
	  printPath(empty);

 
	  //set mode to path
	  mode = 'p';
	}
      else
	{
	  redrawGraph();
	}
    }  
  else
    {
      redrawGraph();
      }
}

//uses dijkstra's algorithm to find the shortest path
//a and b are the start and end of the path
//path is the list of the nodes in the path 
//graph2 is a copy of the graph object for manipulation
int Graph::findNodes(int a, list<int> path, int b,int graph2[])
{
  //set infinity to a large number
  int infinity = 10000000;

  //initialize variables
  int d = a;
  float distance[NUM_OF_NAMES];
  int previous[NUM_OF_NAMES];

  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //initialize variables
      distance[i] = infinity;
      previous[i] = -1;
    }
  
  //initialize distance for start node
  distance[a] = 0;
 
  //while the graph is not empty
  while(!empty(graph2))
    {             
      //if the node is equal to the node to find
      if(a == b)
	{
	  //push the node onto the path
	  path.push_front(a);

	  //initialize cnt
	  int cnt = 0;

	  //while there is still a previous node in the path and the node is not equal to the start node
	  while(previous[a]!=-1 && a != d)
	    {
	      //increment count
	      cnt++;
	      //get the previous node 
	      a = previous[a];
	      //push the node on the path
	      path.push_front(a);
	    }
    
	  //return the path size
	  return path.size();
	}
      
      list<Edge> ch = graph1[a]->getChildren();

      list<Edge>::iterator it;
      //for the number of names
      for(it=ch.begin(); it!=ch.end(); it++)
	{	  
	  int i = (*it).GetNode1();
	  if(i == a)
	    {
	      i = (*it).GetNode2();
	    }

	  //if there is an edge between the nodes
	  if((*it).HasTag(NUM_OF_TAGS))
	    {     
	      //initialize variables
	      int c = i;
	      float alt = distance[a] + 1;

	      //set the distance to the smallest distance
	      if(alt < distance[c])
		{
		  distance[c] = alt;
		  previous[c] = a;
		}
	    }
	}

      //set the graph to -1 to show the node has already been searched 
      graph2[a] = -1;
	
      //initialize variable
      float smallest = infinity;

      //for the number of names
      for(int its=0; its<NUM_OF_NAMES; its++)
	{
	  //if the node has not been search and the distance is less than the smallest distance
	  if(graph2[its] != -1 && distance[graph2[its]] < smallest)
	    {
	      //set this distance to be the smallest
	      smallest = distance[graph2[its]];
	      //set a to the next node with the smallest distance
	      a = graph2[its];
	    }
	}
	}
  //return 0 since path is not found
  return 0;
}
	     
//checks if the list passed in is empty (all -1)
bool Graph::empty(int graph2[])
{
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //if there is a node still left to search return false
      if(graph2[i] != -1)
	{
	  return false;
	}
    }  
  //return true
  return true;
}

//draw the path using the list of nodes
void Graph::printPath(list<int> path)
{
  //initialize variables
  Node* ndP;
  int prev = -1;
  int cnt = 0;
  
  printf("in print path");
  fflush(stdout);

  //for each node in the path
  for(list<int>::iterator it = path.begin(); it != path.end(); it++)
    {
      printf("NODE: %i", *it);
      fflush(stdout);

      //get the node
      ndP = graph1[*it];
      
      //get the coordinates
      x = ndP->getX();
      y = ndP->getY();
      z = ndP->getZ();
      
      //increment the count
      cnt++;
      
      //set toggle connected to true
      toggleConnected[*it] = true;

      //create a sphere
      vtkSphereSource *sphere = vtkSphereSource::New();
      sphere->SetRadius(2);
      sphere->SetCenter(x,y,z);

      //create a mapper
      vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
      mapper->SetInput(sphere->GetOutput());
      
      //create an actor
      vtkActor *actor = vtkActor::New(); 
      actor->SetMapper(mapper);

      //if it is either the first or last node in the path
      if(cnt==1 || cnt == path.size())
	{	  
	  //set the node to white
	  actor->GetProperty()->SetColor(1,1,1);
	}
      else
	{
	  //else set the node to blue
	  actor->GetProperty()->SetColor(0,0,1);
	}

      //add actor to renderer
      rend->AddActor(actor);
      
      //delete vtk objects
      actor->Delete();
      mapper->Delete();
      sphere->Delete();

      //initialize variables
      bool connected = false;      
      list<int>::iterator i;
      
      
      //for all the tags
      for(i=tagsUsed.begin(); i!=tagsUsed.end(); i++)
	{
	  //if the tag is on and is connected to the previous node
	  if(prev != -1 && hasEdgeBetween(prev,*it,*i))
	    {
	      //set connected to true
	      connected = true;
	    }
	}
      
      
      //if connected
      if(connected)
	{
	  //initialize variables
	  list<int>::iterator k;
	  list<int> tagsAround;

	  //change the tags to a different order which reflects the graph's edge order
	  for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	    {
	      tagsAround.push_front(*k);
	    }

	 
	  //for the number of tags
	  for(k=tagsAround.begin(); k!=tagsAround.end(); k++)
	    {
	      //if the tag is on and it is connected
	      if((tagOn[*k] && hasEdgeBetween(prev,*it,*k)) || (*k == NUM_OF_TAGS && hasEdgeBetween(prev,*it,NUM_OF_TAGS)))
		{		
		  //initialize variable
		  int a,b;		  
		  a = prev;
		  b = *it;		  		  
		  
		  drawEdge(a,b, false, *k);
		}
	    }
	}
      //set previous to the current node
      prev = *it;
      }
}

bool Graph::hasEdgeBetween(int nd1, int nd2, int tag)
{
  Node* nd = graph1[nd1];

  bool found = false;

  list<Edge> ch = nd->getChildren();

  list<Edge>::iterator it;
  for(it=ch.begin(); it!=ch.end(); it++)
    {
      int q = it->GetNode1();

      if(q == nd1)
	{
	  q = it->GetNode2();
	}

      if(q == nd2 && it->HasTag(tag))
	{
	  found = true;
	}
    }

  return found;
}

void Graph::highlightNode(int a, int b)
{  
  if(!highlighted)
    {
      int foundName = nodeAtPos(a,b,false);

      if(foundName >= 0 && (!toggleTrue || (toggleTrue && ndCon[foundName])))
	{
	  highlightActor = drawNode(foundName, false, highlightActor,100.0/255.0, 200.0/255.0, 1.0);
	}     

      if(lastHighlighted != -1 && lastHighlighted != selected && lastHighlighted != foundName)
	{
	  if(ndCon[lastHighlighted])
	    {
	      drawNode(lastHighlighted, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0); 
	    }
	  else if(mode == 'h') 
	    {
	      drawNode(lastHighlighted, true, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	    }
	}

      if(foundName >= 0 && (!toggleTrue || (toggleTrue && ndCon[foundName])))
	{
	  highlighted = true;
	  lastHighlighted = foundName;
	}

      renderWin();
      }
}

//turn all name tags off
//if nw is true do not allow name tags, else just clear the name tags
void Graph::allNamesOff(bool nw)
{
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //if the text actor is not NULL delete the object
      if(nameText[i] != NULL)
	{      
	  //remove the text actor
	  rend->RemoveActor(nameText[i]);
	  nameText[i]->Delete();
	  nameText[i] = NULL;
	}

      //turn off the name tag
      nameOn[i] = false;
    }

  //if nw, do not allow name tags
  if(nw)
    namesAllowedOn = false;
}

//allow name tags on and reset the tags
void Graph::allNamesOn(bool all)
{
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //set the name on to false
      nameOn[i] = false;  //if names are allowed on
            
      //if a node was at the mouse position
      if(all && i >= 0)
	{
	  if(nameText[i] != NULL)
	    {
	      //remove actor from renderer
	      rend->RemoveActor(nameText[i]);
	      nameText[i]->Delete();
	      nameText[i] = NULL;
	    }
	  
	  //if the name is not on and the mode is either not toggle or the node is toggle connected
	  if(!nameOn[i] && ((!toggleTrue) || toggleConnected[i]))
	    {
	      //create a new text actor
	      nameText[i] = vtkCaptionActor2D::New();

	      //set the position and input for the name
	      nameText[i]->SetAttachmentPoint(graph1[i]->getX(),graph1[i]->getY(),graph1[i]->getZ());
	      nameText[i]->SetPadding(0);
	      nameText[i]->GetCaptionTextProperty()->BoldOff();
	      nameText[i]->GetCaptionTextProperty()->ShadowOff();
	      nameText[i]->SetHeight(0.015);
	      nameText[i]->SetCaption(names[i]);
	      nameText[i]->BorderOff();

	      //add the actor to the renderer
	      rend->AddActor(nameText[i]); 
	      
	      //turn on the name
	      nameOn[i] = true;
	    }
	  else
	    {	  	  
	      //turn off the name
	      nameOn[i] = false;
	    }     
	} 
    }

  //allow names to be on
  namesAllowedOn = true;
}

//turn the name at the position on or off
void Graph::nameOnOff(int a, int b)
{
  //if names are allowed on
  if(namesAllowedOn)
    {
      //initialize variables
      Node* nd2;
      double coords[3];
      
      //find the 3D coordinates 
      vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
      picker->Pick(a,b,0.0,rend);
      picker->GetPickPosition(coords);
      picker->Delete();

      //initialize variable
      int foundName = -1;
  
      //for the number of names
      for(int i=0; i<NUM_OF_NAMES;i++)
	{
	  //get the node
	  nd2 = graph1[i];

	  //check if all the coordinates match the coordinates of the mouse position passed in
	  if(nd2->getX()-3 <= coords[0] && nd2->getX()+3 >= coords[0])
	    {
	      if(nd2->getY()-3 <= coords[1] && nd2->getY()+3 >=coords[1])
		{
		  if(nd2->getZ()-3 <= coords[2] && nd2->getZ()+3 >= coords[2])
		    {
		      //hold the position of the node picked
		      foundName = i;
		    }
		}
	    }
	}
      
      //if a node was at the mouse position
      if(foundName >= 0)
	{
	  if(nameText[foundName] != NULL)
	    {
	      //remove actor from renderer
	      rend->RemoveActor(nameText[foundName]);
	      nameText[foundName]->Delete();
	      nameText[foundName] = NULL;
	    }
	  
	  //if the name is not on and the mode is either not toggle or the node is toggle connected
	  if(!nameOn[foundName] && ((!toggleTrue) || toggleConnected[foundName]))
	    {
	      //create a new text actor
	      nameText[foundName] = vtkCaptionActor2D::New();

	      //set the position and input for the name
	      nameText[foundName]->SetAttachmentPoint(graph1[foundName]->getX(),graph1[foundName]->getY(),graph1[foundName]->getZ());
	      nameText[foundName]->SetPadding(0);
	      nameText[foundName]->GetCaptionTextProperty()->BoldOff();
	      nameText[foundName]->GetCaptionTextProperty()->ShadowOff();
	      nameText[foundName]->SetHeight(0.015);
	      nameText[foundName]->SetCaption(names[foundName]);
	      nameText[foundName]->BorderOff();

	      //add the actor to the renderer
	      rend->AddActor(nameText[foundName]); 
	      
	      //turn on the name
	      nameOn[foundName] = true;
	    }
	  else
	    {	  	  
	      //turn off the name
	      nameOn[foundName] = false;
	    }     
	} 
    }
}

//destroy the pop-up name tag
void Graph::destroyName()
{
  if(txtAct != NULL)
    {
      //remove the text actor and delete it
      rend->RemoveActor(txtAct);
      txtAct->Delete();
      txtAct = NULL;
    }
}

//show the pop-up name tag at the position passed in
void Graph::showName(int a,int b)
{  
  int foundName = nodeAtPos(a,b, false);

  destroyName();

  //if the name found is not NULL and the name is not already on
  if(foundName >=0 && !nameOn[foundName] &&((!toggleTrue && !pathTrue) || ((toggleTrue || pathTrue) && toggleConnected[foundName]))) 
    {      
      //create a new text actor and set the position to the mouse position
      txtAct = vtkCaptionActor2D::New();
      txtAct->SetCaption(names[foundName]);//add the text actor to the renderer and render the window
      txtAct->SetAttachmentPoint(graph1[foundName]->getX(),graph1[foundName]->getY(),graph1[foundName]->getZ());
      txtAct->SetPadding(0);
      txtAct->GetCaptionTextProperty()->BoldOff();
      txtAct->GetCaptionTextProperty()->ShadowOff();
      txtAct->SetHeight(0.015);
      txtAct->SetCaption(names[foundName]);
      txtAct->BorderOff();
      
      rend->AddActor(txtAct);
      renderWin();
    }
}

//returns the index of the node at the position passed in
int Graph::nodeAtPos(int a, int b, bool setSel)
{
  //initialize variables
  Node* nd2;
  double coords[3];

  //find the coordinates of the position passed in
  vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
  picker->Pick(a,b,0.0,rend);
  picker->GetPickPosition(coords);
  picker->Delete();
  
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //get the node
      nd2 = graph1[i];

      //check if all the coordinates match the coordinates of the mouse position passed in
      if(nd2->getX()-3 <= coords[0] && nd2->getX()+3 >= coords[0])
	{
	  if(nd2->getY()-3 <= coords[1] && nd2->getY()+3 >=coords[1])
	    {
	      if(nd2->getZ()-3 <= coords[2] && nd2->getZ()+3 >= coords[2])
		{
		  if(setSel)
		    {
		      //set the selected node to the node at the position and return it
		      oldSelected = selected;
		      selected = i;
		    }
		  return i;
		}
	    }
	}
    }

  if(setSel)
    {
      //set selected to -1 (null) and return it
      selected = -1;
    }
  return -1;
}

//display the node information 
void Graph::displayNdInfo(int a, int b)
{
  //get the window size
  int* x = window->GetSize();

  //initialize variable
  int i = -1;

  //if a and b are 0 (null)
  if(a==0 && b ==0)
    {
      //set the node to the selected node
      i = selected;
    }
  else
    {
      //else get the node at the position passed in
      i = nodeAtPos(a,b, true);
    }

  //if selected is not null
  if(i>=0)
    {        
      //get the text for the selected node
      char* nam; 
      nam = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
      sprintf(nam,"%s",names[i]);
 
      //set up the renderer
      textSetUp();

      //set the sorting to true for the list so it will be in alphabetical order
      listWidge->setSortingEnabled(true);

      //set the labels to display
      label1->setText("Selected Node:");
      label2->setText("Connected With:");
      labelSelected->setText(nam);
      free(nam);
      
      //get the children and parents of the node
      list<Edge> ch = graph1[i]->getChildren();

      //for the number of children and parents
      list<Edge>::iterator j;
      for(j=ch.begin(); j!=ch.end(); j++)
	{
	  //initialize variables
	  bool connected = false;
	  
	  //for the number of tags
	  list<int>::iterator k;
	  for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	    {
	      //if the tag is on and the edge is connected
	      if((*j).HasTag(*k) && tagOn[*k])
		{
		  //set connected to true
		  connected = true;
		}
	    }
	  
	  //if the node is connected
	  if(connected)
	    {	  
	      int q = (*j).GetNode1();
	      if(q==i)
		{
		  q = (*j).GetNode2();
		}

	      //put the connected name in a string
	      char* nam1 = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
	      sprintf(nam1,"%s", names[q]);
	      strcat(nam1,"  : ");
	      
	      //for the number of tags
	      list<int>::iterator s;
	      for(s=tagsUsed.begin(); s!=tagsUsed.end(); s++)
		{
		  //if the tag is connected and is on
		  if((*j).HasTag(*s) && tagOn[*s] && (*s)!=NUM_OF_TAGS)
		    {
		      //attach the tag name to the string
		      strcat(nam1, " ");
		      strcat(nam1, tags[*s]);
		    }
		}
	      
	      //draw the text
	      drawText(0,nam1,0,false);
	      free(nam1);
	    }
	}
    }
}

//flags which node it is to move
void Graph::moveNode(int a, int b)
{
  ndToMove = nodeAtPos(a,b,true);
}

//changes the position of the node to a new position
void Graph::changeToPos(int a, int b)
{
  //if the node to move is not null
  if(ndToMove >=0)
    {
      //initialize variable
      double coords[3];
      
      //find the coordinates of the mouse position passed in
      vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
      picker->Pick(a,b,0.0,rend);
      picker->GetPickPosition(coords);
      picker->Delete();
      
      //move the node to the new position
      graph1[ndToMove]->setCenter((int)coords[0],(int)coords[1],(int)coords[2]);
    }
}

//turn highlight on
void Graph::highlightOn()
{
  //set mode to highlight
  mode = 'h';

  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //set toggleConnected to false
      toggleConnected[i] = false;
      ndCon[i] = false;
    }
 
  //clear the toggleOn
  toggledOn.clear();

  //turn off names
  allNamesOff(false);

  rendSetUp();

  //for the number of names
  for(int i=0;i<NUM_OF_NAMES;i++)
    {
      //for the number of edges
      for(int j=0;j<NUM_OF_NAMES;j++)
	{
	  //initialize con to false
	  con[i][j] = false;
	}
      //set togged on to false
      toggedOn[i] = false;
    }

  setToggle(false);

  highlight(0,0);

  drawFadedEdges();
}

//highlight the node at the position passed in
void Graph::highlight(int a, int b)
{
  //initialize variable
  int i = 0;

  //if a and b are null 
  if(a == 0 &&  b == 0)
    { 
      //set i to the previously selected node
      i = selected;
    }
  else
    {
      //set i to the node at the position passed in
      i = nodeAtPos(a,b,true);    
    }

  //if selected is not null
  if(i >=0)
    {            
      //set toggle connnected to true 
      toggleConnected[i] = true;     
      
      //put the node in the toggled on list
      toggledOn.push_back(i);
      
      //get the children of the selected node
      list<Edge> lst = graph1[i]->getChildren();
      
      //for the children of the node
      list<Edge>::iterator it;      
      for(it=lst.begin(); it!=lst.end(); it++)
	{
	  int q = it->GetNode1();
	  if(q == i)
	    {
	      q = it->GetNode2();
	    }

	  //set toggle connected of the child to true
	  toggleConnected[q] = true;
	  drawNode(q, false, vtkActor::New(),0.0, 95.0/255.0, 1.0);
	}

      //get the window size
      int* x = window->GetSize();

      //get the name of the node
      char* nam; 
      nam = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
      sprintf(nam,"%s",names[i]);
      
      //set up the renderer
      textSetUp();

      //show the labels
      label1->setText("Selected Node:");
      labelSelected->setText(nam);
      label2->setText("Connected  With:");
      free(nam);
      
      //get the children of the node
      list<Edge> ch = graph1[i]->getChildren();

      //for the number of children
      list<Edge>::iterator j;
      for(j=ch.begin(); j!=ch.end(); j++)
	{
	  //initialize variables
	  bool connected = false;

	  int q = j->GetNode1();
	  if(q == i)
	    {
	      q= j->GetNode2();
	    }

	  //for the number of tags
	  list<int>::iterator k;
	  for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	    {
	      //if the tag is on and the edge is connected
	      if(j->HasTag(*k) && tagOn[*k] && !connected)
		{
		  //set connected to true
		  connected = true; 
		  toggleConnected[q] = true;
		  drawNode(i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
		  drawNode(q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
		  drawEdge(i, q, false, *k);
		}
	    }

	  //if the node is connected
	  if(connected)
	    {	  
	      int q = j->GetNode1();

	      if(q == i)
		{
		  q = j->GetNode2();
		}

	      //put the connected name in a string
	      char* nam1 = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
	      sprintf(nam1,"%s", names[q]);
	      strcat(nam1,"  : ");

	      //for the number of tags
	      list<int>::iterator s;
	      for(s=tagsUsed.begin(); s!=tagsUsed.end(); s++)
		{
		  //if the tag is connected and is on
		  if(j->HasTag(*s) && tagOn[*s] && *s!=NUM_OF_TAGS)
		    {
		      //attach the tag name to the string
		      strcat(nam1, " ");
		      strcat(nam1, tags[*s]);
		    }
		}
	  
	      //draw the text
	      drawText(0,nam1,0,false);
	      free(nam1);
	    }
	}    
    }
  
  //draw the highlighted entries
  //drawHighlighted();
  
}

//draw the highlighted graph
void Graph::drawHighlighted()
{
  bool connected = false;

  //for the toggled objects
  list<int>::iterator it;
  for(it=toggledOn.begin(); it!=toggledOn.end();it++)
    {
      drawNode(*it, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	      
      //get the children of the selected node
      list<Edge> lst = graph1[*it]->getChildren();
      
      //for the children of the node
      list<Edge>::iterator j;      
      for(j=lst.begin(); j!=lst.end(); j++)
	{
	  
	  //initialize connected to false
	  connected = false;

	  int q = (*j).GetNode1();

	  if(q == *it)
	    {
	      q = (*j).GetNode2();
	    }

	  //if toggle connected
	  if(toggleConnected[*it])
	    {
	      //for the number of tags
	      list<int>::iterator k;
	      for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
		{
		  //if the tag is on and is connected
		  if(tagOn[*k] && (*j).HasTag(*k))
		    {
		      //set connected to true
		      connected = true;
		    }
		}   
	    }
		  
	  //if connected
	  if(connected)
	    {	  
	      drawNode(q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	    }
	}
    }
  
  //for the number of names
  for(int i=0; i<NUM_OF_NAMES;i++)
    {
      //if not toggle connected
      if(!toggleConnected[i])
	{
	  drawNode(i, true, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	}
    }

  //draw the highlighted edges
  drawHighlightedEdges();
}

//draw the highlighted edges
void Graph::drawHighlightedEdges()
{

  //for the number of names
  for(int i=0;i<NUM_OF_NAMES;i++)
    {
      //for the number of edges
      for(int j=0;j<NUM_OF_NAMES;j++)
	{
	  //initialize con to false
	  con[i][j] = false;
	}
      //set togged on to false
      toggedOn[i] = false;
    }

  //create iterators
  list<int>::iterator it;
  //list<int>::iterator k;

  //for all of the toggled on nodes
  for(it=toggledOn.begin(); it!=toggledOn.end(); it++)
    {	  
      //set togged on to true
      toggedOn[*it] = true;

      drawEdgesForNode(*it, false);

      //get the children of the node
      list<Edge> ch = graph1[*it]->getChildren();

      //for all the children
      list<Edge>::iterator j;
      for(j=ch.begin(); j!=ch.end(); j++)
	{   
	  int q = (*j).GetNode1();
	  if(q == *it)
	    {
	      q = (*j).GetNode2();
	    }

	  //if both vertices of the edge are connected
	  if(toggleConnected[*it] && toggleConnected[q])
	    {	      
	      list<int>::iterator k;
	      //for all of the tags
	      for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
		{
		  //if the tag is on
		  if(tagOn[*k])
		    {
		      //if the edge is connected and it has not already been drawn
		      if(hasEdgeBetween(*it,q,*k) && !con[*it][q])
			{
			  drawEdge(*it, q, false, *k);
			}		
		      //else if edge is connected and has not already been drawn
		      else if(hasEdgeBetween(*it,q,*k) && !con[q][*it])
			{
			  drawEdge(q, *it, false, *k);
			}
		    }
		}
	    }	
	}
    }
}

void Graph::drawFadedEdges()
{
  //for all the edges
  for(int i =0; i<NUM_OF_NAMES; i++)
    {
      drawNode(i, true, vtkActor::New(), 0.0, 95.0/255.0, 1.0);

      for(int j=0; j<NUM_OF_NAMES; j++)
	{    
	  list<int>::iterator k;
	  //for all the tags
	  for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	    {
	      //if tag is on
	      if(tagOn[*k])
		{
		  //if edge is connected and has not already been drawn
		  if((hasEdgeBetween(j,i,*k) && !con[j][i]) || (hasEdgeBetween(i,j,*k) && !con[i][j]))
		    {
		      drawEdge(j, i, true, *k);
		    }
		}
	    }	    
	}
    }
}

//turn toggle on
void Graph::toggleOn()
{
  //set mode to toggle
  mode = 't';

  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //set toggle connected to false
      toggleConnected[i] = false;
      ndCon[i] = false;
    }

  //clear toggled on
  toggledOn.clear();

  //turn off names
  allNamesOff(false);

  rendSetUp();
}

//toggle the node at position
void Graph::toggle(int a, int b)
{
  //initialize variable
  int i = -1;

  //if position is null
  if(a == 0 &&  b == 0)
    {   
      //set i to previously selected node
      i = selected;
      if(i!=-1)
	{
	  ndCon[i] = true;
	  toggleConnected[i] = true;
	  printf("toggle set up");
	  fflush(stdout);
	}
    }
  else
    {
      //get the node at the position passed in
      i = nodeAtPos(a,b,true);
      if(!ndCon[i])
	{
	  i = -1;
	}
    }

  //if selected node is not null
  if(i >=0 && ndCon[i] && toggleConnected[i])
    {
      //set toggle connected to true
      toggleConnected[i] = true;
      
      //get window size
      int* x = window->GetSize();

      //get the name of the node
      char* nam; 
      nam = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
      sprintf(nam,"%s",names[i]);
      
      //set up the renderer
      textSetUp();

      //set the labels 
      label1->setText("Selected Node:");
      labelSelected->setText(nam);
      label2->setText("Connected  With:");
      free(nam);
    
      //get the children and parents of the node
      list<Edge> ch = graph1[i]->getChildren();

      //for the number of children and parents
      list<Edge>::iterator j;
      for(j=ch.begin(); j!=ch.end(); j++)
	{
	  //initialize variables
	  bool connected = false;
	  
	  int q = (*j).GetNode1();
	  if(q == i)
	    {
	      q = (*j).GetNode2();
	    }

	  //for the number of tags
	  list<int>::iterator k;
	  for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
	    {
	      //if the tag is on and the edge is connected and not already drawn
	      if(tagOn[*k] && (*j).HasTag(*k) && !connected)
		{
		  //set connected to true
		  connected = true;
		  toggleConnected[q] = true;
		  drawNode(i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
		  drawNode(q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
		  drawEdge(i, q, false, *k);
		}
	    }	 

	  //if the node is connected
	  if(connected)
	    {
	      //get the ints
	      a = i;
	      b = q;
	  
	      //put the connected name in a string
	      char* nam1 = (char*) calloc(1000*sizeof(char),1000*sizeof(char));
	      sprintf(nam1,"%s", names[b]);
	      strcat(nam1,"  : ");

	      //for the number of tags
	      list<int>::iterator s;
	      for(s=tagsUsed.begin(); s!=tagsUsed.end(); s++)
		{
		  //if the tag is connected and is on
		  if(hasEdgeBetween(a,b,*s) && tagOn[*s] && *s!=NUM_OF_TAGS)
		    {
		      //attach the tag name to the string
		      strcat(nam1, " ");
		      strcat(nam1, tags[*s]);
		    }
		}
	  
	      //draw the text
	      drawText(0,nam1,0,false);
	      free(nam1);
	    }
	}    
    }

  //draw toggled graph
  //drawToggled();
}

//draw the toggled graph
void Graph::drawToggled()
{
  //initialize variables
  int x=0;
  int y=0;
  int z=0;
  bool connected = false;

  //for the toggled on nodes
  list<int>::iterator it;
  for(it=toggledOn.begin(); it!=toggledOn.end();it++)
    {
      drawNode(*it, false, vtkActor::New(), 0.0,95.0/255.0, 1.0);
	      
      //for the number of names
      for(int j=0; j<NUM_OF_NAMES; j++)
	{	  
	  //initialize connected to false
	  connected = false;

	  //if toggle connected
	  if(toggleConnected[j])
	    {
	      //for the number of tags
	      list<int>::iterator k;
	      for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
		{
		  //if the tag is on and edge is connected
		  if(tagOn[*k] && hasEdgeBetween(*it,j,*k))
		    {
		      //set connected to true
		      connected = true;
		    }
		}   
	    }
		  
	  //if connected
	  if(connected)
	    {	  
	      drawNode(j, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	    }
	}
    }
  //draw toggled edges
  drawToggledEdges();
}

//draw the toggled edges
void Graph::drawToggledEdges()
{
  //for all edges
  for(int i=0;i<NUM_OF_NAMES;i++)
    {
      for(int j=0;j<NUM_OF_NAMES;j++)
	{
	  //initialize connected to false
	  con[i][j] = false;
	}
    }

  //for all the toggled on nodes
  list<int>::iterator it;
  for(it=toggledOn.begin(); it!=toggledOn.end(); it++)
    {	  
      //get the children of the toggled on node
      list<Edge> ch = graph1[*it]->getChildren();
      
      list<Edge>::iterator j;
      //for the children of the toggled node
      for(j=ch.begin(); j!=ch.end(); j++)
	{   
	  int q = (*j).GetNode1();
	  if(q == *it)
	    {
	      q = (*j).GetNode2();
	    }
	  
	  //if both vertices of the edge are toggle connected
	  if(toggleConnected[*it] && toggleConnected[q])
	    {	      
	      list<int>::iterator k;
	      //for all the tags
	      for(k=tagsUsed.begin(); k!=tagsUsed.end(); k++)
		{
		  //if the tag is on
		  if(tagOn[*k])
		    {
		      //if the edge is connected and has not already been done
		      if((*j).HasTag(*k) && !con[*it][q] && !con[q][*it] )
			{
			  drawEdge(*it,q, false, *k);
			}	
		    }
		}
	    }	
	}      
    }
}

//redraw the graph based on current node positions
void Graph::redrawGraph()
{
  rendSetUp();

  setToggle(false);

  for(int a=0; a<NUM_OF_NAMES; a++)
    {
      ndCon[a] = false;
    }

  if(!load || (mode != 'p' && load))
    {
      //set mode to graph
      mode = 'g';
    }

  //set pathTrue to false
  pathTrue = false;

  //for the number of names
  //for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //set the progress
      //float fl = ((float)i)/((float)NUM_OF_NAMES)*40;
      //progBar->setValue(fl);

      //drawNode(i,false,vtkActor::New(),0.0,95.0/255.0,1.0);
    }      
  
  drawEdges();
}

//return if the interactor is user style 
bool Graph::isInteractGraph()
{
  return interactGraph;
}

//change the interactor to user style
void Graph::changeInteractorToGraph()
{
  //create new user style
  userStyle* style = new userStyle(inter,window,this);

  //set the interactor style to the new user style
  Orlando* orl = (Orlando*) orland;
  orl->setUserStyle(style);
  inter->SetInteractorStyle(style);
  rend->SetActiveCamera(cam);

  //set interactGraph to true
  interactGraph = true;
}

//change the interactor to camera
void Graph::changeInteractorToCamera()
{
  //set the mode to camera
  mode = 'c';

  //set the interactor style to joystick camera
  inter->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());

  //set interactGraph to false
  interactGraph = false;
}

void Graph::windowSetup()
{
  //add the renderer to the window
  window->AddRenderer(rend);

  //set the render window for the interactor
  inter->SetRenderWindow(window);
  
  //set up the camera
  cam->SetViewUp(0,0,-1);
  cam->SetPosition(0,1,0);
  cam->SetFocalPoint(0,0,0);
  cam->ComputeViewPlaneNormal();
  cam->SetParallelProjection(true);
  rend->SetActiveCamera(cam);
  cam->Dolly(1.5);
  
  //set the background for the renderers
  rend->SetBackground(0.2,0.2,0.2);
  rendText->SetBackground(0.2,0.2,0.2);

  //create userstyle tags
  userStyleTags* style2 = new userStyleTags(inter2, window2, this);

  //set the interactor style for the interactor
  inter->SetInteractorStyle(vtkInteractorStyleJoystickCamera::New());
}

//seperate the nodes into octants based on their connectivity
void Graph::drawGraph()
{
  windowSetup();
  
   //initialize variables
  list<int> connect;
  bool connected[NUM_OF_NAMES];

  //for the number of names
  for(int i = 0; i<NUM_OF_NAMES; i++)
    {
      //initialize connected to false
      connected[i] = false;
    }

  //initialize coordinate variables
  int x=0;
  int y=0;
  int z=0;

  //draw entries... all 150 apart

  //initialize variables
  list<int>::iterator it;
  int cntx = 0;
  int cntz = 0;

  //calculate the width/length of all the entry nodes
  int half = sqrt(entries.size())*150;

  //for the number of entries
  for(it=entries.begin(); it!=entries.end(); it++)
    {
      printf("Entry: %i", *it);
      fflush(stdout);

      //get coordinates
      x = cntx;
      z = cntz;
      
      //set connected to true
      connected[*it] = true;

      //create a sphere
      vtkSphereSource *sphere = vtkSphereSource::New();
      sphere->SetRadius(2);
      sphere->SetCenter(x,y,z);
      //sphere->SetThetaResolution(RESOLUTION_X);
      //sphere->SetPhiResolution(RESOLUTION_Y);

      //set the center of the node
      graph1[*it]->setCenter(x,y,z);
   
      //create mapper for sphere
      vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
      mapper->SetInput(sphere->GetOutput());
      
      //create actor for sphere
      vtkActor *actor = vtkActor::New();
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(0,95.0/255.0,1);

      //add actor to renderer
      rend->AddActor(actor);

      graph1[*it]->setSphereActor(actor);

      //increment column
      cntx+= 150;

      //if reached the end of the columns
      if(cntx > half)
	{
	  //reset the column
	  cntx=0;
	  //increment row 
	  cntz+=150;
	}
    }

  //for the number of entries
  for(it=entries.begin(); it!=entries.end(); it++)
    {
      //get the coordinates
      int numX = graph1[*it]->getX();
      int numZ = graph1[*it]->getZ();
      
      //initialize variables
      int xVal = 0;
      int zVal = 0;
      int counter = 0;
      
      list<Edge> ch = graph1[*it]->getChildren();

      list<Edge>::iterator iter;
      for(iter=ch.begin(); iter!=ch.end(); iter++)
	{
      ////for the number of names
      //for(int i=0; i<NUM_OF_NAMES;i++)
	  //{

	  int q = (*iter).GetNode1();

	  if(q == *it)
	    {
	      q = (*iter).GetNode2();
	    }
	  
	  //if the edge is connected and has not already been drawn
	  if(!connected[q])
	    {
	      //arrange in a layout around the entry
	      if(counter == 0)
		{
		  x = numX + 8;
		  z = numZ + 0;
		}
	      else if(counter == 1)
		{
		  x = numX + 0;
		  z = numZ + 8;
		}
	      else if(counter == 2)
		{
		  x = numX - 8;
		  z = numZ + 0;
		}
	      else if(counter == 3)
		{
		  x = numX + 0;
		  z = numZ - 8;
		}
	      else if(counter >=4 && counter <8)
		{
		  xVal = 1;
		  zVal = 1;
		  switch(counter%4)
		    {
		    case 0:
			x = numX + xVal*8;
		        z = numZ + zVal*8;
		        break;
		    case 1:
			x = numX + xVal*8;
		        z = numZ - zVal*8;
			break;
		    case 2:
			x = numX - xVal*8;
		        z = numZ + zVal*8;
			break;
		    case 3:
			x = numX - xVal*8;
		        z = numZ - zVal*8;
			break;
		    }
		}
	      else if(counter >=8 && counter <16)
		{
		  if(counter <12)
		    {
		      xVal = 1;
		      zVal = 2;
		    }
		  else
		    {
		      xVal = 2;
		      zVal = 1;
		    }
		  switch(counter%4)
		    {
		    case 0:
		      x = numX + xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 1:
		      x = numX + xVal*8;
		      z = numZ - zVal*8;
		      break;
		    case 2:
		      x = numX - xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 3:
		      x = numX - xVal*8;
		      z = numZ - zVal*8;
		      break;
		    }
		}
	      else if(counter >=16 && counter <32)
		{
		  if(counter <20)
		    {
		      xVal = 1;
		      zVal = 4;
		    }
		  else if(counter <24)
		    {
		      xVal = 4;
		      zVal = 1;
		    }
		  else if(counter <28)
		    {
		      xVal = 2;
		      zVal = 3;
		    }
		  else
		    {
		      xVal = 3;
		      zVal = 2;
		    }
		  switch(counter%4)
		    {
		    case 0:
		      x = numX + xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 1:
		      x = numX + xVal*8;
		      z = numZ - zVal*8;
		      break;
		    case 2:
		      x = numX - xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 3:
		      x = numX - xVal*8;
		      z = numZ - zVal*8;
		      break;
		    }
		}
	      else if(counter >=32 && counter <64)
		{
		  if(counter <36)
		    {
		      xVal = 1;
		      zVal = 6;
		    }
		  else if(counter <40)
		    {
		      xVal = 6;
		      zVal = 1;
		    }
		  else if(counter <44)
		    {
		      xVal = 2;
		      zVal = 6;
		    }
		  else if(counter <48)
		    {
		      xVal = 6;
		      zVal = 2;
		    }
		  else if(counter <52)
		    {
		      xVal = 3;
		      zVal = 5;
		    }
		  else if(counter <56)
		    {
		      xVal = 5;
		      zVal = 3;
		    }
		  else if(counter <60)
		    {
		      xVal = 4;
		      zVal = 5;
		    }
		  else
		    {
		      xVal = 5;
		      zVal = 4;
		    }
		  switch(counter%4)
		    {
		    case 0:
		      x = numX + xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 1:
		      x = numX + xVal*8;
		      z = numZ - zVal*8;
		      break;
		    case 2:
		      x = numX - xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 3:
		      x = numX - xVal*8;
		      z = numZ - zVal*8;
		      break;
		    }
		}
	      else if(counter >=64 && counter <128)
		{
		  if(counter <68)
		    {
		      xVal = 1;
		      zVal = 8;
		    }
		  else if(counter <72)
		    {
		      xVal = 8;
		      zVal = 1;
		    }
		  else if(counter <76)
		    {
		      xVal = 2;
		      zVal = 7;
		    }
		  else if(counter <80)
		    {
		      xVal = 7;
		      zVal = 2;
		    }
		  else if(counter <84)
		    {
		      xVal = 2;
		      zVal = 9;
		    }
		  else if(counter <88)
		    {
		      xVal = 9;
		      zVal = 2;
		    }
		  else if(counter <92)
		    {
		      xVal = 3;
		      zVal = 6;
		    }
		  else if(counter <96)
		    {
		      xVal = 6;
		      zVal = 3;
		    }
		  else if(counter <100)
		    {
		      xVal = 3;
		      zVal = 8;
		    }
		  else if(counter <104)
		    {
		      xVal = 8;
		      zVal = 3;
		    }
		  else if(counter <108)
		    {
		      xVal = 5;
		      zVal = 6;
		    }
		  else if(counter <112)
		    {
		      xVal = 6;
		      zVal = 5;		      
		    }
		  else if(counter <116)
		    {
		      xVal = 5;
		      zVal = 7;
		    }
		  else if(counter <120)
		    {
		      xVal = 7;
		      zVal = 5;
		    }
		  else if(counter <124)
		    {
		      xVal = 5;
		      zVal = 8;
		    }
		  else
		    {
		      xVal = 8;
		      zVal = 5;
		    }
		  switch(counter%4)
		    {
		    case 0:
		      x = numX + xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 1:
		      x = numX + xVal*8;
		      z = numZ - zVal*8;
		      break;
		    case 2:
		      x = numX - xVal*8;
		      z = numZ + zVal*8;
		      break;
		    case 3:
		      x = numX - xVal*8;
		      z = numZ - zVal*8;
		      break;
		    }		  
		}
	      else
		{
		  //if there were too many children for the node then give error message
		  printf("\n\n\n\nTOO MANY CHILDREN::: %i!!!!\n\n\n\n", counter);
		  fflush(stdout);
		}

	      //increment counter
	      counter++;

	      //set connected to true
	      connected[q] = true;

	      //create sphere
	      //vtkSphereSource *sphere = vtkSphereSource::New();
	      //sphere->SetRadius(2);
	      //sphere->SetCenter(x,y,z);
	      
	      //set center of node
	      graph1[q]->setCenter(x,y,z);
	      
	      //create mapper for sphere
	      //vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
	      //mapper->SetInput(sphere->GetOutput());
	      
	      //create actor for sphere
	      //vtkActor *actor = vtkActor::New();
	      //actor->SetMapper(mapper);
	      //actor->GetProperty()->SetColor(0,95.0/255.0,1);

	      //add actor for renderer
	      //rend->AddActor(actor);
	      //graph1[q]->setSphereActor(actor);

	      //delete vtk actors
	      //actor->Delete();
	      //mapper->Delete();
	      //sphere->Delete();
	    }
	}
    }
  //draw the keys for the tags
  drawKeys();
  //draw edges
  //drawEdges();
}

//turn on or off the tag corresponding to tagNum
void Graph::turnOnOffTag(int tagNum)
{
  //if the tag is on, turn the tag off
  if(tagOn[tagNum])
    {
      tagOn[tagNum] = false;
    }
  //else turn the tag on
  else
    {
      tagOn[tagNum] = true;
    }
}

//draw the edges for the nodes
void Graph::drawEdges()
{  
  //for all the edges
  for(int i=0;i<NUM_OF_NAMES;i++)
    {
      for(int j=0;j<NUM_OF_NAMES;j++)
	{
	  //set connected to false
	  con[i][j] = false;
	}
    }

  //for the number of names
  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //change the progress bar 
      float fl = ((float)i)/((float)NUM_OF_NAMES-1) *40;
      progBar->setValue(40+fl);

      drawEdgesForNode(i, false);  
    }
}

void Graph::popUpPath()
{
  Path pathWindow;
  
  pathWindow.setGraph(this);

  QComboBox* bx1 = pathWindow.getCombo1();
  QComboBox* bx2 = pathWindow.getCombo2();
  
  QStringList lst;

  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      lst << names[i];
    }

  lst.sort();
  
  bx1->addItems(lst);
  bx2->addItems(lst);

  inter->Disable();

  pathWindow.exec();

  inter->Enable();
}

//return the color based on the tag number
//the colours are set by RGB values
double *Graph::getColor(int dt)
{
  double *colour = (double*) calloc(3*sizeof(double),3*sizeof(double));

  if(dt == 0)
    {
      colour[0]=120.0/255.0;
      colour[1]=120.0/255.0;
      colour[2]=1;
    }
  else if(dt == 1) 
    {
      colour[0]=0;
      colour[1]=170.0/255.0;
      colour[2]=0;
    }
  else if(dt == 2)
    {
      colour[0]=200.0/255.0;
      colour[1]=0;
      colour[2]=1;
    }
  else if(dt == 3)
    {
      colour[0]=1;
      colour[1]=0;
      colour[2]=0;
    } 
  else if(dt == 4)
    {
      colour[0]=1;
      colour[1]=204.0/255.0;
      colour[2]=50.0/255.0;
    } 
  else if(dt == 5)
    {
      colour[0]=0;
      colour[1]=50.0/255.0;
      colour[2]=1;
    } 
  else if(dt == 6)
    {
      colour[0]=200.0/255.0;
      colour[1]=150.0/255.0;
      colour[2]=55.0/255.0;
    } 
  else if(dt == 7)
    {
      colour[0]=51.0/255.0;
      colour[1]=1;
      colour[2]=0;
    } 
  else if(dt == 8)
    {
      colour[0]=1;
      colour[1]=200.0/255.0;
      colour[2]=200.0/255.0;
    }   
  else if(dt == 9)
    {
      colour[0]=100.0/255.0;
      colour[1]=0;
      colour[2]=205.0/255.0;
    } 

  else if(dt == 10)
    {
      colour[0]=204.0/255.0;
      colour[1]=0;
      colour[2]=150.0/255.0;
    } 
  else if(dt == 11)
    {
      colour[0]=1;
      colour[1]=1;
      colour[2]=0;
    } 
  else if(dt == 12)
    {
      colour[0]=0;
      colour[1]=100.0/255.0;
      colour[2]=0;
    } 
  else if(dt == 13)
    {
      colour[0]=1;
      colour[1]=100.0/255.0;
      colour[2]=0;
    } 
  else if(dt == 14)
    {
      colour[0]=150.0/255.0;
      colour[1]=0;
      colour[2]=0;
    } 
  else if(dt == 15)
    {
      colour[0]=200.0/255.0;
      colour[1]=200.0/255.0;
      colour[2]=1;
    } 
  else if(dt == 16)
    {
      colour[0]=100.0/255.0;
      colour[1]=0;
      colour[2]=150.0/255.0;
    } 
  else if(dt == 17)
    {
      colour[0]=0;
      colour[1]=150.0/255.0;
      colour[2]=100.0/255.0;
    } 
  else if(dt == 18)
    {
      colour[0]=200.0/255.0;
      colour[1]=1;
      colour[2]=200.0/255.0;
    } 
  else if(dt == 19)
    {
      colour[0]=200.0/255.0;
      colour[1]=50.0/255.0;
      colour[2]=0;
    } 
  else if(dt == 20)
    {
      colour[0]=100.0/255.0;
      colour[1]=1;
      colour[2]=1;
    } 
  else if(dt == 21)
    {
      colour[0]=1;
      colour[1]=50.0/255.0;
      colour[2]=150.0/255.0;
    } 
  else if(dt == 22)
    {
      colour[0]=200.0/255.0;
      colour[1]=1;
      colour[2]=0;
    } 
    else if(dt == 23)
    {
      colour[0]=1;
      colour[1]=100.0/255.0;
      colour[2]=100.0/255.0;
    } 
    else if(dt == 24)
    {
      colour[0]=222.0/255.0;
      colour[1]=222.0/255.0;
      colour[2]=222.0/255.0;
    } 
    else
    {
      colour[0]=200.0/255.0;
      colour[1]=200.0/255.0;
      colour[2]=150.0/255.0;
    } 

  return colour;
}
	
//set selected to the name that corresponds with char* sel
void Graph::setSelected(char* sel)
{
  //for the number of names
  for(int i =0; i<NUM_OF_NAMES; i++)
    {
      printf(sel);
      fflush(stdout);
      //find the correct name from the text
      if(strstr(sel, names[i]) != NULL)
	{
	  printf(sel);
	  fflush(stdout);
	  oldSelected = selected;
	  //set selected to the correct index
	  selected = i;
	}
    }
}

//show the node as white if selected
void Graph::select()
{
  //if a node is selected
  if(selected >=0 && ndCon[selected])
    {
      //remove the selected actor and delete it
      //rend->RemoveActor(selActor);
      //selActor->Delete();

      if(oldSelected >= 0 && (mode == 'g' || toggleConnected[oldSelected]))
	{
	  drawNode(oldSelected, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0);
	}

      selActor = drawNode(selected, false, selActor, 1,1,1);
    }
}

//set up the display
void Graph::display()
{
  //set the viewport for the renderer
  rend->SetViewport(0.0,0.0,800.0/800.0,600.0/600.0);
  //add the renderer to the window
  window->AddRenderer(rend);
  //set the render window for the interactor
  inter->SetRenderWindow(window); 
 
  //set up camera
  cam->Dolly(1.5);
  rend->SetActiveCamera(cam);
  rend->ResetCamera();
}

//clean up the remaining vtk objects
void Graph::done()
{
  //delete global variables
  cam->Delete();
  inter->Delete();
  rend->Delete();
  rendText->Delete();
  window->Delete();
}

//case insensitive search [case insensitive strstr]
char* Graph::stristr(char* strToSearch, char* searchStr)
{
  //if the search string is null
  if ( !*searchStr )
    {
      //return the string to search
      return strToSearch;
    }
  
  //for all the characters in the string to search
  for ( ; *strToSearch; ++strToSearch )
    {
      //check if the upper case string to search is equal to the upper case search string
      if ( toupper(*strToSearch) == toupper(*searchStr) )
	{
	  //create variables
	  const char *h, *n;
	  
	  //check through the string to see if the string to search is next within the search string
	  for ( h = strToSearch, n = searchStr; *h && *n; ++h, ++n )
	    {
	      if ( toupper(*h) != toupper(*n) )
		{
		  break;
		}
	    }
	  
	  //if the search string is empty return the string to search
	  if ( !*n ) 
	    {
	      return strToSearch;
	    }
	}
    }
  //if the string is not found return NULL;
  return NULL;
}

//search the names for the string key
void Graph::search(char* key)
{
  //reset the search strings
  resetSearch();
  
  //get the window size
  int* x = window->GetSize();   
  
  //set up the text 
  textSetUp();

  //set the labels and search string
  label1->setText("Searching:");
  labelSelected->setText(key);
  label2->setText("Results:");

  //if the string to search is empty
  if(strcmp(key,"") == 0)
    {
      //display all of the names
      for(int i=0; i<NUM_OF_NAMES; i++)
	{
	  drawText(0,names[i],(x[1]*-1)+60+(z*20),false);
	}
    }
  else
    {
      //duplicate the string to search
      char* str = strdup(key);

      //find the first space in the string to search
      str = strtok(str," ");
  
      //initialize variables
      int count = 0;
      int target = 0;
  
      //while the string is not finished 
      while(str != NULL)
	{
	  //add the search word to the list of strings to search
	  searchStrings.push_back(str);
	  //get the next word
	  str = strtok(NULL, " ");
	}
      
      //for the number of names
      for(int i=0; i<NUM_OF_NAMES; i++)
	{
	  //initialize variables
	  bool found = false;	  
	  list<char*>::iterator it;	  
	  int l = 0;
	  
	  //for the number of search strings
	  for(it=searchStrings.begin(); it!= searchStrings.end(); it++)
	    {
	      //if the string is found and has also been found before or is the first time through the loop
	      if(stristr(names[i],*it) != NULL && (found || l==0))
		{     
		  //set found to true
		  found = true;
		}
	      else
		{
		  //set found to false
		  found = false;
		}
	      //increment l
	      l++;
	    }
	  
	  //if the strings have been found in the entry
	  if(found)
	    {
	      //increment count
	      count++;
	      //set target to i
	      target = i;
	      //draw the text
	      drawText(0,names[i],0,false);
	    }
	}
  
      //if there was only one result then make it the selected node
      if(count == 1)
	{
	  selected = target;
	  select();
	}
    }
  //render the window
  renderWin();
}

//reset the search strings
void Graph::resetSearch()
{
  searchStrings.clear();
}

//Parse the file with filename to get all names in the file
void Graph::GetEntry(char* filename)
{
  progBar->setValue(0);

  stInd = (int*) calloc(NUM_OF_NAMES_C+NUM_OF_NAMES_C+NUM_OF_TAGS+100, sizeof(int));
  endInd = (int*) calloc(NUM_OF_NAMES_C+NUM_OF_NAMES_C+NUM_OF_TAGS+100, sizeof(int));
  nameInd = (int*) calloc(NUM_OF_NAMES_C+NUM_OF_NAMES_C, sizeof(int));

  orlandoDataName = filename;
  entries.clear();

 //create a file
  fstream fil;
  ofstream tempFile("/var/tmp/data.txt");

  //open the file
  fil.open(filename);
  
  //if the file opens properly
  if(fil.is_open())
    {  
      char* line = (char*) calloc(1000, sizeof(char));
      fil.getline(line,1000);

      if(line!= NULL)
	{	
	  free(line);
	  line = NULL;
	}

      line = (char*) calloc(1000, sizeof(char));
      fil.getline(line,1000);

      while(!fil.eof())
	{
	  if(line != NULL)
	    {
	      free(line);
	      line = NULL;
	    }

	  line = (char*) calloc(10000000, sizeof(char));
	  fil.getline(line, 10000000);
	  char* str = strtok(line,"^");
	  tempFile << str << "\n";
	}

      if(line != NULL)
	{
	  free(line);
	  line = NULL;
	}

      tempFile << "^" << endl;

      tempFile.close();

      if(tagsUsed.empty())
	{
	  list<char*> emptyList;
	  setTags(emptyList);
	}

      filename = "/var/tmp/data.txt";
    }  
  //else give error message
  else cerr << "Unable to open file" << endl;

  //file to hold the file
  FILE* file;

  //vaiables to get the file read in to a buffer
  long lSize;
  char * buffer;
  size_t result;

  //initialize number of entries
  int numEntries = 0;

  //variables to hold the entry id
  char* entryID;
  int entryIDNum;

  int numNames = 0;
  int numOfEntries = 8;
  
  //create a list for the names
  list<Name> nms;

  //open file for reading
  file = fopen(filename,"rb");

  //if the file opens with no problems
  if(file != NULL)
    {

      //obtain file size:
      fseek (file , 0 , SEEK_END);
      lSize = ftell (file);
      rewind (file);

      //allocate memory to contain the whole file:
      buffer = (char*) malloc (sizeof(char)*lSize);
      if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
      
      // copy the file into the buffer:
      result = fread (buffer,1,lSize,file);
      if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

      //get the first line
      char* line = strtok(buffer,"\n");

      //get the rest of the file
      char* restOfFile = strtok(NULL, "^");

      int lineN = 0;

      //for the number of entries    
      //for(int i=0;i<numOfEntries;i++)  
      
      //{
	while(restOfFile != NULL)
	{      
	  lineN++;

	  //duplicate the line
	  char* ptr = strdup(line);
	  //char* to hold name
	  char* name2;

	  //ints to calculate the where the names are in the file
	  int lineLen = strlen(line);
	  int nameSpot = 0;

	  //if the line should be included
	  if(includeLine(line))
	  {
	    //if the line is greater than 0 and does not have the end of file character
	    if(lineLen > 0 && (strstr(line, "^") == NULL))
	      {
		//set the entry ID number to number of names
		entryIDNum = numNames;
		//find entryID
		strtok(ptr,"\"");	
		//get entry id
		entryID = strtok(NULL,"\"");
		
		if(entryID == NULL)
		  {
		    restOfFile == NULL;
		  }

		else
		  {
		    //increment the number of entries
		    numEntries++;
		
		    printf("ENTRY: %s", entryID);
		    fflush(stdout);
		    
		    //initialize variables
		    bool unique = true;
		    int keyInt = 0;
		    int m=0;
		    
		    //for the number of names
		    for(int m=0; m<numNames; m++)
		      {
			//check if the name has already been used
			if(strcmp(names[m],entryID) == 0)
			  {
			    //set unique to false
			    unique = false;
			    //set the key to the key already used
			    keyInt = m;			
			  }
		      }
		    
		    //if unique
		    if(unique)
		      {
			//set the name of the entry to the entryID
			names[entryIDNum] = strdup(entryID);
			
			graph1[numNames] = new Node();
			
			//incrememnt the number of names
			numNames++;
		      }
		    else
		      {
			//set the entry id number to key int
			entryIDNum = keyInt;
		      }
		    
		    //push back the entry id number to the entries list
		    entries.push_back(entryIDNum);
		    
		    lineNum[entryIDNum] = lineN;
		    
		    //get the rest of the line
		    char* ptr3 = strtok(NULL,"\n"); 

		    //find names
		    do
		      {		    
			//get the next name
			ptr3 = strstr(ptr3,"<NAME");	
			strtok(ptr3,"\"");
			name2 = strtok(NULL,"\"");
			
			//get the rest of the line
			ptr3 = strtok(NULL,"\n");
			
			//if the name is not null and the name is not the entry find the names in the entry
			if(name2!=NULL && strcmp(name2,entryID) != 0)
			  {		    
			    //set the name spot to the length of the rest of the line
			    nameSpot = strlen(ptr3);
			    
			    //initialize variables
			    unique = true;
			    bool thisEntry = false;
			    keyInt = 0;
			    
			    //for the number of names
			    for(int m=0; m<numNames; m++)
			      {
				//check if the names have already been used
				if(strcmp(names[m],name2) == 0)
				  {
				    //set unique to false
				    unique = false;
				    //set keyInt to the key already used
				    keyInt = m;			  
				  }
			      }
			    
			    //if not unique
			    if(!unique)
			      {
				//iterate through the nms list
				list<Name>::iterator it;
				for(it=nms.begin(); it!=nms.end(); it++)
				  {
				    //if the name is equal to the name in the list
				    if(strcmp(it->getKey(),name2) == 0)
				      {
					//add the file spot to the name
					it->addFileSpot(nameSpot);
					//set this entry to true
					thisEntry = true;
				      }
				  }
				
				//if this entry is true
				if(!thisEntry)
				  {				
				    //create a new name and add it to the list
				    Name *nm = new Name();
				    nm->setKey(name2);
				    nm->addFileSpot(nameSpot);
				    nm->setKeyNum(keyInt);
				    nms.push_front(*nm);
				  }
			      }
			    //else
			    else
			      {
				//create a new name
				Name *nm = new Name();
				nm->setKey(name2);
				nm->addFileSpot(nameSpot);
				nm->setKeyNum(numNames);
				
				//set the name at the number of names to the name string
				names[numNames] = strdup(name2);
				
				graph1[numNames] = new Node();
				
				//increment number of names
				numNames++;
				
				printf("number of names: %i \n",numNames);
				fflush(stdout);
				
				//add the name to the list
				nms.push_front(*nm);
			      }
			  }
		      }
		    while(ptr3 != NULL);
		    //while the line is not empty
		  
		    //for the number of tags find the tags!
		    for(int t=0; t<NUM_OF_TAGS; t++)
		      {
			//copy the line to a char*
			char* ptr2 = (char*) calloc(strlen(line)+1, sizeof(char)); 
			strcpy(ptr2, line);
			
			//initialize variables
			int startTag = 0;
			int endTag = 0;
			int ct = 0;
			
			do
			  {
			    //char* to hold the tag string
			    char* str = (char*) calloc(100,sizeof(char));
			    
			    //add an opening bracket onto the tag string
			    strcpy(str, "<");
			    strcat(str, tags[t]);
			    
			    //find tag in file
			    ptr2 = strstr(ptr2,str);	
			    name2 = strtok(ptr2,">");
			    ptr2 = strtok(NULL,"\n");
			    
			    //if tag is found
			    if(name2 != NULL)
			      {
				//store the place of the start of the tag in the file
				startTag = strlen(ptr2);
				
				//add the ending bracket onto the tag string
				str = (char*) calloc(100,sizeof(char));
				strcpy(str, "</");
				strcat(str, tags[t]);
				strcat(str, ">");
				
				//find the end tag in the file
				ptr2 = strstr(ptr2,str);	
				name2 = strtok(ptr2,">");
				ptr2 = strtok(NULL,"\n");
				
				//if the tag is found
				if(name2 != NULL)
				  endTag = strlen(ptr2);
				
				//check the names to see if any appear in between the start tag and end tag
				list<Name>::iterator it;
				for(it=nms.begin(); it!=nms.end(); it++)
				  {				
				    stInd[entryIDNum+it->getKeyNum()+t+ct] = startTag;
				    endInd[entryIDNum+it->getKeyNum()+t+ct] = endTag;
				    ct++;
				    it->checkTag(startTag, endTag, t);		   
				  }
			      }
			  }
			while(ptr2 != NULL);
			//while the line is not searched through
		      }
		  }
	      }
	  }
	
	    
	  //for all the names 
	  list<Name>::iterator it;
	  for(it=nms.begin(); it!=nms.end(); it++)
	    {
	      //bool to hold tags
	      bool *tgs = it->getTags();

	      list<int> tagList;

	      //for the number of tags
	      for(int j=0; j<NUM_OF_TAGS; j++)
		{
		  //if the name was nested between the tag push the tag on a list
		  if(tgs[j])
		    {
		      tagList.push_back(j);
		    }
		}

	      tagList.push_back(NUM_OF_TAGS);

	      Edge* e = new Edge(entryIDNum, (*it).getKeyNum(), tagList);

	      graph1[entryIDNum]->addChild(*e);
	      graph1[(*it).getKeyNum()]->addChild(*e);

	      //clear the file spots from the names
	      it->clearFileSpots();
	    }

	  //clear the names
	  nms.clear();

	  //free the pointer
	  free(ptr);
	  ptr = 0;	      
	    
	  //if there is still lines left in the file
	  if(restOfFile != NULL)
	    {
	      //get the next line in the file
	      line = strtok(restOfFile,"\n");

	      //store the rest of the file
	      restOfFile = strtok(NULL, "^");
	    }
	}

      NUM_OF_NAMES = numNames;

      //free the buffer
      free(buffer);

      free(stInd);
      free(endInd);
      free(nameInd);
      
      //close the file
      fclose(file);
    }
  //else the file open failed
  else
    {
      printf("FILE OPEN FAILED: ");
      printf(filename);
      printf("\n");
      fflush(stdout);
      }
}

//return the window
vtkRenderWindow* Graph::getWindow()
{
  return window;
}

//return the interactor
vtkRenderWindowInteractor* Graph::getInteractor()
{
  return inter;
}

//Place the tag choices in the list for the tag window
void Graph::tagChoices()
{
  tagList->clear();
  
  //for the number of tags add the name of the tag to the list
  for(int i=0; i<NUM_OF_TAGS; i++)
    {
      new QListWidgetItem(tags[i], tagList);
    }
}

//set tags to the list of tags passed in
void Graph::setTags(list<char*> lst)
{
  tagsUsed.clear();

  //for the number of tags: turn off the tags
  for(int i=0; i<NUM_OF_TAGS; i++)
    {
      tagOn[i] = false;
    }
  
  //iterator for the list
  list<char*>::iterator it;

  //for each item in the list
  for(it=lst.begin(); it!=lst.end(); it++)
    {      
      //for the number of tags 
      for(int i=0; i<NUM_OF_TAGS; i++)
	{	  
	  //if the tag is in the list
	  if(strcmp(*it,tags[i]) == 0)
	    {
	      //put the tag in the tagsUsed list
	      tagsUsed.push_back(i);
	      //turn the tag on
	      tagOn[i] = true;
	    }
	}
    }
  //turn on the whole entry tag
  tagOn[NUM_OF_TAGS] = true;
  //put the whole entry tag in the tagsUsed list
  tagsUsed.push_back(NUM_OF_TAGS);

  //set the number of tags to the size of the tagsUsed list
  numOfTags = tagsUsed.size();
  
  drawKeys();
  //redrawGraph();
  //select();
}

void Graph::initialize(char* filename)
{
  rendSetUp();
  textSetUp();

  //get entries from the file nm
  GetEntry(filename);
}

void Graph::setMode(char a)
{
  mode = a;
}

char Graph::getMode()
{
  return mode;
}

bool Graph::getLoad()
{
  return load;
}

void Graph::tagWindowOn()
{
  Tags* tagWin = (Tags*) tagWindow;

  //put the possible tags in the taglist
  tagChoices();

  //set the graph on the tag window
  tagWin->setGraph(this);

  inter->Disable();

  //show the tag window
  tagWin->exec();

  

  inter->Enable();
}

void Graph::textWindowOn()
{
  Text* textWin = new Text();

  //set the graph on the tag window
  textWin->setGraph(this);

  QComboBox* cb1 = textWin->getCombo1();

  list<int>::iterator it;
  for(it=entries.begin(); it!=entries.end(); it++)
    {
      cb1->addItem(names[*it]);
    }  

  (cb1->model())->sort(0,Qt::AscendingOrder);

  QComboBox* cb2= textWin->getCombo2();

  int first = -1;

  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      //if(dataN[0][i][NUM_OF_TAGS])
	{
	  if(first == -1)
	    {
	      first = i;
	    }
	  cb2->addItem(names[i]);
	}
    }

  (cb2->model())->sort(0,Qt::AscendingOrder);

  QComboBox* cb3= textWin->getCombo3();
  
  for(it=tagsUsed.begin(); it!=tagsUsed.end(); it++)
    {
      if(hasEdgeBetween(0,first,*it))
	{
	  cb3->addItem(tags[*it]);
	}
    }
  
  (cb3->model())->sort(0,Qt::AscendingOrder);

  inter->Disable();

  //show the tag window
  textWin->exec();

  inter->Enable();
}

void Graph::namesFromEntry(char* en, QComboBox* cb)
{
  int ind = findIndexFromName(en);

  cb->clear();

  for(int i=0; i<NUM_OF_NAMES; i++)
    {
      if(hasEdgeBetween(ind,i,NUM_OF_TAGS))
	{
	  cb->addItem(names[i]);
	}
    }
  (cb->model())->sort(0,Qt::AscendingOrder);
}

void Graph::tagsFromNameEntry(char* en, char*nm, QComboBox* cb)
{
  int ind1 = findIndexFromName(en);
  int ind2 = findIndexFromName(nm);

  cb->clear();
  
  list<int>::iterator it; //tagsUsed
  for(it=tagsUsed.begin(); it!=tagsUsed.end(); it++)
    {
      if(hasEdgeBetween(ind1,ind2,*it))
	{
	  cb->addItem(tags[*it]);
	}
    }
  (cb->model())->sort(0,Qt::AscendingOrder);
}

void Graph::showXMLEntry(char* en, char* nm, char* tg, QTextBrowser* tb)
{
  if(en != NULL)
    {
      int ind = findIndexFromName(en);
      
      int cnt = lineNum[ind];
      
      char* filename = "/var/tmp/data.txt";

      FILE* file;
      //vaiables to get the file read in to a buffer
      long lSize;
      char * buffer;
      size_t result;

      //open file for reading
      file = fopen(filename,"rb");
      
      //if the file opens with no problems
      if(file != NULL)
	{      
	  //obtain file size:
	  fseek (file , 0 , SEEK_END);
	  lSize = ftell (file);
	  rewind (file);
	  
	  //allocate memory to contain the whole file:
	  buffer = (char*) malloc (sizeof(char)*lSize);
	  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
	  
	  // copy the file into the buffer:
	  result = fread (buffer,1,lSize,file);
	  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	  
	  char* line = strtok(buffer,"\n");
	  
	  for(int i=1; i<cnt; i++)
	    {
	      line = strtok(NULL,"\n");
	    }

	  tb->setHtml(line);
	  if(nm != NULL)
	    {		 
	      if(tb->find(nm))
		{
		  printf(nm);
		  fflush(stdout);
		}
	      else
		{
		  char* nm1 = strdup(nm);
		  char* str = strtok(nm, "., ");
		  
		  if(str != NULL)
		    {
		      char* str2 = strtok(NULL, " .,");
		      char* str3 = (char*) alloca(1000* sizeof(char));
		      char* str4 = (char*) alloca(1000* sizeof(char));

		      if(str2 != NULL)
			{
			  str3 = strcat(str2," ");
			  str4 = strcat(str3,str);
			}
		      
		      if(!tb->find(str4))
			{
			  str2 = strtok(str2, " ");
			  str2 = strtok(NULL, " \n");
			  tb->find(str2);
			}
		    }
		}	      
	    }
	  
	  free(buffer);
	}
    }
}

int Graph::findIndexFromName(char* nm)
{
  //for the number of names
  for(int i =0; i<NUM_OF_NAMES; i++)
    {
      //if the strings are equal
      if(strstr(nm, names[i]) != NULL)
	{
	  //set selected
	  return i;
	}
    }
  return -1;
}

bool Graph::includeLine(char* line)
{
  bool incl = true;
  bool a = false;
  bool b = false;
  bool first = true;

  fstream fl;

  fl.open("/var/tmp/orlando.inc");

  if(fl.is_open())
    {      
      do
	{     
	  char* ln = (char*) calloc(1000, sizeof(char));
	  fl.getline(ln, 1000);
	 
	  a = false;
	  char* s = strdup(ln);

	  if(strcmp(s, "") != 0)
	    {	     
	      incl = false;

	      list<char*> wd;

	      char* str1 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      char* str2 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      char* str3 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      
	      for(int i=0; i<strlen(s); i++)
		{
		  if(i == 0)
		    {
		      str1[i] = toupper(s[i]);
		      str2[i] = toupper(s[i]);
		      str3[i] = tolower(s[i]);
		    }
		  else
		    {
		      str1[i] = tolower(s[i]);
		      str2[i] = toupper(s[i]);
		      str3[i] = tolower(s[i]);
		    }
		}
	      str1[strlen(s)]='\0';
	      str2[strlen(s)]='\0';
	      str3[strlen(s)]='\0';
	  
	      wd.push_back(s);
	      wd.push_back(str1);
	      wd.push_back(str2);
	      wd.push_back(str3);

	      list<char*>::iterator it2;
	      for(it2=wd.begin(); it2!=wd.end(); it2++)
		{
		  //if the word is found set a to true
		  if(strstr(line,*it2)!= NULL)
		    {	      
		      a = true;
		    }
		}     

	      if(a && (first || b))
		{
		  b = true;
		}
	      else
		{
		  b = false;
		}		      

	      if(a && b)
		{
		  incl = true;
		}
	      else
		{
		  incl = false;
		}
	      first = false;
	    }
	}while(!fl.eof());
      fl.close();
    }

  bool excl = true; 
  a = true;
  b = false;
  first = true;
  
  fl.open("/var/tmp/orlando.exc");

  if(fl.is_open())
    {
      do
	{     
	  char* ln = (char*) calloc(1000, sizeof(char));
	  fl.getline(ln, 1000);
	 
	  a = true;
	  char* s = strdup(ln);

	  if(strcmp(ln,"") != 0)
	    {	     
	      excl = false;

	      list<char*> wd;

	      char* str1 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      char* str2 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      char* str3 = (char*) alloca((strlen(s)+1)* sizeof(char));
	      
	      for(int i=0; i<strlen(s); i++)
		{
		  if(i == 0)
		    {
		      str1[i] = toupper(s[i]);
		      str2[i] = toupper(s[i]);
		      str3[i] = tolower(s[i]);
		    }
		  else
		    {
		      str1[i] = tolower(s[i]);
		      str2[i] = toupper(s[i]);
		      str3[i] = tolower(s[i]);
		    }
		}
	      str1[strlen(s)]='\0';
	      str2[strlen(s)]='\0';
	      str3[strlen(s)]='\0';

	      wd.push_back(s);
	      wd.push_back(str1);
	      wd.push_back(str2);
	      wd.push_back(str3);

	      list<char*>::iterator it2;
	      for(it2=wd.begin(); it2!=wd.end(); it2++)
		{
		  if(strstr(line,*it2)!= NULL)
		    {		      
		      a = false;
		    }
		}     

	      if(a && (first || b))
		{
		  b = true;
		}
	      else
		{
		  b = false;
		}		      

	      if(a && b)
		{
		  excl = true;
		}
	      else
		{
		  excl = false;
		}
	      first = false;
	    }
	}while(!fl.eof());
      fl.close();
      }

  if(incl && excl)
    {
      printf("returned true");
      fflush(stdout);
      return true;
    }
  else
    {
      printf("returned false");
      fflush(stdout);
      return false;
    }
}

void Graph::include(char* s)
{
  //create file
  ofstream file("/var/tmp/orlando.inc");

  int i =0;

  if(s != NULL)
    {      
      char* str = strtok(s, ", ");
      
      if(str == NULL)
	{
	  str = strdup(s);
	}
     
      do
	{
	  if(strcmp(str, "AND") == 0 || strcmp(str, "OR") == 0)
	    {
	      
	    }
	  else
	    { 
	      file << str << endl;
	    }

	  str = strtok(NULL, ", ");

	} while(str != NULL);
    }
}

void Graph::exclude(char* s)
{
  ofstream file("/var/tmp/orlando.exc");

  if(s != NULL)
    {      
      char* str = strtok(s, ", ");
      
      if(str == NULL)
	{
	  str = (char*) calloc(1000, sizeof(char));
	  str = strdup(s);
	}
     
      do
	{
	  if(strcmp(str, "AND") == 0 || strcmp(str, "OR") == 0)
	    {
	      
	    }
	  else
	    { 
	      file << str << endl;
	    }

	  
	  str = (char*) calloc(1000, sizeof(char));
	  str = strtok(NULL, ", ");

	} while(str != NULL);
    }
}

list<char*> Graph::wordCases(char* s)
{
  //create a list for the word cases
  list<char*> lst;
  
  //strings to hold the word cases
  char* str1 = (char*) alloca(strlen(s)* sizeof(char));
  char* str2 = (char*) alloca(strlen(s)* sizeof(char));
  char* str3 = (char*) alloca(strlen(s)* sizeof(char));

  //for the length of the string change the char to upper or lower case to make 
  //EXAMPLE: s = "poetess" str1 = Poetess str2 = "POETESS" str3 = "poetess" 
  for(int i=0; i<strlen(s); i++)
    {
      if(i == 0)
	{
	  str1[i] = toupper(s[i]);
	  str2[i] = toupper(s[i]);
	  str3[i] = tolower(s[i]);
	}
      else
	{
	  str1[i] = tolower(s[i]);
	  str2[i] = toupper(s[i]);
	  str3[i] = tolower(s[i]);
	}
    }

  //add the string cases to the list
  lst.push_back(s);
  lst.push_back(str1);
  lst.push_back(str2);
  lst.push_back(str3);
}

//main function
int main (int argc, char** argv)
{
  //create application
  QApplication app(argc, argv);

  //create main window
  Orlando mainwindow;

  Tags tagWindow;

  //create the graph
  Graph* gra = new Graph(mainwindow.getWindow(), mainwindow.getInteractor(), mainwindow.getList(), mainwindow.getLabel(),mainwindow.getLineEdit(), mainwindow.getWidget(), mainwindow.getButton(), mainwindow.getLabel1(), mainwindow.getLabel2(), mainwindow.getWindow2(), mainwindow.getInteractor2(), mainwindow.getProgressBar(), tagWindow.getTagList(), &mainwindow, &tagWindow);  

  //set main widget for the application to the main window
  app.setMainWidget(&mainwindow);  
  
//show the main window
  mainwindow.show();

  //set the graph in the main window
  mainwindow.setGraph(gra);

  list<char*> lst;
  gra->setTags(lst);

  //return the result of the executed application
  return app.exec();
}
