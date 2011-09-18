/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovApplyColors.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovApplyColors.h"

#include "vtkAnnotation.h"
#include "vtkAnnotationLayers.h"
#include "vtkCellData.h"
#include "vtkConvertSelection.h"
#include "vtkGraph.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkScalarsToColors.h"
#include "vtkSmartPointer.h"
#include "vtkTable.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro( ovApplyColors );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovApplyColors::ovApplyColors()
{
  this->FadingFactor = 0.5;
  this->BackgroundColor[0] = 0.0;
  this->BackgroundColor[1] = 0.0;
  this->BackgroundColor[2] = 0.0;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
int ovApplyColors::RequestData(
  vtkInformation *vtkNotUsed( request ),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector )
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject( 0 );
  vtkInformation* layersInfo = inputVector[1]->GetInformationObject( 0 );
  vtkInformation* outInfo = outputVector->GetInformationObject( 0 );

  if( !this->PointColorOutputArrayName || !this->CellColorOutputArrayName )
  {
    vtkErrorMacro( "Point and cell array names must be valid" );
    return 0;
  }

  // get the input and output
  vtkDataObject* input = inInfo->Get( vtkDataObject::DATA_OBJECT() );
  vtkAnnotationLayers* layers = 0;
  if( layersInfo )
  {
    layers = vtkAnnotationLayers::SafeDownCast(
      layersInfo->Get( vtkDataObject::DATA_OBJECT() ) );
  }
  vtkDataObject* output = outInfo->Get( vtkDataObject::DATA_OBJECT() );

  output->ShallowCopy( input );

  vtkGraph* graph = vtkGraph::SafeDownCast( output );
  vtkDataSet* dataSet = vtkDataSet::SafeDownCast( output );
  vtkTable* table = vtkTable::SafeDownCast( output );

  // initialize color arrays
  vtkSmartPointer<vtkUnsignedCharArray> colorArr1 =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colorArr1->SetName( this->PointColorOutputArrayName );
  colorArr1->SetNumberOfComponents( 4 );
  if( graph )
  {
    colorArr1->SetNumberOfTuples( graph->GetNumberOfVertices() );
    graph->GetVertexData()->AddArray( colorArr1 );
  }
  else if( dataSet )
  {
    colorArr1->SetNumberOfTuples( dataSet->GetNumberOfPoints() );
    dataSet->GetPointData()->AddArray( colorArr1 );
  }
  else
  {
    colorArr1->SetNumberOfTuples( table->GetNumberOfRows() );
    table->AddColumn( colorArr1 );
  }
  vtkSmartPointer<vtkUnsignedCharArray> colorArr2 =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colorArr2->SetName( this->CellColorOutputArrayName );
  colorArr2->SetNumberOfComponents( 4 );
  if( graph )
  {
    colorArr2->SetNumberOfTuples( graph->GetNumberOfEdges() );
    graph->GetEdgeData()->AddArray( colorArr2 );
  }
  else if( dataSet )
  {
    colorArr2->SetNumberOfTuples( dataSet->GetNumberOfCells() );
    dataSet->GetCellData()->AddArray( colorArr2 );
  }

  unsigned char pointColor[4];
  pointColor[0] = static_cast<unsigned char>( 255*this->DefaultPointColor[0] );
  pointColor[1] = static_cast<unsigned char>( 255*this->DefaultPointColor[1] );
  pointColor[2] = static_cast<unsigned char>( 255*this->DefaultPointColor[2] );
  pointColor[3] = static_cast<unsigned char>( 255*this->DefaultPointOpacity );
  vtkAbstractArray* arr1 = 0;
  if( this->PointLookupTable && this->UsePointLookupTable )
  {
    arr1 = this->GetInputAbstractArrayToProcess( 0, inputVector );
  }
  this->ProcessColorArray( colorArr1, this->PointLookupTable, arr1,
    pointColor, this->ScalePointLookupTable );

  unsigned char cellColor[4];
  cellColor[0] = static_cast<unsigned char>( 255*this->DefaultCellColor[0] );
  cellColor[1] = static_cast<unsigned char>( 255*this->DefaultCellColor[1] );
  cellColor[2] = static_cast<unsigned char>( 255*this->DefaultCellColor[2] );
  cellColor[3] = static_cast<unsigned char>( 255*this->DefaultCellOpacity );
  vtkAbstractArray* arr2 = 0;
  if( this->CellLookupTable && this->UseCellLookupTable )
  {
    arr2 = this->GetInputAbstractArrayToProcess( 1, inputVector );
  }
  this->ProcessColorArray( colorArr2, this->CellLookupTable, arr2,
    cellColor, this->ScaleCellLookupTable );

  if( layers )
  {
    vtkSmartPointer<vtkIdTypeArray> list1 =
      vtkSmartPointer<vtkIdTypeArray>::New();
    vtkSmartPointer<vtkIdTypeArray> list2 =
      vtkSmartPointer<vtkIdTypeArray>::New();
    unsigned char annColor[4] = {0, 0, 0, 0};
    unsigned char prev[4] = {0, 0, 0, 0};
    unsigned int numAnnotations = layers->GetNumberOfAnnotations();
    for( unsigned int a = 0; a < numAnnotations; ++a )
    {
      vtkAnnotation* ann = layers->GetAnnotation( a );
      if( ann->GetInformation()->Has( vtkAnnotation::ENABLE() ) && 
          ann->GetInformation()->Get( vtkAnnotation::ENABLE() )==0 )
      {
        continue;
      }
      list1->Initialize();
      list2->Initialize();
      vtkSelection* sel = ann->GetSelection();
      bool hasColor = false;
      bool hasOpacity = false;
      if( ann->GetInformation()->Has( vtkAnnotation::COLOR() ) )
      {
        hasColor = true;
        double* color = ann->GetInformation()->Get( vtkAnnotation::COLOR() );
        annColor[0] = static_cast<unsigned char>( 255*color[0] );
        annColor[1] = static_cast<unsigned char>( 255*color[1] );
        annColor[2] = static_cast<unsigned char>( 255*color[2] );
      }
      if( ann->GetInformation()->Has( vtkAnnotation::OPACITY() ) )
      {
        hasOpacity = true;
        double opacity = ann->GetInformation()->Get( vtkAnnotation::OPACITY() );
        annColor[3] = static_cast<unsigned char>( 255*opacity );
      }
      if( !hasColor && !hasOpacity )
      {
        continue;
      }
      if( graph )
      {
        vtkConvertSelection::GetSelectedVertices( sel, graph, list1 );
        vtkConvertSelection::GetSelectedEdges( sel, graph, list2 );
      }
      else if( dataSet )
      {
        vtkConvertSelection::GetSelectedPoints( sel, dataSet, list1 );
        vtkConvertSelection::GetSelectedCells( sel, dataSet, list2 );
      }
      else
      {
        vtkConvertSelection::GetSelectedRows( sel, table, list1 );
      }
      vtkIdType listIds = list1->GetNumberOfTuples();
      unsigned char curColor[4];
      for( vtkIdType i = 0; i < listIds; ++i )
      {
        if( list1->GetValue( i ) >= colorArr1->GetNumberOfTuples() )
        {
          continue;
        }
        colorArr1->GetTupleValue( list1->GetValue( i ), prev );
        if( hasColor )
        {
          curColor[0] = annColor[0];
          curColor[1] = annColor[1];
          curColor[2] = annColor[2];
        }
        else
        {
          curColor[0] = prev[0];
          curColor[1] = prev[1];
          curColor[2] = prev[2];
        }
        if( hasOpacity )
        {
          // Combine opacities
          curColor[3] = static_cast<unsigned char>( ( prev[3]/255.0 )*annColor[3] );
        }
        else
        {
          curColor[3] = prev[3];
        }
        colorArr1->SetTupleValue( list1->GetValue( i ), curColor );
      }
      listIds = list2->GetNumberOfTuples();
      for( vtkIdType i = 0; i < listIds; ++i )
      {
        if( list2->GetValue( i ) >= colorArr2->GetNumberOfTuples() )
        {
          continue;
        }
        colorArr2->GetTupleValue( list2->GetValue( i ), prev );
        if( hasColor )
        {
          curColor[0] = annColor[0];
          curColor[1] = annColor[1];
          curColor[2] = annColor[2];
        }
        else
        {
          curColor[0] = prev[0];
          curColor[1] = prev[1];
          curColor[2] = prev[2];
        }
        if( hasOpacity )
        {
          // Combine opacities
          curColor[3] = static_cast<unsigned char>( ( prev[3]/255.0 )*annColor[3] );
        }
        else
        {
          curColor[3] = prev[3];
        }
        colorArr2->SetTupleValue( list2->GetValue( i ), curColor );
      }
    }
    if( vtkAnnotation* ann = layers->GetCurrentAnnotation() )
    {
      vtkSelection* selection = ann->GetSelection();
      list1 = vtkSmartPointer<vtkIdTypeArray>::New();
      list2 = vtkSmartPointer<vtkIdTypeArray>::New();
      unsigned char color1[4] = {0, 0, 0, 255};
      unsigned char color2[4] = {0, 0, 0, 255};
      if( this->UseCurrentAnnotationColor )
      {
        if( ann->GetInformation()->Has( vtkAnnotation::COLOR() ) )
        {
          double* color = ann->GetInformation()->Get( vtkAnnotation::COLOR() );
          color1[0] = static_cast<unsigned char>( 255*color[0] );
          color1[1] = static_cast<unsigned char>( 255*color[1] );
          color1[2] = static_cast<unsigned char>( 255*color[2] );
        }
        if( ann->GetInformation()->Has( vtkAnnotation::OPACITY() ) )
        {
          double opacity = ann->GetInformation()->Get( vtkAnnotation::OPACITY() );
          color1[3] = static_cast<unsigned char>( 255*opacity );
        }
        for( int c = 0; c < 4; ++c )
        {
          color2[c] = color1[c];
        }
      }
      else
      {
        color1[0] = static_cast<unsigned char>( 255*this->SelectedPointColor[0] );
        color1[1] = static_cast<unsigned char>( 255*this->SelectedPointColor[1] );
        color1[2] = static_cast<unsigned char>( 255*this->SelectedPointColor[2] );
        color1[3] = static_cast<unsigned char>( 255*this->SelectedPointOpacity );
        color2[0] = static_cast<unsigned char>( 255*this->SelectedCellColor[0] );
        color2[1] = static_cast<unsigned char>( 255*this->SelectedCellColor[1] );
        color2[2] = static_cast<unsigned char>( 255*this->SelectedCellColor[2] );
        color2[3] = static_cast<unsigned char>( 255*this->SelectedCellOpacity );
      }
      if( graph )
      {
        vtkConvertSelection::GetSelectedVertices( selection, graph, list1 );
        vtkConvertSelection::GetSelectedEdges( selection, graph, list2 );
      }
      else if( dataSet )
      {
        vtkConvertSelection::GetSelectedPoints( selection, dataSet, list1 );
        vtkConvertSelection::GetSelectedCells( selection, dataSet, list2 );
      }
      else
      {
        vtkConvertSelection::GetSelectedRows( selection, table, list1 );
      }

      vtkIdType colorIds, listIds;
      double colorFactor = 1 - this->FadingFactor;
      double bgFactor = this->FadingFactor;
      if( 2 != selection->GetNumberOfNodes() )
      {
        listIds = list1->GetNumberOfTuples();
        for( vtkIdType listIdx = 0; listIdx < listIds; ++listIdx )
        {
          if( list1->GetValue( listIdx ) >= colorArr1->GetNumberOfTuples() )
          {
            continue;
          }
          colorArr1->SetTupleValue( list1->GetValue( listIdx ), color1 );
        }
        listIds = list2->GetNumberOfTuples();
        for( vtkIdType listIdx = 0; listIdx < listIds; ++listIdx )
        {
          if( list2->GetValue( listIdx ) >= colorArr2->GetNumberOfTuples() )
          {
            continue;
          }
          colorArr2->SetTupleValue( list2->GetValue( listIdx ), color2 );
        }
      }
      else
      {
        if( 0 < list1->GetNumberOfTuples() || 0 < list2->GetNumberOfTuples() )
        {
          colorIds = colorArr1->GetNumberOfTuples();
          listIds = list1->GetNumberOfTuples();
          for( vtkIdType colorIdx = 0; colorIdx < colorIds; ++colorIdx )
          {
            bool found = false;
            for (vtkIdType listIdx = 0; listIdx < listIds; ++listIdx)
            {
              found = list1->GetValue( listIdx ) == colorIdx;
              if( found ) break;
            }
            if( !found )
            {
              unsigned char c[4];
              colorArr1->GetTupleValue( colorIdx, c );
              c[0] = colorFactor * c[0] + bgFactor * this->BackgroundColor[0]*255;
              c[1] = colorFactor * c[1] + bgFactor * this->BackgroundColor[1]*255;
              c[2] = colorFactor * c[2] + bgFactor * this->BackgroundColor[2]*255;
              colorArr1->SetTupleValue( colorIdx, c );
            }
          }
    
          colorIds = colorArr2->GetNumberOfTuples();
          listIds = list2->GetNumberOfTuples();
          for( vtkIdType colorIdx = 0; colorIdx < colorIds; ++colorIdx )
          {
            bool found = false;
            for (vtkIdType listIdx = 0; listIdx < listIds; ++listIdx)
            {
              found = list2->GetValue( listIdx ) == colorIdx;
              if( found ) break;
            }
            if( !found )
            {
              unsigned char c[4];
              colorArr2->GetTupleValue( colorIdx, c );
              c[0] = colorFactor * c[0] + bgFactor * this->BackgroundColor[0]*255;
              c[1] = colorFactor * c[1] + bgFactor * this->BackgroundColor[1]*255;
              c[2] = colorFactor * c[2] + bgFactor * this->BackgroundColor[2]*255;
              colorArr2->SetTupleValue( colorIdx, c );
            }
          }
        }
      }
    }
  } // end if( layers )

  return 1;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovApplyColors::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os,indent );
  os << indent << "FadingFactor: " << this->FadingFactor << endl;
}
