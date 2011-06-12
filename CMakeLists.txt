CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( ovis )

# Set the version
SET( OVIS_VERSION_MAJOR 0 )
SET( OVIS_VERSION_MINOR 1 )
SET( OVIS_VERSION_PATCH 0 )

# We need VTK
FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

# We need QT4
SET( QT_MIN_VERSION 4.0.0 )
FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE( ${QT_USE_FILE} )

# We need LibXML2
FIND_PACKAGE( LibXml2 REQUIRED )

# Configure the utitlities header
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/ovUtilities.h.in
                ${PROJECT_BINARY_DIR}/ovUtilities.h @ONLY IMMEDIATE )

# Set up headers and source
SET( OVIS_SOURCE
  ovis.cpp
  source/ovOrlandoReader.cpp
  source/ovOrlandoTagInfo.cpp
  source/ovRestrictGraphFilter.cpp
  source/ovSearchPhrase.cpp
  source/ovSession.cpp
  source/ovSessionReader.cpp
  source/ovSessionWriter.cpp
  source/ovXMLReader.cpp
  source/ovXMLWriter.cpp
  interface/ovQDateDialog.cpp
  interface/ovQDateSpanDialog.cpp
  interface/ovQMainWindow.cpp
  interface/ovQTextSearchDialog.cpp
  interface/ovQAuthorSearchDialog.cpp
)

SET_SOURCE_FILES_PROPERTIES(
  ovXMLReader.cxx
  ovXMLWriter.cxx
  ABSTRACT
)

SET( OVIS_UIS
  interface/ovQDateDialog.ui
  interface/ovQDateSpanDialog.ui
  interface/ovQMainWindow.ui
  interface/ovQTextSearchDialog.ui
  interface/ovQAuthorSearchDialog.ui
)

SET( OVIS_HEADERS
  interface/ovQDateDialog.h
  interface/ovQDateSpanDialog.h
  interface/ovQMainWindow.h
  interface/ovQTextSearchDialog.h
  interface/ovQAuthorSearchDialog.h
)

# Wrap QT files
QT4_WRAP_UI( UIHeaders ${OVIS_UIS} )
QT4_WRAP_CPP( MOCSrcs ${OVIS_HEADERS} )
ADD_DEFINITIONS( -DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT )
SET_SOURCE_FILES_PROPERTIES( ${OVIS_SOURCE} PROPERTIES OBJECT_DEPENDS "${UIHeaders}" )

# Use the include path and library for Qt
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_EXECUTABLE( ovis ${OVIS_SOURCE} ${OVIS_UISrcs} ${MOCSrcs} )

TARGET_LINK_LIBRARIES( ovis
  QVTK
  ${QT_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  vtkRendering
  vtkGraphics
  vtkIO
  vtkCommon
)
