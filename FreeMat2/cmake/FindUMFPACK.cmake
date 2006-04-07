
MESSAGE(STATUS "Looking for umfpack")
SET(UMFPACK_FOUND 0)

FIND_PATH(UMFPACK_INCLUDE_DIR umfpack.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(UMFPACK_LIBRARY NAMES umfpack PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (UMFPACK_INCLUDE_DIR AND UMFPACK_LIBRARY)
  SET(UMFPACK_FOUND 1)
ENDIF (UMFPACK_INCLUDE_DIR AND UMFPACK_LIBRARY)
if (NOT UMFPACK_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find umfpack installed.
   FreeMat cannot be built without this library.")
endif (NOT UMFPACK_FOUND)
MESSAGE(STATUS "Looking for umfpack - found")

