#include "ovGraph.h"
#include "vtkCommand.h"
#include "ovUserStyle.h"
#include "ovUserStyleTags.h"
#include "ovDate.h"

#include "interface/ovQMainWindow.h"
#include "interface/ovQPath.h"
#include "interface/ovQTags.h"
#include "interface/ovQText.h"

// constructor for the graph
// passed in all objects necessary from GUI
ovGraph::ovGraph(
  vtkRenderWindow* wind,
  QVTKInteractor* interact,
  QListWidget* lst,
  QLabel* label,
  QLineEdit* line,
  QWidget* widge,
  QPushButton* button,
  QLabel* lab1,
  QLabel* lab2,
  vtkRenderWindow* wind2,
  QVTKInteractor* interact2,
  QProgressBar* pBar,
  QListWidget* tagLst,
  QWidget* mainWind,
  QWidget* tagWind )
{
  // initialize the tag names
  initTags();
  
  // initializes the number of edges to zero  
  edgeCount = 0;

  // initializes the font formatting to the default settings  
  captionBold = false;
  captionItalic = false;
  captionFont = ARIAL; // 0-ARIAL, 1-COURIER, 2-TIMES
  captionSize = 12;
  captionRed = 255;
  captionGreen = 255;
  captionBlue = 255;

  // initializes selected node's details to null settings  
  birthYear = 0;
  birthMonth = 0;
  birthDay = 0;
  deathYear = 0;
  deathMonth = 0;
  deathDay = 0;
  shiftSelected = -1;

  // Initializing Important Variable used in Comparisons while ovis is being set-up
  NUM_OF_NAMES = 0; // Until a graph is loaded or opened, NUM_OF_NAMES should be 0 - NUM_OF_NAMES
                    // also should not be capitalized as this infers that the number is static
  mode = 'g'; // Mode needs to be initialized
  prevMode = 'g'; // Previous mode needs to be initialized for checks...
  
  // sets the default colors for the tags to that found in the default file  
  setDefaultColors( "resources/default.tagCols" );

  // initialize global variables
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
  window->AddRenderer( rend1 );
  window->AddRenderer( rend );
  inter = interact;
  cam = vtkCamera::New();  
  rendText = vtkRenderer::New();
  mouseActor = vtkActor::New();
  rend->AddActor( mouseActor );
  selActor = vtkActor::New();
  selected = -1;
  tagList = tagLst;
  mainWin = mainWind;
  tagWindow = tagWind;
  lastHighlighted = -1;
  highlighted = false;

  sphere1 = vtkSphereSource::New();
  sphere1->SetRadius( 2.5 );
  mapper1 = vtkPolyDataMapper::New();
  mapper1->SetInput( sphere1->GetOutput() );
  sphere1->Delete();

  line1 = vtkLineSource::New();        
  line1->SetPoint1( 0.5, 0, 0 );
  line1->SetPoint2( -0.5, 0, 0 );

  mapper2 = vtkPolyDataMapper::New();
  mapper2->SetInput( line1->GetOutput() );

  line1->Delete();

  // set sorting enabled for the list to get it to be alphabetized
  listWidge->setSortingEnabled( true );
  
  // for the number of tags 
  for( int i=0; i<NUM_OF_TAGS+1; i++ )
  {
    // set tagOn to false
    tagOn[i] = false;
  }

  namesAllowedOn = false;

  rendSetUp();

  // create new renderer and add it to tag legend window
  rend2 = vtkRenderer::New();
  rend2->SetViewport( 0.0 / 600, 0.0 / 800.0, 600.0 / 600.0, 800.0 / 800.0 );
  rend2->SetBackground( 0.2, 0.2, 0.2 );
  window2->AddRenderer( rend2 );

  renderWin();
  window2->Render();
  rend2->Delete();

  ovQMainWindow* orl = ( ovQMainWindow* ) orland;
  orl->setGraph( this );
  
  // create new user style
  ovUserStyle* style = new ovUserStyle( inter, window, this );

  orl->setUserStyle( style );
  orl->graphMode( false );

  onlyEntries = false;

  progBar->setValue( 100 );

  tagOn[NUM_OF_TAGS] = true;
  tagsUsed.push_back( NUM_OF_TAGS );
}

void ovGraph::initTags()
{
  // create a file
  fstream file;

  // open the file
  file.open( "resources/default.tags" );

  // if the file opens properly
  if( file.is_open() )
  {      
    int cntr=0;
    do
    {
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );
      tags[cntr] = strdup( line );
      cout << line << ": " << cntr  << endl;
      free( line );
      cntr++;
    } while( !file.eof() );

    tags[cntr] = "WHOLE ENTRY";
    NUM_OF_TAGS = cntr;
    cout << cntr;
    file.close();
  }
  else
  {
    cout << "NOT FOUND";
  }
}

// saves the graph in its current state for next time
// *** NEEDS TO BE UPDATED FOR ALL ATTRIBUTES ***
void ovGraph::saveFile( char* filename )
{
  // An array of lists to ensure duplicate pairs are not saved   
  list<int> pairBools[25000];

  // Notifies whether a pair has already been saved
  bool match;

  char* str = strtok( filename, " ." );
  if( str != NULL )
  {
    strcat( str, ".orlando" );
  }
  else
  {
    str = filename;
  }

  ofstream oFile( str );
  if ( !oFile )
  {
    cout << "Unable to write to file: " << str << endl;
  }
  else
  {
    oFile << orlandoDataName << "\n";  // Saves Filename
    oFile << selected << "\n"; // Saves Selected Node
    oFile << namesAllowedOn << "\n"; // Saves if Names are allowed On
    oFile << mode << "\n"; // Saves the Mode
  
    // If currently in path mode, save the names associated with that path    
    if( mode == 'p' )
    {
      oFile << names[pathS] << "\n";
      oFile << names[pathF] << "\n";
    }
  
    oFile << NUM_OF_NAMES << endl;
  
    // This section traverses the Nodes in the graph, gets each of the edges associated with them
    // and prints their pair of nodes

    for( int i=0; i < NUM_OF_NAMES; i++ )
    {
      progBar->setValue( (( float )i/( float )NUM_OF_NAMES )*95 ); // Updates the progress bar
      match = false;
      list<ovEdge> children = graph1[i]->getChildren();
      list<ovEdge>::iterator it = children.begin();
      for( int j=0; j < graph1[i]->numOfChildren(); j++ )
      {
        // Checks if pair of nodes has already been saved
        list<int>::const_iterator bii;
        for( bii = pairBools[( *it ).GetNode1()].begin(); bii != pairBools[( *it ).GetNode1()].end(); bii++ )
        {
          if( ( *bii ) == ( *it ).GetNode2() )
          {
            match = true;
          }
        }
        // Pair has not been saved, continue
        if( !match )
        { 
          // Add pair to saved nodes
          pairBools[( *it ).GetNode1()].push_back( (*it ).GetNode2() );
          pairBools[( *it ).GetNode2()].push_back( (*it ).GetNode1() );
           
          // Write Node 1 to the File
          oFile << names[( *it ).GetNode1()] << endl;
          oFile <<  ( *it ).GetNode1() << endl; 

          // Write Node 2 to the File
          oFile << names[( *it ).GetNode2()] << endl;
          oFile << ( *it ).GetNode2() << endl;
 
          // Save tags associated with this pair, 1 for present, 2 for absent
          for( int t=0; t <= NUM_OF_TAGS; t++ )
          {
            if( (*it ).HasTag( t ) == true )
            {
              oFile << "1";
            }
            else
            {
              oFile << "0";
            }
          }
          oFile << endl;
        }
        it++;
      }

      // Special Case for a Node that was unconnected ( thus, had no edges )
      if( graph1[i]->numOfChildren() == 0 )
      {
        // Write Node 1 to the File
        oFile << names[i] << endl;
        oFile <<  i << endl; 

        // Write Node 2 to the File
        oFile << "EMPTY" << endl;
        oFile << "-1" << endl;

        for( int t=0; t <= NUM_OF_TAGS; t++ )
        {
          oFile << "0";
        }
        oFile << endl;
      }   
    }
    oFile.close();
  }

  char* strT = strtok( str, ". " );
  mkdir( strT, 0777 );
  char* pch;
  pch=strrchr( filename, '/' );
  char* pch2 = strdup( pch );
  char* str1 = strtok( filename, " ." );
  strcat( str1, pch2 );
  strcat( str1, ".nodes" );

  // After set-up above, saves node positions into the correct file location
  SaveNodePos( str1 );
  progBar->setValue( 96 ); // Update progress bar

  // Saves the tags Used
  char* str2 = strtok( str1, "." );
  strcat( str2, ".tagsU" );
  saveTagsUsed( str2 );

  // Saves the Line Number  
  char* str13 = strtok( str2, "." );
  strcat( str13, ".lineNum" );
  saveLineNum( str13 );

  progBar->setValue( 97 ); // Update progress bar

  // Saves the tags On  
  char* str3 = strtok( str13, "." );
  strcat( str3, ".tagsO" );
  saveTagsOn( str3 );

  char* str7 = strtok( str3, "." );
  strcat( str7, ".inc" );

  saveInc( str7 );

  progBar->setValue( 98 ); // Update progress bar

  char* str8 = strtok( str3, "." );
  strcat( str8, ".exc" );
  saveExc( str8 );
  
  // Save the entries
  char* str10 = strtok( str3, "." );
  strcat( str10, ".entries" );
  saveEntries( str10 ); 

  // Save the formatting
  char* str11 = strtok( str3, "." );
  strcat( str11, ".format" );
  saveFormatting( str11 );

  progBar->setValue( 99 ); // Update progress bar

  // Save the names that are on ( if they are allowed on )
  if( namesAllowedOn )
  {
    char* str6 = strtok( str3, "." );
    strcat( str6, ".namesO" );
    saveNamesOn( str6 );
  }

  // If an acceptable mode, save toggles
  if( mode == 'h' || mode == 't' )
  {
    char* str4 = strtok( str3, "." );
    strcat( str4, ".togO" );
    saveTogOn( str4 );

    char* str5 = strtok( str4, "." );
    strcat( str5, ".togC" );
    saveTogCon( str5 );
  }
  free( str );
  
  progBar->setValue( 100 ); // Complete the progress bar load
}

// loads the graph in from the state found in the file
void ovGraph::loadFile( char* filename )
{
  char* line;
  char* Name1;
  char* Name2;
  char* EntryNum1;
  char* EntryNum2;
  char* tags;
  char* numNames;

  int EntryNumOne;
  int EntryNumTwo;

  int progInt = 0;

  bool nameBool[25000] = {false};  // Indicates whether the Node at this EntryNum has already been created

  load = true;

  windowSetup();
  rendSetUp();

  // create a file
  fstream file;

  // open the file
  file.open( filename );

  // if the file opens properly
  if( file.is_open() )
  {      
    free( orlandoDataName );

    // get the filename of the data file from the file
    char* fileN = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( fileN, 1000 );

    orlandoDataName = fileN;
  
    // get the selected node from the file
    char* sel = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( sel, 1000 );

    selected = atoi( sel );
    free( sel ); // Leakage

    // find if names are allowed
    char* nameA = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( nameA, 1000 );

    if( strcmp( nameA, "1" ) == 0 )
    {
      allNamesOn( false );
      namesAllowedOn = true;
    }
    else
    {
      allNamesOff( true );
    }  

    free( nameA ); // Leakage    
 
    // get the mode from the file
    char* md = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( md, 1000 );
    mode = md[0];
    free( md ); // Leakage
    char* pch = strtok( filename, ". " );
    pch=strrchr( filename, '/' );
    char* pch2 = strdup( pch );
    char* fileC = strtok( filename, ". " );
    strcat( fileC, pch2 );
    strcat( fileC, ".inc" );
    loadInc( fileC ); 
    fileC = strtok( fileC, ". " );
    strcat( fileC, ".exc" );
    loadExc( fileC );    
    entries.clear(); // Clear the list of entries ( according to int )

    // create variables
    numNames = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( numNames, 1000 );
    NUM_OF_NAMES = atoi( numNames );
    free( numNames ); // Leakage
  
    // set the progress bar to 0
    progBar->setValue( 0 );
    printf( "\r[  %i%]", 0 );
    fflush( stdout );

    int namesREAD = 0;

    // if the file is open
    if( file.is_open() )
    {
      line = ( char* ) calloc( 1000, sizeof( char ));
      Name1 = ( char* ) calloc( 1000, sizeof( char ));
      EntryNum1 = ( char* ) calloc( 1000, sizeof( char ));
      Name2 = ( char* ) calloc( 1000, sizeof( char ));
      EntryNum2 = ( char* ) calloc( 1000, sizeof( char ));
      tags = ( char* ) calloc( 1000, sizeof( char ));

      while( !file.eof() )
      {
        if( (( (( float )namesREAD/( float )NUM_OF_NAMES*2 )*100 )/2 ) < 50 )
        {    
          progBar->setValue( (( (float )namesREAD/( float )NUM_OF_NAMES*2 )*100 )/2 );
          progInt = ( (( (float )namesREAD/( float )NUM_OF_NAMES*2 )*100 )/2 );
          printf( "\r[  %i%]", progInt );
          fflush( stdout );
        }
    
        // get the line from the file
        file.getline( Name1, 1000 );
        file.getline( EntryNum1, 1000 );
        file.getline( Name2, 1000 );
        file.getline( EntryNum2, 1000 );
        file.getline( tags, 1000 );
        EntryNumOne = atoi( EntryNum1 );
        EntryNumTwo = atoi( EntryNum2 );
      
        /* An Exception in case there is a Node with no Edges present in the Saved ovGraph */
        if( EntryNumTwo == -1 )
        {
          // Indicates Node is now being created
          nameBool[EntryNumOne] = true;
          // Adds Name to list of names
          names[EntryNumOne] = strdup( Name1 );
          entries.push_back( EntryNumOne );
          // create a new name
          ovName *nm1 = new ovName();
          nm1->setKey( names[EntryNumOne] );
          nm1->setKeyNum( EntryNumOne );
          graph1[EntryNumOne] = new ovNode( EntryNumOne );
        }
        else
        {    
          /* Checks if this Node has already been created - to avoid overwriting - must still be done correctly */
          if( nameBool[EntryNumOne] == false )
          {  
            // Indicates Node is now being created
            nameBool[EntryNumOne] = true;
            // Adds Name to list of names
            names[EntryNumOne] = strdup( Name1 );
            entries.push_back( EntryNumOne );
            // create a new name
            ovName *nm1 = new ovName();
            nm1->setKey( names[EntryNumOne] );
            nm1->setKeyNum( EntryNumOne );
            graph1[EntryNumOne] = new ovNode( EntryNumOne );
          }

          /* Checks if this Node has already been created - to avoid overwriting - must still be done correctly */
          if( nameBool[EntryNumTwo] == false )
          {
            nameBool[EntryNumTwo] = true;
            names[EntryNumTwo] = strdup( Name2 );
            entries.push_back( EntryNumTwo );
  
            // create a new name
            ovName *nm2 = new ovName();
            nm2->setKey( names[EntryNumTwo] );
            nm2->setKeyNum( EntryNumTwo );
            graph1[EntryNumTwo] = new ovNode( EntryNumTwo );
          }
  
          ovEdge *edge = new ovEdge();
          edge->SetNode1( EntryNumOne );
          edge->SetNode2( EntryNumTwo );
  
          for( int q=0; q<=25; q++ )
          {
            if( tags[q] == '1' )
            {
              edge->AddTag( q );
            }
          }
          
          graph1[EntryNumOne]->addChild( *edge );
          graph1[EntryNumTwo]->addChild( *edge );
          
          // delete edge; // works???
        }
  
        namesREAD++;
      }
        
      // close the file
      file.close();
    }
    // else give error message
    else
    {
      cerr << "Unable to open file: " << filename << endl;
    }

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".nodes" );     
    GetNodePos( fileC ); // Leakage ( for loop w/ line malloc'd )

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".tagsU" );  
    loadTagsUsed( fileC ); // Leakage ( single line malloc'd )

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".lineNum" );
    loadLineNum( fileC ); 

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".tagsO" );
    loadTagsOn( fileC ); // Leakage ( for loop w/ line malloc'd )

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".inc" );
    loadInc( fileC ); // Leakage ( for loop w/ line malloc'd )

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".exc" );
    loadExc( fileC ); // Leakage ( for loop w/ line malloc'd )

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".entries" );
    loadEntries( fileC ); // Leakage ( single line malloc'd )   

    fileC = strtok( fileC, ". " );
    strcat( fileC, ".format" );
    loadFormatting( fileC ); // Leakage ( single line malloc'd )
  
    ovQMainWindow* orl = ( ovQMainWindow* ) orland;
    orl->setGraph( this );
    mergeEdges();
    progBar->setValue( 52 );
    setTitleText();
    progBar->setValue( 54 );
    initNames();
    progBar->setValue( 55 );

    if( namesAllowedOn )
    {   
      orl->nameTagsOn();
      fileC = strtok( fileC, ". " );
      strcat( fileC, ".namesO" );
      loadNamesOn( fileC ); // Need to change loadNamesOn
    }
    else
    {
      orl->nameTagsOff();
    }
  
    if( mode == 'h' || mode == 't' )
    {
      fileC = strtok( fileC, ". " );
      strcat( fileC, ".togO" );
      loadTogOn( fileC );
  
      fileC = strtok( fileC, ". " );
      strcat( fileC, ".togC" );
      loadTogCon( fileC );
    }
  
    // create new user style
    ovUserStyle* style = new ovUserStyle( inter, window, this );

    // if the mode is graph then redraw the graph
    if( mode == 'g' )
    {
      redrawGraph();
      rend->ResetCamera();
      displayNdInfo( 0, 0 );
      orl->setUserStyle( style );
      orl->graphMode( false );
    }
    // else if the mode is camera redraw the graph and go into camera mode
    else if( mode == 'c' )
    {
      redrawGraph();
      rend->ResetCamera();
      displayNdInfo( 0, 0 );
      orl->cameraMode();
    }
    // else if the mode is toggle, draw the toggled graph
    else if( mode == 't' )
    {
      toggle( 0, 0 );
      drawToggled();
      rend->ResetCamera();
      orl->setUserStyle( style );
      orl->toggleMode();
    }
    // else if the mode is highlight, draw the highlighted graph
    else if( mode == 'h' )
    {
      highlight( 0, 0 );
      drawHighlighted();  
      rend->ResetCamera();  
      orl->setUserStyle( style );
      orl->highlightMode();
    }
    // else the mode is path, find the strings of the path ends and display the paths between them
    else
    {
      // get path beginning
      char* nd1 = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( nd1, 1000 );
  
      // get path end
      char* nd2 = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( nd2, 1000 );
  
      orl->setUserStyle( style );
      orl->graphMode( false );
      findPathBtw( nd1, nd2 );
  
      free( nd1 );
      free( nd2 );
    }

    if( namesAllowedOn )
    {  
      selectedNodesOn();
    }
    drawKeys(); 
    load = false;
    orl->enableMenuItems( mode );      
  }
  // else give error message
  else
  {
    cerr << "Unable to open file: " << filename << endl;
  }
}

// Clears the graph of its visual components, without introducing new components
void ovGraph::clearGraph()
{

int oldNumNames = NUM_OF_NAMES;

for( int j = 0; j < NUM_OF_NAMES; j++ )
{

// Would have to find each edge and it's respective VTK aspects to completely clear the graph.
//
/*
  list<ovEdge> nodeEdges = graph1[j]->getChildren();

  for( int r = 0; r < nodeEdges.size(); r++ )
  {

  nodeEdges.pop_front(); // pops all of the edges off ( delete )

  }
*/

graph1[j]->resetEdgeActors(); // Deletes the edge actors associated with each node

graph1[j]->resetChildren(); // Deletes edges associated with each node

delete graph1[j]; // deletes every node in the graph


}
  
  /* The rest resets all of the variables to their initial values */

  edgeCount = 0;

  captionBold = false;
  captionItalic = false;
  captionFont = ARIAL; // 0-ARIAL, 1-COURIER, 2-TIMES
  captionSize = 12;
  captionRed = 255;
  captionGreen = 255;
  captionBlue = 255;

  birthYear = 0;
  birthMonth = 0;
  birthDay = 0;
  deathYear = 0;
  deathMonth = 0;
  deathDay = 0;

  // Initializing Important Variable used in Comparisons while ovis is being set-up
  NUM_OF_NAMES = 0; // Until a graph is loaded or opened, NUM_OF_NAMES should be 0
                    // NUM_OF_NAMES also should not be capitalized as this infers that the number is static
  mode = 'g'; // Mode needs to be initialized
  prevMode = 'g'; // Previous mode needs to be initialized for checks...
  
  setDefaultColors( "resources/default.tagCols" );

  // initialize global variables
  length = 0;
  cutoff = 0;
  cutOff = 0.0;
  x = 0;
  y = 0;
  z = 0;
 // progBar = pBar;
 // inter2 = interact2;
 // window2 = wind2;
//  label1 = lab1;
 // label2 = lab2;
//  searchButton = button;
 // orland = widge;
 // searchLine = line;
//  labelSelected = label;
//  listWidge = lst;
  rend1 = vtkRenderer::New();
  rend = vtkRenderer::New();
//  window = wind;
  window->AddRenderer( rend1 );
  window->AddRenderer( rend );
//  inter = interact;
  cam = vtkCamera::New();  
  rendText = vtkRenderer::New();
  mouseActor = vtkActor::New();
  rend->AddActor( mouseActor );
  selActor = vtkActor::New();
  selected = -1;
 // tagList = tagLst;
//  mainWin = mainWind;
 // tagWindow = tagWind;
  lastHighlighted = -1;
  highlighted = false;

  // create a sphere
  sphere1 = vtkSphereSource::New();
  sphere1->SetRadius( 2.5 );
  
  // create the mapper
  mapper1 = vtkPolyDataMapper::New();
  mapper1->SetInput( sphere1->GetOutput() );
  // initialize the tag names

  sphere1->Delete();

  // create line
  line1 = vtkLineSource::New();        
  line1->SetPoint1( 0.5, 0, 0 );
  line1->SetPoint2( -0.5, 0, 0 );

  mapper2 = vtkPolyDataMapper::New();
  mapper2->SetInput( line1->GetOutput() );

  line1->Delete();

  // initialize the tag names
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

  // set sorting enabled for the list to get it to be alphabetized
  listWidge->setSortingEnabled( true );
  
  // for the number of tags 
  for( int i=0; i<NUM_OF_TAGS+1; i++ )
    {
      // set tagOn to false
      tagOn[i] = false;
    }

  namesAllowedOn = false;

  rendSetUp();

// create new renderer and add it to tag legend window
  rend2 = vtkRenderer::New();
  rend2->SetViewport( 0.0/600, 0.0/800.0, 600.0/600.0, 800.0/800.0 );
  rend2->SetBackground( 0.2, 0.2, 0.2 );
  window2->AddRenderer( rend2 );

  renderWin();
  window2->Render();
  rend2->Delete();

  ovQMainWindow* orl = ( ovQMainWindow* ) orland;
  orl->setGraph( this );
  
  // create new user style
  
  ovUserStyle* style = new ovUserStyle( inter, window, this );

  orl->setUserStyle( style );
  orl->graphMode( false );

  onlyEntries = false;

  progBar->setValue( 100 );

  tagOn[NUM_OF_TAGS] = true;
  tagsUsed.push_back( NUM_OF_TAGS );


cout << "ovGraph Cleared. " << endl;

// Should also set "Clear ovGraph" to unselectable, as well as the rest.

}







// loads an xml file to work with -- must open and add ^ as the last line and take out first 2 lines
void ovGraph::loadXML( char* filename )
{  
  ovQMainWindow* orl = ( ovQMainWindow* ) orland;
  orl->setGraph( this );

  progBar = orl->getProgressBar();

  // set the progress bar to 0
  progBar->setValue( 0 );
  
  initialize( filename );
  
  // create new user style
  ovUserStyle* style = new ovUserStyle( inter, window, this );
  orl->setUserStyle( style );
  
  drawGraph(); 
  display();
  orl->graphMode( true ); 
}

// render the window
void ovGraph::renderWin()
{
  window->Render();

  // set the progress bar to 100
  progBar->setValue( 100 );
}

// save the included words for this dataset
void ovGraph::saveInc( char* filename )
{  
  // create file
  ofstream file( filename );
  fstream fl;

  fl.open( "tmp/orlando.inc" );

  do
    {   
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      fl.getline( line, 1000 );
      
      file << line << "\n";

      free( line );

    }while( !fl.eof() );

  file.close();
  fl.close();
}

// loads the included words from the file
void ovGraph::loadInc( char* filename )
{  
  // create file
  ofstream file( "tmp/orlando.inc" );
  fstream fl;

  fl.open( filename );

   do
    {   
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      fl.getline( line, 1000 );
      
      file << line << "\n";

  free( line );

    }while( !fl.eof() );

  file.close();
  fl.close();
}

// saves the excluded words for the dataset
void ovGraph::saveExc( char* filename )
{  
  // create file
  ofstream file( filename );
  fstream fl;

  fl.open( "tmp/orlando.exc" );

  do
    {   
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      fl.getline( line, 1000 );
      
      file << line << "\n";

  free( line );
      
    }while( !fl.eof() );

  file.close();
  fl.close();
}

// loads the excluded words from the file
void ovGraph::loadExc( char* filename )
{  
  // create file
  ofstream file( "tmp/orlando.exc" );
  fstream fl;

  fl.open( filename );
  
  do
    {   
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      fl.getline( line, 1000 );
      
      file << line << "\n";

      free( line );

    }while( !fl.eof() );

  file.close();
  fl.close();
}

// save the names that have been turned on
void ovGraph::saveNamesOn( char* filename )
{  
  // create file
  ofstream file( filename );

  // for all the tags save the on bool in a file
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      file << nameOn[i] << "\n";
    }

  file.close();
}

// load the names that had been turned on
void ovGraph::loadNamesOn( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );

    nameOn[i] = atoi( line );

       if( nameOn[i] )
    {
    selectedNodes.push_back( i );
      }

    free( line );
  }
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

void ovGraph::saveLineNum( char* filename )
{
  ofstream file( filename );
  
  for( int i=0; i<NUM_OF_NAMES_C; i++ )
  {
    file << lineNum[i] << "\n";
  }
  
  file.close();
  
}


// load the tagsUsed 
void ovGraph::loadLineNum( char* filename )
{
  fstream file;
  
  int i = 0;
  
  file.open( filename );
  
  if( file.is_open() )
    {
    // tagsUsed.clear();
    
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );
    
    while( !file.eof() )
    {
      // tagsUsed.push_back( atoi( line ));
      lineNum[i] = atoi( line );
      i++;
      file.getline( line, 1000 );
    }
    // numOfTags = tagsUsed.size();
    file.close();
    free( line );
    }
  // else give error message
  else cerr << "Unable to open file" << filename << endl;
}




// save tagsUsed to a file
void ovGraph::saveTagsUsed( char* filename )
{
  // create file
  ofstream file( filename );

  list<int>::iterator it;
  for( it=tagsUsed.begin(); it!=tagsUsed.end(); it++ )
    {
      file << *it << "\n";
    }

  file.close();
}

// load the tagsUsed 
void ovGraph::loadTagsUsed( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      tagsUsed.clear();

      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );

      while( !file.eof() )
  {
    tagsUsed.push_back( atoi( line ));
    file.getline( line, 1000 );
  }
      numOfTags = tagsUsed.size();
      file.close();
      free( line );
    }
  // else give error message
  else cerr << "Unable to open file" << filename << endl;
}

// save the nodes that are toggled on
void ovGraph::saveTogOn( char* filename )
{
  // create file
  ofstream file( filename );

  list<int>::iterator it;
  for( it=toggledOn.begin(); it!=toggledOn.end(); it++ )
    {
      file << *it << "\n";
    }

  file.close();
}

// load the nodes that were toggled on
void ovGraph::loadTogOn( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      toggledOn.clear();

      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );

      while( !file.eof() )
  {
    toggledOn.push_back( atoi( line ));
    file.getline( line, 1000 );
  }
      free( line );
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

// save the nodes that are toggle connected
void ovGraph::saveTogCon( char* filename )
{
  // create file
  ofstream file( filename );

  // for all the tags save the on bool in a file
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      file << toggleConnected[i] << "\n";
    }

  file.close();
}

// load the nodes that were toggle connected
void ovGraph::loadTogCon( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );

    toggleConnected[i] = atoi( line );

    free( line );
  }
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

// save the tags that are on
void ovGraph::saveTagsOn( char* filename )
{
  // create file
  ofstream file( filename );

  // for all the tags save the on bool in a file
  for( int i=0; i<NUM_OF_TAGS+1; i++ )
    {
      file << tagOn[i] << "\n";
    }

  file.close();
}

// load the tags that were on
void ovGraph::loadTagsOn( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      for( int i=0; i<NUM_OF_TAGS+1; i++ )
  {
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );

    tagOn[i] = atoi( line );
    free( line );
  }
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}



// save each of the entries
void ovGraph::saveEntries( char* filename )
{
  // create file
  ofstream file( filename );

   list<int>::iterator ei;

  // Write each of the entries to a file
   for( ei=entries.begin(); ei != entries.end(); ++ei )
  {
   file << *ei << "\n";
  }
  
  file.close();
}




// load the tags that were on
void ovGraph::loadEntries( char* filename )
{
  fstream file;

  file.open( filename );

  if( file.is_open() )
    {
      entries.clear();

      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );

      while( !file.eof() )
  {
    entries.push_back( atoi( line ));
    file.getline( line, 1000 );
  }
      file.close();
      free( line );

    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}



/* Must Save the Formatting for the File */
//  bool captionBold;
//  bool captionItalic;
//  int captionFont; // 0-ARIAL, 1-COURIER, 2-TIMES
//  int captionSize; 
//  int captionRed;
//  int captionGreen;
//  int captionBlue;

// Save the formatting options that were chosen
void ovGraph::saveFormatting( char* filename )
{

  // create file
  ofstream file( filename );

  // Save the different formatting options ( text, labels etc. )
  file << captionBold << endl;
  file << captionItalic << endl;
  file << captionFont << endl;
  file << captionSize << endl;
  file << captionRed << endl;
  file << captionGreen << endl;
  file << captionBlue << endl;

  // close the file
  file.close();

}


// Load the formatting options that had been chosen ( text, labels etc. )
void ovGraph::loadFormatting( char* filename )
{
  fstream file;
  int bold;
  int italic;

  file.open( filename );

  if( file.is_open() )
  {
      char* line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );

      bold = atoi( line );

      if( bold == 0 )
      {
        captionBold = false;
      }
      else
      {
        captionBold = true;
      }

      file.getline( line, 1000 );

      italic = atoi( line );

      if( italic == 0 )
      {
        captionItalic = false;
      }
      else
      {
        captionItalic = true;
      }

      file.getline( line, 1000 );
      captionFont = atoi( line );

      file.getline( line, 1000 );
      captionSize = atoi( line );

      file.getline( line, 1000 );
      captionRed = atoi( line );

      file.getline( line, 1000 );
      captionGreen = atoi( line );

      file.getline( line, 1000 );
      captionBlue = atoi( line );

      free( line );

  }
  else cerr << "Unable to open file: " << filename << endl;
}










// save node positions to file
// filename is the name of the file to save to
void ovGraph::SaveNodePos( char* filename )
{
  // create variables
  ofstream file( filename );
  ovNode* nd;

  // for the number of names
  for( int i=0; i <NUM_OF_NAMES; i++ )
    {
      // get the node
      nd=graph1[i];
      // get the coordinates
      x=nd->getX();
      y=nd->getY();
      z=nd->getZ();
      // print the coordinates to the file
      file << x << " " << y << " " << z << "\n";
    }

  file.close();
}

// load node positions from file
// filename is the name of the file to open
void ovGraph::GetNodePos( char* filename )
{
  // create variables
//  char* line;
  int x, y, z;  
  fstream file;
  ovNode* nd;
  
  // open file
  file.open( filename );

  // if the file is open
  if( file.is_open() )
    {
      // for the number of names
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    // get the line from the file
//    line = ( char* ) calloc( 1000, sizeof( char ));
//    file.getline( line, 1000 );
    // get the coordinates from the line
    file >> x;
    file >> y;
    file >> z;
//    x=atoi( strtok( line, " " ));
//    y=atoi( strtok( NULL, " " ));
//    z=atoi( strtok( NULL, " \n" ));
    // set the center to the read in coordinates
    graph1[i]->setCenter( x, y, z );
//    free ( line );
        }
      // close the file
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

// draw the key for a certain colour
// a and b are the first and second coordinates for the start and end of the y axis
// x, y, z are the values for the colour of the background
// nam is the text that is going inside the key
// on is true if the tag is on, else the tag is off
void ovGraph::drawKey( float a, float b, float c, float d, double* col, char* nam, bool on )
{
  // create new renderer
  rend1 = vtkRenderer::New();
  rend1->SetViewport( c/800.0, a/600.0, d/800.0, b/600.0 );

  // if the tag is on
  if( on )
    {
      // set the background colour to the one passed in
      rend1->SetBackground( col );
    }
  else
    {
      // set the colour to the regular background
      rend1->SetBackground( 0.2, 0.2, 0.2 );
    }
  // add the renderer to the window
  window2->AddRenderer( rend1 );

  // create a new text actor
  vtkTextActor *txtAct = vtkTextActor::New();
  txtAct->SetInput( nam );
  txtAct->GetTextProperty()->SetLineOffset( 10 );

  // if the tag is on
  if( on )
    {
      // set the text colour to black
      txtAct->GetTextProperty()->SetColor( 0.0, 0.0, 0.0 );
    }
  else
    {
      // set the text colour to white
      txtAct->GetTextProperty()->SetColor( 1.0, 1.0, 1.0 );
    }
  
  // add the actor to the renderer
  rend1->AddActor( txtAct );

  // delete the actor
  txtAct->Delete();
}

// draw all the keys ( legend )
void ovGraph::drawKeys()
{
  // initialize variables
  float width = 800.0;
  float height = 600.0;

  // create new renderer and add it to tag legend window
  rend2 = vtkRenderer::New();
  rend2->SetViewport( 0.0/width, 0.0/height, width/width, height/height );
  rend2->SetBackground( 0.2, 0.2, 0.2 );
  window2->AddRenderer( rend2 );

  // initialize row and column variables
  int numRows = 0;
  int numCols = 3;

  // find the number of rows that are needed from the number of tags
  if( numOfTags%numCols == 0 )
    {
      numRows = numOfTags/numCols + 1;
    }
  else
    {
      numRows = numOfTags/numCols + 2;
    }

  // get the line width and height allowed for each tag
  float lineWidth = width/numCols;
  float lineHeight = height/numRows;

  // y coordinates
  float a = 0.0;
  float b = lineHeight;

  // y coordinates
  float c = 0.0;
  float d = lineWidth;

  // create a new renderer
  rend1 = vtkRenderer::New();
  rend1->SetViewport( 0.0/800.0, lineHeight*( numRows-1 )/600.0, 800.0/800.0, 600.0/600.0 );
  rend1->SetBackground( 0.2, 0.2, 0.2 );

  // add the renderer to the window
  window2->AddRenderer( rend1 );

  // create text actor for the title of the window
  vtkTextActor *txtAct = vtkTextActor::New();
  txtAct->SetInput( "TAG LEGEND" );
  txtAct->GetTextProperty()->BoldOn();
  txtAct->GetTextProperty()->SetColor( 1.0, 1.0, 1.0 );
  txtAct->GetTextProperty()->SetLineOffset( 10 );
  txtAct->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
  txtAct->GetPosition2Coordinate()->SetValue( 0.6, 0.1 );

  // add the text actor to the renderer
  rend1->AddActor( txtAct );

  // delete the text actor
  txtAct->Delete();

  // initialize variables
  list<int>::iterator it;
  int i=0;
  it=tagsUsed.begin();

  // while the tags have not all been gone through
  while( it!=tagsUsed.end() )
    {
      // for the number of columns
      for( int j=0; j<numCols; j++ )
  {
    // draw the first key
    drawKey( a, b, c, d, getColor( *it ), tags[*it], tagOn[*it] );
    
    // increment the column
    c = d;
    d += lineWidth;
    
    // if the number of columns have not been reached
    if( j<numCols )
      {
        // move on to next tag
        it++;
        // increment i
        i++;
      }
    
    // if all the tags have been gone through
    if( i==numOfTags )
      {
        // break out of the loop
        j = numCols;
      }
  }
      
      // if there are still tags left to go through
      if( i<numOfTags )
  {
    // increment the row
    a = b;
    b += lineHeight;

    // reset the column
    c = 0.0;
    d = lineWidth;
  }
    }  

  // render the tags window
  window2->Render();
}

// reset the graph connectivity
void ovGraph::resetGraphCons()
{
  // for the number of names reset ndCon values to false
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      ndCon[i] = false;
    }
}

// remove the edges for the node passed in
void ovGraph::removeEdgesForNode( int nd )
{
  // get the list of edge actors
  list<vtkActor*> edgs = graph1[nd]->getEdgeActors();
  
  // for all of the edge actors for the node remove the actor
  list<vtkActor*>::iterator it;
  for( it=edgs.begin(); it!=edgs.end(); it++ )
    {
      rend->RemoveActor( *it );
    }

  // reset the edge actors for the node
  graph1[nd]->resetEdgeActors();
}

// remove the node that is passed in
void ovGraph::removeNode( int nd )
{
  vtkActor* act = graph1[nd]->getSphereActor();
  rend->RemoveActor( act );
}

// draw a node to the screen
// returns a pointer to the vtkActor
// ind is the index number of the node
// alpha decides whether the node is fully visible ( false ) or if it is only 25% ( true )
// the actor is the pointer to the actor to be returned
// colR, colG, colB are the RGB numbers for the colour of the node
vtkActor* ovGraph::drawNode( int ind, bool alpha, vtkActor* actor, double colR, double colG, double colB )
{
  // get the selected node
  ovNode* nd = graph1[ind];

  if( !ndCon[ind] || actor == highlightActor || ind == selected || ind == oldSelected || ind == lastHighlighted )
    {
      // get the coordinates of the node
      x = nd->getX();
      y = nd->getY();
      z = nd->getZ();
      
      if( actor != highlightActor && !alpha )
  {
    ndCon[ind] = true;
  }

      // create a new actor
      actor = vtkActor::New();
      actor->SetMapper( mapper1 );
      actor->GetProperty()->SetColor( colR, colG, colB );
      actor->SetPosition( x, y, z );
  
      if( alpha )
  {
    // set the opacity low so the edge is faded
    actor->GetProperty()->SetOpacity( 0.25 );
  }
      
      // if the index is not the selected node
      if( ind != selected )
  {
    // remove the node and set the actor to the new one
    removeNode( ind ); 
    nd->setSphereActor( actor );
  }
      
      // add the actor to the renderer
      rend->AddActor( actor );      
    }

  if( ind != selected && actor != highlightActor )
    {
      actor = nd->getSphereActor();
    }

  // return the actor
  return actor;
}

// draw the edge to the screen
// stInd and endInd are the vertices of the edge
// alpha decides whether the edge is fully visible ( false ) or if it is only 25%  ( true )
// tag is the number of the tag
void ovGraph::drawEdge( int stInd, int endInd, bool alpha, int tag )
{
  // initialize variables
  int a = stInd;
  int b = endInd;
                
  // get the x coordinates
  int x1 = graph1[a]->getX();
  int x2 = graph1[b]->getX();
          
  // get the y coordinates
  int y1 = graph1[a]->getY();
  int y2 = graph1[b]->getY();
          
  // get the z coordinates
  int z1 = graph1[a]->getZ();
  int z2 = graph1[b]->getZ();
  
  // create actor for line
  vtkActor *linAct = vtkActor::New();
  linAct->SetMapper( mapper2 );

  int flip = 1;
  int tagNum = tag;

  if( tag > NUM_OF_TAGS/2 )
    {
      flip = -1;
      tagNum = tag-( NUM_OF_TAGS/2 );
    }  

  vtkMatrix4x4* matrix1 = vtkMatrix4x4::New();
  matrix1->Element[0][0]=( x2+( 0.1*tagNum*flip ))-( x1+( 0.1*tagNum*flip ));
  matrix1->Element[1][0]=y2-y1;
  matrix1->Element[2][0]=( z2+( 0.1*tagNum*flip ))-( z1+( 0.1*tagNum*flip ));
  matrix1->Element[3][0]=0.0;
  matrix1->Element[0][1]=0.0;
  matrix1->Element[1][1]=1.0;
  matrix1->Element[2][1]=0.0;
  matrix1->Element[3][1]=0.0;
  matrix1->Element[0][2]=0.0;
  matrix1->Element[1][2]=0.0;
  matrix1->Element[2][2]=1.0;
  matrix1->Element[3][2]=0.0;
  matrix1->Element[0][3]=( x1+( 0.1*tagNum*flip )+x2-( 0.1*tagNum*flip ))/2.0;
  matrix1->Element[1][3]=( y1+y2 )/2.0;
  matrix1->Element[2][3]=( z1+( 0.1*tagNum*flip )+z2+( 0.1*tagNum*flip ))/2.0;
  matrix1->Element[3][3]=1.0;

  if( abs( x1-x2 )<=( abs( z1-z2 )/2 ))
    {
      matrix1->Element[0][3]=( x1+( 0.1*tagNum*flip )+x2+( 0.1*tagNum*flip ))/2.0;
    }

  linAct->SetUserMatrix( matrix1 );

  // set the colour for the actors
  linAct->GetProperty()->SetColor( getColor( tag ));

  if( alpha )
    {
      // set the opacity low so the edge is faded
      linAct->GetProperty()->SetOpacity( 0.1 );
    }      
    
  // add the actors to the renderers
  rend->AddActor( linAct );

  // add the edge actor to the nodes
  graph1[a]->addEdgeActor( linAct );
  graph1[b]->addEdgeActor( linAct );
  
   
  // delete vtk objects    
  linAct->Delete();
  matrix1->Delete();
}

// draw the edges for the node to the screen
// ind is the index for the node
// alpha decides whether the edges should be fully visible ( false ) or if it is only 25% ( true )
void ovGraph::drawEdgesForNode( int ind, bool alpha )
{
  // get the children
  list<ovEdge> ch = graph1[ind]->getChildren();

  // create iterators
  list<ovEdge>::iterator j;
  list<int>::iterator k;

  bool nodeConnected = false;

  // for the number of children
  for( j=ch.begin(); j!=ch.end(); j++ )
    {       
      bool connected = false;

      // for the number of tags
      for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
  {
    // if the tag is on and the edge has not already beeen connected
    if( tagOn[*k] )// && !con[j->GetNode1()][j->GetNode2()] && !con[j->GetNode2()][j->GetNode1()] )//  && !connected )
      {
        // if the names are connected with this tag
        if( j->HasTag( *k ))
    {
      // draw the edge
      drawEdge( j->GetNode1(), j->GetNode2(), alpha, ( *k ));
      // connected = true;

      // if( !nodeConnected )
        {
          // draw the node if it has not already been drawn
          drawNode( ind, alpha, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
          nodeConnected = true;
        }
      
      // get the other vertex of the edge
      int q = j->GetNode1();

      if( q == ind )
        {
          q = j->GetNode2();
        }

      // draw the node if it has not already been drawn
      drawNode( q, alpha, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
    }   
      }
  }
    }      
}

// tag is touched, find which tag and turn on/off
void ovGraph::tagTouched( int x, int y )
{
  // initialize variables
  int height = 600.0;
  int width = 800.0;
  int numRows = 0;
  int numCols = 3;

  // find the number of rows based on the number of tags
  if( numOfTags%numCols == 0 )
    {
      numRows = numOfTags/numCols +1;
    }
  else
    {
      numRows = numOfTags/numCols + 2;
    }

  // initialize variables
  float lineWidth = width/numCols;
  float lineHeight = height/numRows;

  // initialize x coordinates
  float a = 0.0;
  float b = lineHeight;

  // initialize y coordinates
  float c = 0.0;
  float d = lineWidth;

  // initialize variables
  list<int>::iterator it;
  int i=0;
  it=tagsUsed.begin();

  // while there are still tags to search through
  while( it!=tagsUsed.end() )
    {
      // for the number of columns
      for( int j=0; j<numCols; j++ )
  {  
    // if the coordinates passed in are within the tag's area
    if( y > a && y < b && x > c && x < d )
      {
        // turn on or off the tag
        turnOnOffTag( *it );
      }
    
    // increment the column
    c = d;
    d += lineWidth;

    // get the next tag
    it++;

    // increment the number of tags done
    i++;

    // if all the tags have been gone through
    if( i==numOfTags )
      {
        // break out of the loop
        j = numCols;
      }
  }
      
      // if there are still more tags to go through
      if( i<numOfTags )
  {
    // increment the row
    a = b;
    b += lineHeight;

    // reset the column
    c = 0.0;
    d = lineWidth;
  }
    }

  // draw the keys
  drawKeys();

  // render the tag window
  window2->Render();
      
  // set the mode to graph mode to redraw the graph with the tag off/on
  // ovQMainWindow* orlan = ( ovQMainWindow* ) orland;
  // orlan->graphMode( true );

  if( mode == 'p' )
    {

    }
  else if( mode == 't' )
    {
      drawToggled();
    }
  else if( mode == 'h' )
    {
      drawHighlighted();
    }
  else if( mode == 'c' )
    {
      if( prevMode == 't' )
  {
    drawToggled();
  }
      else if( prevMode == 'h' )
  {
    drawHighlighted();
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

  select( false );

  // render the main window
  renderWin();
}

// set toggle to tog
void ovGraph::setToggle( bool tog )
{
  if( tog )
    {
      // set mode to toggle
      mode = 't';
    }

  // set toggle to variable passed in
  toggleTrue = tog;

  // turn off path
  pathTrue = false;
}

// set up the renderer for rendText
void ovGraph::textSetUp()
{
  listWidge->clear();
}

// set up the renderer for rend
void ovGraph::rendSetUp()
{ 
  // create an actor collection
  vtkActorCollection* actors;
  
  // if the renderer is not NULL
  if( rend !=NULL )
    {
      // remove the renderer from the window
      window->RemoveRenderer( rend );
    }
  
  // delete the renderer and create a new one
  rend->Delete();
  rend = vtkRenderer::New();

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      if( nameOn[i] )
  {
    // add the text actors
    rend->AddActor( nameText[i] );
  }
    }

  // set up the renderer
  rend->SetBackground( 0.2, 0.2, 0.2 );
  rend->SetActiveCamera( cam );

  // add renderer to window
   window->AddRenderer( rend );
}

// display the path information
// a and b are start and end of the path
// path is the list of nodes in the path
void ovGraph::displayPathInfo( int a, int b, list<int> path )
{ 
  // nam to hold the path title
  char* nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
  sprintf( nam, "%s  &  %s", names[a], names[b] );
      
  // set the labels
  label1->setText( "Paths Between:" );
  label2->setText( "Results:" );
  labelSelected->setText( nam );
  free( nam );

  // for the nodes in the path
  list<int>::iterator j;
  for( j=path.begin(); j!=path.end(); j++ )
    {
      // nam1 to hold the name of the node
      char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam1, "%s", names[*j] );
    
      // draw the node in the path results
      drawText( 0, nam1, 0, false );
      free( nam1 );
    }

  // draw a blank line to show end of path
  drawText( 0, "", 0, false );
}

// draw text
// dt -> the data to decide the colour
// nam -> the char* to print out
// offset -> the offset on the screen
// col -> changes to the color based on dt if true else colour is white
void ovGraph::drawText( float dt, char* nam, int offset, bool col )
{
  new QListWidgetItem( nam, listWidge );
}

// recursive algorithm to find all paths
// cuts off at the cutOff value so it will only do the shortest paths
bool ovGraph::findAllPaths( int startNode, int nodeToFind, list<int> pth, int cutOff, bool display )
{
  // if the size cutOff has been reached or the path is empty return
  if( pth.size() == cutOff || pth.size() <= 0 )
    {
      return found;
    }

  // if the node to found has been found
  if( startNode == nodeToFind )
    {
      // show and print the path information
      printPath( pth );
      displayPathInfo( startNode, nodeToFind, pth );
  
      // set found to true and return
      found = true;
      return true;
    }

  // get the children
  list<ovEdge> ch = graph1[startNode]->getChildren();

  // for the adjacent nodes
  list<ovEdge>::iterator it;  
  for( it=ch.begin(); it!=ch.end(); it++ )
    {
      int q = ( *it ).GetNode1();

      if( q == startNode )
  {
    q = ( *it ).GetNode2();
  }

      // put the node in the path
      pth.push_back( q );
      // recall the recursive function with this node 
      found = findAllPaths( q, nodeToFind, pth, cutOff, display );
      // delete the node from the path
      pth.pop_back();       
    }

  // return when finished
  return found;
}

// find out if there is a path between the start node and the node to find
bool ovGraph::findIfPath( int startNode, int nodeToFind, int cutOff )
{
  bool found = false;
  list<int> q;
  q.push_back( startNode );

  int parent[NUM_OF_NAMES];
  bool done[NUM_OF_NAMES];

  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      parent[i] = -1;
      done[i] = false;
    }

  while( !q.empty() )
    {
      int currentVertex = q.front();
      q.pop_front();

      list<ovEdge> ch = graph1[currentVertex]->getChildren();

      list<ovEdge>::iterator it;
      for( it=ch.begin(); it!=ch.end(); it++ )
  {
    int a = ( *it ).GetNode1();
    if( a == currentVertex )
      {
        a = ( *it ).GetNode2();
      }

    if( a == nodeToFind )
      {        
        return true;
      }
    
    if( !done[a] )
      {
        q.push_front( a );
        parent[a] = currentVertex;
      }
  }
      done[currentVertex] = true;
    }

  return found;
}

// finds the path between nodes passed in 
void ovGraph::findPathBtw( char* x, char* y )
{   
  int c = -1;
  int d = -1;

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {    
      if( strcmp( x, names[i] ) == 0 )
  {
    c = i;
    pathS = i;
  }
          
      if( strcmp( y, names[i] ) == 0 )
  {
    pathF = i;
    d = i;
  }
    }

  // set up the renderers
  rendSetUp();
  textSetUp();


  // clear the inPath list
  inPath.clear();
  
  // set sorting enabled to false for the list so the paths show up in order
  listWidge->setSortingEnabled( false );

  // set pathTrue to true
  pathTrue = true;
  
  // turn toggle on
  toggleOn();

  // put the first node in the path list
  list<int> path;
  path.push_back( c );
  
  oldSelected = selected;
  selected = d;

  // create graph2 and initialize it
  int graph2[NUM_OF_NAMES];
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      graph2[i] = i;
    }
  
  // initialize found to false
  found = false;  


  // check to make sure that there is a path between the nodes
  if( findIfPath( c, d, NUM_OF_NAMES ))
    {            
      // find the shortest path between the nodes
       int sz = findNodes( c, path, d, graph2 );
      
      // if there is an indirect path found
      if( sz >= 0 )
  {
    // for the number of names initialize path connected
    for( int i=0; i<NUM_OF_NAMES; i++ )
      {
        pathConnected[i] = false;
      }  
    
    // create new path to search with
    path.clear();      
    path.push_back( c );
    
    // initialize found to false
    found = false;
    
    // find all paths with the cutoff of the shortest path's size
    findAllPaths( c, d, path, sz, true );
  }
    }
  
  // show the path information for the two nodes
  list<int> empty;    
  displayPathInfo( c, d, empty );
  empty.push_back( c );
  empty.push_back( d );

  printPath( empty );

  // set mode to path
  mode = 'p';
}

// finds the path from the old selected node to the node at position passed in
void ovGraph::findPath( int a, int b )
{
  // set the mode to path
  mode = 'p';

  // set up the renderers
  rendSetUp();
  textSetUp();

  // clear the inPath list
  inPath.clear();
  
  // set sorting enabled to false for the list so the paths show up in order
  listWidge->setSortingEnabled( false );

  // set pathTrue to true
  pathTrue = true;

  // get the selected node
  int c = selected;
  
  // turn toggle on
  toggleOn();
 
  // if a node is selected
  if( selected >= 0 )  
    {
      pathS = selected;

      // if there is a node at the position passed in
      if( nodeAtPos( a, b, true )>=0 )
  {    
    pathF = selected;
    
    // put the old selected node in the path list
    list<int> path;
    path.push_back( c );

    // create graph2 and initialize it
    int graph2[NUM_OF_NAMES];
    for( int i=0; i<NUM_OF_NAMES; i++ )
      {
        graph2[i] = i;
      }

    // initialize found to false
    found = false;
    
    // check to make sure that there is a path between the nodes
    if( findIfPath( c, selected, NUM_OF_NAMES ))
      {        
        // find the shortest path between the nodes
        int sz = findNodes( c, path, selected, graph2 );
 
        // if there is an indirect path found
        if( sz >= 0 )
    {
      // for the number of names initialize path connected
      for( int i=0; i<NUM_OF_NAMES; i++ )
        {
          pathConnected[i] = false;
        }  

      // create new path to search with
      path.clear();      
      path.push_back( c );

      // initialize found to false
      found = false;

      // find all paths with the cutoff of the shortest path's size
      findAllPaths( c, selected, path, sz, true );
    }
      }
   
    // show the path information for the two nodes
    list<int> empty;    
    displayPathInfo( c, selected, empty );
    empty.push_back( c );
    empty.push_back( selected );
    printPath( empty );

 
    // set mode to path
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

// uses dijkstra's algorithm to find the shortest path
// a and b are the start and end of the path
// path is the list of the nodes in the path 
// graph2 is a copy of the graph object for manipulation
int ovGraph::findNodes( int a, list<int> path, int b, int graph2[] )
{
  // set infinity to a large number
  int infinity = 10000000;

  // initialize variables
  int d = a;
  float distance[NUM_OF_NAMES];
  int previous[NUM_OF_NAMES];

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // initialize variables
      distance[i] = infinity;
      previous[i] = -1;
    }
  
  // initialize distance for start node
  distance[a] = 0;
 
  // while the graph is not empty
  while( !empty( graph2 ))
    {             
      // if the node is equal to the node to find
      if( a == b )
  {
    // push the node onto the path
    path.push_front( a );

    // initialize cnt
    int cnt = 0;

    // while there is still a previous node in the path and the node is not equal to the start node
    while( previous[a]!=-1 && a != d )
      {
        // increment count
        cnt++;
        // get the previous node 
        a = previous[a];
        // push the node on the path
        path.push_front( a );
      }
    
    // return the path size
    return path.size();
  }
      
      list<ovEdge> ch = graph1[a]->getChildren();

      list<ovEdge>::iterator it;
      // for the number of names
      for( it=ch.begin(); it!=ch.end(); it++ )
  {    
    int i = ( *it ).GetNode1();
    if( i == a )
      {
        i = ( *it ).GetNode2();
      }

    // if there is an edge between the nodes
    if( (*it ).HasTag( NUM_OF_TAGS ))
      {     
        // initialize variables
        int c = i;
        float alt = distance[a] + 1;

        // set the distance to the smallest distance
        if( alt < distance[c] )
    {
      distance[c] = alt;
      previous[c] = a;
    }
      }
  }

      // set the graph to -1 to show the node has already been searched 
      graph2[a] = -1;
  
      // initialize variable
      float smallest = infinity;

      // for the number of names
      for( int its=0; its<NUM_OF_NAMES; its++ )
  {
    // if the node has not been search and the distance is less than the smallest distance
    if( graph2[its] != -1 && distance[graph2[its]] < smallest )
      {
        // set this distance to be the smallest
        smallest = distance[graph2[its]];
        // set a to the next node with the smallest distance
        a = graph2[its];
      }
  }
  }
  // return 0 since path is not found
  return 0;
}
       
// checks if the list passed in is empty ( all -1 )
bool ovGraph::empty( int graph2[] )
{
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // if there is a node still left to search return false
      if( graph2[i] != -1 )
  {
    return false;
  }
    }  
  // return true
  return true;
}

// draw the path using the list of nodes
void ovGraph::printPath( list<int> path )
{
  // initialize variables
  ovNode* ndP;
  int prev = -1;
  int cnt = 0;
  
  // for each node in the path
  for( list<int>::iterator it = path.begin(); it != path.end(); it++ )
    {
      // get the node
      ndP = graph1[*it];
      
      // get the coordinates
      x = ndP->getX();
      y = ndP->getY();
      z = ndP->getZ();
      
      // increment the count
      cnt++;
      
      // set toggle connected to true
      toggleConnected[*it] = true;

      // create an actor
      vtkActor *actor = vtkActor::New(); 
      actor->SetMapper( mapper1 );
      actor->SetPosition( x, y, z );

      // if it is either the first or last node in the path
      if( cnt==1 || cnt == path.size() )
  {    
    // set the node to white
    actor->GetProperty()->SetColor( 1, 1, 1 );
  }
      else
  {
    // else set the node to blue
    actor->GetProperty()->SetColor( 0, 0, 1 );
  }

      // add actor to renderer
      rend->AddActor( actor );
      
      // delete vtk objects
      actor->Delete();

      // initialize variables
      bool connected = false;      
      list<int>::iterator i;
      
      
      // for all the tags
      for( i=tagsUsed.begin(); i!=tagsUsed.end(); i++ )
  {
    // if the tag is on and is connected to the previous node
    if( prev != -1 && hasEdgeBetween( prev, *it, *i ))
      {
        // set connected to true
        connected = true;
      }
  }
      
      
      // if connected
      if( connected )
  {
    // initialize variables
    list<int>::iterator k;
    list<int> tagsAround;

    // change the tags to a different order which reflects the graph's edge order
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        tagsAround.push_front( *k );
      }

   
    // for the number of tags
    for( k=tagsAround.begin(); k!=tagsAround.end(); k++ )
      {
        // if the tag is on and it is connected
        if( (tagOn[*k] && hasEdgeBetween( prev, *it, *k )) || ( *k == NUM_OF_TAGS && hasEdgeBetween( prev, *it, NUM_OF_TAGS )) )
    {    
      // initialize variable
      int a, b;      
      a = prev;
      b = *it;            
      
      drawEdge( a, b, false, *k );
    }
      }
  }
      // set previous to the current node
      prev = *it;
      }
}

// finds if there is an edge between nd1 and nd2 with the tag
bool ovGraph::hasEdgeBetween( int nd1, int nd2, int tag )
{
  ovNode* nd = graph1[nd1];

  bool found = false;

  list<ovEdge> ch = nd->getChildren();

  list<ovEdge>::iterator it;
  for( it=ch.begin(); it!=ch.end(); it++ )
    {
      int q = it->GetNode1();

      if( q == nd1 )
  {
    q = it->GetNode2();
  }

      if( q == nd2 && it->HasTag( tag ))
  {
    found = true;
  }
    }

  return found;
}

// highlight the node ( turn it white ) at the position passed in
void ovGraph::highlightNode( int a, int b )
{  
   
  int foundName = nodeAtPos( a, b, false );

  // if the highlight actor is not NULL
  if( highlightActor != NULL && lastHighlighted != foundName )
    {  
      // remove the actor from the renderer and delete it
      rend->RemoveActor( highlightActor );
      highlightActor->Delete();
      highlightActor = NULL;
    }     
  
  
  if( foundName != selected && foundName >= 0 && ndCon[foundName] && ( !toggleTrue || ( toggleTrue && ndCon[foundName] )) )
    {
      highlightActor = drawNode( foundName, false, highlightActor, 100.0/255.0, 200.0/255.0, 1.0 );
    }
  
  if( lastHighlighted != -1 && lastHighlighted != selected && lastHighlighted != foundName )
    {
      if( ndCon[lastHighlighted] )
  {
    drawNode( lastHighlighted, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 ); 
  }
      else if( mode == 'h' ) 
  {
    drawNode( lastHighlighted, true, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
  }
    }
  
  if( foundName >= 0 && ( !toggleTrue || ( toggleTrue && ndCon[foundName] )) )
    {
      highlighted = true;
      lastHighlighted = foundName;
    }
  
  renderWin();
}

// turn all name tags off
// if nw is true do not allow name tags, else just clear the name tags
void ovGraph::allNamesOff( bool nw )
{
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // if the text actor is not NULL delete the object
      if( nameText[i] != NULL )
  {      
    // remove the text actor
    rend->RemoveActor( nameText[i] );
  }

      // turn off the name tag
      nameOn[i] = false;
    }

  // if nw, do not allow name tags
  if( nw )
    namesAllowedOn = false;
}

// allow name tags on and reset the tags
void ovGraph::allNamesOn( bool all )
{

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {

      // set the name on to false
      nameOn[i] = false;  // if names are allowed on
      
      // if a node was at the mouse position
      if( all )
  {
    if( nameText[i] != NULL )
      {
        // remove actor from renderer
        rend->RemoveActor( nameText[i] );
      }
    
    // if the name is not on and the mode is either not toggle or the node is toggle connected
    if( !nameOn[i] && ( (!toggleTrue ) || toggleConnected[i] ))
      {     

        // set the position and input for the name
        nameText[i]->SetAttachmentPoint( graph1[i]->getX(), graph1[i]->getY(), graph1[i]->getZ() );
        

        // add the actor to the renderer
        rend->AddActor( nameText[i] ); 

        
        // turn on the name
        nameOn[i] = true;
      }
    else
      {        
        // turn off the name
        nameOn[i] = false;
      }     
  }
      else
  {

    // set the position and input for the name
    nameText[selected]->SetAttachmentPoint( graph1[selected]->getX(), graph1[selected]->getY(), graph1[selected]->getZ() );
        
    // add the actor to the renderer
    rend->AddActor( nameText[selected] ); 
    
    // turn on the name
    nameOn[selected] = true;
  }
    }

  // allow names to be on
  namesAllowedOn = true;

}


void ovGraph::selectedNodesOn()
{

    for( int i=0; i<NUM_OF_NAMES; i++ )
    {
  nameOn[i] = false;
    }

    for ( list<int>::iterator it=selectedNodes.begin(); it!=selectedNodes.end(); ++it )
    {

    if( (!toggleTrue ) | ( toggleConnected[( *it )] ))
    {
              // set the position and input for the name
             nameText[( *it )]->SetAttachmentPoint( graph1[( *it )]->getX(), graph1[( *it )]->getY(), graph1[( *it )]->getZ() );
          

               // add the actor to the renderer
            rend->AddActor( nameText[( *it )] ); 

        nameOn[( *it )] = true;
    }

    }

    namesAllowedOn = true;

}





// initialize the names of the nodes for labels
void ovGraph::initNames()
{
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {      
      // create a new text actor
      nameText[i] = vtkCaptionActor2D::New();
  
      // set the position and input for the name
      nameText[i]->SetAttachmentPoint( graph1[i]->getX(), graph1[i]->getY(), graph1[i]->getZ() );
      nameText[i]->SetPadding( 0 );

      nameText[i]->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );

      if( captionItalic )
  {
    nameText[i]->GetCaptionTextProperty()->ItalicOn();
  }
      else
  {
    nameText[i]->GetCaptionTextProperty()->ItalicOff();
  }

      if( captionBold )
  {
    nameText[i]->GetCaptionTextProperty()->BoldOn();
  }
      else
  {
    nameText[i]->GetCaptionTextProperty()->BoldOff();
  }

      switch( captionFont )
  {
  case ARIAL:
    nameText[i]->GetCaptionTextProperty()->SetFontFamilyToArial();
    break;
  case COURIER:
    nameText[i]->GetCaptionTextProperty()->SetFontFamilyToCourier();
    break;
  case TIMES:
    nameText[i]->GetCaptionTextProperty()->SetFontFamilyToTimes();
    break;
  }

      nameText[i]->GetCaptionTextProperty()->SetFontSize( captionSize );

      nameText[i]->GetCaptionTextProperty()->SetColor( captionRed/255.0, captionGreen/255.0, captionBlue/255.0 );

      nameText[i]->GetCaptionTextProperty()->ShadowOff();
      nameText[i]->SetCaption( names[i] );
      nameText[i]->BorderOff();
    }
}

// turn the name at the position on or off
void ovGraph::nameOnOff( bool on, int a, int b )
{
  // if names are allowed on
  if( namesAllowedOn )
    {
      // initialize variables
      ovNode* nd2;
      double coords[3];
      
      // find the 3D coordinates 
      vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
      picker->Pick( a, b, 0.0, rend );
      picker->GetPickPosition( coords );
      picker->Delete();

      // initialize variable
      int foundName = -1;
  
      // for the number of names
      for( int i=0; i<NUM_OF_NAMES;i++ )
  {
    // get the node
    nd2 = graph1[i];

    // check if all the coordinates match the coordinates of the mouse position passed in
    if( nd2->getX()-3 <= coords[0] && nd2->getX()+3 >= coords[0] )
      {
        if( nd2->getY()-3 <= coords[1] && nd2->getY()+3 >=coords[1] )
    {
      if( nd2->getZ()-3 <= coords[2] && nd2->getZ()+3 >= coords[2] )
        {
          // hold the position of the node picked
          foundName = i;
        }
    }
      }
  }
      
      // if a node was at the mouse position
      if( foundName >= 0 )
  {
    if( nameText[foundName] != NULL )
      {
        // remove actor from renderer
        rend->RemoveActor( nameText[foundName] );
      }
    
    // if the name is not on and the mode is either not toggle or the node is toggle connected
    if( on && ( (!toggleTrue ) || toggleConnected[foundName] ))
      {    

        // set the position and input for the name
        // nameText[foundName]->SetAttachmentPoint( graph1[foundName]->getX(), graph1[foundName]->getY(), graph1[foundName]->getZ() );
        nameText[foundName]->SetAttachmentPoint( coords[0], coords[1], coords[2] );

        nameText[foundName]->SetPadding( 0 );

        nameText[foundName]->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );
        
        if( captionItalic )
    {
      nameText[foundName]->GetCaptionTextProperty()->ItalicOn();
    }
        else
    {
      nameText[foundName]->GetCaptionTextProperty()->ItalicOff();
    }
        
        if( captionBold )
    {
      nameText[foundName]->GetCaptionTextProperty()->BoldOn();
    }
        else
    {
      nameText[foundName]->GetCaptionTextProperty()->BoldOff();
    }
        
        switch( captionFont )
    {
    case ARIAL:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToArial();
      break;
    case COURIER:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToCourier();
      break;
    case TIMES:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToTimes();
      break;
    }

        nameText[foundName]->GetCaptionTextProperty()->SetFontSize( captionSize );
        
        nameText[foundName]->GetCaptionTextProperty()->SetColor( captionRed/255.0, captionGreen/255.0, captionBlue/255.0 );
        
        nameText[foundName]->GetCaptionTextProperty()->ShadowOff();
        nameText[foundName]->BorderOff();

        // add the actor to the renderer
        rend->AddActor( nameText[foundName] ); 
        
        // turn on the name
        nameOn[foundName] = true;
      }
    else if( !on )
      {        
        // turn off the name
        nameOn[foundName] = false;
      }     
  } 
    }
}

// turn the name on or off depending on the on parameter
// nm is the string of the label
void ovGraph::nameOnOff( bool on, char* nm )
{  
  // if names are allowed on
  if( namesAllowedOn )
    {
      int foundName = -1;
      
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    if( strstr( nm, names[i] ) != NULL )
      {
        foundName = i;
      }
  }
      
      // if a node was at the mouse position
      if( foundName >= 0 )
  {
    if( nameText[foundName] != NULL )
      {
        // remove actor from renderer
        rend->RemoveActor( nameText[foundName] );
      }
    
    // if the name is not on and the mode is either not toggle or the node is toggle connected
    if( on && ( (!toggleTrue ) || toggleConnected[foundName] ))
      {        
        // set the position and input for the name
        nameText[foundName]->SetAttachmentPoint( graph1[foundName]->getX(), graph1[foundName]->getY(), graph1[foundName]->getZ() );
      
        nameText[foundName]->SetPadding( 0 );

        nameText[foundName]->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );
        
        if( captionItalic )
    {
      nameText[foundName]->GetCaptionTextProperty()->ItalicOn();
    }
        else
    {
      nameText[foundName]->GetCaptionTextProperty()->ItalicOff();
    }
        
        if( captionBold )
    {
      nameText[foundName]->GetCaptionTextProperty()->BoldOn();
    }
        else
    {
      nameText[foundName]->GetCaptionTextProperty()->BoldOff();
    }
        
        switch( captionFont )
    {
    case ARIAL:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToArial();
      break;
    case COURIER:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToCourier();
      break;
    case TIMES:
      nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToTimes();
      break;
    }

        nameText[foundName]->GetCaptionTextProperty()->SetFontSize( captionSize );
        
        nameText[foundName]->GetCaptionTextProperty()->SetColor( captionRed/255.0, captionGreen/255.0, captionBlue/255.0 );
        
        nameText[foundName]->GetCaptionTextProperty()->ShadowOff();
        nameText[foundName]->BorderOff();

        // add the actor to the renderer
        rend->AddActor( nameText[foundName] ); 
        
        // turn on the name
        nameOn[foundName] = true;
      }
    else if( !on )
      {        
        // turn off the name
        nameOn[foundName] = false;
      }     
  } 
    }
}
  
// destroy the pop-up name tag
void ovGraph::destroyName()
{
  if( txtAct != NULL )
    {
      // remove the text actor and delete it
      rend->RemoveActor( txtAct );
      txtAct->Delete();
      txtAct = NULL;
    }
}

// show the pop-up name tag at the position passed in
void ovGraph::showName( int a, int b )
{  
  int foundName = nodeAtPos( a, b, false );

  double coords[3];
      
  // find the 3D coordinates 
  vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
  picker->Pick( a, b, 0.0, rend );
  picker->GetPickPosition( coords );
  picker->Delete();

  destroyName();

  // if the name found is not NULL and the name is not already on
  if( foundName >=0 && ndCon[foundName]  && ( (!toggleTrue && !pathTrue ) || ( (toggleTrue || pathTrue ) && toggleConnected[foundName] )) )// !nameOn[foundName] &&( (!toggleTrue && !pathTrue ) || ( (toggleTrue || pathTrue ) && toggleConnected[foundName] )) ) 
    {      
      // create a new text actor and set the position to the mouse position
      txtAct = vtkCaptionActor2D::New();

      txtAct->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );

      txtAct->SetCaption( names[foundName] );// add the text actor to the renderer and render the window
      // txtAct->SetAttachmentPoint( graph1[foundName]->getX(), graph1[foundName]->getY(), graph1[foundName]->getZ() );
      txtAct->SetAttachmentPoint( coords[0], coords[1], coords[2] );

      txtAct->SetPadding( 0 );
      txtAct->GetCaptionTextProperty()->SetColor( captionRed/255.0, captionGreen/255.0, captionBlue/255.0 );

      if( captionItalic )
  {
    txtAct->GetCaptionTextProperty()->ItalicOn();
  }
      else
  {
    txtAct->GetCaptionTextProperty()->ItalicOff();
  }

      if( captionBold )
  {
    txtAct->GetCaptionTextProperty()->BoldOn();
  }
      else
  {
    txtAct->GetCaptionTextProperty()->BoldOff();
  }

      switch( captionFont )
  {
  case ARIAL:
    txtAct->GetCaptionTextProperty()->SetFontFamilyToArial();
    break;
  case COURIER:
    txtAct->GetCaptionTextProperty()->SetFontFamilyToCourier();
    break;
  case TIMES:
    txtAct->GetCaptionTextProperty()->SetFontFamilyToTimes();
    break;
  }

      txtAct->GetCaptionTextProperty()->SetFontSize( captionSize );

      txtAct->GetCaptionTextProperty()->ShadowOff();
      txtAct->SetCaption( names[foundName] );
      txtAct->BorderOff();
      
      rend->AddActor( txtAct );
      renderWin();
    }
}

// redraw the name tags
void ovGraph::redrawNameTags()
{
  for( int foundName=0; foundName<NUM_OF_NAMES; foundName++ )
    {
      // set the position and input for the name
      nameText[foundName]->SetAttachmentPoint( graph1[foundName]->getX(), graph1[foundName]->getY(), graph1[foundName]->getZ() );
      
      nameText[foundName]->SetPadding( 0 );
      
      nameText[foundName]->GetTextActor()->SetTextScaleMode( vtkTextActor::TEXT_SCALE_MODE_NONE );
      
      if( captionItalic )
  {
    nameText[foundName]->GetCaptionTextProperty()->ItalicOn();
  }
      else
  {
    nameText[foundName]->GetCaptionTextProperty()->ItalicOff();
  }
      
      if( captionBold )
  {
    nameText[foundName]->GetCaptionTextProperty()->BoldOn();
  }
      else
  {
    nameText[foundName]->GetCaptionTextProperty()->BoldOff();
  }
      
      switch( captionFont )
  {
  case ARIAL:
    nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToArial();
    break;
  case COURIER:
    nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToCourier();
    break;
  case TIMES:
    nameText[foundName]->GetCaptionTextProperty()->SetFontFamilyToTimes();
    break;
  }
      
      nameText[foundName]->GetCaptionTextProperty()->SetFontSize( captionSize );
      
      nameText[foundName]->GetCaptionTextProperty()->SetColor( captionRed/255.0, captionGreen/255.0, captionBlue/255.0 );
      
      nameText[foundName]->GetCaptionTextProperty()->ShadowOff();
      nameText[foundName]->BorderOff();
      
      if( nameOn[foundName] )
  {
    // add the actor to the renderer
    rend->AddActor( nameText[foundName] ); 
  }
    }
}

// set the caption to be italic
void ovGraph::setCaptionItalic( bool b )
{
  captionItalic = b;
}

// set the caption to be bold
void ovGraph::setCaptionBold( bool b )
{
  captionBold = b;
}

// set the caption to the size
void ovGraph::setCaptionSize( int sz )
{
  captionSize = sz;
}

// set the caption font
void ovGraph::setCaptionFont( int fnt )
{
  captionFont = fnt;
}

// set the caption to the colour
void ovGraph::setCaptionColour( int r, int g, int b )
{
  captionRed = r;
  captionGreen = g;
  captionBlue = b;
}

// get whether the caption is italic
bool ovGraph::getCaptionItalic()
{
  return captionItalic;
}

// get whether the caption is bold
bool ovGraph::getCaptionBold()
{
  return captionBold;
}

// get the caption font
int ovGraph::getCaptionFont()
{
  return captionFont;
}

// get the caption size
int ovGraph::getCaptionSize()
{
  return captionSize;
}

// get the caption red property
int ovGraph::getCaptionRed()
{
  return captionRed;
}

// get the caption green property
int ovGraph::getCaptionGreen()
{
  return captionGreen;
}

// get the caption blue property
int ovGraph::getCaptionBlue()
{
  return captionBlue;
}

// returns the index of the node at the position passed in
int ovGraph::nodeAtPos( int a, int b, bool setSel )
{

  // initialize variables
  ovNode* nd2;
  double coords[3];

  // find the coordinates of the position passed in
  vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
  picker->Pick( a, b, 0.0, rend );
  picker->GetPickPosition( coords );
  picker->Delete();
  
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // get the node
      nd2 = graph1[i];

      // check if all the coordinates match the coordinates of the mouse position passed in
      if( nd2->getX()-3 <= coords[0] && nd2->getX()+3 >= coords[0] )
  {
    if( nd2->getY()-3 <= coords[1] && nd2->getY()+3 >=coords[1] )
      {
        if( nd2->getZ()-3 <= coords[2] && nd2->getZ()+3 >= coords[2] )
    {
      if( setSel )
        {
          // set the selected node to the node at the position and return it
          oldSelected = selected;
          selected = i;
      
        }
      return i;
    }
      }
  }
    }

  if( setSel )
    {
      // set selected to -1 ( null ) and return it
      selected = -1;
    }
  return -1;
}


int ovGraph::nodeAtPosNoSelect( int a, int b )
{

  // initialize variables
  ovNode* nd2;
  double coords[3];

  // find the coordinates of the position passed in
  vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
  picker->Pick( a, b, 0.0, rend );
  picker->GetPickPosition( coords );
  picker->Delete();
  
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // get the node
      nd2 = graph1[i];

      // check if all the coordinates match the coordinates of the mouse position passed in
      if( nd2->getX()-3 <= coords[0] && nd2->getX()+3 >= coords[0] )
  {
    if( nd2->getY()-3 <= coords[1] && nd2->getY()+3 >=coords[1] )
      {
        if( nd2->getZ()-3 <= coords[2] && nd2->getZ()+3 >= coords[2] )
    {
          // set the selected node to the node at the position and return it
          // oldSelected = selected;
          shiftSelected = i;
    
        
      return i;
    }
      }
  }
    }

      // set selected to -1 ( null ) and return it
      shiftSelected = -1;
    
  return -1;

}






// display the node information 
void ovGraph::displayNdInfo( int a, int b )
{
  // get the window size
  int* x = window->GetSize();

  // initialize variable
  int i = -1;

  // if a and b are 0 ( null )
  if( a==0 && b ==0 )
    {
      // set the node to the selected node
      i = selected;
    }
  else
    {
      // else get the node at the position passed in
      i = nodeAtPos( a, b, true );
    }

  // if selected is not null
  if( i>=0 )
    {        
      // get the text for the selected node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
 
      // set up the renderer
      textSetUp();

      // set the sorting to true for the list so it will be in alphabetical order
      listWidge->setSortingEnabled( true );

      // set the labels to display
      label1->setText( "Selected Node:" );
      label2->setText( "Connected With:" );
      labelSelected->setText( nam );
      free( nam );
      
      // get the children and parents of the node
      list<ovEdge> ch = graph1[i]->getChildren();

      // for the number of children and parents
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;
    
    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected
        if( (*j ).HasTag( *k ) && tagOn[*k] )
    {
      // set connected to true
      connected = true;
    }
      }
    
    // if the node is connected
    if( connected )
      {    
        int q = ( *j ).GetNode1();
        if( q==i )
    {
      q = ( *j ).GetNode2();
    }

        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[q] );
        strcat( nam1, "  : " );
        
        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( (*j ).HasTag( *s ) && tagOn[*s] && ( *s )!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
        
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }
    }
}

// flags which node it is to move
void ovGraph::moveNode( int a, int b )
{
  ndToMove = nodeAtPos( a, b, true );
}

// changes the position of the node to a new position
void ovGraph::changeToPos( int a, int b )
{
  // if the node to move is not null
  if( ndToMove >=0 )
    {
      // initialize variable
      double coords[3];
      
      // find the coordinates of the mouse position passed in
      vtkWorldPointPicker *picker = vtkWorldPointPicker::New();
      picker->Pick( a, b, 0.0, rend );
      picker->GetPickPosition( coords );
      picker->Delete();
      
      // move the node to the new position
      graph1[ndToMove]->setCenter( (int )coords[0], ( int )coords[1], ( int )coords[2] );
      nameText[ndToMove]->SetAttachmentPoint( (int )coords[0], ( int )coords[1], ( int )coords[2] );
    }
}

// turn highlight on
void ovGraph::highlightOn()
{
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // set toggleConnected to false
      toggleConnected[i] = false;
      ndCon[i] = false;
    }
 
  // clear the toggleOn
  toggledOn.clear();

  // turn off names
  allNamesOff( false );

  rendSetUp();

  // for the number of names
  for( int i=0;i<NUM_OF_NAMES;i++ )
    {
      // for the number of edges
      for( int j=0;j<NUM_OF_NAMES;j++ )
  {
    // initialize con to false
    con[i][j] = false;
  }
      // set togged on to false
      toggedOn[i] = false;
    }

  setToggle( false );

  toggleTrue = true;

  // set mode to highlight
  mode = 'h';

  highlight( 0, 0 );

  drawFadedEdges();
}

// highlight the node at the position passed in
void ovGraph::highlight( int a, int b )
{
  // initialize variable
  int i = 0;

  // if a and b are null 
  if( a == 0 &&  b == 0 )
    { 
      // set i to the previously selected node
      i = selected;
    }
  else
    {
      // set i to the node at the position passed in
      i = nodeAtPos( a, b, true );    
    }

  // if selected is not null
  if( i >=0 )
    {            
      // set toggle connnected to true 
      toggleConnected[i] = true;     
      
      // put the node in the toggled on list
      toggledOn.push_back( i );

      // get the window size
      int* x = window->GetSize();

      // get the name of the node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
      
      // set up the renderer
      textSetUp();

      // show the labels
      label1->setText( "Selected Node:" );
      labelSelected->setText( nam );
      label2->setText( "Connected  With:" );
      free( nam );
      
      // get the children of the node
      list<ovEdge> ch = graph1[i]->getChildren();

      // for the number of children
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;

    int q = j->GetNode1();
    if( q == i )
      {
        q= j->GetNode2();
      }

    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected
        if( j->HasTag( *k ) && tagOn[*k] ) // && !connected )
    {
      // set connected to true
      connected = true; 
      toggleConnected[q] = true;
      drawNode( i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawNode( q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawEdge( i, q, false, *k );
    }
      }

    // if the node is connected
    if( connected )
      {    
        int q = j->GetNode1();

        if( q == i )
    {
      q = j->GetNode2();
    }

        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[q] );
        strcat( nam1, "  : " );

        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( j->HasTag( *s ) && tagOn[*s] && *s!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
    
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }

      // printf( "done drawing highlighted first" );
      // fflush( stdout );
    }  
}

// draw the highlighted graph
void ovGraph::drawHighlighted()
{
  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // set toggleConnected to false
      toggleConnected[i] = false;
      ndCon[i] = false;      
      toggedOn[i] = false;

      // for the number of edges
      for( int j=0;j<NUM_OF_NAMES;j++ )
  {
    // initialize con to false
    con[i][j] = false;
  }
    }
 
  // turn off names
  // allNamesOff( false );

  rendSetUp();
  
  list<int>::iterator it;
  for( it=toggledOn.begin(); it!=toggledOn.end(); it++ )
    {
       // get the window size
      int* x = window->GetSize();
      int i = *it;

      toggleConnected[i] = true;

      // get the name of the node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
      
      // set up the renderer
      textSetUp();

      // show the labels
      label1->setText( "Selected Node:" );
      labelSelected->setText( nam );
      label2->setText( "Connected  With:" );
      free( nam );
      
      // get the children of the node
      list<ovEdge> ch = graph1[i]->getChildren();

      // for the number of children
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;

    int q = j->GetNode1();
    if( q == i )
      {
        q= j->GetNode2();
      }

    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected
        if( j->HasTag( *k ) && tagOn[*k] )// && !connected )
    {
      // set connected to true
      connected = true; 
      toggleConnected[q] = true;
      drawNode( i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawNode( q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawEdge( i, q, false, *k );
    }
      }

    // if the node is connected
    if( connected )
      {    
        int q = j->GetNode1();

        if( q == i )
    {
      q = j->GetNode2();
    }

        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[q] );
        strcat( nam1, "  : " );

        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( j->HasTag( *s ) && tagOn[*s] && *s!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
    
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }
    }

  drawFadedEdges();
}

// draw the faded edges in the graph
void ovGraph::drawFadedEdges()
{
  // for all the edges
  for( int i =0; i<NUM_OF_NAMES; i++ )
    {
      drawEdgesForNode( i, true );
    }
}

// turn toggle on
void ovGraph::toggleOn()
{
  // set mode to toggle
  mode = 't';

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // set toggle connected to false
      toggleConnected[i] = false;
      ndCon[i] = false;
    }

  // clear toggled on
  toggledOn.clear();

  // turn off names
  allNamesOff( false );

  rendSetUp();
}

// toggle the node at position
void ovGraph::toggle( int a, int b )
{
  // initialize variable
  int i = -1;

  // if position is null
  if( a == 0 &&  b == 0 )
    {   
      // set i to previously selected node
      i = selected;
      if( i!=-1 )
  {
    ndCon[i] = true;
    toggleConnected[i] = true;
    // printf( "toggle set up" );
    // fflush( stdout );
  }
    }
  else
    {
      // get the node at the position passed in
      i = nodeAtPos( a, b, true );
      if( !ndCon[i] )
  {
    i = -1;
  }
    }

  // if selected node is not null
  if( i >=0 )// && ndCon[i] && toggleConnected[i] )
    {
      // set toggle connected to true
      toggleConnected[i] = true;
      
      toggledOn.push_back( i );

      // get window size
      int* x = window->GetSize();

      // get the name of the node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
      
      // set up the renderer
      textSetUp();

      // set the labels 
      label1->setText( "Selected Node:" );
      labelSelected->setText( nam );
      label2->setText( "Connected  With:" );
      free( nam );
    
      // get the children and parents of the node
      list<ovEdge> ch = graph1[i]->getChildren();

      // for the number of children and parents
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;
    
    int q = ( *j ).GetNode1();
    if( q == i )
      {
        q = ( *j ).GetNode2();
      }

    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected and not already drawn
        if( tagOn[*k] && ( *j ).HasTag( *k ))// && !connected )
    {
      // set connected to true
      connected = true;
      toggleConnected[q] = true;
      drawNode( i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawNode( q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawEdge( i, q, false, *k );
    }
      }   

    // if the node is connected
    if( connected )
      {
        // get the ints
        a = i;
        b = q;
    
        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[b] );
        strcat( nam1, "  : " );

        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( hasEdgeBetween( a, b, *s ) && tagOn[*s] && *s!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
    
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }    
    }
}

// draw the toggled graph
void ovGraph::drawToggled()
{

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // set toggleConnected to false
      toggleConnected[i] = false;
      ndCon[i] = false;      
      toggedOn[i] = false;

      // for the number of edges
      for( int j=0;j<NUM_OF_NAMES;j++ )
  {
    // initialize con to false
    con[i][j] = false;
  }
    }

  rendSetUp();

  list<int>::iterator it;
  for( it=toggledOn.begin(); it!= toggledOn.end(); it++ )
    {
      int i = *it;

      toggleConnected[i] = true;
      
      // get window size
      int* x = window->GetSize();
  
      // get the name of the node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
      
      // set up the renderer
      textSetUp();
      
      // set the labels 
      label1->setText( "Selected Node:" );
      labelSelected->setText( nam );
      label2->setText( "Connected  With:" );
      free( nam );
      
      // get the children and parents of the node
      list<ovEdge> ch = graph1[i]->getChildren();
      
      // for the number of children and parents
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;
    
    int q = ( *j ).GetNode1();
    if( q == i )
      {
        q = ( *j ).GetNode2();
      }

    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected and not already drawn
        if( tagOn[*k] && ( *j ).HasTag( *k ))// && !connected )
    {
      // set connected to true
      connected = true;
      toggleConnected[q] = true;
      drawNode( i, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawNode( q, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
      drawEdge( i, q, false, *k );
    }
      }   

    // if the node is connected
    if( connected )
      {
        // get the ints
        int a = i;
        int b = q;
    
        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[b] );
        strcat( nam1, "  : " );

        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( hasEdgeBetween( a, b, *s ) && tagOn[*s] && *s!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
    
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }    
    }
}

// redraw the graph based on current node positions
void ovGraph::redrawGraph()
{

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // set toggleConnected to false
      toggleConnected[i] = false;
      ndCon[i] = false;      
      toggedOn[i] = false;

      // for the number of edges
      for( int j=0;j<NUM_OF_NAMES;j++ )
  {
    // initialize con to false
    con[i][j] = false;
  }
    }

  rendSetUp();

  setToggle( false );


  if( !load || ( mode != 'p' && load ))
    {
      // set mode to graph
      mode = 'g';
    }

  // set pathTrue to false
  pathTrue = false;

  int i = selected;

  if( i >= 0 )
    {
      // get the name of the node
      char* nam; 
      nam = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
      sprintf( nam, "%s", names[i] );
      
      // set up the renderer
      textSetUp();
      
      // set the labels 
      label1->setText( "Selected Node:" );
      labelSelected->setText( nam );
      label2->setText( "Connected  With:" );
      free( nam );
      
      // get the children and parents of the node
      list<ovEdge> ch = graph1[i]->getChildren();
      
      // for the number of children and parents
      list<ovEdge>::iterator j;
      for( j=ch.begin(); j!=ch.end(); j++ )
  {
    // initialize variables
    bool connected = false;
    
    int q = ( *j ).GetNode1();
    if( q == i )
      {
        q = ( *j ).GetNode2();
      }
    
    // for the number of tags
    list<int>::iterator k;
    for( k=tagsUsed.begin(); k!=tagsUsed.end(); k++ )
      {
        // if the tag is on and the edge is connected and not already drawn
        if( tagOn[*k] && j->HasTag( *k ))// && !connected )
    {
      // set connected to true
      connected = true;
    }
      }   
    
    // if the node is connected
    if( connected )
      {
        // get the ints
        int a = i;
        int b = q;
        
        // put the connected name in a string
        char* nam1 = ( char* ) calloc( 1000*sizeof( char ), 1000*sizeof( char ));
        sprintf( nam1, "%s", names[b] );
        strcat( nam1, "  : " );
        
        // for the number of tags
        list<int>::iterator s;
        for( s=tagsUsed.begin(); s!=tagsUsed.end(); s++ )
    {
      // if the tag is connected and is on
      if( hasEdgeBetween( a, b, *s ) && tagOn[*s] && *s!=NUM_OF_TAGS )
        {
          // attach the tag name to the string
          strcat( nam1, " " );
          strcat( nam1, tags[*s] );
        }
    }
        
        // draw the text
        drawText( 0, nam1, 0, false );
        free( nam1 );
      }
  }
    }

  drawEdges();
}

// return if the interactor is user style 
bool ovGraph::isInteractGraph()
{
  return interactGraph;
}

// change the interactor to user style
void ovGraph::changeInteractorToGraph()
{
  // create new user style
  ovUserStyle* style = new ovUserStyle( inter, window, this );

  // set the interactor style to the new user style
  ovQMainWindow* orl = ( ovQMainWindow* ) orland;
  orl->setUserStyle( style );
  inter->SetInteractorStyle( style );
  rend->SetActiveCamera( cam );

  // set interactGraph to true
  interactGraph = true;

  mode = prevMode;
}

// change the interactor to camera
void ovGraph::changeInteractorToCamera()
{
  prevMode = mode;
  // set the mode to camera
  mode = 'c';

  // set the interactor style to joystick camera
  inter->SetInteractorStyle( vtkInteractorStyleTrackballCamera::New() );

  // set interactGraph to false
  interactGraph = false;
}

// set up the window
void ovGraph::windowSetup()
{
  // add the renderer to the window
  window->AddRenderer( rend );

  // set the render window for the interactor
  inter->SetRenderWindow( window );
  
  // set up the camera
  cam->SetViewUp( 0, 0, -1 );
  cam->SetPosition( 0, 1, 0 );
  cam->SetFocalPoint( 0, 0, 0 );
  cam->ComputeViewPlaneNormal();
  cam->SetParallelProjection( true );
  rend->SetActiveCamera( cam );
  cam->Dolly( 1.5 );
  
  // set the background for the renderers
  rend->SetBackground( 0.2, 0.2, 0.2 );
  rendText->SetBackground( 0.2, 0.2, 0.2 );

  // create userstyle tags
  ovUserStyleTags* style2 = new ovUserStyleTags( inter2, window2, this );

  // set the interactor style for the interactor
  inter->SetInteractorStyle( vtkInteractorStyleJoystickCamera::New() );
}

// seperate the nodes into octants based on their connectivity
void ovGraph::drawGraph()
{
  windowSetup();
  
   // initialize variables
  list<int> connect;
  bool connected[NUM_OF_NAMES];

  // for the number of names
  for( int i = 0; i<NUM_OF_NAMES; i++ )
    {
      // initialize connected to false
      connected[i] = false;
    }

  // initialize coordinate variables
  int x=0;
  int y=0;
  int z=0;

  int counter = 8;
  
  // draw entries... all 150 apart

  // initialize variables
  list<int>::iterator it;
  int cntx = 0;
  int cntz = 0;

  // calculate the width/length of all the entry nodes
  int half = sqrt( entries.size() )*150;

  // for the number of entries
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
      // get coordinates
      x = cntx;
      z = cntz;
      
      // set connected to true
      connected[*it] = true;

      // set the center of the node
      graph1[*it]->setCenter( x, y, z );
      
      // create actor for sphere
      vtkActor *actor = vtkActor::New();
      actor->SetMapper( mapper1 );
      actor->GetProperty()->SetColor( 0, 95.0/255.0, 1 );
      actor->SetPosition( x, y, z );

      // add actor to renderer
      rend->AddActor( actor );

      graph1[*it]->setSphereActor( actor );

      // increment column
      cntx+= 150;

      // if reached the end of the columns
      if( cntx > half )
  {
    // reset the column
    cntx=0;
    // increment row 
    cntz+=150;
  }
    }

  // for the number of entries
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
      // get the coordinates
      int numX = graph1[*it]->getX();
      int numZ = graph1[*it]->getZ();
      
      // initialize variables
      int xVal = 0;
      int zVal = 0;
      int counter = 0;
      
      list<ovEdge> ch = graph1[*it]->getChildren();

      list<ovEdge>::iterator iter;
      for( iter=ch.begin(); iter!=ch.end(); iter++ )
  {
      // for the number of names
      // for( int i=0; i<NUM_OF_NAMES;i++ )
    // {

    int q = ( *iter ).GetNode1();

    if( q == *it )
      {
        q = ( *iter ).GetNode2();
      }
    
    // if the edge is connected and has not already been drawn
    if( !connected[q] )
      {
        // arrange in a layout around the entry
        if( counter == 0 )
    {
      x = numX + 8;
      z = numZ + 0;
    }
        else if( counter == 1 )
    {
      x = numX + 0;
      z = numZ + 8;
    }
        else if( counter == 2 )
    {
      x = numX - 8;
      z = numZ + 0;
    }
        else if( counter == 3 )
    {
      x = numX + 0;
      z = numZ - 8;
    }
        else if( counter >=4 && counter <8 )
    {
      xVal = 1;
      zVal = 1;
      switch( counter%4 )
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
        else if( counter >=8 && counter <16 )
    {
      if( counter <12 )
        {
          xVal = 1;
          zVal = 2;
        }
      else
        {
          xVal = 2;
          zVal = 1;
        }
      switch( counter%4 )
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
        else if( counter >=16 && counter <32 )
    {
      if( counter <20 )
        {
          xVal = 1;
          zVal = 4;
        }
      else if( counter <24 )
        {
          xVal = 4;
          zVal = 1;
        }
      else if( counter <28 )
        {
          xVal = 2;
          zVal = 3;
        }
      else
        {
          xVal = 3;
          zVal = 2;
        }
      switch( counter%4 )
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
        else if( counter >=32 && counter <64 )
    {
      if( counter <36 )
        {
          xVal = 1;
          zVal = 6;
        }
      else if( counter <40 )
        {
          xVal = 6;
          zVal = 1;
        }
      else if( counter <44 )
        {
          xVal = 2;
          zVal = 6;
        }
      else if( counter <48 )
        {
          xVal = 6;
          zVal = 2;
        }
      else if( counter <52 )
        {
          xVal = 3;
          zVal = 5;
        }
      else if( counter <56 )
        {
          xVal = 5;
          zVal = 3;
        }
      else if( counter <60 )
        {
          xVal = 4;
          zVal = 5;
        }
      else
        {
          xVal = 5;
          zVal = 4;
        }
      switch( counter%4 )
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
        else if( counter >=64 && counter <128 )
    {
      if( counter <68 )
        {
          xVal = 1;
          zVal = 8;
        }
      else if( counter <72 )
        {
          xVal = 8;
          zVal = 1;
        }
      else if( counter <76 )
        {
          xVal = 2;
          zVal = 7;
        }
      else if( counter <80 )
        {
          xVal = 7;
          zVal = 2;
        }
      else if( counter <84 )
        {
          xVal = 2;
          zVal = 9;
        }
      else if( counter <88 )
        {
          xVal = 9;
          zVal = 2;
        }
      else if( counter <92 )
        {
          xVal = 3;
          zVal = 6;
        }
      else if( counter <96 )
        {
          xVal = 6;
          zVal = 3;
        }
      else if( counter <100 )
        {
          xVal = 3;
          zVal = 8;
        }
      else if( counter <104 )
        {
          xVal = 8;
          zVal = 3;
        }
      else if( counter <108 )
        {
          xVal = 5;
          zVal = 6;
        }
      else if( counter <112 )
        {
          xVal = 6;
          zVal = 5;          
        }
      else if( counter <116 )
        {
          xVal = 5;
          zVal = 7;
        }
      else if( counter <120 )
        {
          xVal = 7;
          zVal = 5;
        }
      else if( counter <124 )
        {
          xVal = 5;
          zVal = 8;
        }
      else
        {
          xVal = 8;
          zVal = 5;
        }
      switch( counter%4 )
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
      if( counter < 132 )
        {
          xVal = 7;
          zVal = 7;
        }
      else if( counter < 136 )
        {
          xVal = 8;
          zVal = 8;
        }
      else if( counter%4 == 0 )
        {
          xVal++;
          zVal++;
        }

      switch( counter%4 )
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

        // increment counter
        counter++;

        // set connected to true
        connected[q] = true;
        
        // set center of node
        graph1[q]->setCenter( x, y, z );
      }
  }
    }
  // draw the keys for the tags
  drawKeys();
}

// turn on or off the tag corresponding to tagNum
void ovGraph::turnOnOffTag( int tagNum )
{
  // if the tag is on, turn the tag off
  if( tagOn[tagNum] )
    {
      tagOn[tagNum] = false;
    }
  // else turn the tag on
  else
    {
      tagOn[tagNum] = true;
    }
}

// draw the edges for the nodes
void ovGraph::drawEdges()
{  
  // for all the edges
  for( int i=0;i<NUM_OF_NAMES;i++ )
    {
      for( int j=0;j<NUM_OF_NAMES;j++ )
  {
    // set connected to false
    con[i][j] = false;
  }
    }

   fflush( stdout );

  // for the number of names
  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      // change the progress bar 
      float fl = ( (float )i )/( (float )NUM_OF_NAMES-1 ) *40;
      progBar->setValue( 55+fl );
      int flint = fl + 55;

      if( flint < 100 )
      {
      printf( "\r[  %i%]", flint );
      fflush( stdout );
      }

     //
     // JON - commented out updating progress bar here to do it properly in individual places

      drawEdgesForNode( i, false );  

    }

     printf( "\r[  %i%]     Loaded.\n", 100 );
     fflush( stdout );

}

// pop up the path dialog
void ovGraph::popUpPath()
{
  ovQPath pathWindow;
  
  pathWindow.setGraph( this );

  QComboBox* bx1 = pathWindow.getCombo1();
  QComboBox* bx2 = pathWindow.getCombo2();
  
  QStringList lst;

  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      lst << names[i];
    }

  lst.sort();
  
  bx1->addItems( lst );
  bx2->addItems( lst );

  inter->Disable();

  pathWindow.exec();

  inter->Enable();
}

// set the default colours for the tags
void ovGraph::setDefaultColors( char* filename )
{    
  fstream file;
    
  file.open( filename );

  if( file.is_open() )
    {
      for( int i=0; i<NUM_OF_TAGS+1; i++ )
  {
    for( int j=0; j<3; j++ )
      {
        char* line = ( char* ) calloc( 1000, sizeof( char ));
        file.getline( line, 1000 );

        tagCols[i][j] = atof( line );
        free( line );
      }
  }
      file.close();
    }
  // else give error message
  else cerr << "Unable to open file: " << filename << endl;
}

// save the tag colours to a file 
void ovGraph::saveTagColors( char* filename )
{
  ofstream oFile( filename );

  for( int i=0; i<NUM_OF_TAGS+1; i++ )
    {
      oFile << tagCols[i][0] << "\n";
      oFile << tagCols[i][1] << "\n";
      oFile << tagCols[i][2] << "\n";
    }

  oFile.close();
}

// return the color based on the tag number
// the colours are set by RGB values
double *ovGraph::getColor( int dt )
{
  double *colour = ( double* ) calloc( 3*sizeof( double ), 3*sizeof( double ));

  colour[0] = tagCols[dt][0];
  colour[1] = tagCols[dt][1];
  colour[2] = tagCols[dt][2];

  return colour;
}
  
// set selected to the name that corresponds with char* sel
void ovGraph::setSelected( char* sel )
{

  bool match = false;

  // for the number of names
  for( int i =0; i<NUM_OF_NAMES; i++ )
    {
       // find the correct name from the text
      if( strstr( sel, names[i] ) != NULL ) 
  {
    oldSelected = selected;
    // set selected to the correct index
    selected = i;
  }
    }

  for ( list<int>::iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++ )
  {

    if( (*it ) == selected )
    {
      
      selectedNodes.remove( selected );
      match = true;
    }

  }
    
  if( !match )
  {
    selectedNodes.push_back( selected );
  }

  allNamesOn( true );

  
}





// show the node as white if selected
void ovGraph::select( bool shift )
{
  bool match = false;

  
  // if a node is selected
  if( selected >=0 && ndCon[selected] )
    {
  
  for ( list<int>::iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++ )
  {

    if( (*it ) == selected )
    {
      selectedNodes.remove( selected );

      // Actually removes the text actor ( the label ) from the selected node
            if( nameText[selected] != NULL )
      {      
        // remove the text actor
        rend->RemoveActor( nameText[selected] );
      }

      match = true; // To indicate it had already been selected
      
      it = selectedNodes.end(); // To exit loop
    }

  }
    
  if( !match )
  {
    selectedNodes.push_back( selected );
  }

  if( namesAllowedOn )
  {
  
    selectedNodesOn();

  }

  if( !shift )
  {

          // remove the selected actor and delete it
          rend->RemoveActor( selActor );
          selActor->Delete();


          if( oldSelected >= 0 && ( mode == 'g' || toggleConnected[oldSelected] ))
    {
      drawNode( oldSelected, false, vtkActor::New(), 0.0, 95.0/255.0, 1.0 );
    }

  

          selActor = drawNode( selected, false, selActor, 1, 1, 1 );
          rend->AddActor( selActor );

  }
    }
}



// show the node as white if selected
void ovGraph::shiftSelect( bool shift )
{
  bool match = false;

  
  // if a node is selected
  if( shiftSelected >=0 && ndCon[shiftSelected] )
    {
  
    // Iterates through all currently selected nodes
  for ( list<int>::iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++ )
  {

    // Checks to see if the node shiftselected is already selected
    if( (*it ) == shiftSelected )
    {
      selectedNodes.remove( shiftSelected ); // If so, remove it from selected nodes

      // Actually removes the text actor ( the label ) from the selected node
            if( nameText[shiftSelected] != NULL )
      {      
        // remove the text actor
        rend->RemoveActor( nameText[shiftSelected] );
      }

      match = true; // To indicate it had already been selected
      
      it = selectedNodes.end(); // To exit loop
    }

  }
    
  // If not already selected, add it to selected nodes  
  if( !match )
  {
    selectedNodes.push_back( shiftSelected );
  }

  // If name tags are allowed on, update them  
  if( namesAllowedOn )
  {
  
    selectedNodesOn();

  }
          
    }
}







// set up the display
void ovGraph::display()
{
  // set the viewport for the renderer
  rend->SetViewport( 0.0, 0.0, 800.0/800.0, 600.0/600.0 );
  // add the renderer to the window
  window->AddRenderer( rend );
  // set the render window for the interactor
  inter->SetRenderWindow( window ); 
 
  // set up camera
  cam->Dolly( 1.5 );
  rend->SetActiveCamera( cam );
  rend->ResetCamera();
}

// clean up the remaining vtk objects
void ovGraph::done()
{
  // delete global variables
  cam->Delete();
  inter->Delete();
  rend->Delete();
  rendText->Delete();
  window->Delete();
}

// case insensitive search [case insensitive strstr]
char* ovGraph::stristr( char* strToSearch, char* searchStr )
{
  // if the search string is null
  if ( !*searchStr )
    {
      // return the string to search
      return strToSearch;
    }
  
  // for all the characters in the string to search
  for ( ; *strToSearch; ++strToSearch )
    {
      // check if the upper case string to search is equal to the upper case search string
      if ( toupper( *strToSearch ) == toupper( *searchStr ) )
  {
    // create variables
    char *h, *n;
    
    // check through the string to see if the string to search is next within the search string
    for ( h = strToSearch, n = searchStr; *h && *n; ++h, ++n )
      {
        if ( toupper( *h ) != toupper( *n ) )
    {
      break;
    }
      }
    
    // if the search string is empty return the string to search
    if ( !*n ) 
      {
        return strToSearch;
      }
  }
    }
  // if the string is not found return NULL;
  return NULL;
}

// search the names for the string key
void ovGraph::search( char* key )
{
  // reset the search strings
  resetSearch();
  
  // get the window size
  int* x = window->GetSize();   
  
  // set up the text 
  textSetUp();

  // set the labels and search string
  label1->setText( "Searching:" );
  labelSelected->setText( key );
  label2->setText( "Results:" );

  // if the string to search is empty
  if( strcmp( key, "" ) == 0 )
    {
      // display all of the names
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    drawText( 0, names[i], ( x[1]*-1 )+60+( z*20 ), false );
  }
    }
  else
    {
      // duplicate the string to search
      char* str = strdup( key );

      // find the first space in the string to search
      str = strtok( str, " " );
  
      // initialize variables
      int count = 0;
      int target = 0;
  
      // while the string is not finished 
      while( str != NULL )
  {
    // add the search word to the list of strings to search
    searchStrings.push_back( str );
    // get the next word
    str = strtok( NULL, " " );
  }
      
      // for the number of names
      for( int i=0; i<NUM_OF_NAMES; i++ )
  {
    // initialize variables
    bool found = false;    
    list<char*>::iterator it;    
    int l = 0;
    
    // for the number of search strings
    for( it=searchStrings.begin(); it!= searchStrings.end(); it++ )
      {
        // if the string is found and has also been found before or is the first time through the loop
        if( stristr( names[i], *it ) != NULL && ( found || l==0 ))
    {     
      // set found to true
      found = true;
    }
        else
    {
      // set found to false
      found = false;
    }
        // increment l
        l++;
      }
    
    // if the strings have been found in the entry
    if( found )
      {
        // increment count
        count++;
        // set target to i
        target = i;
        // draw the text
        drawText( 0, names[i], 0, false );
      }
  }
  
      // if there was only one result then make it the selected node
      if( count == 1 )
  {
    oldSelected = selected;
    selected = target;    
    select( false );
  }
    }
  // render the window
  renderWin();
}

// reset the search strings
void ovGraph::resetSearch()
{
  searchStrings.clear();
}

// Parse the file with filename to get all names in the file
void ovGraph::GetEntry( char* filename )
{
  orlandoDataName = filename;
  entries.clear();

  // create a file
  fstream fil;
  ofstream tempFile( "tmp/data.txt" );

  // open the file
  fil.open( filename );
  
  // if the file opens properly
  if( fil.is_open() )
  {  
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    fil.getline( line, 1000 );

    // Finds the beginning of the file based on the "<ORLANDO>" tag  
    while( strstr( line, "<ORLANDO" ) == NULL )
    {
      // Skips blank space, or headers
      if( line!= NULL )
      {  
        free( line );
        line = NULL;
      }
      else
      {
        free( line );
      }

      line = ( char* ) calloc( 1000, sizeof( char ));
      fil.getline( line, 1000 );
    }

    while( !fil.eof() )
    {
      if( line != NULL )
      {
        free( line );
        line = NULL;
      }
      else
      {
        free( line );
      }
      
      // Writes entire XML file to a temp file that includes seperation markers
      line = ( char* ) calloc( 10000000, sizeof( char ));
      fil.getline( line, 10000000 );
      char* str = strtok( line, "^" );
      tempFile << str << "\n";
    }

    if( line != NULL )
    {
      free( line );
      line = NULL;
    }
    else
    {
      free( line );
    }

    // Adds marker to end of file
    tempFile << "^" << endl;
    tempFile.close();

    if( tagsUsed.empty() )
    {
      list<char*> emptyList;
      setTags( emptyList );
    }

    filename = "tmp/data.txt";
  }  
  // else give error message
  else cerr << "Unable to open file" << endl;

  // file to hold the file
  FILE* file;

  // vaiables to get the file read in to a buffer
  long lSize;
  char * buffer;
  size_t result;

  // initialize number of entries
  int numEntries = 0;

  // variables to hold the entry id
  char* entryID;
  int entryIDNum;
  int numNames = 0;
  
  // create a list for the names
  list<ovName> nms;

  // open file for reading
  file = fopen( filename, "rb" );

  // if the file opens with no problems
  if( NULL == file )
  {
    printf( "FILE OPEN FAILED: " );
    printf( filename );
    printf( "\n" );
    fflush( stdout );
  }
  else
  {
    // obtain file size:
    fseek ( file , 0 , SEEK_END );
    lSize = ftell ( file );
    rewind ( file );

    // allocate memory to contain the whole file:
    buffer = ( char* ) malloc ( sizeof( char )*lSize );
    if ( buffer == NULL ) {fputs ( "Memory error", stderr ); exit ( 2 );}
      
    // copy the file into the buffer:
    result = fread ( buffer, 1, lSize, file );
    if ( result != lSize ) {fputs ( "Reading error", stderr ); exit ( 3 );}

    // get the first line
    char* line = strtok( buffer, "\n" );

    // get the rest of the file
    char* restOfFile = strtok( NULL, "^" );
    int lineN = 0;
  
    while( restOfFile != NULL )
    {      
      lineN++;
      int prog = ( int ) ( (( (float )lSize-strlen( restOfFile ))/lSize )*40 ); // Calculates progress
      
      printf( "\r[  %i%]", prog ); // Updates textual progress bar
      fflush( stdout );
      progBar->setValue( prog ); // Updates progress bar
  
      // duplicate the line
      char* ptr = strdup( line );
      // char* to hold name
      char* name2;
  
      // ints to calculate the where the names are in the file
      int lineLen = strlen( line );
      int nameSpot = 0;
  
      bool includeLn = true;
  
      // if the line should be included
      if( includeLine( line ))
      {
        // if the line is greater than 0 and does not have the end of file character
        if( lineLen > 0 && ( strstr( line, "^" ) == NULL ))
        {
          // set the entry ID number to number of names
          entryIDNum = numNames;
          // find entryID
          strtok( ptr, "\"" );  
          // get entry id
          entryID = strtok( NULL, "\"" );
              
          // get the rest of the line
          char* ptr3 = strtok( NULL, "\n" ); 
          
          if( entryID == NULL || !includeByTime( strdup( entryID )) )
          {
            // restOfFile == NULL;
            includeLn = false;
            // printf( "\n\nNOT INCLUDING LINE\n" );
            // fflush( stdout );
          }
          else
          {
            // increment the number of entries
            numEntries++;
        
            // initialize variables
            bool unique = true;
            int keyInt = 0;
            int m=0;
            
            // for the number of names
            for( int m=0; m<numNames; m++ )
            {
              // check if the name has already been used
              if( strcmp( names[m], entryID ) == 0 )
              {
                // set unique to false
                unique = false;
                // set the key to the key already used
                keyInt = m;      
              }
            }

            // if unique
            if( unique )
            {
              // set the name of the entry to the entryID
              names[entryIDNum] = strdup( entryID );
              graph1[numNames] = new ovNode();
      
              // incrememnt the number of names
              numNames++;
            }
            else
            {
              // set the entry id number to key int
              entryIDNum = keyInt;
            }
        
            // push back the entry id number to the entries list
            entries.push_back( entryIDNum );
            lineNum[entryIDNum] = lineN;

            // find names
            do
            {        
              // get the next name
              ptr3 = strstr( ptr3, "<NAME" );  
              strtok( ptr3, "\"" );
              name2 = strtok( NULL, "\"" );
      
              // get the rest of the line
              ptr3 = strtok( NULL, "\n" );
      
              // if the name is not null and the name is not the entry find the names in the entry
              if( name2!=NULL && strcmp( name2, entryID ) != 0 )
              {        
                // set the name spot to the length of the rest of the line
                nameSpot = strlen( ptr3 );
          
                // initialize variables
                unique = true;
                bool thisEntry = false;
                keyInt = 0;
          
                // for the number of names
                for( int m=0; m<numNames; m++ )
                {
                  // check if the names have already been used
                  if( strcmp( names[m], name2 ) == 0 )
                  {
                    // set unique to false
                    unique = false;
                    // set keyInt to the key already used
                    keyInt = m;        
                  }
                }
          
                if( !unique )
                {
                  // iterate through the nms list
                  list<ovName>::iterator it;
                  for( it=nms.begin(); it!=nms.end(); it++ )
                  {
                    // if the name is equal to the name in the list
                    if( strcmp( it->getKey(), name2 ) == 0 )
                    {
                      // add the file spot to the name
                      it->addFileSpot( nameSpot );
                      // set this entry to true
                      thisEntry = true;
                    }
                  }
        
                  if( !thisEntry )
                  {        
                    // create a new name and add it to the list
                    ovName *nm = new ovName();
                    nm->setKey( name2 );
                    nm->addFileSpot( nameSpot );
                    nm->setKeyNum( keyInt );
                    nms.push_front( *nm );
                  }
                }
                else
                {
                  // create a new name
                  ovName *nm = new ovName();
                  nm->setKey( name2 );
                  nm->addFileSpot( nameSpot );
                  nm->setKeyNum( numNames );
        
                  // set the name at the number of names to the name string
                  names[numNames] = strdup( name2 );
                  graph1[numNames] = new ovNode();
                  numNames++;
        
                  // add the name to the list
                  nms.push_front( *nm );
                }
              }
            }
            while( ptr3 != NULL );
            // while the line is not empty
      
            // for the number of tags find the tags!
            for( int t=0; t<NUM_OF_TAGS; t++ )
            {
              // copy the line to a char*
              char* ptr2 = ( char* ) calloc( strlen( line )+1, sizeof( char )); 
              strcpy( ptr2, line );
      
              // initialize variables
              int startTag = 0;
              int endTag = 0;
              int ct = 0;
      
              do
              {
                // char* to hold the tag string
                char* str = ( char* ) calloc( 100, sizeof( char ));
          
                // add an opening bracket onto the tag string
                strcpy( str, "<" );
                strcat( str, tags[t] );
          
                // find tag in file
                ptr2 = strstr( ptr2, str );  
                name2 = strtok( ptr2, ">" );
                ptr2 = strtok( NULL, "\n" );
          
                // if tag is found
                if( name2 != NULL )
                {
                  // store the place of the start of the tag in the file
                  startTag = strlen( ptr2 );
        
                  // add the ending bracket onto the tag string
                  str = ( char* ) calloc( 100, sizeof( char ));
                  strcpy( str, "</" );
                  strcat( str, tags[t] );
                  strcat( str, ">" );
        
                  // find the end tag in the file
                  ptr2 = strstr( ptr2, str );  
                  name2 = strtok( ptr2, ">" );
                  ptr2 = strtok( NULL, "\n" );
        
                  // if the tag is found
                  if( name2 != NULL )
                  endTag = strlen( ptr2 );
        
                  // check the names to see if any appear in between the start tag and end tag
                  list<ovName>::iterator it;
                  for( it=nms.begin(); it!=nms.end(); it++ )
                  {        
                    ct++;
                    it->checkTag( startTag, endTag, t );       
                  }
                }
                free( str );
              }
              while( ptr2 != NULL );
              // while the line is not searched through
      
              free( ptr2 );
            }
          }
        }
      }  
      
      if( includeLn )
      {
        // for all the names 
        list<ovName>::iterator it;
        for( it=nms.begin(); it!=nms.end(); it++ )
        {
          // bool to hold tags
          bool *tgs = it->getTags();
          list<int> tagList;
      
          // for the number of tags
          for( int j=0; j<NUM_OF_TAGS; j++ )
          {
            // if the name was nested between the tag push the tag on a list
            if( tgs[j] )
            {
              tagList.push_back( j );
            }
          }
      
          tagList.push_back( NUM_OF_TAGS );
          ovEdge* e = new ovEdge( entryIDNum, ( *it ).getKeyNum(), tagList );
          edgeCount++;
          graph1[entryIDNum]->addChild( *e );
          graph1[( *it ).getKeyNum()]->addChild( *e );
      
          // clear the file spots from the names
          it->clearFileSpots();
        }
      }        

      // clear the names
      nms.clear();
    
      // free the pointer
      free( ptr );
      ptr = 0;        
    
      // if there is still lines left in the file
      if( restOfFile != NULL )
      {
        // get the next line in the file
        line = strtok( restOfFile, "\n" );
        
        // store the rest of the file
        restOfFile = strtok( NULL, "^" );
      }
    }
  
    NUM_OF_NAMES = numNames;

    if( onlyEntries )
    {
      includeOnlyEntries();
    }

    mergeEdges();
    setTitleText();
    initNames();

    // free the buffer
    free( buffer );
      
    // close the file
    fclose( file );

    ovQMainWindow* orl = ( ovQMainWindow* ) orland;
    orl->enableMenuItems( mode ); 

  }

  printf( "\r" );
}

// set the title bar to show what the program is visualizing
void ovGraph::setTitleText()
{  
  char* txt = ( char* ) calloc( 10000, sizeof( char ));

  strcat( txt, "Visualization of " );

  strcat( txt, orlandoDataName );

  if( strlen( inclWords ) > 1 )
    {
      strcat( txt, ".   Words Included:" );
      strcat( txt, inclWords );
      strcat( txt, "." );
    }

  if( strlen( exclWords ) > 1 )
    {
      strcat( txt, "   Words Excluded:" );
      strcat( txt, exclWords );
      strcat( txt, "." );
    }
  
  char* per = ( char* ) calloc( 200, sizeof( char ));

  if( birthYear != 0 && deathYear != 0 )
    {
      sprintf( per, "   Period: %i - %i. ", birthYear, deathYear );
    }

  strcat( txt, per );

  if( onlyEntries )
    {
      strcat( txt, "   Entries only." );
    }

  ovQMainWindow* orl = ( ovQMainWindow* ) orland;

  orl->setVisualizationText( txt );

  free( txt );
  free( per );
}


// merge the edges that may have duplicate values... 
// ( these will all be entries with other entries as a connection because the connection may be mentioned in either or both entries )
void ovGraph::mergeEdges()
{
  list<int>::iterator it;
  
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
       list<ovEdge> edg = graph1[*it]->getChildren();
       
       int q = -1;

       ovEdge* newEdges[NUM_OF_NAMES];

       list<ovEdge> newEdg;

       for( int i=0; i<NUM_OF_NAMES; i++ )
   {
     newEdges[i] = new ovEdge();
   }

       list<ovEdge>::iterator it2;
       for( it2=edg.begin(); it2!=edg.end(); it2++ )
   {
     q=it2->GetNode1();
     if( q == *it )
       {
         q=it2->GetNode2(); 
       }

     bool isEntry = false;

     list<int>::iterator it3;
     for( it3=entries.begin(); it3!=entries.end(); it3++ )
       {
         if( q == *it3 )
     {
       isEntry = true;
     }
       }

     if( isEntry )
       {
         newEdges[q]->AddTags( it2->GetTags() );
         newEdges[q]->SetNode1( *it );
         newEdges[q]->SetNode2( q );
       }
     else
       {
         newEdg.push_back( *it2 );
       }
   }

       for( int i=0; i<NUM_OF_NAMES; i++ )
   {
     if( newEdges[i]->GetNode1() != -1 )
       {
         newEdg.push_back( *newEdges[i] );         
       }
   }

       graph1[*it]->resetChildren();

       list<ovEdge>::iterator it4;
       for( it4=newEdg.begin(); it4!=newEdg.end(); it4++ )
   {
     graph1[*it]->addChild( *it4 );
   }
    }
}

// include only the entries of the graph
void ovGraph::includeOnlyEntries()
{  
  edgeCount=0;

  entries.unique();

  int tmp[entries.size()];
  list<ovEdge> chldrn[entries.size()];
  char* nms[entries.size()];
  int i=0;  

  list<int>::iterator it;
  for( it=entries.begin(); it!=entries.end(); it++ )
    {      
      tmp[i]= *it;
      nms[i]= strdup( names[*it] );
      i++;
    }

  i=0;
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
      list<ovEdge> edgs;
      edgs = graph1[*it]->getChildren();

      list<ovEdge>::iterator it2;
      for( it2=edgs.begin(); it2!=edgs.end(); it2++ )
  {
    int nd1 = isEntry( it2->GetNode1() );
    int nd2 = isEntry( it2->GetNode2() );
      
    if( nd1 >=0 && nd2 >=0 )
      {        
        ovEdge* nwEdge = new ovEdge( nd1, nd2, it2->GetTags() );
        edgeCount++;
        chldrn[i].push_back( *nwEdge );
      }
  }
      i++;
    }
  
  for( int j=0; j<NUM_OF_NAMES; j++ )
    {
      graph1[j]->resetChildren();
      names[j] = NULL;
    }
   
  for( int k=0; k<entries.size(); k++ )
    {
      names[k] = nms[k];

      list<ovEdge>::iterator it3;
      for( it3=chldrn[k].begin(); it3!=chldrn[k].end(); it3++ )
  {
    graph1[k]->addChild( *it3 );
  }
    }

    NUM_OF_NAMES = entries.size();

    entries.clear();
    
    for( int l=0; l<NUM_OF_NAMES; l++ )
      {
  entries.push_back( l );
      }
}

// check to see if the node n is an entry
int ovGraph::isEntry( int n )
{
  int ent=-1;
  int i=0;

  list<int>::iterator it;
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
      if( n == *it )
  {
    ent=i;
  }
      i++;
    }

  return ent;
}

// return the window
vtkRenderWindow* ovGraph::getWindow()
{
  return window;
}

// return the interactor
vtkRenderWindowInteractor* ovGraph::getInteractor()
{
  return inter;
}

// set the new tag rgb colour r, g, b for the index ind 
void ovGraph::setNewTagCol( int ind, int r, int g, int b )
{
  tagCols[ind][0] = r/255.0;
  tagCols[ind][1] = g/255.0;
  tagCols[ind][2] = b/255.0;
}

// Place the tag choices in the list for the tag window
void ovGraph::tagChoices()
{
  tagList->clear();
  tagsUsed.clear();

  // for the number of tags add the name of the tag to the list
  for( int i=0; i<NUM_OF_TAGS+1; i++ )
    {
      QListWidgetItem* tagItem = new QListWidgetItem( tags[i], tagList );
      double* rgbCol = getColor( i );
      QColor* tagCol = new QColor();
      tagCol->setRed( (int )( rgbCol[0]*255 ));
      tagCol->setGreen( (int )( rgbCol[1]*255 ));
      tagCol->setBlue( (int )( rgbCol[2]*255 ));
      tagItem->setBackgroundColor( *tagCol );      
      tagItem->setFlags( Qt::ItemIsUserCheckable );
      tagItem->setFlags( Qt::ItemIsEnabled );

      if( tagOn[i] )
  {
    tagItem->setCheckState( Qt::Checked );
  }
      else
  {
    tagItem->setCheckState( Qt::Unchecked );
  }
    }
}

// set tags to the list of tags passed in
void ovGraph::setTags( list<char*> lst )
{
  // for the number of tags: turn off the tags
  for( int i=0; i<NUM_OF_TAGS; i++ )
    {
      tagOn[i] = false;
    }
  
  // iterator for the list
  list<char*>::iterator it;

  // for each item in the list
  for( it=lst.begin(); it!=lst.end(); it++ )
    {      
      // for the number of tags 
      for( int i=0; i<NUM_OF_TAGS+1; i++ )
  {    
    // if the tag is in the list
    if( strcmp( *it, tags[i] ) == 0 )
      {
        // put the tag in the tagsUsed list
        tagsUsed.push_back( i );
        // turn the tag on
        tagOn[i] = true;
      }
  }
    }
  // turn on the whole entry tag
  // tagOn[NUM_OF_TAGS] = true;
  // put the whole entry tag in the tagsUsed list
  // tagsUsed.push_back( NUM_OF_TAGS );

  // set the number of tags to the size of the tagsUsed list
  numOfTags = tagsUsed.size();
  
  drawKeys();

  if( mode == 'p' )
    {
      
    }
  else if( mode == 't' )
    {
      drawToggled();
    }
  else if( mode == 'h' )
    {
      drawHighlighted();
    }
  else
    {
      redrawGraph();
    }

  select( false );

  renderWin();
}

// initialize the file from the xml file sent in
void ovGraph::initialize( char* filename )
{

  rendSetUp();
  textSetUp();
  
  // get entries from the file nm
  GetEntry( filename );
}

// set the mode to the mode passed in
// 'g' = graph
// 't' = toggle
// 'h' = highlight
// 'c' = camera
void ovGraph::setMode( char a )
{

  mode = a;

}

// get the mode
// 'g' = graph
// 't' = toggle
// 'h' = highlight
// 'c' = camera
char ovGraph::getMode()
{
  return mode;
}

// get whether or not the file is loading
bool ovGraph::getLoad()
{
  return load;
}

// pop up the tag window
void ovGraph::tagWindowOn()
{
  ovQTags* tagWin = ( ovQTags* ) tagWindow;

  // put the possible tags in the taglist
  tagChoices();

  // set the graph on the tag window
  tagWin->setGraph( this );

  inter->Disable();

  // show the tag window
  tagWin->exec();  

  inter->Enable();
}

// pop up the text window
void ovGraph::textWindowOn()
{
  ovQText* textWin = new ovQText();

  // set the graph on the tag window
  textWin->setGraph( this );

  QComboBox* cb1 = textWin->getCombo1();

  list<int>::iterator it;
  for( it=entries.begin(); it!=entries.end(); it++ )
    {
      cb1->addItem( names[*it] );
    }  

  ( cb1->model() )->sort( 0, Qt::AscendingOrder );
  int comboIndex = 0;

  // if the selected node is an entry
  if( selected != -1 && isEntry( selected ) != -1 ){
    comboIndex = cb1->findText( names[selected] );
    if( comboIndex != -1 )
      {
  cb1->setCurrentIndex( comboIndex );
      }
    else
      {
  comboIndex = 0;
      }
    QComboBox* cb2= textWin->getCombo2();
    namesFromEntry( names[selected], cb2 );
  }
  else {
    QComboBox* cb2= textWin->getCombo2();
    char* str = ( char* ) calloc( 200, sizeof( char ));
    strcpy( str, cb1->currentText() );        
    namesFromEntry( str, cb2 );
    free( str );
  }

  char* e = ( char* ) calloc( 200, sizeof( char ));
  strcpy( e, textWin->getCombo1()->itemText( comboIndex ));

  char* n = ( char* ) calloc( 200, sizeof( char ));
  strcpy( n, textWin->getCombo2()->itemText( 0 ));

  tagsFromNameEntry( e, n, textWin->getCombo3() );
     
  inter->Disable();
      
  // show the tag window
  textWin->exec();

  inter->Enable();
}

// sets up the names in the combobox cb from the entry en passed in
void ovGraph::namesFromEntry( char* en, QComboBox* cb )
{
  int ind = findIndexFromName( en );

  cb->clear();

  for( int i=0; i<NUM_OF_NAMES; i++ )
    {
      if( hasEdgeBetween( ind, i, NUM_OF_TAGS ))
  {
    cb->addItem( names[i] );
  }
    }
  ( cb->model() )->sort( 0, Qt::AscendingOrder );
}

// sets up the tags in the combobox cb from the information of the entry en and the name nm
void ovGraph::tagsFromNameEntry( char* en, char*nm, QComboBox* cb )
{
  int ind1 = findIndexFromName( en );
  int ind2 = findIndexFromName( nm );

  cb->clear();
  
  list<int>::iterator it; // tagsUsed
  for( it=tagsUsed.begin(); it!=tagsUsed.end(); it++ )
    {
      if( hasEdgeBetween( ind1, ind2, *it ))
  {
    cb->addItem( tags[*it] );
  }
    }
  ( cb->model() )->sort( 0, Qt::AscendingOrder );
}

// show the xml entry in the text browser tb based on the entry en, name nm, tag tg
void ovGraph::showXMLEntry( char* en, char* nm, char* tg, QTextBrowser* tb )
{
  if( en != NULL )
    {
      int ind = findIndexFromName( en );
      
      int cnt = lineNum[ind];
      
      char* filename = "tmp/data.txt";

      FILE* file;
      // vaiables to get the file read in to a buffer
      long lSize;
      char * buffer;
      size_t result;

      // open file for reading
      file = fopen( filename, "rb" );
      
      // if the file opens with no problems
      if( file != NULL )
  {      
    // obtain file size:
    fseek ( file , 0 , SEEK_END );
    lSize = ftell ( file );
    rewind ( file );
    
    // allocate memory to contain the whole file:
    buffer = ( char* ) malloc ( sizeof( char )*lSize );
    if ( buffer == NULL ) {fputs ( "Memory error", stderr ); exit ( 2 );}
    
    // copy the file into the buffer:
    result = fread ( buffer, 1, lSize, file );
    if ( result != lSize ) {fputs ( "Reading error", stderr ); exit ( 3 );}
    
    char* line = strtok( buffer, "\n" );
    
    for( int i=1; i<cnt; i++ )
      {
        line = strtok( NULL, "\n" );
      }

    tb->setText( line );

    int secondSelectionStart = 0;

    if( strcmp( tg, "WHOLE ENTRY" ) != 0 )
      {

        QString* str = new QString();
        str->append( '<' );
        str->append( tg );
        str->append( ".*" );
        str->append( "<NAME STANDARD=" );
        str->append( '"' );
        str->append( nm );
        str->append( '"' );
        str->append( ".*" );
        str->append( "</" );
        str->append( tg );
        str->append( '>' );
        
        QRegExp* regEx = new QRegExp();
        regEx->setPattern( *str );
        
        QTextCharFormat* format = new QTextCharFormat();
        format->setBackground( QBrush( QColor( 0, 255, 0, 100 )) );
        
        QTextCursor* cursor = new QTextCursor();
        *cursor = tb->document()->find( *regEx, 0 );
        cursor->setCharFormat( format->toCharFormat() );

        tb->setTextCursor( *cursor );    
        
        secondSelectionStart = cursor->selectionStart();

      }    
    
    QString* str2 = new QString();
    str2->append( "<NAME STANDARD=" );
    str2->append( '"' );
    str2->append( nm );
    str2->append( '"' );

    QRegExp* regEx2 = new QRegExp();
    regEx2->setPattern( *str2 );

    QTextCharFormat* format2 = new QTextCharFormat();
    format2->setBackground( QBrush( QColor( 255, 0, 0, 100 )) );

    QTextCursor* cursor2 = new QTextCursor();
    *cursor2 = tb->document()->find( *regEx2, secondSelectionStart );
    cursor2->setCharFormat( format2->toCharFormat() );

    tb->setTextCursor( *cursor2 );
    

    free( buffer );
  }
    }
  else{
    printf( "file problem" );
    fflush( stdout );
  }

}

// find the index of the node from a name nm
int ovGraph::findIndexFromName( char* nm )
{
  // for the number of names
  for( int i =0; i<NUM_OF_NAMES; i++ )
    {
      // if the strings are equal
      if( strstr( nm, names[i] ) != NULL )
  {
    // set selected
    return i;
  }
    }
  return -1;
}

// set the dates that the information should be between
void ovGraph::includeDatesBtw( int bY, int bM, int bD, int dY, int dM, int dD )
{
  birthYear = bY;
  birthMonth = bM;
  birthDay = bD;
    
  deathYear = dY;
  deathMonth = dM;
  deathDay = dD;
}

// check the name to see if it is included based on the time constraints
bool ovGraph::includeByTime( char* stdName )
{
  
  bool inclu = true;
  bool fnd = false;
  
  ovDate* b1 = NULL;
  ovDate* d1 = NULL;
  ovDate* b2 = new ovDate( birthYear, birthMonth, birthDay );
  ovDate* d2 = new ovDate( deathYear, deathMonth, deathDay );
  
  if( birthYear != 0 && deathYear != 0 )
    {
      ifstream file;
      
      file.open( "resources/catalog.txt", ios::in );
  
      if( file.is_open() )
  {
    char* line = ( char* ) calloc( 1000, sizeof( char ));
    file.getline( line, 1000 );
    while( !file.eof() && line!=NULL )
      {    
        strtok( line, "\"" );
        char* str1 = strtok( NULL, "\"" ); 
        strtok( NULL, "\"" );
        
        // printf( "Name: " );
        // printf( str1 );
        // fflush( stdout );
        
        // printf( "STANDARD NAME: " );
        // printf( stdName );
        // fflush( stdout );
        
        if( strcmp( str1, stdName ) == 0 )
    {
      fnd = true;
      
      char* str2 = strtok( NULL, "\"" );
      char* strBDay = strdup( str2 );

      strtok( NULL, "\"" );
      char* str3 = strtok( NULL, "\"" );

      b1 = new ovDate( str2, true );
      d1 = new ovDate( str3, false );

      line = NULL;       
    }
        else
    {        
      free( line );
      line = ( char* ) calloc( 1000, sizeof( char ));
      file.getline( line, 1000 );
    }
      }
    free( line );
    file.close();
  }
      
      if( fnd )
  {
    // printf( "\n\n CHECKING DATES \n\n" );
    // fflush( stdout );
    
    inclu = false;

    /*
    printf( "Year of b1: %i", b1->GetYear() );
    printf( "Year of b2: %i", b2->GetYear() );
    printf( "Year of d1: %i", d1->GetYear() );
    printf( "Year of d2: %i", d2->GetYear() );
    fflush( stdout );*/

    if( b1->lessThan( b2 ))
      {
        // printf( "b1 < b2" );
        // fflush( stdout );
        
        if( d1->greaterThan( b2 ))
    {
      // printf( "include" );
      // fflush( stdout );
      inclu = true;
    }
      }
    else if( b1->lessThan( d2 ))
      {  
        // printf( "include" );
        // fflush( stdout );
        inclu = true;
      }
  }
    }
  
  return inclu;
}

// check to see if you should include the line based on words excluded or included
bool ovGraph::includeLine( char* line )
{
  bool incl = true;
  bool a = false;
  bool b = false;
  bool first = true;

  fstream fl;

  fl.open( "tmp/orlando.inc" );

  if( fl.is_open() )
    {      
      do
  {     
    char* ln = ( char* ) calloc( 1000, sizeof( char ));
    fl.getline( ln, 1000 );
   
    a = false;
    char* s = strdup( ln );

    if( strcmp( s, "" ) != 0 )
      {       
        incl = false;

        list<char*> wd;

        char* str1 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        char* str2 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        char* str3 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        
        for( int i=0; i<strlen( s ); i++ )
    {
      if( i == 0 )
        {
          str1[i] = toupper( s[i] );
          str2[i] = toupper( s[i] );
          str3[i] = tolower( s[i] );
        }
      else
        {
          str1[i] = tolower( s[i] );
          str2[i] = toupper( s[i] );
          str3[i] = tolower( s[i] );
        }
    }
        str1[strlen( s )]='\0';
        str2[strlen( s )]='\0';
        str3[strlen( s )]='\0';

        wd.push_back( s );
        wd.push_back( str1 );
        wd.push_back( str2 );
        wd.push_back( str3 );

        list<char*>::iterator it2;
        for( it2=wd.begin(); it2!=wd.end(); it2++ )
    {
      // if the word is found set a to true
      if( strstr( line, *it2 )!= NULL )
        {        
          a = true;
        }
    }     

        if( a && ( first || b ))
    {
      b = true;
    }
        else
    {
      b = false;
    }          

        if( a && b )
    {
      incl = true;
    }
        else
    {
      incl = false;
    }
        first = false;
      }
  free( ln );
  }while( !fl.eof() );
      fl.close();
    }

  bool excl = true; 
  a = true;
  b = false;
  first = true;
  
  fl.open( "tmp/orlando.exc" );

  if( fl.is_open() )
    {
      do
  {     
    char* ln = ( char* ) calloc( 1000, sizeof( char ));
    fl.getline( ln, 1000 );
   
    a = true;
    char* s = strdup( ln );

    if( strcmp( ln, "" ) != 0 )
      {       
        excl = false;

        list<char*> wd;

        char* str1 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        char* str2 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        char* str3 = ( char* ) alloca( (strlen( s )+1 )* sizeof( char ));
        
        for( int i=0; i<strlen( s ); i++ )
    {
      if( i == 0 )
        {
          str1[i] = toupper( s[i] );
          str2[i] = toupper( s[i] );
          str3[i] = tolower( s[i] );
        }
      else
        {
          str1[i] = tolower( s[i] );
          str2[i] = toupper( s[i] );
          str3[i] = tolower( s[i] );
        }
    }
        str1[strlen( s )]='\0';
        str2[strlen( s )]='\0';
        str3[strlen( s )]='\0';

        wd.push_back( s );
        wd.push_back( str1 );
        wd.push_back( str2 );
        wd.push_back( str3 );

        list<char*>::iterator it2;
        for( it2=wd.begin(); it2!=wd.end(); it2++ )
    {
      if( strstr( line, *it2 )!= NULL )
        {          
          a = false;
        }
    }     

        if( a && ( first || b ))
    {
      b = true;
    }
        else
    {
      b = false;
    }          

        if( a && b )
    {
      excl = true;
    }
        else
    {
      excl = false;
    }
        first = false;
      }

  free( ln );
  }while( !fl.eof() );
      fl.close();
      }

  if( incl && excl )
    {
      // printf( "returned true" );
      // fflush( stdout );
      return true;
    }
  else
    {
      // printf( "returned false" );
      // fflush( stdout );
      return false;
    }
}

// include the words that are in the string s
void ovGraph::include( char* s )
{
  // create file
  ofstream file( "tmp/orlando.inc" );

  int i =0;

  if( s != NULL )
    {      
      char* str = strtok( s, ", " );
      
      if( str == NULL )
  {
    str = strdup( s );
  }
     
      do
  {
    if( strcmp( str, "AND" ) == 0 || strcmp( str, "OR" ) == 0 )
      {
        
      }
    else
      { 
        file << str << endl;    
        strcat( inclWords, " " );
        strcat( inclWords, str );
      }

    str = strtok( NULL, ", " );

  } while( str != NULL );
    }
}

// exclude the words that are in the string s
void ovGraph::exclude( char* s )
{
  ofstream file( "tmp/orlando.exc" );

  if( s != NULL )
    {      
      char* str = strtok( s, ", " );
      
      if( str == NULL )
  {
    str = ( char* ) calloc( 1000, sizeof( char ));
    str = strdup( s );
  }
     
      do
  {
    if( strcmp( str, "AND" ) == 0 || strcmp( str, "OR" ) == 0 )
      {
        
      }
    else
      { 
        file << str << endl;        
        strcat( exclWords, " " );
        strcat( exclWords, str );
      }

    free( str );
    str = ( char* ) calloc( 1000, sizeof( char ));
    str = strtok( NULL, ", " );

  } while( str != NULL );

  free( str );
    }
}

// set whether or not to include a node based on it being an entry
void ovGraph::setEntriesOnly( bool b )
{
  onlyEntries = b;
}

// get a list of all of the cases that might be seen in the file
// EXAMPLE: "poetess" = Poetess, POETESS, poetess 
list<char*> ovGraph::wordCases( char* s )
{
  // create a list for the word cases
  list<char*> lst;
  
  // strings to hold the word cases
  char* str1 = ( char* ) alloca( strlen( s )* sizeof( char ));
  char* str2 = ( char* ) alloca( strlen( s )* sizeof( char ));
  char* str3 = ( char* ) alloca( strlen( s )* sizeof( char ));

  // for the length of the string change the char to upper or lower case to make 
  // EXAMPLE: s = "poetess" str1 = Poetess str2 = "POETESS" str3 = "poetess" 
  for( int i=0; i<strlen( s ); i++ )
    {
      if( i == 0 )
  {
    str1[i] = toupper( s[i] );
    str2[i] = toupper( s[i] );
    str3[i] = tolower( s[i] );
  }
      else
  {
    str1[i] = tolower( s[i] );
    str2[i] = toupper( s[i] );
    str3[i] = tolower( s[i] );
  }
    }

  // add the string cases to the list
  lst.push_back( s );
  lst.push_back( str1 );
  lst.push_back( str2 );
  lst.push_back( str3 );
}

// deselect the node based on the position
void ovGraph::deselect( int a, int b )
{
   // initialize variable
  int i = -1;

  // if position is null
  if( a != 0 &&  b != 0 )
    {   
      // get the node at the position passed in
      i = nodeAtPos( a, b, false );
      if( !ndCon[i] )
  {
    i = -1;
  }
    }

  // if selected node is not null
  if( i >=0 && ndCon[i] && toggleConnected[i] )
    {
      for( int j=0; j<NUM_OF_NAMES; j++ )
  {
    // set toggle connected to false
    toggleConnected[j] = false;
    ndCon[j] = false;      
  }

      toggledOn.remove( i );

      oldSelected = -1;

      if( mode == 't' )
  {
    drawToggled();
  }
      else
  {
    drawHighlighted();
  }
      
      for( int j=0; j<NUM_OF_NAMES; j++ )
  {
    if( nameOn[j] && ndCon[j] )
      {
        nameOnOff( true, names[j] );
      }      
  }
    }
}

// save a screenshot of the graph
void ovGraph::saveScreenshot( char* filename, int filetype, int magnification )
{  

  vtkSmartPointer<vtkImageWriter> writer;

  window->Render();

  switch( filetype )
    {
    case JPG: 
      writer = vtkSmartPointer<vtkJPEGWriter>::New();
      break;
    
    case BMP: 
      writer = vtkSmartPointer<vtkBMPWriter>::New(); 
      break;
    
    case TIF: 
      writer = vtkSmartPointer<vtkTIFFWriter>::New();
      break;
    default:
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }

  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput( window );
  windowToImageFilter->SetMagnification( magnification ); // set the resolution of the output image
  windowToImageFilter->Update();

  writer->SetFileName( filename );
  writer->SetInput( windowToImageFilter->GetOutput() );
  writer->Update();
  writer->Write();
}
