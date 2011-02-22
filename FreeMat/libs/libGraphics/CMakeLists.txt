INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.moc.cpp )

macro(wrapvtkkit classlist destdir kitname)
  set(wrapcmd "${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkWrapFreeMat")
  set(VTK_BLOCK "")
  foreach(class ${classlist})
    if (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
      set(src_name "${VTK_COMMON_HEADER_DIR}/${class}.h")
      set(dest_name "${destdir}/${class}.cpp")
      set(VTK_CLASSES ${VTK_CLASSES} ${dest_name})
      set(VTK_BLOCK ${VTK_BLOCK} ${dest_name})
      add_custom_command(
        OUTPUT ${dest_name}
	DEPENDS ${wrapcmd}
	COMMAND ${wrapcmd}
	ARGS ${src_name} ${CMAKE_SOURCE_DIR}/tools/vtkwrap/hints 1 ${dest_name}
	COMMENT "Generating ${dest_name} from ${src_name}"
	VERBATIM)
    endif (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
  endforeach(class)
  add_custom_command(
	OUTPUT ${destdir}/Loader.cpp
	DEPENDS ${VTK_BLOCK}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${destdir} ${kitname}
	COMMENT "Generating function signatures for ${destdir} with name ${kitname}"
	VERBATIM
	)
  set(VTK_CLASSES ${VTK_CLASSES} ${destdir}/Loader.cpp)
endmacro(wrapvtkkit)

if (USE_VTK)
ADD_SUBDIRECTORY(vtk/common)
ADD_SUBDIRECTORY(vtk/filtering)
ADD_SUBDIRECTORY(vtk/volumerendering)
ADD_SUBDIRECTORY(vtk/rendering)
ADD_SUBDIRECTORY(vtk/io)
#ADD_SUBDIRECTORY(vtk/genericfiltering)
ADD_SUBDIRECTORY(vtk/geovis)
ADD_SUBDIRECTORY(vtk/graphics)
ADD_SUBDIRECTORY(vtk/hybrid)
ADD_SUBDIRECTORY(vtk/imaging)
ADD_SUBDIRECTORY(vtk/infovis)
ADD_SUBDIRECTORY(vtk/parallel)
ADD_SUBDIRECTORY(vtk/views)
ADD_SUBDIRECTORY(vtk/widgets)
endif(USE_VTK)

set (VTK_WRAPPER_LIBS 
vtkCommonWrapper        vtkIOWrapper
vtkFilteringWrapper  vtkParallelWrapper
vtkGeovisWrapper        vtkRenderingWrapper
vtkGraphicsWrapper    vtkViewsWrapper
vtkHybridWrapper        vtkVolumeRenderingWrapper
vtkImagingWrapper      vtkWidgetsWrapper
vtkInfovisWrapper)


set( GRAPHICS_SOURCES
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
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
	DEPENDS ${GRAPHICS_SOURCES}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} Graphics
	COMMENT "Generating function signatures"
	VERBATIM
	)


ADD_LIBRARY( Graphics 
${GRAPHICS_SOURCES}
${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp
${VTK_CLASSES}
)


