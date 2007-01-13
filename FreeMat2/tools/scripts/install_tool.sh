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
QT_MAC_FILE="qt-mac-opensource-src-4.2.2.tar.gz"
QT_MAC="qt-mac-opensource-src-4.2.2"
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
VERSION="3.0"
FREEMAT="FreeMat-$VERSION"
FREEMAT_FILE="$FREEMAT.tar.gz"
ZLIB_FILE="zlib-1.2.3.tar.gz"
ZLIB="zlib-1.2.3"
XWIN_NSIS="nsis-2.22-setup.exe"
QT_PLUGIN_DIR="/usr/lib/qt4/plugins/imageformats"

MakeDirectory()
{
    if [ ! -d $1 ] 
	then
	echo "Making directory $1..."
	mkdir -p $1
    fi
}

CopyFile()
{
  echo "Copying file $1 to $2"
  cp $1 $2
}

CopyDirectory()
{
  echo "Copying directory $1 to $2"
  test -d $2 && rm -rf $2
  cp -R $1 $2
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
    MakeDirectory $BASE/$3
    cd $BASE/$3
    if [ ! -d $BASE/$3/$1 ]
	then
	echo "Unpacking $1 from $2..."
	tar xfz $BASE/Files/$2
    fi
}

# First is the filename, then the URL, then the directory it creates
DownloadAndUnpackTarBall()
{
    DownloadFile $1 "$2/"
    UnpackTarBall $3 $1 $4
}

CrossConfigureAndBuildBinUtils()
{
    MakeDirectory $BASE/Cross
    cd $BASE/XRoot/$1
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
    cd $BASE/$4/$1
    $3/configure $2
    make
    make install
}

MakeOverride()
{
    echo "Making package $1..."
    cd $BASE/XRoot/$1
    make CC=$2-gcc AR="$2-ar cr" RANLIB=$2-ranlib 
}


SetupXWinCommon()
{
    PATH=$PATH:$BASE/Cross/bin
    PREFIX=$BASE/Cross
    MakeDirectory $PREFIX
    MakeDirectory $PREFIX/bin
    MakeDirectory $PREFIX/lib
    MakeDirectory $PREFIX/include
}

SetupCommon()
{
    PATH=$PATH:$BASE/Build/bin
    PREFIX=$BASE/Build
    MakeDirectory $PREFIX
    MakeDirectory $PREFIX/bin
    MakeDirectory $PREFIX/lib
    MakeDirectory $PREFIX/include
}

SetupXWinBinUtils()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $XWIN_BINUTILS_FILE http://downloads.sf.net/mingw $XWIN_BINUTILS XRoot
    ConfigureMakeInstall $XWIN_BINUTILS "--target=$MINGW_TARGET --prefix=$PREFIX" . XRoot
}

SetupXWinGCCFirst()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $XWIN_GCC_FILE http://downloads.sf.net/mingw $XWIN_GCC XRoot
    DownloadAndUnpackTarBall $XWIN_GPP_FILE http://downloads.sf.net/mingw foo XRoot
    DownloadAndUnpackTarBall $XWIN_G77_FILE http://downloads.sf.net/mingw foo XRoot
    DownloadAndUnpackTarBall $XWIN_MINGW_RUNTIME_FILE http://downloads.sf.net/mingw $XWIN_MINGW_RUNTIME XRoot
    DownloadAndUnpackTarBall $XWIN_WIN32API_FILE http://downloads.sf.net/mingw $XWIN_WIN32API XRoot
    Link $PREFIX/include $PREFIX/$MINGW_TARGET/include
    cp -r $BASE/XRoot/$XWIN_MINGW_RUNTIME/include/* $PREFIX/include
    cp -r $BASE/XRoot/$XWIN_WIN32API/include/* $PREFIX/include
    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c" . XRoot
}

SetupXWinWin32API()
{
    SetupXWinCommon
    ConfigureMakeInstall $XWIN_WIN32API "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" . XRoot
}

SetupXWinMingwRuntime()
{
    SetupXWinCommon
    Link $BASE/XRoot/$XWIN_WIN32API $BASE/XRoot/w32api
    mv $PREFIX/$MINGW_TARGET/lib/* $PREFIX/lib
    rmdir $PREFIX/$MINGW_TARGET/lib
    Link $PREFIX/lib $PREFIX/$MINGW_TARGET/lib
    cd $BASE/XRoot/$XWIN_MINGW_RUNTIME 
    find . -name configure -exec dos2unix \{\} \;
    dos2unix config.guess config.sub mkinstalldirs
    cp $PREFIX/lib/*.a $BASE/XRoot/$XWIN_MINGW_RUNTIME/mingwex/.
    ConfigureMakeInstall $XWIN_MINGW_RUNTIME "--prefix=$PREFIX --target=$MINGW_TARGET --host=$MINGW_TARGET --build=$(./config.guess)" . XRoot
}

SetupXWinGCC()
{
    SetupXWinCommon
    ConfigureMakeInstall $XWIN_GCC "--prefix=$PREFIX --target=$MINGW_TARGET --enable-threads --enable-languages=c,c++,f77" . XRoot
}

SetupXWinFFTW()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $FFTW_FILE http://www.fftw.org $FFTW XRoot
    MakeDirectory $BASE/XRoot/$FFTW/single
    MakeDirectory $BASE/XRoot/$FFTW/double
    ConfigureMakeInstall $FFTW/single "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/XRoot/$FFTW/config.guess) --enable-single" .. XRoot
    ConfigureMakeInstall $FFTW/double "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/XRoot/$FFTW/config.guess)" .. XRoot
}

SetupXWinFFCALL()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $FFCALL_FILE ftp://ftp.santafe.edu/pub/gnu $FFCALL XRoot
    ConfigureMakeInstall $FFCALL "--prefix=$PREFIX --host=$MINGW_TARGET --build=$($BASE/XRoot/$FFCALL/autoconf/config.guess)" . XRoot
    $MINGW_TARGET-ranlib $PREFIX/lib/libavcall.a
}

SetupXWinSparse()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $SUITESPARSE_FILE http://www.cise.ufl.edu/research/sparse/SuiteSparse $SUITESPARSE XRoot
    MakeOverride $SUITESPARSE/AMD/Source $MINGW_TARGET
    cp $BASE/XRoot/$SUITESPARSE/AMD/Lib/libamd.a $PREFIX/lib
    cp $BASE/XRoot/$SUITESPARSE/AMD/Include/*.h $PREFIX/include
    MakeOverride $SUITESPARSE/UMFPACK/Source $MINGW_TARGET
    cp $BASE/XRoot/$SUITESPARSE/UMFPACK/Lib/libumfpack.a $PREFIX/lib
    cp $BASE/XRoot/$SUITESPARSE/UMFPACK/Include/*.h $PREFIX/include
    cp $BASE/XRoot/$SUITESPARSE/UFconfig/UFconfig.h $PREFIX/include
}

SetupXWinLAPACK()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall  $LAPACK_FILE http://www.netlib.org/lapack $LAPACK XRoot
    cd $BASE/XRoot/$LAPACK
    touch make.inc
    cd $BASE/XRoot/$LAPACK/INSTALL
    make FORTRAN="$MINGW_TARGET-g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="$MINGW_TARGET-ranlib" ARCH="$MINGW_TARGET-ar" ARCHFLAGS="cr" LOADER="$MINGW_TARGET-g77"
    cd $BASE/XRoot/$LAPACK/SRC
    make FORTRAN="$MINGW_TARGET-g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="$MINGW_TARGET-ranlib" ARCH="$MINGW_TARGET-ar" ARCHFLAGS="cr"
    cp $BASE/XRoot/$LAPACK/liblapack.a $PREFIX/lib
}

SetupXWinATLAS()
{
    SetupXWinCommon
    DownloadFile $MINGW_ATLAS_FILE "http://www.scipy.org/Installing_SciPy/Windows?action=AttachFile&do=get&target="
    MakeDirectory $BASE/XRoot/ATLAS
    cd $BASE/XRoot/ATLAS
    unzip $BASE/Files/$MINGW_ATLAS_FILE
    cp libatlas.a $PREFIX/lib/.
    cp libf77blas.a $PREFIX/lib/.
    cp libcblas.a $PREFIX/lib/.
}

SetupXWinARPACK()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $ARPACK_FILE $ARPACK_URL $ARPACK XRoot
    cd $BASE/XRoot/$ARPACK/SRC
    make FC="$MINGW_TARGET-g77" FFLAGS="-O2" AR="$MINGW_TARGET-ar" ARFLAGS="rv" ARPACKLIB="../libarpack.a" RANLIB="$MINGW_TARGET-ranlib" all
    cd $BASE/XRoot/$ARPACK/UTIL
    make FC="$MINGW_TARGET-g77" FFLAGS="-O2" AR="$MINGW_TARGET-ar" ARFLAGS="rv" ARPACKLIB="../libarpack.a" RANLIB="$MINGW_TARGET-ranlib" all
    cp $BASE/XRoot/$ARPACK/libarpack.a $PREFIX/lib
}

SetupXWinZlib()
{
    SetupXWinCommon
    DownloadAndUnpackTarBall $ZLIB_FILE http://downloads.sourceforge.net/libpng $ZLIB XRoot
    cd $BASE/XRoot/$ZLIB
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

SetupXWinNSIS()
{
    SetupXWinCommon
    DownloadFile $XWIN_NSIS http://downloads.sourceforge.net/nsis/
    cd $BASE/Files
    wine $XWIN_NSIS
}


MakeCrossWinBundle()
{
    SetupXWinCommon
    baseDir="$BASE/XRoot/$FREEMAT/build/$FREEMAT-Win32"
    buildDir="$BASE/XRoot/$FREEMAT/build"
    srcDir="$BASE/XRoot/$FREEMAT"
    MakeDirectory "$baseDir"
    MakeDirectory "$baseDir/Contents"
    MakeDirectory "$baseDir/Contents/bin"
    MakeDirectory "$baseDir/Contents/Resources"
    MakeDirectory "$baseDir/Contents/Resources/help"
    MakeDirectory "$baseDir/Contents/Resources/help/html"
    MakeDirectory "$baseDir/Contents/Resources/help/text"
    MakeDirectory "$baseDir/Contents/Resources/help/pdf"
    MakeDirectory "$baseDir/Contents/Resources/toolbox"
    MakeDirectory "$baseDir/Contents/Plugins/imageformats"
    CopyFile "$BASE/Cross/Qt/$XWIN_QT_VER/plugins/imageformats/*.dll" "$baseDir/Contents/Plugins/imageformats/"
    CopyFile "$buildDir/src/FreeMat.exe" "$baseDir/Contents/bin/FreeMat.exe"
    CopyFile "$BASE/Cross/Qt/$XWIN_QT_VER/bin/QtCore4.dll" "$baseDir/Contents/bin/QtCore4.dll"
    CopyFile "$BASE/Cross/Qt/$XWIN_QT_VER/bin/QtGui4.dll" "$baseDir/Contents/bin/QtGui4.dll"
    CopyFile "$BASE/Cross/Qt/$XWIN_QT_VER/bin/QtOpenGL4.dll" "$baseDir/Contents/bin/QtOpenGL4.dll"
    CopyFile "$BASE/Cross/Qt/$XWIN_QT_VER/bin/QtNetwork4.dll" "$baseDir/Contents/bin/QtNetwork4.dll"
    CopyFile "$BASE/Cross/bin/mingwm10.dll" "$baseDir/Contents/bin/mingwm10.dll"
    CopyDirectory "$srcDir/help/html" "$baseDir/Contents/Resources/help/html"
    CopyDirectory "$srcDir/help/text" "$baseDir/Contents/Resources/help/text"
    CopyDirectory "$srcDir/help/toolbox" "$baseDir/Contents/Resources/toolbox"
    CopyFile "$srcDir/help/latex/main.pdf" "$baseDir/Contents/Resources/help/pdf/$FREEMAT.pdf"
    echo "Generating NSI file..."
    cdir=`pwd`
    cd $baseDir
    find -type f -printf "  SetOutPath \"\$INSTDIR/%h\"\n  FILE \"%p\"\n" | sed -e 's@/@\\@g' > /tmp/blist
    find -type f -printf "  Delete \"\$INSTDIR/%p\"\n" | sed -e 's@/@\\@g' > /tmp/elist
    cd $cdir
    repcmd="s/<VERSION_NUMBER>/$VERSION/g"
    sed -e '/<BUNDLE FILES>/r /tmp/blist' -e '/<DELLIST>/r /tmp/elist' -e $repcmd -e 's/<BUNDLE FILES>//g' -e 's/<DELLIST>//g' < $srcDir/tools/disttool/freemat_nsi.in > $baseDir/freemat.nsi
    cd $baseDir
    wine ~/.wine/drive_c/Program\ Files/NSIS/makensis.exe freemat.nsi
}

SetupXWinFreeMat()
{
    SetupXWinCommon
    cd $BASE/XRoot
    tar xfz $BASE/Files/$FREEMAT_FILE
    MakeDirectory $BASE/XRoot/$FREEMAT/build
    cd $BASE/XRoot/$FREEMAT/build
    ../configure --prefix=$PREFIX --host=$MINGW_TARGET --build=$(../config.guess) --with-qt4dir=$PREFIX/Qt/$XWIN_QT_VER CPPFLAGS=-I$PREFIX/include LDFLAGS=-L$PREFIX/lib WINDRES="$MINGW_TARGET-windres"
    make
    MakeCrossWinBundle
}

SetupXWinAll()
{
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
    SetupXWinNSIS
    SetupXWinZlib
    SetupXWinFreeMat
}

SetupFFTW()
{
    SetupCommon
    DownloadAndUnpackTarBall $FFTW_FILE http://www.fftw.org $FFTW Root
    MakeDirectory $BASE/Root/$FFTW/single
    MakeDirectory $BASE/Root/$FFTW/double
    ConfigureMakeInstall $FFTW/single "--prefix=$PREFIX --enable-single" .. Root 
    ConfigureMakeInstall $FFTW/double "--prefix=$PREFIX" .. Root 
}

SetupZlib()
{
    SetupCommon
    DownloadAndUnpackTarBall $ZLIB_FILE http://downloads.sourceforge.net/libpng $ZLIB Root
    cd $BASE/Root/$ZLIB
    make 
    cp zlib.h zconf.h $PREFIX/include/.
    cp libz.a $PREFIX/lib/.
}

SetupSparse()
{
    SetupCommon
    DownloadAndUnpackTarBall  $SUITESPARSE_FILE http://www.cise.ufl.edu/research/sparse/SuiteSparse $SUITESPARSE Root
    cd $BASE/Root/$SUITESPARSE/AMD/Source
    make
    cp $BASE/Root/$SUITESPARSE/AMD/Lib/libamd.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/AMD/Include/*.h $PREFIX/include
    cd $BASE/Root/$SUITESPARSE/UMFPACK/Source
    make
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Lib/libumfpack.a $PREFIX/lib
    cp $BASE/Root/$SUITESPARSE/UMFPACK/Include/*.h $PREFIX/include
    cp $BASE/Root/$SUITESPARSE/UFconfig/UFconfig.h $PREFIX/include
}

SetupARPACK()
{
    SetupCommon
    DownloadAndUnpackTarBall $ARPACK_FILE $ARPACK_URL $ARPACK Root
    cd $BASE/Root/$ARPACK/UTIL
    if  [ ! -f second_orig.f ]
	then
	mv second.f second_orig.f
	touch second.f
    fi
    cd $BASE/Root/$ARPACK/SRC
    if  [ ! -f /usr/bin/g77 ] 
	then
	make FC="gfortran" FFLAGS="-O2" ARPACKLIB="../libarpack.a" all
	cd $BASE/Root/$ARPACK/UTIL
	make FC="gfortran" FFLAGS="-O2" ARPACKLIB="../libarpack.a" all
	cp $BASE/Root/$ARPACK/libarpack.a $PREFIX/lib
    else
	make FFLAGS="-O2" ARPACKLIB="../libarpack.a" all
	cd $BASE/Root/$ARPACK/UTIL
	make FFLAGS="-O2" ARPACKLIB="../libarpack.a" all
	cp $BASE/Root/$ARPACK/libarpack.a $PREFIX/lib
    fi
}

ImportLibs()
{
    libset=`ldd $1 | awk '{print $3}'`
    for lib in $libset
      do
      isXlib=`expr match $lib ".*libc\|.*libm\|.*libdl\|.*libpthread\|.*libX\|.*libSM\|.*libICE\|.*libGL\|.*linux"`
      if [ $isXlib == 0 ]
	  then
	  if [ -f $lib ]
	      then
	      CopyFile $lib $2
	  fi
      else
	  echo "Ignored $lib"
      fi
    done
}

MakeLinuxBundle()
{
    SetupCommon
    baseDir="$BASE/Root/$FREEMAT/build/$FREEMAT-Binary"
    buildDir="$BASE/Root/$FREEMAT/build"
    srcDir="$BASE/Root/$FREEMAT"
    rm -rf "$baseDir"
    MakeDirectory "$baseDir"
    MakeDirectory "$baseDir/Contents"
    MakeDirectory "$baseDir/Contents/bin"
    MakeDirectory "$baseDir/Contents/Resources"
    MakeDirectory "$baseDir/Contents/Resources/help"
    MakeDirectory "$baseDir/Contents/Resources/help/html"
    MakeDirectory "$baseDir/Contents/Resources/help/text"
    MakeDirectory "$baseDir/Contents/Resources/help/pdf"
    MakeDirectory "$baseDir/Contents/Resources/toolbox"
    MakeDirectory "$baseDir/Contents/Plugins/imageformats"
    CopyFile "$buildDir/src/FreeMat" "$baseDir/Contents/bin/FreeMatMain"
    MakeDirectory "$baseDir/Contents/lib"
    ImportLibs "$baseDir/Contents/bin/FreeMatMain" "$baseDir/Contents/lib"
    echo    "Making run script"
    cat > "$baseDir/Contents/bin/FreeMat" <<EOF
#!/bin/bash
mypath=\`which \$0\`
mypath=\${mypath%/*}
declare -x LD_LIBRARY_PATH=\$mypath/../lib
\$mypath/FreeMatMain \$*
EOF
    chmod +x "$baseDir/Contents/bin/FreeMat"
    CopyDirectory "$srcDir/help/html" "$baseDir/Contents/Resources/help/html"
    CopyDirectory "$srcDir/help/text" "$baseDir/Contents/Resources/help/text"
    CopyDirectory "$srcDir/help/toolbox" "$baseDir/Contents/Resources/toolbox"
    CopyFile "$srcDir/help/latex/main.pdf" "$baseDir/Contents/Resources/help/pdf/$FREEMAT.pdf"
    CopyFile "$QT_PLUGIN_DIR/libqjpeg.so" "$baseDir/Contents/Plugins/imageformats/libqjpeg.so"
    CopyFile "$QT_PLUGIN_DIR/libqmng.so" "$baseDir/Contents/Plugins/imageformats/libqmng.so"
    CopyFile "$QT_PLUGIN_DIR/libqsvg.so" "$baseDir/Contents/Plugins/imageformats/libqsvg.so"
    CopyFile "$QT_PLUGIN_DIR/libqgif.so" "$baseDir/Contents/Plugins/imageformats/libqgif.so"
    ImportLibs "$baseDir/Contents/Plugins/imageformats/libqjpeg.so" "$baseDir/Contents/lib"
    ImportLibs "$baseDir/Contents/Plugins/imageformats/libqmng.so" "$baseDir/Contents/lib"
    ImportLibs "$baseDir/Contents/Plugins/imageformats/libqsvg.so" "$baseDir/Contents/lib"
    ImportLibs "$baseDir/Contents/Plugins/imageformats/libqgif.so" "$baseDir/Contents/lib"
    echo "Making tar file"
    rm -rf $FREEMAT-Binary-Linux.tar.gz
    tar cfz $FREEMAT-Binary-Linux.tar.gz $FREEMAT-Binary
}

SetupFreeMat()
{
    SetupCommon
    MakeDirectory $BASE/Root
    cd $BASE/Root
    tar xfz $BASE/Files/$FREEMAT_FILE
    MakeDirectory $BASE/Root/$FREEMAT/build
    cd $BASE/Root/$FREEMAT/build
    ../configure --prefix=$PREFIX LDFLAGS="-L/usr/lib/atlas -L$PREFIX/lib" CPPFLAGS="-I$PREFIX/include -I/usr/include/ufsparse"
    make
    MakeLinuxBundle
}

SetupFFCALL()
{
    SetupCommon
    DownloadAndUnpackTarBall  $FFCALL_FILE ftp://ftp.santafe.edu/pub/gnu $FFCALL Root
    ConfigureMakeInstall $FFCALL "--prefix=$PREFIX" . Root
}

SetupLAPACK()
{
    SetupCommon
    DownloadAndUnpackTarBall  $LAPACK_FILE http://www.netlib.org/lapack $LAPACK Root
    cd $BASE/Root/$LAPACK
    touch make.inc
    cd $BASE/Root/$LAPACK/INSTALL
    make FORTRAN="g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="ranlib" ARCH="ar" ARCHFLAGS="cr" LOADER="g77"
    cd $BASE/Root/$LAPACK/SRC
    make FORTRAN="g77" OPTS="-O2" LAPACKLIB="liblapack.a" RANLIB="ranlib" ARCH="ar" ARCHFLAGS="cr"
    cp $BASE/Root/$LAPACK/liblapack.a $PREFIX/lib
}

SetupMacQt()
{
    SetupCommon
    DownloadAndUnpackTarBall $QT_MAC_FILE ftp://ftp.trolltech.com/qt/source $QT_MAC Root
    ConfigureMakeInstall $QT_MAC "-no-qt3support -prefix $PREFIX" . Root
}


Relink()
{
    SetupCommon
    echo "Relinking $1 --> $2"
    install_name_tool -change "$PREFIX/lib/$1.framework/Versions/4/$1" "@executable_path/../Frameworks/$1.framework/Versions/4/$1" "$2"
}

InstallFramework()
{
    SetupCommon
    echo "Installing framework $1"
    cp -R "$PREFIX/lib/$1.framework" "$baseDir/Contents/Frameworks/$1.framework"
    install_name_tool -id "@executable_path../Frameworks/$1.framework/Versions/4/$1" "$baseDir/Contents/Frameworks/$1.framework/Versions/4/$1"
    Relink $1 "$baseDir/Contents/MacOS/FreeMat"
}

CrossLinkFramework()
{
    SetupCommon
    echo "Cross Linking $2 -> $1"
    Relink "$2" "$baseDir/Contents/Frameworks/$1.framework/Versions/4/$1"
}

RelinkPlugin() {
    SetupCommon
    echo "Relinking plugin $1 to framework $2"
    Relink $2 "$baseDir/Contents/Plugins/imageformats/$1"
}

RelinkPlugins() {
    SetupCommon
    list=`ls $baseDir/Contents/Plugins/imageformats`
    for plugin in $list
    do
	RelinkPlugin $plugin "QtGui"
	RelinkPlugin $plugin "QtCore"
	RelinkPlugin $plugin "QtOpenGL"
	RelinkPlugin $plugin "QtNetwork"
	RelinkPlugin $plugin "QtXml"
	RelinkPlugin $plugin "QtSvg"
    done
}

CrossLinkFrameworkAll() {
    CrossLinkFramework $1 "QtCore"
    CrossLinkFramework $1 "QtGui"
    CrossLinkFramework $1 "QtOpenGL"
    CrossLinkFramework $1 "QtNetwork"
    CrossLinkFramework $1 "QtXml"
    CrossLinkFramework $1 "QtSvg"
}

MakeMacBundle()
{
    SetupCommon
    baseDir="$BASE/Root/$FREEMAT/build/$FREEMAT.app"
    rm -rf $baseDir
    buildDir="$BASE/Root/$FREEMAT/build"
    srcDir="$BASE/Root/$FREEMAT"
    MakeDirectory "$baseDir"
    MakeDirectory "$baseDir/Contents"
    MakeDirectory "$baseDir/Contents/MacOS"
    MakeDirectory "$baseDir/Contents/Resources"
    MakeDirectory "$baseDir/Contents/Resources/help"
    MakeDirectory "$baseDir/Contents/Resources/help/html"
    MakeDirectory "$baseDir/Contents/Resources/help/text"
    MakeDirectory "$baseDir/Contents/Resources/help/pdf"
    MakeDirectory "$baseDir/Contents/Resources/toolbox"
    MakeDirectory "$baseDir/Contents/Frameworks"
    CopyFile "$buildDir/src/FreeMat" "$baseDir/Contents/MacOS/FreeMat"
    strip "$baseDir/Contents/MacOS/FreeMat"
    ln -s "$baseDir/Contents/MacOS/FreeMat" "$baseDir/FreeMat"
    CopyFile "$srcDir/src/appIcon.icns" "$baseDir/Contents/Resources/appIcon.icns"
    echo "APPL????" > "$baseDir/Contents/PkgInfo"
    cat > "$baseDir/Contents/Info.plist" <<EOF
 <?xml version="1.0" encoding="UTF-8"?>
 <!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
 <plist version="0.9">
 <dict>
 <key>CFBundleIconFile</key>
 <string>appIcon.icns</string>
 <key>CFBundlePackageType</key>
 <string>APPL</string>
 <key>CFBundleExecutable</key>
 <string>FreeMat</string>
 </dict>
 </plist>
EOF
    InstallFramework "QtGui"
    InstallFramework "QtCore"
    InstallFramework "QtOpenGL"
    InstallFramework "QtNetwork"
    InstallFramework "QtXml"
    InstallFramework "QtSvg"
    CrossLinkFrameworkAll "QtGui" 
    CrossLinkFrameworkAll "QtCore"
    CrossLinkFrameworkAll "QtOpenGL"
    CrossLinkFrameworkAll "QtNetwork"
    CrossLinkFrameworkAll "QtXml"
    CrossLinkFrameworkAll "QtSvg"
    CopyDirectory "$srcDir/help/html" "$baseDir/Contents/Resources/help/html"
    CopyDirectory "$srcDir/help/text" "$baseDir/Contents/Resources/help/text"
    CopyDirectory "$srcDir/help/toolbox" "$baseDir/Contents/Resources/toolbox"
    CopyFile "$srcDir/help/latex/main.pdf" "$baseDir/Contents/Resources/help/pdf/$FREEMAT.pdf"
    MakeDirectory "$baseDir/Contents/Plugins/imageformats"
    CopyDirectory "$PREFIX/plugins/imageformats"  "$baseDir/Contents/Plugins/imageformats"
    RelinkPlugins
    rm -rf `find $baseDir/Contents/Frameworks -name '*debug*'`
    rm -rf `find $baseDir/Contents/Plugins -name '*debug*'`
    rm -rf "$buildDir/$FREEMAT.dmg"
    hdiutil create -fs HFS+ -srcfolder $baseDir "$buildDir/$FREEMAT.dmg"
}


SetupMacFreeMat()
{
   SetupCommon
   MakeDirectory $BASE/Root
   cd $BASE/Root
   tar xfz $BASE/Files/$FREEMAT_FILE
   MakeDirectory $BASE/Root/$FREEMAT/build
   cd $BASE/Root/$FREEMAT/build
   ../configure --prefix=$PREFIX LDFLAGS="-L$PREFIX/lib -F$PREFIX/lib" CPPFLAGS="-I$PREFIX/include -I$PREFIX/include/QtCore -I$PREFIX/include/QtGui -I$PREFIX/include/QtOpenGL -I$PREFIX/include/QtNetwork"
   make
   MakeMacBundle
}

SetupMacInplaceBuild()
{
   SetupCommon
   ../configure --prefix=$PREFIX LDFLAGS="-L$PREFIX/lib -F$PREFIX/lib" CPPFLAGS="-I$PREFIX/include -I$PREFIX/include/QtCore -I$PREFIX/include/QtGui -I$PREFIX/include/QtOpenGL -I$PREFIX/include/QtNetwork"
   make
}

SetupInplaceBuild() 
{
  SetupCommon
  ../configure --prefix=$PREFIX LDFLAGS="-L/usr/lib/atlas -L$PREFIX/lib" CPPFLAGS="-I$PREFIX/include -I/usr/include/ufsparse"
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
      --xwin-nsis        Setup the Win32 cross of NSIS
      --xwin-zlib        Setup the Win32 cross of zlib
      --xwin-freemat     Build the Win32 cross of FreeMat
      --xwin-all         Setup the Win32 cross compilation (all steps)

      --fftw             Setup FFTW
      --ffcall           Setup ffcall
      --sparse           Setup SuiteSparse
      --lapack           Setup LAPACK
      --atlas            Setup ATLAS/BLAS
      --arpack           Setup ARPACK
      --qt               Setup Qt
      --zlib             Setup zlib
      --freemat          Setup FreeMat
      --all

      --mac-qt           Setup Mac Qt
      --mac-freemat      Build the Mac FreeMat
      --mac-inplace      Build the Mac FreeMat in place

      --inplace          Build FreeMat in place (off the subversion tree)
"
    exit
}

for arg
  do
  case $arg in 
      --xwin-binutils) SetupXWinBinUtils ;;
      --xwin-gccfirst) SetupXWinGCCFirst ;;
      --xwin-win32api) SetupXWinWin32API ;;
      --xwin-mingw)    SetupXWinMingwRuntime ;;
      --xwin-gcc)      SetupXWinGCC ;;
      --xwin-fftw)     SetupXWinFFTW ;;
      --xwin-ffcall)   SetupXWinFFCALL ;;
      --xwin-sparse)   SetupXWinSparse ;;
      --xwin-lapack)   SetupXWinLAPACK ;;
      --xwin-atlas)    SetupXWinATLAS ;;
      --xwin-arpack)   SetupXWinARPACK ;;
      --xwin-qt)       SetupXWinQt ;;
      --xwin-nsis)     SetupXWinNSIS ;;
      --xwin-zlib)     SetupXWinZlib ;;
      --xwin-freemat)  SetupXWinFreeMat ;;
      --xwin-all)      SetupXWinAll ;;
      --fftw)          SetupFFTW ;;
      --ffcall)        SetupFFCALL ;;
      --sparse)        SetupSparse ;;
      --lapack)        SetupLAPACK ;;
      --atlas)         SetupATLAS ;;
      --arpack)        SetupARPACK ;;
      --qt)            SetupQt ;;
      --zlib)          SetupZlib ;;
      --freemat)       SetupFreeMat ;;
      --all)           SetupAll ;;
      --mac-qt)        SetupMacQt ;;
      --mac-freemat)   SetupMacFreeMat ;;
      --mac-inplace)   SetupMacInplaceBuild ;;
      --inplace)       SetupInplaceBuild ;;
      *)               Usage;
  esac
done
