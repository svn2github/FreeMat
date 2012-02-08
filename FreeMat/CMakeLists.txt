######## CMake debugging macro
MACRO(WRITE_OUT_ALL_VARIABLES)
SET(DstFile ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt)

#MESSAGE("DEBUG -- Writing out all variables to file ${DstFile}")

FILE(WRITE ${DstFile} "")
GET_CMAKE_PROPERTY(VARS VARIABLES)
FOREACH(var ${VARS})
FILE(APPEND ${DstFile} "${var} \"${${var}}\"\n")
ENDFOREACH(var ${VARS})
ENDMACRO(WRITE_OUT_ALL_VARIABLES)
### End Debugging tools


PROJECT(FreeMat CXX C )

SET(CPACK_PACKAGE_VERSION_MAJOR "4")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "")
SET(CPACK_PACKAGE_FILE_NAME "FreeMat-4.2")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "FreeMat 4.2")
SET(CPACK_PACKAGE_VERSION "4.2")
SET(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

SET( RESOURCEDIR "${CMAKE_INSTALL_PREFIX}/share/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
#SET( CMAKE_VERBOSE_MAKEFILE TRUE )
SET( LOCAL_PATH ${PROJECT_SOURCE_DIR}/dependencies/bin ${PROJECT_BINARY_DIR} /mingw/lib INTERNAL )

IF( MSVC )
	ADD_DEFINITIONS( /Dsnprintf=_snprintf /DNOMINMAX /DFREEMAT_VERSION="${VERSION}" /DRESOURCEDIR="${RESOURCEDIR}")
	#	ADD_DEFINITIONS( /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0 )
ELSE( MSVC )
	ADD_DEFINITIONS( -DFREEMAT_VERSION="${VERSION}" -DRESOURCEDIR="${RESOURCEDIR}")
	SET(CMAKE_SHARED_MODULE_LINK_FLAGS "${CMAKE_SHARED_MODULE_LINK_FLAGS} -export-dynamic")
	SET(CMAKE_SHARED_MODULE_LINK_CXX_FLAGS "${CMAKE_SHARED_MODULE_LINK_CXX_FLAGS} -export-dynamic")
ENDIF( MSVC )

IF( MINGW )
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -shared-libgcc -shared-libstdc++")
ENDIF(MINGW)

ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS)

FIND_PACKAGE(OpenGL REQUIRED)

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTSVG TRUE)
SET(QT_USE_QTWEBKIT TRUE)

FIND_PACKAGE(Qt4 REQUIRED)

FIND_PACKAGE(PythonInterp REQUIRED)

ADD_DEFINITIONS( ${QT_DEFINITIONS} )
INCLUDE_DIRECTORIES( ${QT_INCLUDES} )

MACRO (QT4_GENERATE_UI infile outfile )
     GET_FILENAME_COMPONENT(infile ${infile} ABSOLUTE)

     ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_UIC_EXECUTABLE}
        ARGS -o ${outfile} ${infile}
        DEPENDS ${infile})

     MACRO_ADD_FILE_DEPENDENCIES(${infile} ${outfile})
ENDMACRO (QT4_GENERATE_UI)

SET(OPTIONAL_LIBS "")

OPTION( FORCE_SYSTEM_LIBS "Only use system libraries. Don't build third party libs." )

######################################################################
# PCRE Package
######################################################################
FIND_LIBRARY(PCRE_SYS_LIBRARY NAMES pcre DOC "Location of the PCRE library" PATHS ${LOCAL_PATH})
FIND_PATH(PCRE_SYS_INCLUDE_DIR NAMES pcre.h DOC "Location of pcre.h" PATHS ${LOCAL_PATH})

IF (PCRE_SYS_LIBRARY AND PCRE_SYS_INCLUDE_DIR)
  SET (SYSTEM_PCRE_AVAILABLE TRUE)
ELSE (PCRE_SYS_LIBRARY AND PCRE_SYS_INCLUDE_DIR)
  SET (SYSTEM_PCRE_AVAILABLE FALSE)
ENDIF (PCRE_SYS_LIBRARY AND PCRE_SYS_INCLUDE_DIR)

OPTION(FORCE_BUNDLED_PCRE "Force use of bundled PCRE" OFF)

IF (FORCE_BUNDLED_PCRE OR NOT SYSTEM_PCRE_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  ADD_SUBDIRECTORY(libs/thirdparty/pcre)
  INCLUDE_DIRECTORIES(libs/thirdparty/pcre)
  SET (PCRE_LIBRARY pcre)
ELSE (FORCE_BUNDLED_PCRE OR NOT SYSTEM_PCRE_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  INCLUDE_DIRECTORIES(${PCRE_SYS_INCLUDE_DIR})
  SET (PCRE_LIBRARY ${PCRE_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_PCRE OR NOT SYSTEM_PCRE_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_DEFINITIONS( -DHAVE_PCRE)
SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PCRE_LIBRARY})

######################################################################
# FFTW Package
######################################################################

FIND_LIBRARY(FFTW_LIBRARY NAMES fftw3 fftw3-3 libfftw3-3 DOC "Location of the FFTW library" PATHS ${LOCAL_PATH} )
FIND_PATH(FFTW_INCLUDE_DIR NAMES fftw3.h DOC "Location of fftw3.h" PATHS ${LOCAL_PATH} /mingw/include)
IF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR )
	ADD_DEFINITIONS( -DHAVE_FFTW )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTW_LIBRARY} )
ENDIF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR )

######################################################################
# FFTWF Package
######################################################################

FIND_LIBRARY(FFTWF_LIBRARY NAMES fftw3f fftw3f-3 libfftw3f-3 DOC "Location of the FFTW (single precision) library" PATHS ${LOCAL_PATH} )
IF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR )
	ADD_DEFINITIONS( -DHAVE_FFTWF )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTWF_LIBRARY} )
ENDIF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR )

######################################################################
# ZLIB Package
######################################################################

FIND_LIBRARY(ZLIB_SYS_LIBRARY NAMES zlib z DOC "Location of the ZLIB library" PATHS ${LOCAL_PATH})
FIND_PATH(ZLIB_SYS_INCLUDE_DIR NAMES zlib.h DOC "Location of zlib.h" PATHS ${LOCAL_PATH})

IF (ZLIB_SYS_LIBRARY AND ZLIB_SYS_INCLUDE_DIR)
  SET (SYSTEM_ZLIB_AVAILABLE TRUE)
ELSE (ZLIB_SYS_LIBRARY AND ZLIB_SYS_INCLUDE_DIR)
  SET (SYSTEM_ZLIB_AVAILABLE FALSE)
ENDIF (ZLIB_SYS_LIBRARY AND ZLIB_SYS_INCLUDE_DIR)

OPTION(FORCE_BUNDLED_ZLIB "Force use of bundled ZLIB" OFF)

IF (FORCE_BUNDLED_ZLIB OR NOT SYSTEM_ZLIB_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  ADD_SUBDIRECTORY(libs/thirdparty/zlib-1.2.3)
  INCLUDE_DIRECTORIES(libs/thirdparty/zlib-1.2.3)
  SET (ZLIB_LIBRARY zlib)
ELSE (FORCE_BUNDLED_ZLIB OR NOT SYSTEM_ZLIB_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  INCLUDE_DIRECTORIES(${ZLIB_SYS_INCLUDE_DIR})
  SET (ZLIB_LIBRARY ${ZLIB_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_ZLIB OR NOT SYSTEM_ZLIB_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_DEFINITIONS( -DHAVE_ZLIB)
SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_LIBRARY})

######################################################################
# AVCALL Package
######################################################################

if(MINGW)
    FIND_LIBRARY(FFI_LIBRARY_DLL NAMES ffi-5 DOC "Location of the FFI dll library" PATHS /mingw/bin ${PROJECT_SOURCE_DIR}/dependencies/libffi/build/.libs )
    FIND_LIBRARY(FFI_LIBRARY NAMES ffi DOC "Location of the FFI library" PATHS /mingw/bin ${PROJECT_SOURCE_DIR}/dependencies/libffi/build/.libs )
    FIND_PATH(FFI_INCLUDE_DIR ffi.h doc "Location of ffi.h" PATHS ${LOCAL_PATH} ${PROJECT_SOURCE_DIR}/dependencies/libffi/build/include /mingw/include/ffi)
    SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFI_LIBRARY})
    INCLUDE_DIRECTORIES(${FFI_INCLUDE_DIR})
    INSTALL(FILES ${FFI_LIBRARY_DLL} DESTINATION bin )
else(MINGW)
    FIND_LIBRARY(FFI_LIBRARY NAMES ffi DOC "Location of the FFI library" PATHS ${LOCAL_PATH})
    FIND_PATH(FFI_INCLUDE_DIR ffi.h doc "Location of ffi.h" PATHS ${LOCAL_PATH} /usr/include/ffi /usr/lib64/libffi-3.0.9/include /usr/lib64/libffi-3.0.10/include)
    SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFI_LIBRARY})
    INCLUDE_DIRECTORIES(${FFI_INCLUDE_DIR})
endif(MINGW)

######################################################################
# UMFPACK Package
######################################################################

FIND_LIBRARY(UMFPACK_SYS_LIBRARY NAMES umfpack DOC "Location of the UMFPACK library" PATHS ${LOCAL_PATH})
FIND_PATH(UMFPACK_SYS_INCLUDE_DIR NAMES umfpack.h DOC "Location of umfpack.h" PATHS ${LOCAL_PATH} /usr/include/ufsparse /usr/include/suitesparse)

IF (UMFPACK_SYS_LIBRARY AND UMFPACK_SYS_INCLUDE_DIR)
  SET (SYSTEM_UMFPACK_AVAILABLE TRUE)
ELSE (UMFPACK_SYS_LIBRARY AND UMFPACK_SYS_INCLUDE_DIR)
  SET (SYSTEM_UMFPACK_AVAILABLE FALSE)
ENDIF (UMFPACK_SYS_LIBRARY AND UMFPACK_SYS_INCLUDE_DIR)

OPTION(FORCE_BUNDLED_UMFPACK "Force use of bundled UMFPACK" OFF)

IF (FORCE_BUNDLED_UMFPACK OR NOT SYSTEM_UMFPACK_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  ADD_SUBDIRECTORY(libs/thirdparty/UMFPACK)
  INCLUDE_DIRECTORIES(libs/thirdparty/UMFPACK/Include)
  INCLUDE_DIRECTORIES(libs/thirdparty/UFconfig)
  SET (UMFPACK_LIBRARY umfpack)
ELSE (FORCE_BUNDLED_UMFPACK OR NOT SYSTEM_UMFPACK_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  INCLUDE_DIRECTORIES(${UMFPACK_SYS_INCLUDE_DIR})
  SET (UMFPACK_LIBRARY ${UMFPACK_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_UMFPACK OR NOT SYSTEM_UMFPACK_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_DEFINITIONS( -DHAVE_UMFPACK)
SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${UMFPACK_LIBRARY})


######################################################################
# AMD Package
######################################################################

FIND_LIBRARY(AMD_SYS_LIBRARY NAMES amd DOC "Location of the AMD library" PATHS ${LOCAL_PATH})
FIND_PATH(AMD_SYS_INCLUDE_DIR NAMES amd.h DOC "Location of amd.h" PATHS ${LOCAL_PATH} /usr/include/ufsparse /usr/include/suitesparse)

IF (AMD_SYS_LIBRARY AND AMD_SYS_INCLUDE_DIR)
SET (SYSTEM_AMD_AVAILABLE TRUE)
ELSE (AMD_SYS_LIBRARY AND AMD_SYS_INCLUDE_DIR)
SET (SYSTEM_AMD_AVAILABLE FALSE)
ENDIF (AMD_SYS_LIBRARY AND AMD_SYS_INCLUDE_DIR)

OPTION(FORCE_BUNDLED_AMD "Force use of bundled AMD" OFF)

IF (FORCE_BUNDLED_AMD OR NOT SYSTEM_AMD_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_SUBDIRECTORY(libs/thirdparty/AMD)
INCLUDE_DIRECTORIES(libs/thirdparty/AMD/Include)
INCLUDE_DIRECTORIES(libs/thirdparty/UFConfig)
SET (AMD_LIBRARY amd)
ELSE (FORCE_BUNDLED_AMD OR NOT SYSTEM_AMD_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
INCLUDE_DIRECTORIES(${AMD_SYS_INCLUDE_DIR})
SET (AMD_LIBRARY ${AMD_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_AMD OR NOT SYSTEM_AMD_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_DEFINITIONS( -DHAVE_AMD)
SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${AMD_LIBRARY})

######################################################################
# PORTAUDIO Package
######################################################################


OPTION(FORCE_BUNDLED_PORTAUDIO "Force use of bundled PORTAUDIO" OFF)

IF ( NOT FORCE_BUNDLED_PORTAUDIO )
  FIND_LIBRARY(PORTAUDIO_SYS_LIBRARY NAMES portaudio DOC "Location of the PORTAUDIO library" )
  FIND_PATH(PORTAUDIO_SYS_INCLUDE_DIR NAMES portaudio.h DOC "Location of portaudio.h" )

  IF (PORTAUDIO_SYS_LIBRARY AND PORTAUDIO_SYS_INCLUDE_DIR)
  SET (SYSTEM_PORTAUDIO_AVAILABLE TRUE)
  ELSE (PORTAUDIO_SYS_LIBRARY AND PORTAUDIO_SYS_INCLUDE_DIR)
  SET (SYSTEM_PORTAUDIO_AVAILABLE FALSE)
  ENDIF (PORTAUDIO_SYS_LIBRARY AND PORTAUDIO_SYS_INCLUDE_DIR)
ENDIF ( NOT FORCE_BUNDLED_PORTAUDIO )


IF (FORCE_BUNDLED_PORTAUDIO OR NOT SYSTEM_PORTAUDIO_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  ADD_SUBDIRECTORY(libs/thirdparty/portaudio)
  INCLUDE_DIRECTORIES(libs/thirdparty/portaudio/include)
  INCLUDE_DIRECTORIES(libs/thirdparty/portaudio/src/common)
  SET (PORTAUDIO_LIBRARY portaudio)
ELSE (FORCE_BUNDLED_PORTAUDIO OR NOT SYSTEM_PORTAUDIO_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
  INCLUDE_DIRECTORIES(${PORTAUDIO_SYS_INCLUDE_DIR})
  SET (PORTAUDIO_LIBRARY ${PORTAUDIO_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_PORTAUDIO OR NOT SYSTEM_PORTAUDIO_AVAILABLE AND NOT FORCE_SYSTEM_LIBS)
ADD_DEFINITIONS( -DHAVE_PORTAUDIO19)
SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PORTAUDIO_LIBRARY})
IF(MINGW)
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} -lwinmm)
ENDIF(MINGW)
IF (APPLE)
   FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
   FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
   FIND_LIBRARY(AUDIOUNIT_LIBRARY AudioUnit)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${COREAUDIO_LIBRARY} ${AUDIOTOOLBOX_LIBRARY} ${AUDIOUNIT_LIBRARY} ${CARBON_LIBRARY})
ENDIF (APPLE)

######################################################################
# VTK Support
######################################################################
OPTION(USE_VTK "Build with VTK support?" OFF)
IF (USE_VTK)
FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  ADD_DEFINITIONS( -DHAVE_VTK )
  INCLUDE(${VTK_USE_FILE})
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} QVTK vtkFiltering vtkRendering vtkVolumeRendering vtkGraphics vtkIO vtkCommon vtkGeovis vtkImaging vtkInfovis vtkWidgets vtkHybrid vtkParallel)
ENDIF (VTK_FOUND)
ENDIF (USE_VTK)

######################################################################
# ITK Support
######################################################################
OPTION(USE_ITK "Build with InsightToolkit support?" OFF)
IF (USE_ITK)
FIND_PACKAGE(ITK)
IF (ITK_FOUND)
  ADD_DEFINITIONS( -DHAVE_ITK)
  INCLUDE(${ITK_USE_FILE})
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ITKBasicFilters)
ENDIF(ITK_FOUND)
ENDIF(USE_ITK)

######################################################################
# Boost Support
######################################################################
if(UNIX)
  find_package( Boost)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DHAVE_BOOST)
  endif(Boost_FOUND)
elseif( MINGW )
	include_directories(${PROJECT_SOURCE_DIR}/dependencies)
	#set(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PROJECT_SOURCE_DIR}/dependencies/bin/libboost_math_c99f-mgw46-1_46_1.a ${PROJECT_SOURCE_DIR}/dependencies/bin/libboost_math_c99-mgw46-1_46_1.a)
	add_definitions(-DHAVE_BOOST)
else ()
  find_package( Boost COMPONENTS math_c99 )
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DHAVE_BOOST)
    set(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${Boost_LIBRARIES})
  endif()
endif(UNIX)
######################################################################
# LLVM Support
######################################################################
OPTION(USE_LLVM "Build with LLVM support?" ON)

IF( USE_LLVM )
  set(LLVM_FOUND FALSE)
  if (MSVC)
    set(LLVM_ROOT "C:/Program Files/LLVM" CACHE PATH "Directory where LLVM is installed")
    macro(FIND_LLVM_MSVC_LIB __libname__  __varname__ )
      find_library(${__varname__} ${__libname__} DOC "Location of ${__libname__}.lib"
      				  PATHS ${LOCAL_PATH} "${LLVM_ROOT}/lib")
      mark_as_advanced(${__varname__})
    endmacro(FIND_LLVM_MSVC_LIB)
    find_path(LLVM_INCLUDE_DIR NAMES llvm-c/Core.h DOC "Location of llvm-c"
    			       PATHS ${LOCAL_PATH} ${LLVM_ROOT}/include)
    if (LLVM_INCLUDE_DIR)
        FIND_LLVM_MSVC_LIB( LLVMInstrumentation LIB_LLVMInstrumentation )
    	FIND_LLVM_MSVC_LIB( LLVMipo LIB_LLVMipo )
		FIND_LLVM_MSVC_LIB( LLVMX86CodeGen LIB_LLVMX86CodeGen )
		FIND_LLVM_MSVC_LIB( LLVMX86Info LIB_LLVMX86Info )
    	FIND_LLVM_MSVC_LIB( LLVMSelectionDAG LIB_LLVMSelectionDAG )
    	FIND_LLVM_MSVC_LIB( LLVMAsmPrinter LIB_LLVMAsmPrinter )
    	FIND_LLVM_MSVC_LIB( LLVMExecutionEngine LIB_LLVMExecutionEngine )
    	FIND_LLVM_MSVC_LIB( LLVMJIT LIB_LLVMJIT )
    	FIND_LLVM_MSVC_LIB( LLVMCodeGen LIB_LLVMCodeGen )
    	FIND_LLVM_MSVC_LIB( LLVMScalarOpts LIB_LLVMScalarOpts )
    	FIND_LLVM_MSVC_LIB( LLVMInstCombine LIB_LLVMInstCombine )
    	FIND_LLVM_MSVC_LIB( LLVMTransformUtils LIB_LLVMTransformUtils )
    	FIND_LLVM_MSVC_LIB( LLVMipa LIB_LLVMipa )
    	FIND_LLVM_MSVC_LIB( LLVMAnalysis LIB_LLVMAnalysis )
    	FIND_LLVM_MSVC_LIB( LLVMTarget LIB_LLVMTarget )
    	FIND_LLVM_MSVC_LIB( LLVMMC LIB_LLVMMC )
    	FIND_LLVM_MSVC_LIB( LLVMMCParser LIB_LLVMMCParser )
    	FIND_LLVM_MSVC_LIB( LLVMCore LIB_LLVMCore )
    	FIND_LLVM_MSVC_LIB( LLVMSupport LIB_LLVMSupport )
    	FIND_LLVM_MSVC_LIB( LLVMSystem LIB_LLVMSystem )
    	set(LLVM_LIBS ${LIB_LLVMInstrumentation} ${LIB_LLVMipo} ${LIB_LLVMX86CodeGen} ${LIB_LLVMX86Info}
		  ${LIB_LLVMSelectionDAG}
    		  ${LIB_LLVMAsmPrinter} ${LIB_LLVMExecutionEngine} ${LIB_LLVMJIT} ${LIB_LLVMCodeGen}
		  ${LIB_LLVMTransformUtils} ${LIB_LLVMipa} ${LIB_LLVMAnalysis}
		  ${LIB_LLVMTarget} ${LIB_LLVMCore} ${LIB_LLVMSupport} ${LIB_LLVMMC} ${LIB_LLVMMCParser} ${LIB_LLVMSystem}
		  ${LIB_LLVMScalarOpts} ${LIB_LLVMInstCombine}
		  CACHE STRING "LLVM Link Libraries" )
	set(LLVM_VERSION "2.9" CACHE STRING "LLVM Version found")
    endif (LLVM_INCLUDE_DIR)
  elseif(MINGW)
    set(LLVM_ROOT ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/build CACHE PATH "Root of LLVM install.")
    include_directories( ${LLVM_ROOT}/include
    ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/include
    ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/tools/clang/include
    ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/build/tools/clang/include/clang/Basic
    )
    link_directories( ${LLVM_ROOT}/lib )
    # A bit of a sanity check:
    if( NOT EXISTS ${LLVM_ROOT}/include/llvm )
    message(FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install")
    endif()
      set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
         ${LLVM_ROOT}/share/llvm/cmake
        ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/cmake/modules)
      include(LLVM)
      include( LLVMConfig )
    llvm_map_components_to_libraries(LLVM_LIBS jit native support ipo bitwriter asmparser bitreader)
    set(LLVM_LIBDIR ${CMAKE_SOURCE_DIR}/dependencies/llvm-2.9/build/lib)
    FIND_LIBRARY(CLANGFRONTEND clangFrontend  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGSERIALIZATION clangSerialization   ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGDRIVER  clangDriver  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGCODEGEN  clangCodeGen  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGSEMA  clangSema  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGINDEX  clangIndex  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGANALYSIS  clangAnalysis  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGREWRITE  clangRewrite  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGAST  clangAST  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGPARSE  clangParse  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGLEX  clangLex ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGBASIC  clangBasic ${LLVM_LIBDIR})
    SET(LLVM_LIBS ${CLANGFRONTEND} ${CLANGSERIALIZATION} ${CLANGDRIVER} ${CLANGCODEGEN} ${CLANGSEMA} ${CLANGCHECKER} ${CLANGINDEX} ${CLANGANALYSIS} ${CLANGREWRITE} ${CLANGAST} ${CLANGPARSE} ${CLANGLEX} ${CLANGBASIC} ${LLVM_LIBS} )
    set(LLVM_VERSION "2.9" CACHE STRING "LLVM Version found")
  else(MINGW)
    find_program(LLVM_CONFIG llvm-config)
    execute_process(COMMAND ${LLVM_CONFIG} --libfiles jit nativecodegen asmparser bitreader bitwriter codegen ipo instrumentation scalaropts selectiondag interpreter
    			    OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
#    execute_process(COMMAND ${LLVM_CONFIG} --libfiles OUTPUT_VARIABLE LLVM_LIBS #OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(LLVM_LIBS)
# These shouldn't be necessary, but for now... I have added them
    FIND_LIBRARY(PTHREADS_LIB pthread)
    FIND_LIBRARY(DL_LIB dl)
    FIND_LIBRARY(CLANGFRONTEND clangFrontend /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGSERIALIZATION clangSerialization /usr/lib64/llvm  ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGDRIVER  clangDriver /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGCODEGEN  clangCodeGen /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGSEMA  clangSema /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGINDEX  clangIndex /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGANALYSIS  clangAnalysis /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGREWRITE  clangRewrite /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGAST  clangAST /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGPARSE  clangParse /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGLEX  clangLex /usr/lib64/llvm ${LLVM_LIBDIR})
    FIND_LIBRARY(CLANGBASIC  clangBasic /usr/lib64/llvm ${LLVM_LIBDIR})
    SET(LLVM_LIBS ${CLANGFRONTEND} ${CLANGSERIALIZATION} ${CLANGDRIVER} ${CLANGCODEGEN} ${CLANGSEMA} ${CLANGCHECKER} ${CLANGINDEX} ${CLANGANALYSIS} ${CLANGREWRITE} ${CLANGAST} ${CLANGPARSE} ${CLANGLEX} ${CLANGBASIC} ${LLVM_LIBS} ${PTHREADS_LIB} ${DL_LIB})
#    SET(LLVM_LIBS ${CLANG} ${LLVM_LIBS} ${PTHREADS_LIB} ${DL_LIB})
  endif (MSVC)
  if (NOT LLVM_VERSION STREQUAL "2.9" AND NOT LLVM_VERSION STREQUAL "2.8")
    message(FATAL_ERROR "LLVM version 2.9 or 2.8 required.")
    set(USE_LLVM FALSE)
  else ()
    set(LLVM_FOUND TRUE)
    include_directories( ${LLVM_INCLUDE_DIR} )
    add_definitions( -DHAVE_LLVM )
    SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${LLVM_LIBS})
    if (LLVM_VERSION STREQUAL "2.8")
      add_definitions( -DLLVM_28)
    endif()
  endif ()
ENDIF (USE_LLVM)


##############################################################################
#Extra libraries
IF(NOT MSVC)
  FIND_LIBRARY(NCURSES_LIBRARY NAMES ncurses DOC "Location of the ncurses library." PATH /mingw/lib)
  SET(EXTRA_LIBRARIES ${NCURSES_LIBRARY}  )
ENDIF(NOT MSVC)

INCLUDE_DIRECTORIES( libs/libFreeMat libs/libGraphics
libs/libMex libs/libXP libs/libCore libs/libFN libs/libMatC
libs/libMath/libDynBlas ${QT_LIBRARIES} src toolbox/jit)


##############################################################################
# BLAS is always linked dynamically
ADD_DEFINITIONS( -DDYN_BLAS )


if( MSVC )
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_WARNINGS -DCRT_NONSTDC_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_DEPRECATE )
  #add_definitions( -wd4146 -wd4503 -wd4996 -wd4800 -wd4244 -wd4624 )
  #add_definitions( -wd4355 -wd4715 )
endif( MSVC )

ADD_SUBDIRECTORY( libs )
ADD_SUBDIRECTORY( src )

####################################################################################
#Install section
IF( MSVC )
	FILE(GLOB INST_DLL "dependencies/bin/*.dll" )
	INSTALL(FILES ${INST_DLL} DESTINATION bin )
ENDIF( MSVC )

####################################################################################
#Setup section

INCLUDE(InstallRequiredSystemLibraries)

GET_TARGET_PROPERTY(FreeMat_Loc FreeMat LOCATION)
# Do not run helpgen automatically -- there are too many things that
# have to be checked after it is run.
#INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${FreeMat_Loc} -f run_helpgen WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})" )
#INSTALL(DIRECTORY toolbox DESTINATION share/FreeMat PATTERN "*.m" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
#INSTALL(DIRECTORY help DESTINATION share/FreeMat PATTERN "*" PERMISSIONS WORLD_READ )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat")

SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat is a free environment for rapid engineering and scientific prototyping and data processing.")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "FreeMat")

IF(UNIX AND NOT WIN32 AND NOT APPLE)
  SET(INST_DIR "share/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
  INSTALL(DIRECTORY toolbox DESTINATION ${INST_DIR} PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/html DESTINATION "${INST_DIR}/help" PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/text DESTINATION "${INST_DIR}/help" PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(FILES "help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf" DESTINATION "${INST_DIR}/help/pdf" )
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

IF(WIN32)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\FreeMat.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "FreeMat")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\freemat.sf.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\freemat.sf.net")
  #SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  INSTALL(FILES
	${QT_LIBRARY_DIR}/QtCore4.dll
	${QT_LIBRARY_DIR}/QtGui4.dll
	${QT_LIBRARY_DIR}/QtNetwork4.dll
	${QT_LIBRARY_DIR}/QtOpenGL4.dll
	${QT_LIBRARY_DIR}/QtXml4.dll
	${QT_LIBRARY_DIR}/QtSvg4.dll
	${QT_LIBRARY_DIR}/QtWebKit4.dll
	DESTINATION bin PERMISSIONS WORLD_EXECUTE )

  INSTALL(FILES
	${QT_LIBRARY_DIR}/../plugins/imageformats/qjpeg4.dll
	${QT_LIBRARY_DIR}/../plugins/imageformats/qgif4.dll
	${QT_LIBRARY_DIR}/../plugins/imageformats/qsvg4.dll
	${QT_LIBRARY_DIR}/../plugins/imageformats/qtiff4.dll
	DESTINATION bin/imageformats PERMISSIONS WORLD_EXECUTE )

  INSTALL(DIRECTORY toolbox DESTINATION . PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/html DESTINATION help PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(DIRECTORY help/text DESTINATION help PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
  INSTALL(FILES "help/latex/FreeMat-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.pdf" DESTINATION help/pdf )

ENDIF(WIN32)

IF( MINGW )
    FIND_LIBRARY(LIBSTDCPP_DLL libstdc++-6.dll)
    FIND_LIBRARY(LIBSTDC_DLL libgcc_s_sjlj-1.dll)
    FIND_LIBRARY(LIBBLAS_GENERIC blas_generic.dll ${LOCAL_PATH} )
  INSTALL(FILES
    ${LIBSTDCPP_DLL} ${LIBSTDC_DLL} ${FFTW_LIBRARY} ${FFTWF_LIBRARY} ${LIBBLAS_GENERIC}
	DESTINATION bin PERMISSIONS WORLD_EXECUTE )

ENDIF( MINGW )

SET(CPACK_PACKAGE_EXECUTABLES "FreeMat" "FreeMat")


INCLUDE(CPack)

#TODO Handle gfortran vs g77 issue: libgfortran vs libg2c

#add_custom_target( helpgen
#	COMMENT "Building help files"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	COMMAND ${FreeMat_Loc} "-nogui" "-p" ${CMAKE_SOURCE_DIR} "-f" "fraggen ${CMAKE_SOURCE_DIR}/help/tmp"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergefragments.py ${CMAKE_SOURCE_DIR}/help
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
#	VERBATIM
#	)

add_custom_target( fragdb
  COMMENT "Building fragment database"
  COMMAND ${FreeMat_Loc} "-nogui" "-r" ${CMAKE_SOURCE_DIR} "-f" "fraggen ${CMAKE_SOURCE_DIR}/doc/fragments"
  VERBATIM
  )

#add_custom_target( helpgen_fast
#	COMMENT "Building help files (no fragment generation)"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergefragments.py ${CMAKE_SOURCE_DIR}/help
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
#	VERBATIM
#	)

#add_custom_target( helpgen_only
#	COMMENT "Building help files"
#	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/helpgen.py ${CMAKE_SOURCE_DIR}
#	VERBATIM
#	)

add_custom_target( mergem
	COMMENT "Merging M files"
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/mergemfiles.py ${CMAKE_SOURCE_DIR}/src
	VERBATIM
	)

add_custom_target( jitgen
        COMMENT "Building list of scalar JIT functions"
	COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/jitgen.py ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/toolbox/jit/CScalarFuncs.hpp
	VERBATIM
	)


WRITE_OUT_ALL_VARIABLES()

ENABLE_TESTING()
add_subdirectory(tests)

find_package(Doxygen)

if (DOXYGEN_FOUND)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR/doc/Doxyfile} 
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
  DEPENDS fragdb
  COMMENT "Generating documentation"
)
endif(DOXYGEN_FOUND)

add_custom_target(index
  COMMENT "Building index"
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/makeindex.py ${CMAKE_SOURCE_DIR}/help/xml/index.xml ${CMAKE_SOURCE_DIR}/help/modules.txt
  DEPENDS doc
  VERBATIM
)

add_custom_target(latexdoc
  COMMENT "LaTeX Documentation"
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help/latex
  DEPENDS doc
)

add_custom_target(textdoc
  COMMENT "Text generation"
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/help/maketxt.sh"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help
  DEPENDS doc
)

add_custom_target(fulldoc
  DEPENDS fragdb doc index latexdoc textdoc
)
