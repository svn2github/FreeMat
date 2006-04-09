project(FreeMat)

include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)

set(CMAKE_MODULE_PATH ./cmake)

find_package(AVCALL)
find_package(FFTW)
find_package(FFTWF)
find_package(LAPACK)
find_package(BLAS)
find_package(AMD)
find_package(UMFPACK)
find_package(ARPACK)
find_package(MATIO)

IF(NOT WIN32)
  message(STATUS "Looking for curses library")
  find_package(Curses)
  include_directories(${CURSES_INCLUDE_PATH})
ENDIF(NOT WIN32)

include(${QT_USE_FILE})

include_directories(libs/libFreeMat libs/libCore libs/libFN
libs/libGraphics libs/libXP src libs/libMex ${AMD_INCLUDE_DIR}
${AVCALL_INCLUDE_DIR} ${FFTW_INCLUDE_DIR} ${FFTWF_INCLUDE_DIR}
${MATIO_INCLUDE_DIR} ${UMFPACK_INCLUDE_DIR} ${QT_INCLUDES}) 

add_subdirectory(libs)
add_subdirectory(src)
