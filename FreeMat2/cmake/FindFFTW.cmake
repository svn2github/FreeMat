MESSAGE(STATUS "Looking for fftw")

SET(FFTW_FOUND 0)

FIND_PATH(FFTW_INCLUDE_DIR fftw3.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(FFTW_LIBRARY NAMES fftw3 PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (FFTW_INCLUDE_DIR AND FFTW_LIBRARY)
  SET(FFTW_FOUND 1)
ENDIF (FFTW_INCLUDE_DIR AND FFTW_LIBRARY)

if (NOT FFTW_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find fftw installed.
   FreeMat cannot be built without this library.")
endif (NOT FFTW_FOUND)

MESSAGE(STATUS "Looking for fftw - found")
