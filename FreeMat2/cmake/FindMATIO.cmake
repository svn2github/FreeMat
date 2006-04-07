
MESSAGE(STATUS "Looking for matio")
SET(MATIO_FOUND 0)

FIND_PATH(MATIO_INCLUDE_DIR matio.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(MATIO_LIBRARY NAMES matio PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (MATIO_INCLUDE_DIR AND MATIO_LIBRARY)
  SET(MATIO_FOUND 1)
ENDIF (MATIO_INCLUDE_DIR AND MATIO_LIBRARY)
if (NOT MATIO_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find matio installed.
   FreeMat cannot be built without this library.")
endif (NOT MATIO_FOUND)
MESSAGE(STATUS "Looking for matio - found")

