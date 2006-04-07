MESSAGE(STATUS "Looking for fortran-c link library")
SET(FORT_LIBRARY_FOUND 0)

MESSAGE(STATUS "Fortran compiler is ${CMAKE_Fortran_COMPILER}")

IF(${CMAKE_Fortran_COMPILER} STREQUAL "g77")
   FIND_LIBRARY(FORT_LIBRARY NAMES g2c PATHS /usr/lib /usr/local/lib)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libg2c.so PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libg2c.so.0 PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libg2c.so.0.0 PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
ENDIF(${CMAKE_Fortran_COMPILER} STREQUAL "g77")

IF(${CMAKE_Fortran_COMPILER} STREQUAL "gfortran")
   FIND_LIBRARY(FORT_LIBRARY NAMES gfortran PATHS /usr/lib
   /usr/local/lib)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libgfortran.so PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libgfortran.so.0 PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
   IF (NOT FORT_LIBRARY)
      FIND_FILE(FORT_LIBRARY libgfortran.so.0.0 PATHS /usr/lib /usr/local/lib)
   ENDIF (NOT FORT_LIBRARY)
ENDIF(${CMAKE_Fortran_COMPILER} STREQUAL "gfortran")

IF (FORT_LIBRARY)
  SET(FORT_LIBRARY_FOUND 1)
ENDIF (FORT_LIBRARY)

if (NOT FORT_LIBRARY_FOUND)
   MESSAGE(FATAL_ERROR "cmake could not find the FORTRAN library to
   link to C/C++ code (for GNU g++ this is either libg2c or libgfortran).
   FreeMat cannot be built without this library.")
endif (NOT FORT_LIBRARY_FOUND)
MESSAGE(STATUS "Looking for fortran-c link library - found")

