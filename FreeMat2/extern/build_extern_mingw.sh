#!/bin/sh
#
# A script to build the external dependencies of FreeMat using Mingw32.
# Note that the following additional tools are required:
#  curl - download from http://curl.haxx.se/latest.cgi?curl=win32-nossl-sspi
# and put the executable where msys can find it (e.g., c:\mingw\bin)
#  unzip - download from (e.g.) http://www.mame.net/zips/compile/unzip.exe

FetchFile()
{
  echo "Looking for $2..."
  if [ ! -f Packages/$2 ] 
  then
    echo "Fetching $2 from $1..."
    curl $1/$2 -o Packages/$2 --disable-epsv
  fi
  if [ ! -f Packages/$2 ] 
  then
    echo "ERROR: Unable to retrieve $2 from $1... please download manually into the Packages directory and rerun the build script"
    exit 1
  fi
}

UnpackTarball()
{
  if [ ! -d Build/$2 ]
  then
    echo "Unpacking $1..."
    cd Build
    tar xfz ../Packages/$1
    cd ..
  fi
}

UnpackZip()
{
  if [ ! -d Build/$2 ]
  then
    echo "Unpacking $1..."
    cd Build
    unzip -q ../Packages/$1
    cd ..
  fi
}

ConfigureBuildAutoConf()
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$2 ]
  then
      cdir=`pwd`
      cd Build/$1
      ./configure --prefix=$cdir/Root $4
      make
      make install
      cd $cdir
  fi
  if [ -f Root/lib/$3 ]
  then
    touch State/build_$2
  fi
}

ConfigureBuildAMD() 
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$1 ]
  then
      cdir=`pwd`
      cd Build/$1/Source
      make CC=gcc
      cp ../Lib/libamd.a $cdir/Root/lib/.
      cp ../Include/amd.h $cdir/Root/include/.
      cd $cdir
  fi
  if [ -f Root/lib/libamd.a ]
  then
    touch State/build_$1
  fi
}

ConfigureBuildUMFPACK() 
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$1 ]
  then
      cdir=`pwd`
      cd Build/$1/Source
      make CC=gcc
      cp ../Lib/libumfpack.a $cdir/Root/lib/.
      cp ../Include/*.h $cdir/Root/include/.
      cd $cdir
  fi
  if [ -f Root/lib/libumfpack.a ]
  then
    touch State/build_$1
  fi
}

ConfigureBuildLAPACK()
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$1 ]
  then
     cdir=`pwd`
     cp patches/LAPACK/Makefile_freemat Build/$1/SRC/.
     cd Build/$1/SRC
     make -f Makefile_freemat $2
     cd $cdir
     cp Build/LAPACK/liblapack.a $cdir/Root/lib/.
  fi
  if [ -f Build/LAPACK/liblapack.a ]
  then
    touch State/build_$1
  fi
}

ConfigureBuildBLAS()
{
  echo "Configure and building $1..."
  if [ ! -f State/build_$1 ]
  then
    cdir=`pwd`
    cp patches/BLAS/Makefile_freemat Build/LAPACK/BLAS/SRC/.
    cd Build/LAPACK/BLAS/SRC
    make -f Makefile_freemat $2
    cd $cdir
    cp Build/LAPACK/BLAS/libblas.a $cdir/Root/lib/.
    gcc -shared -o Root/lib/blas.dll patches/BLAS/blas.def Root/lib/libblas.a -Wl,--enable-auto-import -Wl,--no-whole-archive $mingwclib -Wl,--out-implib=Root/lib/blas.lib
  fi
  if [ -f Build/LAPACK/BLAS/libblas.a ]
  then
    touch State/build_$1
  fi
}

ConfigureBuildARPACK()
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$1 ]
  then
    cdir=`pwd`
    cp patches/ARPACK/Makefile_freemat Build/$1/.
    cp patches/ARPACK/stat.h Build/$1/SRC/.
    cd Build/$1
    make -f Makefile_freemat $2
    cd $cdir
    cp Build/ARPACK/libarpack.a $cdir/Root/lib/.
  fi
  if [ -f Build/ARPACK/libarpack.a ]
  then
    touch State/build_$1
  fi
}

RepackATLASDLL()
{
   echo "Repacking ATLAS libraries into DLL for $1"
   lcdir=`pwd`
   cd atlas
   cp $lcdir/../Packages/$1.zip .
   unzip -q $1.zip
   echo "  Building DLL out of .a"
   gcc -shared -o blas_$2.dll $lcdir/../patches/BLAS/blas.def libf77blas.a libatlas.a -Wl,--enable-auto-import -Wl,--no-whole-archive $mingwclib
   cp blas_$2.dll $lcdir/../Root/lib/.
   rm -rf *.a Make.*
   cd $lcdir
 }

ConfigureBuildATLAS()
{
   echo "Building ATLAS BLAS dlls..."
   if [ ! -f State/build_atlas ]
   then
     cdir=`pwd`
     cd Build
     RepackATLASDLL atlas3.6.0_WinNT_P4 P4
     RepackATLASDLL atlas3.6.0_WinNT_P4SSE2 P4SSE2
     RepackATLASDLL atlas3.6.0_WinNT_PIII PIII
     RepackATLASDLL atlas3.6.0_WinNT_PII PII
     cd $cdir
     rm Root/lib/libblas.a
   fi
   if [ -f Root/lib/blas_P4.dll ]
   then
     touch State/build_atlas
   fi
}

SetupDirs()
{
  echo "Setting up for builds..."
  mkdir -p Packages
  mkdir -p Build
  mkdir -p Build/atlas
  mkdir -p Root/include
  mkdir -p Root/lib
  mkdir -p State
}

ReportStatusPackage()
{
  if [ -f State/build_$1 ]
  then
    echo "Package $1 build: ok"
  else
    echo "Package $1 build: failed"
  fi
}

ReportStatus()
{
  ReportStatusPackage ffcall
  ReportStatusPackage fftw
  ReportStatusPackage fftwf
  ReportStatusPackage AMD
  ReportStatusPackage UMFPACK
  ReportStatusPackage LAPACK
  ReportStatusPackage BLAS
  ReportStatusPackage ARPACK
  ReportStatusPackage matio
  ReportStatusPackage atlas
}

BuildFFCall()
{
  echo "Configure and building $1..."
  if [ ! -f State/build_ffcall ]
  then
    cdir=`pwd`
    cd Build/$1
    make -f Makefile.mingw32
    cp avcall/avcall.h $cdir/Root/include/.
    cp avcall/libavcall.a $cdir/Root/lib/.
    cd $cdir
  fi
  if [ -f Root/lib/libavcall.a ]
  then
    touch State/build_ffcall
  fi
}

BuildMATIO()
{
  echo "Configure and building libmatio..."
  if [ ! -f State/build_matio ]
  then
    cdir=`pwd`
    cd Build/matio/zlib
    CFLAGS='-O3 -DZ_PREFIX' \
    ./configure --prefix=$cdir/Root 
    make
    make install
    cd ..
    CFLAGS='-O3 -DZ_PREFIX' \
    ./configure --with-zlib=$cdir/Root --prefix=$cdir/Root 
    make
    make install
    cd $cdir
  fi
  if [ -f Root/lib/libmatio.a ]
  then
    touch State/build_matio
  fi
}

CLIBPATH=c:/mingw/lib
mingwclib="$CLIBPATH/libg2c.a $CLIBPATH/libmoldname.a $CLIBPATH/libmsvcrt.a"
MAKEOPTS=$1
SetupDirs
FetchFile ftp://ftp.santafe.edu/pub/gnu ffcall-1.10.tar.gz
FetchFile http://www.fftw.org fftw-3.1.1.tar.gz
FetchFile http://www.cise.ufl.edu/research/sparse/umfpack/current UMFPACK.zip
FetchFile http://www.cise.ufl.edu/research/sparse/UFconfig/current UFconfig.zip
FetchFile http://www.cise.ufl.edu/research/sparse/amd/current AMD.zip
FetchFile http://www.netlib.org/lapack lapack.tgz
FetchFile http://www.caam.rice.edu/software/ARPACK/SRC arpack96.tar.gz
FetchFile http://www.mathworks.com/matlabcentral/files/8187 matio.zip
FetchFile http://old.scipy.org/download/atlasbinaries/winnt atlas3.6.0_WinNT_P4SSE2.zip
FetchFile http://old.scipy.org/download/atlasbinaries/winnt atlas3.6.0_WinNT_P4.zip
FetchFile http://old.scipy.org/download/atlasbinaries/winnt atlas3.6.0_WinNT_PIII.zip
FetchFile http://old.scipy.org/download/atlasbinaries/winnt atlas3.6.0_WinNT_PII.zip
UnpackTarball ffcall-1.10.tar.gz ffcall-1.10
UnpackTarball fftw-3.1.1.tar.gz fftw-3.1.1
UnpackZip UMFPACK.zip UMFPACK
UnpackZip UFconfig.zip UFconfig
UnpackZip AMD.zip AMD
UnpackTarball lapack.tgz LAPACK
UnpackTarball arpack96.tar.gz ARPACK
UnpackZip matio.zip matio
BuildFFCall ffcall-1.10 $MAKEOPTS
ConfigureBuildAutoConf fftw-3.1.1 fftw libfftw3.a $MAKEOPTS 
ConfigureBuildAutoConf fftw-3.1.1 fftwf libfftw3f.a --enable-single
ConfigureBuildAMD AMD 
ConfigureBuildUMFPACK UMFPACK 
ConfigureBuildLAPACK LAPACK FC=g77
ConfigureBuildBLAS BLAS FC=g77
ConfigureBuildARPACK ARPACK FC=g77
BuildMATIO
ConfigureBuildATLAS
ReportStatus
