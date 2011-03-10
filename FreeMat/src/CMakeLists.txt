INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}  ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/application.hpp ${CMAKE_CURRENT_SOURCE_DIR}/application.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/FuncMode.hpp ${CMAKE_CURRENT_SOURCE_DIR}/FuncMode.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/MainApp.hpp ${CMAKE_CURRENT_SOURCE_DIR}/MainApp.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Terminal.hpp ${CMAKE_CURRENT_SOURCE_DIR}/Terminal.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DumbTerminal.hpp ${CMAKE_CURRENT_SOURCE_DIR}/DumbTerminal.moc.cpp )

QT4_ADD_RESOURCES( FreeMat_qrc ${CMAKE_CURRENT_SOURCE_DIR}/FreeMat.qrc  )

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
	DEPENDS MainApp.cpp
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} Main
	COMMENT "Generating function signatures"
	VERBATIM
	)


ADD_EXECUTABLE( FreeMat WIN32
${CMAKE_CURRENT_SOURCE_DIR}/application.moc.cpp 
application.cpp
${CMAKE_CURRENT_SOURCE_DIR}/FuncMode.moc.cpp 
FuncMode.cpp
FuncTerminal.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/MainApp.moc.cpp
MainApp.cpp 
main.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/DumbTerminal.moc.cpp
DumbTerminal.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/Terminal.moc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Loader.cpp
Terminal.cpp 
${FreeMat_qrc}
freemat.rc 
)

#${X86TARGET_OBJ}

#IF( MSVC )
#	SET( LAPACK_LIBRARY ${lapack} )
#ENDIF( MSVC )

IF( USE_DYNLINK )
	TARGET_LINK_LIBRARIES( FreeMat blas )
ENDIF( USE_DYNLINK )


if (USE_VTK)
set (VTK_WRAPPER_LIBS 
vtkCommonWrapper        vtkIOWrapper
vtkFilteringWrapper  vtkParallelWrapper
vtkGeovisWrapper        vtkRenderingWrapper
vtkGraphicsWrapper    vtkViewsWrapper
vtkHybridWrapper        vtkVolumeRenderingWrapper
vtkImagingWrapper      vtkWidgetsWrapper
vtkInfovisWrapper)
endif (USE_VTK)

TARGET_LINK_LIBRARIES( FreeMat 
Core 
FN 
${VTK_WRAPPER_LIBS}
Graphics 
FreeMatlib 
XP 
Mex
MatC
levmar
${QT_QTCORE_LIBRARY} 
${QT_QTGUI_LIBRARY} 
${QT_QTNETWORK_LIBRARY} 
${QT_QTOPENGL_LIBRARY} 
${QT_QTXML_LIBRARY} 
${QT_QTSVG_LIBRARY} 
${QT_QTMAIN_LIBRARY}
${OPENGL_LIBRARIES}
${EXTRA_LIBRARIES}
${OPTIONAL_LIBS}
arpack_c
lapack_c
dynblas
blasref
)

#IF(MSVC)
#  SET_TARGET_PROPERTIES(FreeMat PROPERTIES LINK_FLAGS "/INCLUDE:_X86TargetMachineModule")
#ENDIF(MSVC)

#IF( MSVC )
#	set_target_properties( FreeMat PROPERTIES LINK_FLAGS "/MANIFEST:NO  /ALLOWISOLATION:NO" )
#ENDIF( MSVC )

INSTALL(TARGETS FreeMat DESTINATION bin )
WRITE_OUT_ALL_VARIABLES()
