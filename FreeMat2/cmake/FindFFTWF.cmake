MESSAGE(STATUS "Looking for fftw (float version)")
SET(FFTWF_FOUND 0)

FIND_PATH(FFTWF_INCLUDE_DIR fftw3.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/tmp/include)
FIND_LIBRARY(FFTWF_LIBRARY NAMES fftw3f PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/tmp/lib)

IF (FFTWF_INCLUDE_DIR AND FFTWF_LIBRARY)
  SET(FFTWF_FOUND 1)
ENDIF (FFTWF_INCLUDE_DIR AND FFTWF_LIBRARY)

if (NOT FFTWF_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find fftwf (float version of fftw) installed.
   FreeMat cannot be built without this library.")
endif (NOT FFTWF_FOUND)
MESSAGE(STATUS "Looking for fftw (float version) - found")
