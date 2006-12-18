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
XWIN_QT="qt-win-opensource-4.2.2-mingw.exe"
MINGW_TARGET="i686-mingw32"
FFTW_FILE="fftw-3.1.2.tar.gz"
FFTW="fftw-3.1.2"
FFCALL_FILE="ffcall-1.10.tar.gz"
FFCALL="ffcall-1.10"
SUITESPARSE_FILE="SuiteSparse-2.3.1.tar.gz"
SUITESPARSE="SuiteSparse"
BASE=$PWD

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
	wget $2/$1
    fi
}

UnpackTarBall()
{
    MakeDirectory $BASE/Root
    cd $BASE/Root
    if [ ! -d $BASE/Root/$1 ]
	then
	echo "Unpacking $1 from $2..."
	tar xf $BASE/Files/$2
    fi
}

# First is the filename, then the URL, then the directory it creates
DownloadAndUnpackTarBall()
{
    DownloadFile $1 $2
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

SetupXWin()
{
    PATH=$PATH:$BASE/Cross/bin
    PREFIX=$BASE/Cross
    DownloadAndUnpackTarBall $XWIN_GCC_FILE http://downloads.sf.net/mingw $XWIN_GCC
    DownloadAndUnpackTarBall $XWIN_GPP_FILE http://downloads.sf.net/mingw $XWIN_GCC
    DownloadAndUnpackTarBall $XWIN_G77_FILE http://downloads.sf.net/mingw $XWIN_GCC
    DownloadAndUnpackTarBall $XWIN_MINGW_RUNTIME_FILE http://downloads.sf.net/mingw $XWIN_MINGW_RUNTIME
    DownloadAndUnpackTarBall $XWIN_WIN32API_FILE http://downloads.sf.net/mingw $XWIN_WIN32API
    DownloadAndUnpackTarBall $XWIN_BINUTILS_FILE http://downloads.sf.net/mingw $XWIN_BINUTILS
#    DownloadFile $XWIN_QT  ftp://ftp.trolltech.com/qt/source
    DownloadAndUnpackTarBall $FFTW_FILE http://www.fftw.org $FFTW
    DownloadAndUnpackTarBall  $FFCALL_FILE ftp://ftp.santafe.edu/pub/gnu $FFCALL
    DownloadAndUnpackTarBall  $SUITESPARSE_FILE http://www.cise.ufl.edu/research/sparse/SuiteSparse $SUITESPARSE
#    MakeDirectory $PREFIX
#    ConfigureMakeInstall $XWIN_BINUTILS "--target=$MINGW_TARGET --prefix=$PREFIX" .
#    MakeDirectory $PREFIX/include
#    Link $PREFIX/include $PREFIX/$MINGW_TARGET/include
#    cp -r $BASE/Root/$XWIN_MINGW_RUNTIME/include/* $PREFIX/include
#    cp -r $BASE/Root/$XWIN_WIN32API/include/* $PREFIX/include
#    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c" .
#    ConfigureMakeInstall $XWIN_WIN32API "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" .
#    Link $BASE/Root/$XWIN_WIN32API $BASE/Root/w32api
#    mv $PREFIX/$MINGW_TARGET/lib/* $PREFIX/lib
#    rmdir $PREFIX/$MINGW_TARGET/lib
#    Link $PREFIX/lib $PREFIX/$MINGW_TARGET/lib
#    cd $BASE/Root/$XWIN_MINGW_RUNTIME 
#    find . -name configure -exec dos2unix \{\} \;
#    dos2unix config.guess config.sub mkinstalldirs
#    cp $PREFIX/lib/*.a $BASE/Root/$XWIN_MINGW_RUNTIME/mingwex/.
#    ConfigureMakeInstall $XWIN_MINGW_RUNTIME "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" .
#    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c,c++,f77" .
#    MakeDirectory $BASE/Root/$FFTW/single
#    MakeDirectory $BASE/Root/$FFTW/double
#    ConfigureMakeInstall $FFTW/single "--prefix=$PREFIX --host=$MINGW_TARGET --build=$(../config.guess) --enable-single" ..
#    ConfigureMakeInstall $FFTW/double "--prefix=$PREFIX --host=$MINGW_TARGET --build=$(../config.guess)" ..
#    ConfigureMakeInstall $FFCALL "--prefix=$PREFIX --host=$MINGW_TARGET --build=$(../config.guess)" .
    MakeOverride $SUITESPARSE/AMD/Source $MINGW_TARGET
    cp $BASE/Root/$SUITESPARSE/AMD/Lib/libamd.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/AMD/Include/*.h $PREFIX/include
    MakeOverride $SUITESPARSE/UMFPACK/Source $MINGW_TARGET
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Lib/libumfpack.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Include/*.h $PREFIX/include
}


SetupXWin
