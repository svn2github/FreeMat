
MESSAGE(STATUS "Looking for arpack")
SET(ARPACK_FOUND 0)

FIND_LIBRARY(ARPACK_LIBRARY NAMES arpack PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (ARPACK_LIBRARY)
  SET(ARPACK_FOUND 1)
ENDIF (ARPACK_LIBRARY)
if (NOT ARPACK_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find arpack installed.
   FreeMat cannot be built without this library.")
endif (NOT ARPACK_FOUND)
MESSAGE(STATUS "Looking for arpack - found")

