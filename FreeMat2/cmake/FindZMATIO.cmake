#
# Find the native ZMATIO includes and library
#
# ZMATIO_INCLUDE_DIR - where to find zmatiolib.h, etc.
# ZMATIO_LIBRARIES   - List of fully qualified libraries to link against when using zmatiolib.
# ZMATIO_FOUND       - Do not attempt to use zmatiolib if "no" or undefined.

MESSAGE(STATUS "Looking for zmatio")
SET(ZMATIO_FOUND 0)

FIND_PATH(ZMATIO_INCLUDE_DIR zlib.h ${PROJECT_SOURCE_DIR}/extern/Root/include /usr/local/include /usr/include /sw/include )
FIND_LIBRARY(ZMATIO_LIBRARY zmatio /usr/lib /usr/local/lib /sw/lib ${PROJECT_SOURCE_DIR}/extern/Root/lib)

IF (ZMATIO_INCLUDE_DIR AND ZMATIO_LIBRARY)
  SET(ZMATIO_FOUND 1)
ENDIF (ZMATIO_INCLUDE_DIR AND ZMATIO_LIBRARY)
if (NOT ZMATIO_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find zmatio installed.
   FreeMat cannot be built without this library.")
endif (NOT ZMATIO_FOUND)
MESSAGE(STATUS "Looking for zmatio - found")
