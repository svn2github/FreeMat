
MESSAGE(STATUS "Looking for amd")
SET(AMD_FOUND 0)

FIND_PATH(AMD_INCLUDE_DIR amd.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(AMD_LIBRARY NAMES amd PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (AMD_INCLUDE_DIR AND AMD_LIBRARY)
  SET(AMD_FOUND 1)
ENDIF (AMD_INCLUDE_DIR AND AMD_LIBRARY)
if (NOT AMD_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find amd installed.
   FreeMat cannot be built without this library.")
endif (NOT AMD_FOUND)
MESSAGE(STATUS "Looking for amd - found")

