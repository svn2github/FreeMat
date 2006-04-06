#!/bin/bash
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

ConfigureBuildUMFPACK() 
{
  echo "Configuring and building $1..."
  if [ ! -f State/build_$1 ]
  then
      cdir=`pwd`
      cd Build/$1/AMD/Source
      make
      cp ../Lib/libamd.a $cdir/Root/lib/.
      cd ../../UMFPACK/Source
      make 
      cp ../Lib/libumfpack.a $cdir/Root/lib/.
      cd $cdir
  fi
  if [ -f Build/$1/UMFPACK/Lib/libumfpack.a ]
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

SetupDirs()
{
  echo "Setting up for builds..."
  mkdir -p Packages
  mkdir -p Build
  mkdir -p Root
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
  ReportStatusPackage UMFPACKv4.1
  ReportStatusPackage LAPACK
  ReportStatusPackage BLAS
  ReportStatusPackage ARPACK
  ReportStatusPackage matio
}

MAKEOPTS=$1
SetupDirs
FetchFile ftp://ftp.santafe.edu/pub/gnu ffcall-1.10.tar.gz
FetchFile http://www.fftw.org fftw-3.1.1.tar.gz
FetchFile http://www.cise.ufl.edu/research/sparse/umfpack/v4.1 UMFPACKv4.1.tar.gz
FetchFile http://www.netlib.org/lapack lapack.tgz
FetchFile http://www.caam.rice.edu/software/ARPACK/SRC arpack96.tar.gz
FetchFile http://www.mathworks.com/matlabcentral/files/8187 matio.zip
UnpackTarball ffcall-1.10.tar.gz ffcall-1.10
UnpackTarball fftw-3.1.1.tar.gz fftw-3.1.1
UnpackTarball UMFPACKv4.1.tar.gz UMFPACKv4.1
UnpackTarball lapack.tgz LAPACK
UnpackTarball arpack96.tar.gz ARPACK
UnpackZip matio.zip matio
ConfigureBuildAutoConf ffcall-1.10 ffcall libavcall.a $MAKEOPTS 
ConfigureBuildAutoConf fftw-3.1.1 fftw libfftw3.a $MAKEOPTS 
ConfigureBuildAutoConf fftw-3.1.1 fftwf libfftw3f.a --enable-single
ConfigureBuildUMFPACK UMFPACKv4.1 $MAKEOPTS
ConfigureBuildLAPACK LAPACK $MAKEOPTS
ConfigureBuildBLAS BLAS $MAKEOPTS
ConfigureBuildARPACK ARPACK $MAKEOPTS
ConfigureBuildAutoConf matio matio $MAKEOPTS libmatio.a
ReportStatus
