
MESSAGE(STATUS "Looking for avcall")
SET(AVCALL_FOUND 0)

FIND_PATH(AVCALL_INCLUDE_DIR avcall.h /usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(AVCALL_LIBRARY NAMES avcall PATHS /usr/lib /usr/local/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (AVCALL_INCLUDE_DIR AND AVCALL_LIBRARY)
  SET(AVCALL_FOUND 1)
ENDIF (AVCALL_INCLUDE_DIR AND AVCALL_LIBRARY)
if (NOT AVCALL_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find avcall installed.
   FreeMat cannot be built without this library.")
endif (NOT AVCALL_FOUND)
MESSAGE(STATUS "Looking for avcall - found")

