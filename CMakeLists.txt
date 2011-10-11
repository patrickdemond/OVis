CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( ovis )

# Set the version
SET( OVIS_VERSION_MAJOR 0 )
SET( OVIS_VERSION_MINOR 1 )
SET( OVIS_VERSION_PATCH 0+git20111008 )

# Set archive name
SET( OVIS_ARCHIVE_NAME ovis-${OVIS_VERSION_MAJOR}.${OVIS_VERSION_MINOR}.${OVIS_VERSION_PATCH} )

# Icons sizes to build
SET( OVIS_ICON_SIZES 16x16 22x22 24x24 32x32 48x48 64x64 128x128 )

# We need VTK
FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

# We need QT4
SET( QT_MIN_VERSION 4.0.0 )
FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE( ${QT_USE_FILE} )

# We need LibXML2
FIND_PACKAGE( LibXml2 REQUIRED )

# We need convert
IF( UNIX AND NOT APPLE )
  FIND_PACKAGE( ImageMagick COMPONENTS convert REQUIRED )
ENDIF( UNIX AND NOT APPLE )

# Configure the utitlities header
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/ovUtilities.h.in
                ${PROJECT_BINARY_DIR}/ovUtilities.h @ONLY IMMEDIATE )

# Copy the readme file to build
CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/readme.md
                ${PROJECT_BINARY_DIR}/readme.md COPYONLY )

# Set up headers and source
SET( OVIS_SOURCE
  ovis.cpp
  source/ovApplyColors.cpp
  source/ovGraphLayoutView.cpp
  source/ovOrlandoReader.cpp
  source/ovOrlandoTagInfo.cpp
  source/ovRenderedGraphRepresentation.cpp
  source/ovRestrictGraphFilter.cpp
  source/ovSearchPhrase.cpp
  source/ovSession.cpp
  source/ovSessionReader.cpp
  source/ovSessionWriter.cpp
  source/ovXMLReader.cpp
  source/ovXMLWriter.cpp
  interface/ovQAboutDialog.cpp
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
  interface/ovQAboutDialog.ui
  interface/ovQDateSpanDialog.ui
  interface/ovQMainWindow.ui
  interface/ovQTextSearchDialog.ui
  interface/ovQAuthorSearchDialog.ui
)

SET( OVIS_HEADERS
  interface/ovQAboutDialog.h
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

# Targets
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
INSTALL( TARGETS ovis RUNTIME DESTINATION bin )

ADD_CUSTOM_TARGET( dist
  COMMAND git archive --prefix=${OVIS_ARCHIVE_NAME}/ HEAD
    | bzip2 > ${CMAKE_BINARY_DIR}/${OVIS_ARCHIVE_NAME}.tar.bz2
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Desktop entries and icons
IF( UNIX AND NOT APPLE )
  INSTALL( FILES desktop/ovis.desktop DESTINATION share/applications )

  INSTALL( FILES desktop/orlando-tree.svg DESTINATION share/icons/hicolor/scalable/apps )
  FOREACH( SIZE ${OVIS_ICON_SIZES} )
    ADD_CUSTOM_COMMAND( OUTPUT orlando-tree-${SIZE}.png
      COMMAND convert -background none ${CMAKE_SOURCE_DIR}/desktop/orlando-tree.svg[${SIZE}] orlando-tree-${SIZE}.png
      COMMENT "Generating ${SIZE} icon"
    )
    SET( OVIS_ICON_FILES ${OVIS_ICON_FILES} orlando-tree-${SIZE}.png )
    INSTALL( FILES ${CMAKE_BINARY_DIR}/orlando-tree-${SIZE}.png
      DESTINATION share/icons/hicolor/${SIZE}/apps
      RENAME orlando-tree.png
    )
  ENDFOREACH( SIZE )
  ADD_CUSTOM_TARGET( icons ALL DEPENDS ${OVIS_ICON_FILES} )
ENDIF( UNIX AND NOT APPLE )
