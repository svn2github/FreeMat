#
# Find the native ZLIB includes and library
#
# ZLIB_INCLUDE_DIR - where to find zlib.h, etc.
# ZLIB_LIBRARIES   - List of fully qualified libraries to link against when using zlib.
# ZLIB_FOUND       - Do not attempt to use zlib if "no" or undefined.

MESSAGE(STATUS "Looking for zlib")
SET(ZLIB_FOUND 0)

FIND_PATH(ZLIB_INCLUDE_DIR zlib.h /usr/local/include /usr/include /sw/include ${PROJECT_SOURCE_DIR}/extern/Root/include)
FIND_LIBRARY(ZLIB_LIBRARY z /usr/lib /usr/local/lib /sw/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
  SET(ZLIB_FOUND 1)
ENDIF (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
if (NOT ZLIB_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find zlib installed.
   FreeMat cannot be built without this library.")
endif (NOT ZLIB_FOUND)
MESSAGE(STATUS "Looking for zlib - found")
