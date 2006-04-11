project(FreeMat)

include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)

set(CMAKE_MODULE_PATH ${FreeMat_SOURCE_DIR}/cmake)

find_package(AVCALL)
find_package(FFTW)
find_package(FFTWF)
find_package(LAPACK)
find_package(BLAS)
find_package(AMD)
find_package(UMFPACK)
find_package(ARPACK)
find_package(Z)
find_package(MATIO)

IF(NOT WIN32)
  message(STATUS "Looking for curses library")
  find_package(Curses)
  include_directories(${CURSES_INCLUDE_PATH})
ENDIF(NOT WIN32)

include(${QT_USE_FILE})

include_directories(${FreeMat_SOURCE_DIR}/libs/libFreeMat 
${FreeMat_SOURCE_DIR}/libs/libCore ${FreeMat_SOURCE_DIR}/libs/libFN
${FreeMat_SOURCE_DIR}/libs/libGraphics ${FreeMat_SOURCE_DIR}/libs/libXP 
${FreeMat_SOURCE_DIR}/src ${FreeMat_SOURCE_DIR}/libs/libMex 
${AMD_INCLUDE_DIR}
${AVCALL_INCLUDE_DIR} ${FFTW_INCLUDE_DIR} ${FFTWF_INCLUDE_DIR}
${MATIO_INCLUDE_DIR} ${UMFPACK_INCLUDE_DIR} ${QT_INCLUDES}) 

add_subdirectory(libs)
add_subdirectory(src)
add_subdirectory(tools)

add_custom_target(help 
		  COMMAND ${FreeMat_BINARY_DIR}/tools/helpgen/FMHelpGen
		  ${FreeMat_SOURCE_DIR} ${FreeMat_BINARY_DIR}
		  DEPENDS ${FreeMat_BINARY_DIR}/tools/helpgen/FMHelpGen
		   ${FreeMat_BINARY_DIR}/src/FreeMat)
add_custom_target(install
		  COMMAND
		  ${FreeMat_BINARY_DIR}/tools/disttool/DistTool
		  -linux ${FreeMat_SOURCE_DIR} ${FreeMat_BINARY_DIR}
		  DEPENDS ${FreeMat_BINARY_DIR}/tools/disttool/DistTool
		   ${FreeMat_BINARY_DIR}/src/FreeMat)
add_custom_target(dist
		  ${FreeMat_BINARY_DIR}/tools/disttool/DistTool
		  -src ${FreeMat_SOURCE_DIR} ${FreeMat_BINARY_DIR}
		  DEPENDS ${FreeMat_BINARY_DIR}/tools/disttool/DistTool)
add_custom_target(check
		  ${FreeMat_BINARY_DIR}/src/FreeMat -f 
		  "cd ${FreeMat_SOURCE_DIR}/tests/core\; test_core"
		  DEPENDS ${FreeMat_BINARY_DIR}/src/FreeMat)

