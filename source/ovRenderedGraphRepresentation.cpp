/*=========================================================================

  Program:  ovis ( OrlandoVision )
  Module:   ovRenderedGraphRepresentation.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovRenderedGraphRepresentation.h"

#include "ovApplyColors.h"

#include "vtkActor.h"
#include "vtkApplyIcons.h"
#include "vtkGraphToGlyphs.h"
#include "vtkGraphToPolyData.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkRemoveHiddenData.h"
#include "vtkSmartPointer.h"
#include "vtkVertexDegree.h"
#include "vtkViewTheme.h"

vtkStandardNewMacro( ovRenderedGraphRepresentation );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovRenderedGraphRepresentation::ovRenderedGraphRepresentation()
{
  this->ApplyColors = vtkSmartPointer< ovApplyColors >::New();

  // transfer over properties from old apply settings
  this->ApplyColors->SetInputConnection( this->VertexDegree->GetOutputPort() );
  this->ApplyVertexIcons->SetInputConnection( this->ApplyColors->GetOutputPort() );
  this->VertexGlyph->SetInputConnection( this->ApplyColors->GetOutputPort() );
  this->GraphToPoly->SetInputConnection( this->ApplyColors->GetOutputPort() );
  this->SetVertexColorArrayName( this->VertexColorArrayNameInternal );
  this->VertexMapper->SelectColorArray( "vtkApplyColors color" );
  this->EdgeMapper->SelectColorArray( "vtkApplyColors color" );

  vtkSmartPointer<vtkViewTheme> theme = vtkSmartPointer<vtkViewTheme>::New();
  this->ApplyViewTheme( theme );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovApplyColors* ovRenderedGraphRepresentation::GetApplyColors()
{
  return ovApplyColors::SafeDownCast( this->ApplyColors.GetPointer() );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovRenderedGraphRepresentation::ApplyViewTheme( vtkViewTheme* theme )
{
  this->Superclass::ApplyViewTheme( theme );
  this->GetApplyColors()->SetBackgroundColor( theme->GetBackgroundColor() );
}
