INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleUIControl.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleMultiEdit.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/HandleWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/GLWindow.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.hpp ${CMAKE_CURRENT_SOURCE_DIR}/VTKWindow.moc.cpp )



macro(wrapvtkkit classlist destdir)
  set(wrapcmd "${CMAKE_SOURCE_DIR}/tools/vtkwrap/vtkWrapFreeMat")
  foreach(class ${classlist})
    if (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
      set(src_name "${VTK_COMMON_HEADER_DIR}/${class}.h")
      set(dest_name "${destdir}/${class}.cpp")
      set(VTK_CLASSES ${VTK_CLASSES} ${dest_name})
      add_custom_command(
        OUTPUT ${dest_name}
	DEPENDS ${wrapcmd}
	COMMAND ${wrapcmd}
	ARGS ${src_name} ${CMAKE_SOURCE_DIR}/tools/vtkwrap/hints 1 ${dest_name}
	COMMENT "Generating ${dest_name} from ${src_name}"
	VERBATIM)
    endif (NOT VTK_CLASS_WRAP_EXCLUDE_${class})
  endforeach(class)
endmacro(wrapvtkkit)

INCLUDE(${VTK_KITS_DIR}/vtkCommonKit.cmake)
wrapvtkkit("${VTK_COMMON_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/common")
INCLUDE(${VTK_KITS_DIR}/vtkFilteringKit.cmake)
wrapvtkkit("${VTK_FILTERING_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/filtering")
INCLUDE(${VTK_KITS_DIR}/vtkVolumeRenderingKit.cmake)
wrapvtkkit("${VTK_VOLUMERENDERING_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/volumerendering")
INCLUDE(${VTK_KITS_DIR}/vtkRenderingKit.cmake)
wrapvtkkit("${VTK_RENDERING_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/rendering")
INCLUDE(${VTK_KITS_DIR}/vtkIOKit.cmake)
wrapvtkkit("${VTK_IO_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/io")
INCLUDE(${VTK_KITS_DIR}/vtkGenericFilteringKit.cmake)
wrapvtkkit("${VTK_GENERICFILTERING_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/genericfiltering")
INCLUDE(${VTK_KITS_DIR}/vtkGeovisKit.cmake)
wrapvtkkit("${VTK_GEOVIS_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/geovis")
INCLUDE(${VTK_KITS_DIR}/vtkGraphicsKit.cmake)
wrapvtkkit("${VTK_GRAPHICS_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/graphics")
INCLUDE(${VTK_KITS_DIR}/vtkHybridKit.cmake)
wrapvtkkit("${VTK_HYBRID_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/hybrid")
INCLUDE(${VTK_KITS_DIR}/vtkImagingKit.cmake)
wrapvtkkit("${VTK_IMAGING_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/imaging")
INCLUDE(${VTK_KITS_DIR}/vtkInfovisKit.cmake)
wrapvtkkit("${VTK_INFOVIS_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/infovis")
INCLUDE(${VTK_KITS_DIR}/vtkParallelKit.cmake)
wrapvtkkit("${VTK_PARALLEL_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/parallel")
INCLUDE(${VTK_KITS_DIR}/vtkViewsKit.cmake)
wrapvtkkit("${VTK_VIEWS_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/views")
INCLUDE(${VTK_KITS_DIR}/vtkWidgetsKit.cmake)
wrapvtkkit("${VTK_WIDGETS_CLASSES}" "${CMAKE_CURRENT_SOURCE_DIR}/vtk/widgets")

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


