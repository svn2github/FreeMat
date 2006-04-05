#!/bin/bash
FetchFile()
{
  echo "Looking for $2..."
  if [ ! -f $2 ] 
  then
    echo "Fetching $2 from $1..."
    wget $1/$2
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
      cd $1
      ./configure --prefix=`pwd`/tmp $2
      make
      make install 
      cd ..
  fi
  touch build_$1
}

ConfigureBuildUMFPACK() 
{
  echo "Configuring and building $1..."
  if [ ! -f build_$1 ]
  then
      cd $1/AMD
      make lib
      cd ../UMFPACK
      make lib
  fi
  touch build_$1
}

FetchFile ftp://ftp.santafe.edu/pub/gnu ffcall-1.10.tar.gz
FetchFile http://www.fftw.org fftw-3.1.1.tar.gz
FetchFile http://www.cise.ufl.edu/research/sparse/umfpack/v4.1 UMFPACKv4.1.tar.gz
FetchFile http://www.netlib.org/lapack lapack.tgz
UnpackTarball ffcall-1.10.tar.gz
UnpackTarball fftw-3.1.1.tar.gz
UnpackTarball UMFPACKv4.1.tar.gz
ConfigureBuildAutoConf ffcall-1.10
ConfigureBuildAutoConf fftw-3.1.1
ConfigureBuildAutoConf fftw-3.1.1 --enable-single
ConfigureBuildUMFPACK UMFPACKv4.1

