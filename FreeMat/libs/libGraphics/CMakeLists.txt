INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp )

ADD_LIBRARY( Graphics 
GLRenderEngine.cpp 
HandleAxis.cpp 
HandleCommands.cpp 
HandleContour.cpp 
HandleFigure.cpp 
HandleImage.cpp 
HandleLineSeries.cpp 
HandleObject.cpp 
HandleProperty.cpp 
HandleSurface.cpp 
HandlePatch.cpp
HandleText.cpp 
HandleWindow.cpp 
QTRenderEngine.cpp 
RenderEngine.cpp 
HandleUIControl.cpp 
HandleMultiEdit.cpp 
GLWindow.cpp
trackball.cpp
${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp
)
