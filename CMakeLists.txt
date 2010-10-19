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

SET( OVIS_SOURCE
  ovis.cpp
  source/ovDate.cpp
  source/ovEdge.cpp
  source/ovGraph.cpp
  source/ovName.cpp
  source/ovNode.cpp
  source/ovPeriod.cpp
  source/ovUserStyle.cpp
  source/ovUserStyleTags.cpp
  interface/ovQFont.cpp
  interface/ovQMainWindow.cpp
  interface/ovQPath.cpp
  interface/ovQScreenshot.cpp
  interface/ovQTags.cpp
  interface/ovQText.cpp
  interface/ovQWords.cpp
  interface/ovQWordHelp.cpp
)

SET( OVIS_UIS
  interface/ovQFont.ui
  interface/ovQMainWindow.ui
  interface/ovQPath.ui
  interface/ovQScreenshot.ui
  interface/ovQTags.ui
  interface/ovQText.ui
  interface/ovQWords.ui
  interface/ovQWordHelp.ui
)

SET( OVIS_HEADERS
  interface/ovQFont.h
  interface/ovQMainWindow.h
  interface/ovQPath.h
  interface/ovQScreenshot.h
  interface/ovQTags.h
  interface/ovQText.h
  interface/ovQWords.h
  interface/ovQWordHelp.h
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

# Make the resources directory and copy files
MAKE_DIRECTORY( ${CMAKE_CURRENT_BINARY_DIR}/tmp )
MAKE_DIRECTORY( ${CMAKE_CURRENT_BINARY_DIR}/resources )
FILE( GLOB resources RELATIVE
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/* )
FOREACH( resource ${resources} )
  CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/${resource}"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/${resource}" COPYONLY )
ENDFOREACH( resource )
