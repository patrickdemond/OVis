/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovOrlandoReader.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovOrlandoReader.h"

#include "ovOrlandoTagInfo.h"

#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkVariantArray.h"

#include <vtkstd/stdexcept>

vtkCxxRevisionMacro( ovOrlandoReader, "$Revision: $" );
vtkStandardNewMacro( ovOrlandoReader );

// this undef is required on the hp. vtkMutexLock ends up including
// /usr/inclue/dce/cma_ux.h which has the gall to #define read as cma_read
#ifdef read
#undef read
#endif

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
vtkGraph* ovOrlandoReader::GetOutput(int idx)
{
  return vtkGraph::SafeDownCast( this->GetOutputDataObject( idx ) );
} 

//----------------------------------------------------------------------------
void ovOrlandoReader::SetOutput( vtkGraph *output )
{
  this->GetExecutive()->SetOutputData( 0, output );
} 

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoReader::RequestDataObject(
  vtkInformation* request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkMutableDirectedGraph *output = vtkMutableDirectedGraph::New();
  this->SetOutput( output );
  // Releasing data for pipeline parallism.
  // Filters will know it is empty.
  output->ReleaseData();
  output->Delete();
  
  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovOrlandoReader::ProcessRequest(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject( 0 );

  if( request->Has( vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT() ) )
  {
    return this->RequestDataObject( request, inputVector, outputVector );
  }

  if( request->Has( vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT() ) )
  {
    return 1;
  }

  if( request->Has( vtkDemandDrivenPipeline::REQUEST_INFORMATION() ) )
  {
    return 1;
  }

  if( request->Has( vtkDemandDrivenPipeline::REQUEST_DATA() ) &&
      0 == outInfo->Get( vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER() ) )
  {
    vtkDebugMacro( << "Reading orlando data ...");

    ovOrlandoTagInfo *tagInfo = ovOrlandoTagInfo::GetInfo();
    
    // create and set up a graph with pedigree and tag arrays
    // (the pedigree array is used to track vertices by name)
    vtkSmartPointer< vtkMutableDirectedGraph > graph =
      vtkSmartPointer< vtkMutableDirectedGraph >::New();
    vtkSmartPointer< vtkStringArray > pedigreeArray = vtkSmartPointer< vtkStringArray >::New();
    pedigreeArray->SetName( "pedigree" );
    graph->GetVertexData()->AddArray( pedigreeArray );
    graph->GetVertexData()->SetPedigreeIds( pedigreeArray );

    // the color array is used for edge coloring, this will be manipulated internally by the
    // restrict graph algorithm
    vtkSmartPointer< vtkIntArray > vertexColorArray = vtkSmartPointer< vtkIntArray >::New();
    vertexColorArray->SetName( "color" );
    graph->GetVertexData()->AddArray( vertexColorArray );

    // the size array is used to determine the size of each vertex
    vtkSmartPointer< vtkDoubleArray > sizeArray = vtkSmartPointer< vtkDoubleArray >::New();
    sizeArray->SetName( "size" );
    graph->GetVertexData()->AddArray( sizeArray );
    graph->GetVertexData()->SetActiveScalars( sizeArray->GetName() );

    // the label array is used to track each entries full un-marked-up label
    vtkSmartPointer< vtkStringArray > labelArray = vtkSmartPointer< vtkStringArray >::New();
    labelArray->SetName( "label" );
    graph->GetVertexData()->AddArray( labelArray );

    // the content array is used to track each entries full un-marked-up content
    vtkSmartPointer< vtkStringArray > authorContentArray = vtkSmartPointer< vtkStringArray >::New();
    authorContentArray->SetName( "content" );
    graph->GetVertexData()->AddArray( authorContentArray );

    // the stemmed content array is used to track each entries full un-marked-up, stemmed content
    vtkSmartPointer< vtkStringArray > authorStemmedContentArray =
      vtkSmartPointer< vtkStringArray >::New();
    authorStemmedContentArray->SetName( "stemmedContent" );
    graph->GetVertexData()->AddArray( authorStemmedContentArray );

    // the birth and death arrays track author birth and death dates
    vtkSmartPointer< vtkIntArray > birthArray = vtkSmartPointer< vtkIntArray >::New();
    birthArray->SetName( "birth" );
    graph->GetVertexData()->AddArray( birthArray );
    vtkSmartPointer< vtkIntArray > deathArray = vtkSmartPointer< vtkIntArray >::New();
    deathArray->SetName( "death" );
    graph->GetVertexData()->AddArray( deathArray );

    // the gender and writer type of the author
    vtkSmartPointer< vtkIntArray > genderArray = vtkSmartPointer< vtkIntArray >::New();
    genderArray->SetName( "gender" );
    graph->GetVertexData()->AddArray( genderArray );
    vtkSmartPointer< vtkIntArray > writerTypeArray = vtkSmartPointer< vtkIntArray >::New();
    writerTypeArray->SetName( "writerType" );
    graph->GetVertexData()->AddArray( writerTypeArray );

    // the tag array is used to keep a list of tags which associates the two vertices
    vtkSmartPointer< vtkStringArray > tagArray = vtkSmartPointer< vtkStringArray >::New();
    tagArray->SetName( "tags" );
    graph->GetEdgeData()->AddArray( tagArray );

    // the color array is used for edge coloring, this will be manipulated internally by the
    // restrict graph algorithm
    vtkSmartPointer< vtkVariantArray > edgeColorArray = vtkSmartPointer< vtkVariantArray >::New();
    edgeColorArray->SetName( "color" );
    graph->GetEdgeData()->AddArray( edgeColorArray );
    
    // the content array is used to keep a string of raw text associated with every edge
    vtkSmartPointer< vtkStringArray > contentArray = vtkSmartPointer< vtkStringArray >::New();
    contentArray->SetName( "content" );
    graph->GetEdgeData()->AddArray( contentArray );

    // the stemmed content array is used to keep a string of raw stemmed text associated with
    // every edge
    vtkSmartPointer< vtkStringArray > stemmedContentArray = vtkSmartPointer< vtkStringArray >::New();
    stemmedContentArray->SetName( "stemmedContent" );
    graph->GetEdgeData()->AddArray( stemmedContentArray );

    // A string to store the current active tags and mark them all as '0' chars to begin with.
    // Every time we open a tag in the association types vector we will set the tag to '1',
    // and every time the tag closes we will set it back to '0'.  This string will then be
    // copied every time we add a new edge.
    ovString currentTagArray( tagInfo->GetNumberOfTags(), '0' );
    
    // Keep track of the content so that it can be added to the author node
    ovString authorContent;

    try
    {
      vtkIdType currentVertexId = -1, currentConnectingVertexId = -1;
      vtkStdString currentVertexPedigree = "";
      bool inBirthTag = false, inDeathTag = false, inOrlandoHeaderTag = false;
      this->CreateReader();
      
      // count how many entries are in the file (for progress reporting)
      double totalEntries = 0;
      ovString line;
      vtkstd::ifstream fileStream( this->FileName.c_str() );
      if( !fileStream.is_open() ) throw vtkstd::runtime_error( "Unable to stream file." );
      // this count may be inaccurate since it doesn't account for spacing (ie: "< ENTRY"),
      // however, it is nice and fast and if the count is incorrect then the only side effect
      // is incorrect progress reporting which really isn't so bad (so worth the uncertainty)
      while( getline( fileStream, line ) ) if( line.find( "<ENTRY" ) ) totalEntries++;
      fileStream.close();
      
      // process each node, one at a time
      double numEntries = 0, progress;

      bool inDiv0 = false, inDiv1 = false, inDiv2 = false, inP = false, inP2 = false;
      bool inDate = false, inBibcit = false, inHeading = false, inScholarnote = false;
      ovString div0Content, div1Content, div2Content, pContent, p2Content;
      ovIntVector div0Vertices, div1Vertices, div2Vertices, pVertices, p2Vertices;

      while( this->ParseNode() )
      {
        // ignore node type 14 (#text stuff that we don't need)
        if( 14 == this->CurrentNode.NodeType ) continue;
        
        // make sure to skip anything inside the orlando header tag
        if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "ORLANDOHEADER" ) )
        {
          if( !( this->CurrentNode.IsEmptyElement ) && this->CurrentNode.IsOpeningElement() )
          {
            inOrlandoHeaderTag = true;
          }
          else if( this->CurrentNode.IsClosingElement() )
          {
            inOrlandoHeaderTag = false;
          }
          continue;
        }

        if( inOrlandoHeaderTag ) continue;

        // This node is an entry (vertex)
        if( 1 == this->CurrentNode.Depth &&
            0 == xmlStrcmp( BAD_CAST "ENTRY", this->CurrentNode.Name ) )
        {
          if( this->CurrentNode.IsOpeningElement() )
          {
            progress = numEntries / totalEntries;
            this->InvokeEvent( vtkCommand::ProgressEvent, &( progress ) );
            
            // get the pedigree (author's name) from this node's id attribute
            const char *pedigree =
              ( char* )( xmlTextReaderGetAttribute( this->Reader, BAD_CAST "standard" ) );

            // create a new vertex using the Id (author name) as the pedigree
            vtkSmartPointer< vtkVariantArray > array = vtkSmartPointer< vtkVariantArray >::New();
            array->SetNumberOfComponents( 10 );
            array->SetNumberOfTuples( 1 );
            array->SetValue( 0, vtkVariant( pedigree ) ); // pedigree
            array->SetValue( 1, 1 ); // color
            array->SetValue( 2, 2 ); // size
            array->SetValue( 3, vtkVariant( "" ) ); // label
            array->SetValue( 4, vtkVariant( "" ) ); // content
            array->SetValue( 5, vtkVariant( "" ) ); // stemmed content
            array->SetValue( 6, 0 ); // birth date
            array->SetValue( 7, 0 ); // death date
            array->SetValue( 8, ovOrlandoReader::GenderTypeUnknown ); // gender
            array->SetValue( 9, ovOrlandoReader::WriterTypeNone ); // writer type
            currentVertexId = graph->AddVertex( array );
            currentVertexPedigree = pedigreeArray->GetValue( currentVertexId );
          }
          
          if( this->CurrentNode.IsEmptyElement ||
              this->CurrentNode.IsClosingElement() )
          { // no more child vertices for this vertex

            // add the matching content
            authorContentArray->SetValue( currentVertexId, authorContent );
            authorStemmedContentArray->SetValue( currentVertexId, stemWords( authorContent ) );
            authorContent = "";
            currentVertexId = -1;
            numEntries++;
          }
        }
        else if( 0 <= currentVertexId )
        {
          // we're in an open NAME tag which is closing
          if( 0 <= currentConnectingVertexId &&
              this->CurrentNode.IsClosingElement() &&
              0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "NAME" ) )
          {
            currentConnectingVertexId = -1;
          }
          else
          {
            if( this->CurrentNode.HasContent )
            {
              // get and clean the content
              ovString content = ( char* )( this->CurrentNode.Content );
              size_t start = content.find_first_not_of("\t\n\r");
              if( start != vtkstd::string::npos )
                content = content.substr( start, content.find_last_not_of("\t\n\r") - start + 1 );
              
              if( 0 < content.length() )
              {
                // format text from certain tags
                if( inDate ) content = content + " ";
                else if( inBibcit ) content = " (" + content + ") ";
                else if( inHeading ) content = content + ". ";
                else if( inScholarnote ) content = " " + content + " ";

                // add to author content
                authorContent.append( content );
                
                // add to currently open div tags
                if( inP2 ) p2Content.append( content );
                if( inP ) pContent.append( content );
                if( inDiv2 ) div2Content.append( content );
                if( inDiv1 ) div1Content.append( content );
                if( inDiv0 ) div0Content.append( content );
              }
            }
            
            // Concerning the state of DIV0, DIV1, DIV2 and P tags when a NAME tag is found:
            // All NAME tags are inside of DIV1, DIV2 or P tags
            // A DIV2 tag is always inside of a DIV1 tag, and P tags are always inside a DIV0,
            // DIV1, DIV2 or another P tag.
            // We use the variables inDiv1, inDiv2 and inP to track which tag and level we are
            // currently in, so that when we close that tag then any names which were found
            // inside can have the text added as their content.
            if( !this->CurrentNode.IsEmptyElement )
            {
              if( this->CurrentNode.IsOpeningElement() )
              {
                if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "P" ) )
                {
                  if( inP ) inP2 = true;
                  else inP = true;
                }
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV2" ) )
                  inDiv2 = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV1" ) )
                  inDiv1 = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV0" ) )
                  inDiv0 = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) )
                  inDate = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "BIBCIT" ) )
                  inBibcit = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "HEADING" ) )
                  inHeading = true;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "SCHOLARNOTE" ) )
                  inScholarnote = true;
              }
              else if( this->CurrentNode.IsClosingElement() )
              {
                if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "P" ) )
                {
                  if( inP2 )
                  {
                    // set the content array for any NAMES which were opened inside this tag
                    while( !p2Vertices.empty() )
                    {
                      contentArray->SetValue( p2Vertices.back(), p2Content );
                      stemmedContentArray->SetValue( p2Vertices.back(), stemWords( p2Content ) );
                      p2Vertices.pop_back();
                    }

                    inP2 = false;
                    p2Content = "";
                  }
                  else
                  {
                    // set the content array for any NAMES which were opened inside this tag
                    while( !pVertices.empty() )
                    {
                      contentArray->SetValue( pVertices.back(), pContent );
                      stemmedContentArray->SetValue( pVertices.back(), stemWords( pContent ) );
                      pVertices.pop_back();
                    }

                    inP = false;
                    pContent = "";
                  }
                }
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV2" ) )
                {
                  // set the content array for any NAMES which were opened inside this tag
                  while( !div2Vertices.empty() )
                  {
                    contentArray->SetValue( div2Vertices.back(), div2Content );
                    stemmedContentArray->SetValue( div2Vertices.back(), stemWords( div2Content ) );
                    div2Vertices.pop_back();
                  }

                  inDiv2 = false;
                  div2Content = "";
                }
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV1" ) )
                {
                  // set the content array for any NAMES which were opened inside this tag
                  while( !div1Vertices.empty() )
                  {
                    contentArray->SetValue( div1Vertices.back(), div1Content );
                    stemmedContentArray->SetValue( div1Vertices.back(), stemWords( div1Content ) );
                    div1Vertices.pop_back();
                  }

                  inDiv1 = false;
                  div1Content = "";
                }
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DIV0" ) )
                {
                  // set the content array for any NAMES which were opened inside this tag
                  while( !div0Vertices.empty() )
                  {
                    contentArray->SetValue( div0Vertices.back(), div0Content );
                    stemmedContentArray->SetValue( div0Vertices.back(), stemWords( div0Content ) );
                    div0Vertices.pop_back();
                  }

                  inDiv0 = false;
                  div0Content = "";
                }
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) )
                  inDate = false;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "BIBCIT" ) )
                  inBibcit = false;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "HEADING" ) )
                  inHeading = false;
                else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "SCHOLARNOTE" ) )
                  inScholarnote = false;
              }
            }

            // get the pedigree (associate's name) from this node's standard attribute
            const char *pedigree =
              ( char* )( xmlTextReaderGetAttribute( this->Reader, BAD_CAST "STANDARD" ) );

            // This node describes an association (edge)
            if( this->CurrentNode.IsOpeningElement() &&
                0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "NAME" ) &&
                currentVertexPedigree != vtkStdString( pedigree ) )
            {
              // Create a new vertex (if necessary) using the standard (association's name) as the
              // pedigree
              if( -1 == graph->FindVertex( pedigree ) )
              {
                vtkSmartPointer< vtkVariantArray > array = vtkSmartPointer< vtkVariantArray >::New();
                array->SetNumberOfComponents( 10 );
                array->SetNumberOfTuples( 1 );
                array->SetValue( 0, vtkVariant( pedigree ) ); // pedigree
                array->SetValue( 1, 0 ); // color
                array->SetValue( 2, 1 ); // size
                array->SetValue( 3, vtkVariant( "" ) ); // label
                array->SetValue( 4, vtkVariant( "" ) ); // content (blank for now)
                array->SetValue( 5, vtkVariant( "" ) ); // stemmed content (blank for now)
                array->SetValue( 6, 0 ); // birth date
                array->SetValue( 7, 0 ); // death date
                array->SetValue( 8, ovOrlandoReader::GenderTypeUnknown ); // gender
                array->SetValue( 9, ovOrlandoReader::WriterTypeNone ); // writer type
                graph->AddVertex( array );
              }
              
              // Create an edge from the current vertex to the new vertex
              vtkSmartPointer< vtkVariantArray > array = vtkSmartPointer< vtkVariantArray >::New();
              array->SetNumberOfComponents( 4 );
              array->SetNumberOfTuples( 1 );
              array->SetValue( 0, vtkVariant( currentTagArray ) ); // tags
              array->SetValue( 1, 0 ); // color (set by the restrict filter)
              array->SetValue( 2, vtkVariant( "" ) ); // content (blank for now)
              array->SetValue( 3, vtkVariant( "" ) ); // stemmed content (blank for now)
              vtkEdgeType e = graph->AddEdge( currentVertexPedigree, pedigree, array );

              // and track which content paragraph tag type we are currently in for content
              if( inP2 ) p2Vertices.push_back( e.Id );
              else if( inP ) pVertices.push_back( e.Id );
              else if( inDiv2 ) div2Vertices.push_back( e.Id );
              else if( inDiv1 ) div1Vertices.push_back( e.Id );
              else if( inDiv0 ) div0Vertices.push_back( e.Id );

              // determine the connecting vertex' id if this isn't an empty element
              if( !( this->CurrentNode.IsEmptyElement ) )
              {
                currentConnectingVertexId = pedigreeArray->LookupValue( pedigree );
              }
            }
            // This node may describe an association type (edge tag) or birth/death dates
            else
            {
              // if this is a biography tag then record the author's gender and writer type
              if( this->CurrentNode.IsOpeningElement() &&
                  0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "BIOGRAPHY" ) )
              {
                // get the current author's gender and writer type from the the biography tag
                genderArray->SetValue( currentVertexId,
                  ovOrlandoReader::GenderTypeFromString(
                    ( char* )( xmlTextReaderGetAttribute( this->Reader, BAD_CAST "SEX" ) ) ) );
                writerTypeArray->SetValue( currentVertexId,
                  ovOrlandoReader::WriterTypeFromString(
                    ( char* )( xmlTextReaderGetAttribute( this->Reader, BAD_CAST "PERSON" ) ) ) );
              }

              // first let's see if the name is in our tag list
              int index = -1;
              if( !( this->CurrentNode.IsEmptyElement ) && this->CurrentNode.IsOpeningElement() )
              {
                index = tagInfo->FindTagIndexFromName( ( char* )( this->CurrentNode.Name ) );
                if( 0 <= index ) // we found a match
                {
                  // are we already in a connecting vertex (NAME) tag?
                  if( 0 <= currentConnectingVertexId )
                  {
                    tagArray->GetValue( currentConnectingVertexId ).at( index ) = '1';
                  }

                  // opening element, mark the tag as true
                  currentTagArray[index] = '1';
                }
              }
              else if( this->CurrentNode.IsClosingElement() )
              {
                index = tagInfo->FindTagIndexFromName( ( char* )( this->CurrentNode.Name ) );
                if( 0 <= index ) // we found a match
                { // closing element, mark the tag as false
                  currentTagArray[index] = '0';
                }
              }
              
              // Now handle birth and death tags, and any dates associated with them
              // Is this a date inside a birth tag?
              if( inBirthTag && 
                  this->CurrentNode.IsOpeningElement() &&
                  ( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) ||
                    0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATESTRUCT" ) ||
                    0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATERANGE" ) ) )
              {
                ovDate newDate( ( char* )(
                  xmlTextReaderGetAttribute( this->Reader, 
                  0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATERANGE" ) ?
                  BAD_CAST "FROM" : BAD_CAST "VALUE" ) ) );
                ovDate curDate( ( birthArray->GetValue( currentVertexId ) ) );
                
                // make sure the date is valid
                if( newDate.IsSet() )
                {
                  // if we don't currently have a date, use the new one
                  if( !curDate.IsSet() )
                  {
                    birthArray->SetValue( currentVertexId, newDate.ToInt() );
                  }
                  // We already have a date, so make sure the new date is earlier and has as much
                  // information
                  else if( ( curDate > newDate ) &&
                           ( 0 == curDate.day || 0 < newDate.day ) &&
                           ( 0 == curDate.month || 0 < newDate.month ) )
                  {
                    birthArray->SetValue( currentVertexId, newDate.ToInt() );
                  }
                }
              }
              // ...or maybe a date inside a death tag?
              else if( inDeathTag &&
                       this->CurrentNode.IsOpeningElement() &&
                       ( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATE" ) ||
                         0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATESTRUCT" ) ||
                         0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATERANGE" ) ) )
              {
                ovDate newDate( ( char* )(
                  xmlTextReaderGetAttribute( this->Reader, 
                  0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DATERANGE" ) ?
                  BAD_CAST "TO" : BAD_CAST "VALUE" ) ) );
                ovDate curDate( deathArray->GetValue( currentVertexId ) );
  
                // make sure the date is valid
                if( newDate.IsSet() )
                {
                  // if we don't currently have a date, use the new one
                  if( !curDate.IsSet() )
                  {
                    deathArray->SetValue( currentVertexId, newDate.ToInt() );
                  }
                  // We already have a date, so make sure the new date is later and has as much
                  // information
                  else if( ( curDate < newDate ) &&
                           ( 0 == curDate.day || 0 < newDate.day ) &&
                           ( 0 == curDate.month || 0 < newDate.month ) )
                  {
                    deathArray->SetValue( currentVertexId, newDate.ToInt() );
                  }
                }
              }
              // ...or maybe a birth tag?
              else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "BIRTH" ) )
              {
                if( !( this->CurrentNode.IsEmptyElement ) && this->CurrentNode.IsOpeningElement() )
                {
                  inBirthTag = true;
                }
                else if( this->CurrentNode.IsClosingElement() )
                {
                  inBirthTag = false;
                }
              }
              // ...or maybe a death tag?
              else if( 0 == xmlStrcmp( this->CurrentNode.Name, BAD_CAST "DEATH" ) )
              {
                if( !( this->CurrentNode.IsEmptyElement ) && this->CurrentNode.IsOpeningElement() )
                {
                  inDeathTag = true;
                }
                else if( this->CurrentNode.IsClosingElement() )
                {
                  inDeathTag = false;
                }
              }
            }
          }
        }
      } // end while

      this->FreeReader();
    }
    catch( vtkstd::exception &e )
    {
      vtkErrorMacro( << e.what() );
      return 0;
    }
   
    vtkDebugMacro(<< "Read " 
      << graph->GetNumberOfVertices() 
      << " vertices and "
      << graph->GetNumberOfEdges() 
      << " edges.\n");
  
    // Copy graph into output.
    vtkGraph* const output =
      vtkGraph::SafeDownCast( outInfo->Get( vtkDataObject::DATA_OBJECT() ) );
    if( !output->CheckedShallowCopy( graph ) )
    {
      vtkErrorMacro( << "Invalid graph structure, returning empty graph." );
      return 0;
    }
  }

  return this->Superclass::ProcessRequest( request, inputVector, outputVector );
}


//----------------------------------------------------------------------------
int ovOrlandoReader::FillOutputPortInformation( int, vtkInformation* info )
{
  info->Set( vtkDataObject::DATA_TYPE_NAME(), "vtkGraph" );
  return 1;
}
