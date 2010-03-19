CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(test)

SET(VTK_DIR /usr/lib64/vtk-5.4)

FIND_PACKAGE(VTK REQUIRED)
IF(NOT VTK_DIR)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
INCLUDE(${VTK_USE_FILE})

# use what QVTK built with
SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
SET(DESIRED_QT_VERSION ${VTK_DESIRED_QT_VERSION} CACHE FILEPATH "")
FIND_PACKAGE(Qt)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)

SET (OrlandoSrcs graph.cpp node.cpp userStyle.cpp name.cpp userStyleTags.cpp edge.cpp date.cpp period.cpp)

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

IF(DESIRED_QT_VERSION MATCHES 4)

SET(OrlandoSrcs ${OrlandoSrcs} Orlando4.cpp Tags.cpp Path.cpp Words.cpp WordHelp.cpp Text.cpp Font.cpp)
SET(UIS uiOrlando4.ui tags.ui path.ui words.ui wordHelp.ui text.ui font.ui)
SET (OrlandoHeaders Orlando4.h Tags.h Path.h Words.h WordHelp.h Text.h Font.h)
QT4_WRAP_UI(UIHeaders ${UIS})
QT4_WRAP_CPP(MOCSrcs ${OrlandoHeaders} )
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
SET_SOURCE_FILES_PROPERTIES(${OrlandoSrcs} PROPERTIES OBJECT_DEPENDS "${UIHeaders}")

ENDIF(DESIRED_QT_VERSION MATCHES 4)

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)
IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (OrlandoSrcs ${OrlandoSrcs} ${UIS})
  SET (TagSrcs ${TagSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (OrlandoSrcs ${OrlandoSrcs} ${UIS})
  SET (TagSrcs ${TagSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")

ADD_EXECUTABLE(tst MACOSX_BUNDLE ${OrlandoSrcs} ${UISrcs} ${MOCSrcs})

SET_TARGET_PROPERTIES(tst PROPERTIES SKIP_BUILD_RPATH ON COMPILE_FLAGS -pg  LINK_FLAGS -pg)

#COMPILE_FLAGS -pg  LINK_FLAGS -pg

TARGET_LINK_LIBRARIES( tst
  QVTK
  ${QT_LIBRARIES}
  vtkRendering
  vtkGraphics
  vtkIO
  vtkCommon
)
