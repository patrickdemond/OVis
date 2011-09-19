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
    this->BackgroundColor[i] = 0;
    this->AuthorVertexColor[i] = 0;
    this->AssociationVertexColor[i] = 0;
  }
  this->CustomAnnotationText = "";
  this->ShowAnnotation = true;
  this->VertexStyle = 0;
  this->LayoutStrategy = "";
  this->IncludeWriters = true;
  this->IncludeOthers = true;
  this->IncludeFemale = true;
  this->IncludeMale = true;
  this->IncludeBRWType = true;
  this->IncludeWriterType = true;
  this->IncludeIBRType = true;
  this->VertexSize = 0;
  this->EdgeSize = 0;
  this->FadingFactor = 0;
  this->TextSearchPhrase = "";
  this->TextSearchNarrow = false;
  this->AuthorSearchPhrase = "";
  this->Camera = vtkCamera::New();
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
ovSession::~ovSession()
{
  this->SelectedVertexList.clear();
  this->SelectedEdgeList.clear();
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
void ovSession::SetCustomAnnotationText( const ovString &text )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "CustomAnnotationText to " << text.c_str() );

  if( text != this->CustomAnnotationText )
  {
    this->CustomAnnotationText = text;
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
void ovSession::SetTextSearchPhrase( const ovString &phrase )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "TextSearchPhrase to " << phrase.c_str() );

  if( phrase != this->TextSearchPhrase )
  {
    this->TextSearchPhrase = phrase;
    this->Modified();
  }
}

//-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
void ovSession::SetAuthorSearchPhrase( const ovString &phrase )
{
  vtkDebugMacro( << this->GetClassName() << " (" << this << "): setting "
                 << "AuthorSearchPhrase to " << phrase.c_str() );

  if( phrase != this->AuthorSearchPhrase )
  {
    this->AuthorSearchPhrase = phrase;
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
      this->BackgroundColor[0] == rhs.BackgroundColor[0] &&
      this->BackgroundColor[1] == rhs.BackgroundColor[1] &&
      this->BackgroundColor[2] == rhs.BackgroundColor[2] &&
      this->BackgroundColor[3] == rhs.BackgroundColor[3] &&
      this->CustomAnnotationText == rhs.CustomAnnotationText &&
      this->ShowAnnotation == rhs.ShowAnnotation &&
      this->VertexStyle == rhs.VertexStyle &&
      this->LayoutStrategy == rhs.LayoutStrategy &&
      this->IncludeWriters == rhs.IncludeWriters &&
      this->IncludeOthers == rhs.IncludeOthers &&
      this->IncludeFemale == rhs.IncludeFemale &&
      this->IncludeMale == rhs.IncludeMale &&
      this->IncludeBRWType == rhs.IncludeBRWType &&
      this->IncludeWriterType == rhs.IncludeWriterType &&
      this->IncludeIBRType == rhs.IncludeIBRType &&
      this->VertexSize == rhs.VertexSize &&
      this->EdgeSize == rhs.EdgeSize &&
      this->FadingFactor == rhs.FadingFactor &&
      this->AuthorVertexColor[0] == rhs.AuthorVertexColor[0] &&
      this->AuthorVertexColor[1] == rhs.AuthorVertexColor[1] &&
      this->AuthorVertexColor[2] == rhs.AuthorVertexColor[2] &&
      this->AuthorVertexColor[3] == rhs.AuthorVertexColor[3] &&
      this->AssociationVertexColor[0] == rhs.AssociationVertexColor[0] &&
      this->AssociationVertexColor[1] == rhs.AssociationVertexColor[1] &&
      this->AssociationVertexColor[2] == rhs.AssociationVertexColor[2] &&
      this->AssociationVertexColor[3] == rhs.AssociationVertexColor[3] &&
      this->TextSearchPhrase == rhs.TextSearchPhrase &&
      this->TextSearchNarrow == rhs.TextSearchNarrow &&
      this->AuthorSearchPhrase == rhs.AuthorSearchPhrase &&
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
    // now make sure the vertex, edge and tag vectors are the same
    for( int i = 0; i < this->SelectedVertexList.size() && i < rhs.SelectedVertexList.size(); ++i )
      if( this->SelectedVertexList[i] != rhs.SelectedVertexList[i] ) return false;
    for( int i = 0; i < this->SelectedEdgeList.size() && i < rhs.SelectedEdgeList.size(); ++i )
      if( this->SelectedEdgeList[i] != rhs.SelectedEdgeList[i] ) return false;
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
  this->CustomAnnotationText = copy->CustomAnnotationText;
  this->ShowAnnotation = copy->ShowAnnotation;
  this->VertexStyle = copy->VertexStyle;
  this->LayoutStrategy = copy->LayoutStrategy;
  this->IncludeWriters = copy->IncludeWriters;
  this->IncludeOthers = copy->IncludeOthers;
  this->IncludeFemale = copy->IncludeFemale;
  this->IncludeMale = copy->IncludeMale;
  this->IncludeBRWType = copy->IncludeBRWType;
  this->IncludeWriterType = copy->IncludeWriterType;
  this->IncludeIBRType = copy->IncludeIBRType;
  this->VertexSize = copy->VertexSize;
  this->EdgeSize = copy->EdgeSize;
  this->FadingFactor = copy->FadingFactor;
  this->TextSearchPhrase = copy->TextSearchPhrase;
  this->TextSearchNarrow = copy->TextSearchNarrow;
  this->AuthorSearchPhrase = copy->AuthorSearchPhrase;
  this->StartDateRestriction = copy->StartDateRestriction;
  this->EndDateRestriction = copy->EndDateRestriction;

  for( int i = 0; i < 4; ++i )
  {
    this->BackgroundColor[i] = copy->BackgroundColor[i];
    this->AuthorVertexColor[i] = copy->AuthorVertexColor[i];
    this->AssociationVertexColor[i] = copy->AssociationVertexColor[i];
  }
  
  this->SelectedVertexList = copy->SelectedVertexList;
  this->SelectedEdgeList = copy->SelectedEdgeList;
  
  this->TagList.clear();
  for( ovTagVector::iterator it = copy->TagList.begin();
       it != copy->TagList.end();
       ++it )
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
  os << indent << "BackgroundColor[4] = " << this->BackgroundColor[0] << ", "
               << this->BackgroundColor[1] << ", "
               << this->BackgroundColor[2] << ", "
               << this->BackgroundColor[3] << endl;
  os << indent << "CustomAnnotationText = \"" << this->CustomAnnotationText << "\"" << endl;
  os << indent << "ShowAnnotation = " << ( this->ShowAnnotation ? "true" : "false" ) << endl;
  os << indent << "VertexStyle = " << this->VertexStyle << "" << endl;
  os << indent << "LayoutStrategy = \"" << this->LayoutStrategy << "\"" << endl;
  os << indent << "IncludeWriters = " << ( this->IncludeWriters ? "true" : "false" ) << endl;
  os << indent << "IncludeOthers = " << ( this->IncludeOthers ? "true" : "false" ) << endl;
  os << indent << "IncludeFemale = " << ( this->IncludeFemale ? "true" : "false" ) << endl;
  os << indent << "IncludeMale = " << ( this->IncludeMale ? "true" : "false" ) << endl;
  os << indent << "IncludeBRWType = " << ( this->IncludeBRWType ? "true" : "false" ) << endl;
  os << indent << "IncludeWriterType = " << ( this->IncludeWriterType ? "true" : "false" ) << endl;
  os << indent << "IncludeIBRType = " << ( this->IncludeIBRType ? "true" : "false" ) << endl;
  os << indent << "VertexSize = " << this->VertexSize << "" << endl;
  os << indent << "EdgeSize = " << this->EdgeSize << "" << endl;
  os << indent << "FadingFactor = " << this->FadingFactor << "" << endl;
  os << indent << "AuthorVertexColor[4] = " << this->AuthorVertexColor[0] << ", "
               << this->AuthorVertexColor[1] << ", "
               << this->AuthorVertexColor[2] << ", "
               << this->AuthorVertexColor[3] << endl;
  os << indent << "AssociationVertexColor[4] = " << this->AssociationVertexColor[0] << ", "
               << this->AssociationVertexColor[1] << ", "
               << this->AssociationVertexColor[2] << ", "
               << this->AssociationVertexColor[3] << endl;
  os << indent << "TextSearchPhrase = \"" << this->TextSearchPhrase << "\"" << endl;
  os << indent << "TextSearchNarrow = " << ( this->TextSearchNarrow ? "true" : "false" ) << endl;
  os << indent << "AuthorSearchPhrase = \"" << this->AuthorSearchPhrase << "\"" << endl;
  this->StartDateRestriction.ToString( date );
  os << indent << "StartDateRestriction = " << date << endl;
  this->EndDateRestriction.ToString( date );
  os << indent << "EndDateRestriction = " << date << endl;
  os << indent << "SelectedVertexList: ovSelectedVertexVector" << endl;
  os << indent.GetNextIndent() << "  size: " << this->SelectedVertexList.size() << endl;
  os << indent << "SelectedEdgeList: ovSelectedEdgeVector" << endl;
  os << indent.GetNextIndent() << "  size: " << this->SelectedEdgeList.size() << endl;
  os << indent << "TagList: ovTagVector" << endl;
  os << indent.GetNextIndent() << "  size: " << this->TagList.size() << endl;
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
