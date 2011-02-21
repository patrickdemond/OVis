/*=========================================================================

  Program:  ovis (OrlandoVision)
  Module:   ovSession.cpp
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
#include "ovSession.h"

#include "ovOrlandoTagInfo.h"

#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include <vtkstd/algorithm>

vtkCxxRevisionMacro( ovSession, "$Revision: $" );
vtkStandardNewMacro( ovSession );
vtkCxxSetObjectMacro( ovSession, Camera, vtkCamera );

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession::ovSession()
{
  this->DataFile = "";
  for( int i = 0; i < 4; ++i )
  {
    this->BackgroundColor1[i] = 0;
    this->BackgroundColor2[i] = 0;
    this->AuthorVertexColor[i] = 0;
    this->AssociationVertexColor[i] = 0;
  }
  this->VertexStyle = 0;
  this->LayoutStrategy = "";
  this->AuthorsOnly = false;
  this->GenderTypeRestriction = 0;
  this->WriterTypeRestriction = 0;
  this->VertexSize = 0;
  this->EdgeSize = 0;
  this->Camera = vtkCamera::New();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession::~ovSession()
{
  vtkstd::for_each( this->TagList.begin(), this->TagList.end(), safe_delete() );
  this->TagList.clear();
  this->SetCamera( NULL );
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
void ovSession::SetStartDateRestriction( const ovDate &date )
{
  ovString str;
  date.ToString( str );
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "StartDateRestriction to " << str.c_str() );

  if( date != this->StartDateRestriction )
  {
    this->StartDateRestriction = date;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetEndDateRestriction( const ovDate &date )
{
  ovString str;
  date.ToString( str );
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "EndDateRestriction to " << str.c_str() );

  if( date != this->EndDateRestriction )
  {
    this->EndDateRestriction = date;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
bool ovSession::operator == ( const ovSession &rhs ) const
{
  if( this->DataFile == rhs.DataFile &&
      this->BackgroundColor1[0] == rhs.BackgroundColor1[0] &&
      this->BackgroundColor1[1] == rhs.BackgroundColor1[1] &&
      this->BackgroundColor1[2] == rhs.BackgroundColor1[2] &&
      this->BackgroundColor1[3] == rhs.BackgroundColor1[3] &&
      this->BackgroundColor2[0] == rhs.BackgroundColor2[0] &&
      this->BackgroundColor2[1] == rhs.BackgroundColor2[1] &&
      this->BackgroundColor2[2] == rhs.BackgroundColor2[2] &&
      this->BackgroundColor2[3] == rhs.BackgroundColor2[3] &&
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
      this->AuthorVertexColor[3] == rhs.AuthorVertexColor[3] &&
      this->AssociationVertexColor[0] == rhs.AssociationVertexColor[0] &&
      this->AssociationVertexColor[1] == rhs.AssociationVertexColor[1] &&
      this->AssociationVertexColor[2] == rhs.AssociationVertexColor[2] &&
      this->AssociationVertexColor[3] == rhs.AssociationVertexColor[3] &&
      this->StartDateRestriction == rhs.StartDateRestriction &&
      this->EndDateRestriction == rhs.EndDateRestriction &&
      this->Camera->GetPosition()[0] == rhs.Camera->GetPosition()[0] &&
      this->Camera->GetPosition()[1] == rhs.Camera->GetPosition()[1] &&
      this->Camera->GetPosition()[2] == rhs.Camera->GetPosition()[2] &&
      this->Camera->GetFocalPoint()[0] == rhs.Camera->GetFocalPoint()[0] &&
      this->Camera->GetFocalPoint()[1] == rhs.Camera->GetFocalPoint()[1] &&
      this->Camera->GetFocalPoint()[2] == rhs.Camera->GetFocalPoint()[2] &&
      this->Camera->GetViewUp()[0] == rhs.Camera->GetViewUp()[0] &&
      this->Camera->GetViewUp()[1] == rhs.Camera->GetViewUp()[1] &&
      this->Camera->GetViewUp()[2] == rhs.Camera->GetViewUp()[2] &&
      this->Camera->GetClippingRange()[0] == rhs.Camera->GetClippingRange()[0] &&
      this->Camera->GetClippingRange()[1] == rhs.Camera->GetClippingRange()[1] &&
      this->Camera->GetParallelScale() == rhs.Camera->GetParallelScale() )
  {
    // now make sure the tag vector is the same
    for( int i = 0; i < this->TagList.size() && i < rhs.TagList.size(); ++i )
      if( *( this->TagList[i] ) != *( rhs.TagList[i] ) ) return false;

    return true;
  }

  return false;
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::DeepCopy( ovSession *copy )
{
  this->DataFile = copy->DataFile;
  this->VertexStyle = copy->VertexStyle;
  this->LayoutStrategy = copy->LayoutStrategy;
  this->AuthorsOnly = copy->AuthorsOnly;
  this->GenderTypeRestriction = copy->GenderTypeRestriction;
  this->WriterTypeRestriction = copy->WriterTypeRestriction;
  this->VertexSize = copy->VertexSize;
  this->EdgeSize = copy->EdgeSize;
  this->StartDateRestriction = copy->StartDateRestriction;
  this->EndDateRestriction = copy->EndDateRestriction;

  for( int i = 0; i < 4; ++i )
  {
    this->BackgroundColor1[i] = copy->BackgroundColor1[i];
    this->BackgroundColor2[i] = copy->BackgroundColor2[i];
    this->AuthorVertexColor[i] = copy->AuthorVertexColor[i];
    this->AssociationVertexColor[i] = copy->AssociationVertexColor[i];
  }
  
  this->TagList.clear();
  for( ovTagVector::iterator it = copy->TagList.begin(); it != copy->TagList.end(); ++it )
  {
    ovTag *tag = new ovTag;
    tag->DeepCopy( *it );
    this->TagList.push_back( tag );
  }
  this->Camera->DeepCopy( copy->Camera );
}


//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::PrintSelf( ostream &os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );

  ovString date;
  os << indent << "DataFile = \"" << this->DataFile << "\"" << endl;
  os << indent << "BackgroundColor1[4] = " << this->BackgroundColor1[0] << ", "
               << this->BackgroundColor1[1] << ", "
               << this->BackgroundColor1[2] << ", "
               << this->BackgroundColor1[3] << endl;
  os << indent << "BackgroundColor2[4] = " << this->BackgroundColor2[0] << ", "
               << this->BackgroundColor2[1] << ", "
               << this->BackgroundColor2[2] << ", "
               << this->BackgroundColor2[3] << endl;
  os << indent << "VertexStyle = " << this->VertexStyle << "" << endl;
  os << indent << "LayoutStrategy = \"" << this->LayoutStrategy << "\"" << endl;
  os << indent << "AuthorsOnly = " << ( this->AuthorsOnly ? "true" : "false" ) << endl;
  os << indent << "GenderTypeRestriction = " << this->GenderTypeRestriction << "" << endl;
  os << indent << "WriterTypeRestriction = " << this->WriterTypeRestriction << "" << endl;
  os << indent << "VertexSize = " << this->VertexSize << "" << endl;
  os << indent << "EdgeSize = " << this->EdgeSize << "" << endl;
  os << indent << "AuthorVertexColor[4] = " << this->AuthorVertexColor[0] << ", "
               << this->AuthorVertexColor[1] << ", "
               << this->AuthorVertexColor[2] << ", "
               << this->AuthorVertexColor[3] << endl;
  os << indent << "AssociationVertexColor[4] = " << this->AssociationVertexColor[0] << ", "
               << this->AssociationVertexColor[1] << ", "
               << this->AssociationVertexColor[2] << ", "
               << this->AssociationVertexColor[3] << endl;
  this->StartDateRestriction.ToString( date );
  os << indent << "StartDateRestriction = " << date << endl;
  this->EndDateRestriction.ToString( date );
  os << indent << "EndDateRestriction = " << date << endl;
  os << indent << "TagList: ovTagVector";
  os << indent.GetNextIndent() << "size: " << this->TagList.size();
  os << indent << "Camera: ";
  if( NULL == this->Camera )
  {
    os << "NULL" << endl;
  }
  else
  {
    os << this->Camera << endl;
    this->Camera->PrintSelf( os, indent.GetNextIndent() );
  }
}
