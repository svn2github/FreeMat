
MESSAGE(STATUS "Looking for blas")
SET(BLAS_FOUND 0)

FIND_LIBRARY(BLAS_LIBRARY NAMES blas PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (BLAS_LIBRARY)
  SET(BLAS_FOUND 1)
ENDIF (BLAS_LIBRARY)
if (NOT BLAS_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find blas installed.
   FreeMat cannot be built without this library.")
endif (NOT BLAS_FOUND)
MESSAGE(STATUS "Looking for blas - found")

