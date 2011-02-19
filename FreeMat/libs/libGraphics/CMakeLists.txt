INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.moc.cpp )


INCLUDE(${VTK_KITS_DIR}/vtkCommonKit.cmake)

FOREACH(class ${VTK_COMMON_CLASSES})
	 IF (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
	 SET(VTK_CLASSES ${VTK_CLASSES} ${CMAKE_CURRENT_SOURCE_DIR}/vtk/${class}.cpp )
	 SET(full_name "${VTK_COMMON_HEADER_DIR}/${class}.h")
	ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vtk/${class}.cpp
	DEPENDS ${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkWrapFreeMat
	COMMAND ${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkWrapFreeMat
	ARGS ${full_name} ${CMAKE_SOURCE_DIR}/tools/vtkwrap/hints 1 ${CMAKE_CURRENT_SOURCE_DIR}/vtk/${class}.cpp
	COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/vtk/${class}.cpp from ${full_name}"	
	VERBATIM)
      ENDIF(NOT VTK_CLASS_WRAP_EXCLUDE_${class})
ENDFOREACH(class)

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
VTKWindow.cpp
${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp
${VTK_CLASSES}
)


