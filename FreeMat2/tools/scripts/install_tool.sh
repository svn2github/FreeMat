#! /bin/sh

# These may evolve with time
XWIN_GCC_FILE="gcc-core-3.4.5-20060117-1-src.tar.gz"
XWIN_GPP_FILE="gcc-g++-3.4.5-20060117-1-src.tar.gz"
XWIN_G77_FILE="gcc-g77-3.4.5-20060117-1-src.tar.gz"
XWIN_GCC="gcc-3.4.5-20060117-1"
XWIN_MINGW_RUNTIME_FILE="mingw-runtime-3.9-src.tar.gz"
XWIN_MINGW_RUNTIME="mingw-runtime-3.9"
XWIN_WIN32API_FILE="w32api-3.6-src.tar.gz"
XWIN_WIN32API="w32api-3.6"
XWIN_BINUTILS_FILE="binutils-2.16.91-20060119-1-src.tar.gz"
XWIN_BINUTILS="binutils-2.16.91-20060119-1"
XWIN_QT_VER="4.2.2"
XWIN_QT="qt-win-opensource-$XWIN_QT_VER-mingw.exe"
MINGW_TARGET="i686-mingw32"
FFTW_FILE="fftw-3.1.2.tar.gz"
FFTW="fftw-3.1.2"
FFCALL_FILE="ffcall-1.10.tar.gz"
FFCALL="ffcall-1.10"
SUITESPARSE_FILE="SuiteSparse-2.3.1.tar.gz"
SUITESPARSE="SuiteSparse"
LAPACK_FILE="lapack-3.1.0.tgz"
LAPACK="lapack-3.1.0"
BASE=$PWD
MINGW_ATLAS_FILE="atlas3.6.0_WinNT_P2.zip"
ARPACK_FILE="arpack96.tar.Z"
ARPACK_URL="http://www.caam.rice.edu/software/ARPACK/SRC"
ARPACK="ARPACK"
FREEMAT="FreeMat-3.0"
FREEMAT_FILE="$FREEMAT.tar.gz"
XWIN_ZLIB_FILE="zlib-1.2.3.tar.gz"
XWIN_ZLIB="zlib-1.2.3"

MakeDirectory()
{
    if [ ! -d $1 ] 
	then
	echo "Making directory $1..."
	mkdir -p $1
    fi
}

DownloadFile()
{
    MakeDirectory $BASE/Files
    if [ ! -f $BASE/Files/$1 ] 
	then
	echo "Retrieving $1 from URL $2..."
	cd $BASE/Files
	wget $2$1 -O $1
    fi
}

UnpackTarBall()
{
    MakeDirectory $BASE/Root
    cd $BASE/Root
    if [ ! -d $BASE/Root/$1 ]
	then
	echo "Unpacking $1 from $2..."
	tar xfz $BASE/Files/$2
    fi
}

# First is the filename, then the URL, then the directory it creates
DownloadAndUnpackTarBall()
{
    DownloadFile $1 "$2/"
    UnpackTarBall $3 $1
}

CrossConfigureAndBuildBinUtils()
{
    MakeDirectory $BASE/Cross
    cd $BASE/Root/$1
    ./configure --target=$MINGW_TARGET --prefix=$BASE/Cross
    make && make install
}

Link()
{
    ln -s $1 $2
}

ConfigureMakeInstall()
{
    echo "Configuring package $1..."
    cd $BASE/Root/$1
    $3/configure $2
    make
    make install
}

MakeOverride()
{
    echo "Making package $1..."
    cd $BASE/Root/$1
    make CC=$2-gcc AR="$2-ar cr" RANLIB=$2-ranlib 
}


SetupXWinCommon()
{
    PATH=$PATH:$BASE/Cross/bin
    PREFIX=$BASE/Cross
    MakeDirectory $PREFIX
    MakeDirectory $PREFIX/include
}

SetupXWinBinUtils()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $XWIN_BINUTILS_FILE http://downloads.sf.net/mingw $XWIN_BINUTILS
    ConfigureMakeInstall $XWIN_BINUTILS "--target=$MINGW_TARGET --prefix=$PREFIX" .
}

SetupXWinGCCFirst()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $XWIN_GCC_FILE http://downloads.sf.net/mingw $XWIN_GCC
    DownloadAndUnpackTarBall $XWIN_GPP_FILE http://downloads.sf.net/mingw foo
    DownloadAndUnpackTarBall $XWIN_G77_FILE http://downloads.sf.net/mingw foo
    DownloadAndUnpackTarBall $XWIN_MINGW_RUNTIME_FILE http://downloads.sf.net/mingw $XWIN_MINGW_RUNTIME
    DownloadAndUnpackTarBall $XWIN_WIN32API_FILE http://downloads.sf.net/mingw $XWIN_WIN32API
    Link $PREFIX/include $PREFIX/$MINGW_TARGET/include
    cp -r $BASE/Root/$XWIN_MINGW_RUNTIME/include/* $PREFIX/include
    cp -r $BASE/Root/$XWIN_WIN32API/include/* $PREFIX/include
    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c" .
}

SetupXWinWin32API()
{
    SetupXWinCommon
    ConfigureMakeInstall $XWIN_WIN32API "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" .
}

SetupXWinMingwRuntime()
{
    SetupXWinCommon
    Link $BASE/Root/$XWIN_WIN32API $BASE/Root/w32api
    mv $PREFIX/$MINGW_TARGET/lib/* $PREFIX/lib
    rmdir $PREFIX/$MINGW_TARGET/lib
    Link $PREFIX/lib $PREFIX/$MINGW_TARGET/lib
    cd $BASE/Root/$XWIN_MINGW_RUNTIME 
    find . -name configure -exec dos2unix \{\} \;
    dos2unix config.guess config.sub mkinstalldirs
    cp $PREFIX/lib/*.a $BASE/Root/$XWIN_MINGW_RUNTIME/mingwex/.
    ConfigureMakeInstall $XWIN_MINGW_RUNTIME "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" .
}

SetupXWinGCC()
{
    SetupXWinCommon
    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c,c++,f77" .
}

SetupXWinFFTW()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $FFTW_FILE http://www.fftw.org $FFTW
    MakeDirectory $BASE/Root/$FFTW/single
    MakeDirectory $BASE/Root/$FFTW/double
    ConfigureMakeInstall $FFTW/single "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/Root/$FFTW/config.guess) --enable-single" ..
    ConfigureMakeInstall $FFTW/double "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/Root/$FFTW/config.guess)" ..
}

SetupXWinFFCALL()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $FFCALL_FILE ftp://ftp.santafe.edu/pub/gnu $FFCALL
    ConfigureMakeInstall $FFCALL "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/Root/$FFCALL/autoconf/config.guess)" .
    $MINGW_TARGET-ranlib $PREFIX/lib/libavcall.a
}

SetupXWinSparse()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $SUITESPARSE_FILE http://www.cise.ufl.edu/research/sparse/SuiteSparse $SUITESPARSE
    MakeOverride $SUITESPARSE/AMD/Source $MINGW_TARGET
    cp $BASE/Root/$SUITESPARSE/AMD/Lib/libamd.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/AMD/Include/*.h $PREFIX/include
    MakeOverride $SUITESPARSE/UMFPACK/Source $MINGW_TARGET
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Lib/libumfpack.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Include/*.h $PREFIX/include
    cp $BASE/Root/$SUITESPARSE/UFconfig/UFconfig.h $PREFIX/include
}

SetupXWinLAPACK()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $LAPACK_FILE http://www.netlib.org/lapack $LAPACK
    cd $BASE/Root/$LAPACK
    touch make.inc
    cd $BASE/Root/$LAPACK/INSTALL
    make FORTRAN="$MINGW_TARGET-g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="$MINGW_TARGET-ranlib" ARCH="$MINGW_TARGET-ar" ARCHFLAGS="cr" LOADER="$MINGW_TARGET-g77"
    cd $BASE/Root/$LAPACK/SRC
    make FORTRAN="$MINGW_TARGET-g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="$MINGW_TARGET-ranlib" ARCH="$MINGW_TARGET-ar" ARCHFLAGS="cr"
    cp $BASE/Root/$LAPACK/liblapack.a $PREFIX/lib
}

SetupXWinATLAS()
{
    SetupXWinCommon
    DownloadFile $MINGW_ATLAS_FILE "http://www.scipy.org/Installing_SciPy/Windows?action=AttachFile&do=get&target="
    MakeDirectory $BASE/Root/ATLAS
    cd $BASE/Root/ATLAS
    unzip $BASE/Files/$MINGW_ATLAS_FILE
    cp libatlas.a $PREFIX/lib/.
    cp libf77blas.a $PREFIX/lib/.
    cp libcblas.a $PREFIX/lib/.
}

SetupXWinARPACK()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $ARPACK_FILE $ARPACK_URL $ARPACK
    cd $BASE/Root/$ARPACK/SRC
    make FC="$MINGW_TARGET-g77" FFLAGS="-O2" AR="$MINGW_TARGET-ar" ARFLAGS="rv" ARPACKLIB="../libarpack.a" RANLIB="$MINGW_TARGET-ranlib" all
    cd $BASE/Root/$ARPACK/UTIL
    make FC="$MINGW_TARGET-g77" FFLAGS="-O2" AR="$MINGW_TARGET-ar" ARFLAGS="rv" ARPACKLIB="../libarpack.a" RANLIB="$MINGW_TARGET-ranlib" all
    cp $BASE/Root/$ARPACK/libarpack.a $PREFIX/lib
}

SetupXWinZlib()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $XWIN_ZLIB_FILE http://downloads.sourceforge.net/libpng $XWIN_ZLIB
    cd $BASE/Root/$XWIN_ZLIB
    make CC="$MINGW_TARGET-gcc" AR="$MINGW_TARGET-ar rc" RANLIB="$MINGW_TARGET-ranlib"
    cp zlib.h zconf.h $PREFIX/include/.
    cp libz.a $PREFIX/lib/.
}


SetupXWinQt()
{
    SetupXWinCommon
    DownloadFile $XWIN_QT ftp://ftp.trolltech.com/qt/source/
    cd $BASE/Files
    wine $XWIN_QT
    cp -R ~/.wine/drive_c/Qt $PREFIX/.
}

SetupXWinFreeMat()
{
    SetupXWinCommon
    cd $BASE/Root
    tar xfz $BASE/Files/$FREEMAT_FILE
    MakeDirectory $BASE/Root/$FREEMAT/build
    cd $BASE/Root/$FREEMAT/build
    ../configure --prefix=$PREFIX --host=$MINGW_TARGET --build=$(../config.guess) --with-qt4dir=$PREFIX/Qt/$XWIN_QT_VER CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib
    make
}

Usage() 
{
    echo \
"FreeMat Source Code Installation Helper Script

This script performs some useful tasks (for me) when building 
FreeMat from source (especially on new machines).  It requries
web access (via wget) to work automatically.  Alternately, you
can download the required files and put them in a 'Files' 
subdirectory.  Here are the tasks manages by this script.

      --xwin-binutils    Setup the Win32 cross compilation binary utilities
      --xwin-gccfirst    First pass (bootstrap) Win32 cross build of gcc
      --xwin-win32api    Setup the Win32 cross API
      --xwin-mingw       Setup the Win32 cross MINGW runtime
      --xwin-gcc         Setup Win32 cross build of gcc, g++, g77
      --xwin-fftw        Setup the Win32 cross of FFTW
      --xwin-ffcall      Setup the Win32 cross of ffcall
      --xwin-sparse      Setup the Win32 cross of SuiteSparse
      --xwin-lapack      Setup the Win32 cross of Lapack
      --xwin-atlas       Setup the Win32 cross of ATLAS
      --xwin-arpack      Setup the Win32 cross of ARPACK
      --xwin-qt          Setup the Win32 cross of QT
      --xwin-zlib        Setup the Win32 cross of zlib
      --xwin-freemat     Build the Win32 cross of FreeMat
      --xwin-all         Setup the Win32 cross compilation (all steps)
"
    exit
}

# Parse the input arguments
build_xwin_binutils="no"
build_xwin_gccfirst="no"
build_xwin_win32api="no"
build_xwin_mingw="no"
build_xwin_gcc="no"
build_xwin_fftw="no"
build_xwin_ffcall="no"
build_xwin_sparse="no"
build_xwin_lapack="no"
build_xwin_atlas="no"
build_xwin_arpack="no"
build_xwin_qt="no"
build_xwin_zlib="no"
build_xwin_freemat="no"
build_xwin_all="no"

for arg
  do
  case $arg in 
      --xwin-binutils) build_xwin_binutils="yes" ;;
      --xwin-gccfirst) build_xwin_gccfirst="yes" ;;
      --xwin-win32api) build_xwin_win32api="yes" ;;
      --xwin-mingw)    build_xwin_mingw="yes" ;;
      --xwin-gcc)      build_xwin_gcc="yes" ;;
      --xwin-fftw)     build_xwin_fftw="yes" ;;
      --xwin-ffcall)   build_xwin_ffcall="yes" ;;
      --xwin-sparse)   build_xwin_sparse="yes" ;;
      --xwin-lapack)   build_xwin_lapack="yes" ;;
      --xwin-atlas)    build_xwin_atlas="yes" ;;
      --xwin-arpack)   build_xwin_arpack="yes" ;;
      --xwin-qt)       build_xwin_qt="yes" ;;
      --xwin-zlib)     build_xwin_zlib="yes" ;;
      --xwin-freemat)  build_xwin_freemat="yes" ;;
      --xwin-all)      build_xwin_all="yes" ;;
      *)               Usage;
  esac
done

if test "$build_xwin_binutils" == "yes"; then
    SetupXWinBinUtils
fi
if test "$build_xwin_gccfirst" == "yes"; then
    SetupXWinGCCFirst
fi
if test "$build_xwin_win32api" == "yes"; then
    SetupXWinWin32API
fi
if test "$build_xwin_mingw" == "yes"; then
    SetupXWinMingwRuntime
fi
if test "$build_xwin_gcc" == "yes"; then
    SetupXWinGCC
fi
if test "$build_xwin_fftw" == "yes"; then
    SetupXWinFFTW
fi
if test "$build_xwin_ffcall" == "yes"; then
    SetupXWinFFCALL
fi
if test "$build_xwin_sparse" == "yes"; then
    SetupXWinSparse
fi
if test "$build_xwin_lapack" == "yes"; then
    SetupXWinLAPACK
fi
if test "$build_xwin_atlas" == "yes"; then
    SetupXWinATLAS
fi
if test "$build_xwin_arpack" == "yes"; then
    SetupXWinARPACK
fi
if test "$build_xwin_qt" == "yes"; then
    SetupXWinQt
fi
if test "$build_xwin_zlib" == "yes"; then
    SetupXWinZlib
fi
if test "$build_xwin_freemat" == "yes"; then
    SetupXWinFreeMat
fi
if test "$build_xwin_all" == "yes"; then
    SetupXWinBinUtils
    SetupXWinGCCFirst
    SetupXWinWin32API
    SetupXWinMingwRuntime
    SetupXWinGCC
    SetupXWinFFTW
    SetupXWinFFCALL
    SetupXWinSparse
    SetupXWinLAPACK
    SetupXWinATLAS
    SetupXWinARPACK
    SetupXWinQt
    SetupXWinZlib
    SetupXWinFreeMat
fi
