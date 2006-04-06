#!/bin/bash
FetchFile()
{
  echo "Looking for $2..."
  if [ ! -f $2 ] 
  then
    echo "Fetching $2 from $1..."
    curl $1/$2 -o $2 --disable-epsv
  fi
  if [ ! -f $2 ] 
  then
    echo "ERROR: Unable to retrieve $2 from $1... please download manually and rerun"
    exit 1
  fi
}

UnpackTarball()
{
  echo "Unpacking $1..."
  tar xfz $1
}

ConfigureBuildAutoConf()
{
  echo "Configuring and building $1..."
  if [ ! -f build_$1 ]
  then
      cdir=`pwd`
      cd $1
      ./configure --prefix=$cdir/tmp $2
      make
      make install
      cd $cdir
  fi
  touch build_$1
}

ConfigureBuildUMFPACK() 
{
  echo "Configuring and building $1..."
  if [ ! -f build_$1 ]
  then
      cdir=`pwd`
      cd $1/AMD
      make lib
      cd ../UMFPACK
      make lib
      cd $cdir
  fi
  touch build_$1
}

ConfigureBuildLAPACK()
{
  echo "Configuring and building $1..."
  if [ ! -f build_$1 ]
  then
     cp patches/LAPACK/Makefile_freemat $1/SRC/.
     cd $1/SRC
     make
  fi
}

FetchFile ftp://ftp.santafe.edu/pub/gnu ffcall-1.10.tar.gz
FetchFile http://www.fftw.org fftw-3.1.1.tar.gz
FetchFile http://www.cise.ufl.edu/research/sparse/umfpack/v4.1 UMFPACKv4.1.tar.gz
FetchFile http://www.netlib.org/lapack lapack.tgz
UnpackTarball ffcall-1.10.tar.gz
UnpackTarball fftw-3.1.1.tar.gz
UnpackTarball UMFPACKv4.1.tar.gz
UnpackTarball lapack.tgz
ConfigureBuildAutoConf ffcall-1.10
ConfigureBuildAutoConf fftw-3.1.1
ConfigureBuildAutoConf fftw-3.1.1 --enable-single
ConfigureBuildUMFPACK UMFPACKv4.1
ConfigureBuildLAPACK LAPACK
