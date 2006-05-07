ffcall-1.10.tar.gz:
	curl -O ftp://ftp.santafe.edu/pub/gnu/ffcall-1.10.tar.gz --disable-epsv

lib/libavcall.a: ffcall-1.10.tar.gz
	tar xfz ffcall-1.10.tar.gz
	mkdir -p ffcall-1.10/build
	cd ffcall-1.10/build && ../configure --prefix=`pwd`/../../
	cd ffcall-1.10/build && make && make install

avcall: lib/libavcall.a

UFconfig.tar.gz:
	curl -O http://www.cise.ufl.edu/research/sparse/UFconfig/current/UFconfig.tar.gz

AMD.tar.gz:
	curl -O http://www.cise.ufl.edu/research/sparse/amd/current/AMD.tar.gz 

UMFPACK.tar.gz:
	curl -O http://www.cise.ufl.edu/research/sparse/umfpack/current/UMFPACK.tar.gz

lib/libumfpack.a: UFconfig.tar.gz AMD.tar.gz UMFPACK.tar.gz	
	tar xfz UFconfig.tar.gz
	tar xfz AMD.tar.gz
	tar xfz UMFPACK.tar.gz
	cd AMD/Source && make CC=gcc
	cp AMD/Lib/libamd.a lib/.
	cp AMD/Include/amd.h include/.
	cd UMFPACK/Source && make CC=gcc
	cp UMFPACK/Lib/libumfpack.a lib/.
	cp UMFPACK/Include/*.h include/.

umfpack: lib/libumfpack.a

fftw-3.1.1.tar.gz:
	curl -O http://www.fftw.org/fftw-3.1.1.tar.gz

lib/libfftw3.a: fftw-3.1.1.tar.gz
	tar xfz fftw-3.1.1.tar.gz
	mkdir -p fftw-3.1.1/build_double
	mkdir -p fftw-3.1.1/build_single
	cd fftw-3.1.1/build_double && ../configure --prefix=`pwd`/../../
	cd fftw-3.1.1/build_double && make && make install
	cd fftw-3.1.1/build_single && ../configure --prefix=`pwd`/../../ --enable-single
	cd fftw-3.1.1/build_single && make && make install

fftw3: lib/libfftw3.a 

lapack.tgz: 
	curl -O http://www.netlib.org/lapack/lapack.tgz

lib/liblapack.a: lapack.tgz
	tar xfz lapack.tgz
	cp @top_srcdir@/extern/patches/LAPACK/Makefile_freemat LAPACK/SRC/.
	cd LAPACK/SRC && make -f Makefile_freemat FC=@F77@
	cp LAPACK/liblapack.a lib/.

lib/libblas.a: lapack.tgz
	tar xfz lapack.tgz
	cp @top_srcdir@/extern/patches/BLAS/Makefile_freemat LAPACK/BLAS/SRC/.
	cd LAPACK/BLAS/SRC && make -f Makefile_freemat FC=@F77@
	cp LAPACK/BLAS/libblas.a lib/.

lapack: lib/liblapack.a

blas: lib/libblas.a

arpack96.tar.gz:
	curl -O http://www.caam.rice.edu/software/ARPACK/SRC/arpack96.tar.gz

matio.zip:
	curl -O http://www.mathworks.com/matlabcentral/files/8187/matio.zip

lib/libarpack.a: arpack96.tar.gz
	tar xfz arpack96.tar.gz
	cp @top_srcdir@/extern/patches/ARPACK/Makefile_freemat ARPACK/.
	cp @top_srcdir@/extern/patches/ARPACK/stat.h ARPACK/SRC/.
	cd ARPACK && make -f Makefile_freemat FC=@F77@
	cp ARPACK/libarpack.a lib/.

arpack: lib/libarpack.a

lib/libmatio.a: matio.zip
	unzip matio.zip
	mkdir -p matio/zlib/build
	mkdir -p matio/build
	cd matio/zlib/build && CFLAGS=-DZ_PREFIX ../configure --prefix=`pwd`/../../../
	cd matio/zlib/build && make && make install
	cd matio/build && CFLAGS=-DZ_PREFIX ../configure --prefix=`pwd`/../../ 
	cd matio/build && make && make install

matio: lib/libmatio.a

all: avcall umfpack fftw3 blas lapack arpack matio
