
MESSAGE(STATUS "Looking for lapack")
SET(LAPACK_FOUND 0)

FIND_LIBRARY(LAPACK_LIBRARY NAMES lapack PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (LAPACK_LIBRARY)
  SET(LAPACK_FOUND 1)
ENDIF (LAPACK_LIBRARY)
if (NOT LAPACK_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find lapack installed.
   FreeMat cannot be built without this library.")
endif (NOT LAPACK_FOUND)
MESSAGE(STATUS "Looking for lapack - found")

