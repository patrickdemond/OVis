/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSession.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSession.h"

#include "ovOrlandoTagInfo.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro( ovSession, "$Revision: $" );
vtkStandardNewMacro( ovSession );
vtkCxxSetObjectMacro( ovSession, ActiveTags, vtkStringArray );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession::ovSession()
{
  this->DataFile = "";
  for( int i = 0; i < 3; ++i ) this->BackgroundColor1[i] = 0;
  for( int i = 0; i < 3; ++i ) this->BackgroundColor2[i] = 0;
  this->VertexStyle = 0;
  this->LayoutStrategy = "";
  this->AuthorsOnly = false;
  this->GenderTypeRestriction = 0;
  this->WriterTypeRestriction = 0;
  this->VertexSize = 0;
  this->EdgeSize = 0;
  for( int i = 0; i < 3; ++i ) this->AuthorVertexColor[i] = 0;
  for( int i = 0; i < 3; ++i ) this->AssociationVertexColor[i] = 0;
  this->ActiveTags = NULL;
  for( int i = 0; i < 3; ++i ) this->CameraPosition[i] = 0;
  for( int i = 0; i < 3; ++i ) this->CameraFocalPoint[i] = 0;
  for( int i = 0; i < 3; ++i ) this->CameraViewUp[i] = 0;
  for( int i = 0; i < 2; ++i ) this->CameraClippingRange[i] = 0;
  this->CameraParallelScale = 0;
  for( int i = 0; i < 3; ++i ) this->CameraComputeViewPlaneNormal[i] = 0;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession::~ovSession()
{
  this->SetActiveTags( NULL );
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetDataFile( const ovString &dataFile )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "DataFile to " << dataFile.c_str() );

  if( dataFile != this->DataFile )
  {
    this->DataFile = dataFile;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetLayoutStrategy( const ovString &strategy )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "LayoutStrategy to " << strategy.c_str() );

  if( strategy != this->LayoutStrategy )
  {
    this->LayoutStrategy = strategy;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetStartDate( const ovDate &date )
{
  ovString str;
  date.ToString( str );
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "StartDate to " << str.c_str() );

  if( date != this->StartDate )
  {
    this->StartDate = date;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetEndDate( const ovDate &date )
{
  ovString str;
  date.ToString( str );
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "EndDate to " << str.c_str() );

  if( date != this->EndDate )
  {
    this->EndDate = date;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
bool ovSession::operator == ( const ovSession& rhs ) const
{
  return this->DataFile == rhs.DataFile &&
         this->BackgroundColor1[0] == rhs.BackgroundColor1[0] &&
         this->BackgroundColor1[1] == rhs.BackgroundColor1[1] &&
         this->BackgroundColor1[2] == rhs.BackgroundColor1[2] &&
         this->BackgroundColor2[0] == rhs.BackgroundColor2[0] &&
         this->BackgroundColor2[1] == rhs.BackgroundColor2[1] &&
         this->BackgroundColor2[2] == rhs.BackgroundColor2[2] &&
         this->VertexStyle == rhs.VertexStyle &&
         this->LayoutStrategy == rhs.LayoutStrategy &&
         this->AuthorsOnly == rhs.AuthorsOnly &&
         this->GenderTypeRestriction == rhs.GenderTypeRestriction &&
         this->WriterTypeRestriction == rhs.WriterTypeRestriction &&
         this->VertexSize == rhs.VertexSize &&
         this->EdgeSize == rhs.EdgeSize &&
         this->AuthorVertexColor[0] == rhs.AuthorVertexColor[0] &&
         this->AuthorVertexColor[1] == rhs.AuthorVertexColor[1] &&
         this->AuthorVertexColor[2] == rhs.AuthorVertexColor[2] &&
         this->AssociationVertexColor[0] == rhs.AssociationVertexColor[0] &&
         this->AssociationVertexColor[1] == rhs.AssociationVertexColor[1] &&
         this->AssociationVertexColor[2] == rhs.AssociationVertexColor[2] &&
         this->StartDate == rhs.StartDate &&
         this->EndDate == rhs.EndDate &&
         this->ActiveTags == rhs.ActiveTags &&
         this->CameraPosition[0] == rhs.CameraPosition[0] &&
         this->CameraPosition[1] == rhs.CameraPosition[1] &&
         this->CameraPosition[2] == rhs.CameraPosition[2] &&
         this->CameraFocalPoint[0] == rhs.CameraFocalPoint[0] &&
         this->CameraFocalPoint[1] == rhs.CameraFocalPoint[1] &&
         this->CameraFocalPoint[2] == rhs.CameraFocalPoint[2] &&
         this->CameraViewUp[0] == rhs.CameraViewUp[0] &&
         this->CameraViewUp[1] == rhs.CameraViewUp[1] &&
         this->CameraViewUp[2] == rhs.CameraViewUp[2] &&
         this->CameraClippingRange[0] == rhs.CameraClippingRange[0] &&
         this->CameraClippingRange[1] == rhs.CameraClippingRange[1] &&
         this->CameraParallelScale == rhs.CameraParallelScale &&
         this->CameraComputeViewPlaneNormal[0] == rhs.CameraComputeViewPlaneNormal[0] &&
         this->CameraComputeViewPlaneNormal[1] == rhs.CameraComputeViewPlaneNormal[1] &&
         this->CameraComputeViewPlaneNormal[2] == rhs.CameraComputeViewPlaneNormal[2];
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  ovString date;
  os << indent << "DataFile = \"" << this->DataFile << "\"" << endl;
  os << indent << "BackgroundColor1[3] = " << this->BackgroundColor1[0] << ", "
               << this->BackgroundColor1[1] << ", "
               << this->BackgroundColor1[2] << endl;
  os << indent << "BackgroundColor2[3] = " << this->BackgroundColor2[0] << ", "
               << this->BackgroundColor2[1] << ", "
               << this->BackgroundColor2[2] << endl;
  os << indent << "VertexStyle = " << this->VertexStyle << "" << endl;
  os << indent << "LayoutStrategy = \"" << this->LayoutStrategy << "\"" << endl;
  os << indent << "AuthorsOnly = " << ( this->AuthorsOnly ? "true" : "false" ) << endl;
  os << indent << "GenderTypeRestriction = " << this->GenderTypeRestriction << "" << endl;
  os << indent << "WriterTypeRestriction = " << this->WriterTypeRestriction << "" << endl;
  os << indent << "VertexSize = " << this->VertexSize << "" << endl;
  os << indent << "EdgeSize = " << this->EdgeSize << "" << endl;
  os << indent << "AuthorVertexColor[3] = " << this->AuthorVertexColor[0] << ", "
               << this->AuthorVertexColor[1] << ", "
               << this->AuthorVertexColor[2] << endl;
  os << indent << "AssociationVertexColor[3] = " << this->AssociationVertexColor[0] << ", "
               << this->AssociationVertexColor[1] << ", "
               << this->AssociationVertexColor[2] << endl;
  this->StartDate.ToString( date );
  os << indent << "StartDate = " << date << endl;
  this->EndDate.ToString( date );
  os << indent << "EndDate = " << date << endl;
  os << indent << "ActiveTags: ";
  if( NULL == this->ActiveTags )
  {
    cout << "NULL" << endl;
  }
  else
  {
    cout << this->ActiveTags << endl;
    this->ActiveTags->PrintSelf( os, indent.GetNextIndent() );
  }
  os << indent << "CameraPosition[3] = " << this->CameraPosition[0] << ", "
               << this->CameraPosition[1] << ", "
               << this->CameraPosition[2] << endl;
  os << indent << "CameraFocalPoint[3] = " << this->CameraFocalPoint[0] << ", "
               << this->CameraFocalPoint[1] << ", "
               << this->CameraFocalPoint[2] << endl;
  os << indent << "CameraViewUp[3] = " << this->CameraViewUp[0] << ", "
               << this->CameraViewUp[1] << ", "
               << this->CameraViewUp[2] << endl;
  os << indent << "CameraClippingRange[2] = " << this->CameraClippingRange[0] << ", "
               << this->CameraClippingRange[1] << endl;
  os << indent << "CameraParallelScale = " << this->CameraParallelScale << endl;
  os << indent << "CameraComputeViewPlaneNormal[3] = " << this->CameraComputeViewPlaneNormal[0] << ", "
               << this->CameraComputeViewPlaneNormal[1] << ", "
               << this->CameraComputeViewPlaneNormal[2] << endl;
}
