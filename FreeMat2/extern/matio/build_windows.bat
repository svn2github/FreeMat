set CC=cl
set FC=ifort
set LD=LINK
set AR=LIB
set prefix=c:\cygwin\home\chulbert\matio_v1.1.4
set exec_prefix=c:\cygwin\home\chulbert\matio_v1.1.4

REM Set to 1 to build the fortran interface, otherwise set to 0
set build_fortran=1
REM Set build_zlib to 1 to build the zlib with matio, or set to 0 and uncomment
REM the ZLIB_CFLAGS and ZLIB_LIBS if zlib has already been built
set build_zlib=1
REM set ZLIB_CFLAGS=-Ic:\include
REM set ZLIB_LIBS=-LIBPATH:"c:\lib" zlib.lib

cd build-windows
nmake
nmake install
cd ..
