wget "http://prdownloads.sf.net/mingw/gcc-core-3.4.5-20060117-1-src.tar.gz?download"
wget "http://prdownloads.sf.net/mingw/gcc-g++-3.4.5-20060117-1-src.tar.gz?download"
wget "http://prdownloads.sf.net/mingw/gcc-g77-3.4.5-20060117-1-src.tar.gz?download"
wget "http://prdownloads.sf.net/mingw/mingw-runtime-3.9-src.tar.gz?download"
wget "http://prdownloads.sf.net/mingw/w32api-3.6-src.tar.gz?download"
wget "http://prdownloads.sf.net/mingw/binutils-2.16.91-20060119-1-src.tar.gz?download"
wget "ftp://ftp.trolltech.com/qt/source/qt-win-opensource-4.2.1-mingw.exe"
wine qt-win-opensource-4.2.1-mingw.exe
wget "http://www.fftw.org/fftw-3.1.2.tar.gz"
tar xvfz fftw-3.1.2.tar.gz
cd fftw-3.1.2
mkdir build
mkdir cbuild
cd build
../configure --host=i686-mingw32 --build=i686-pc-linux --prefix=/home/basu/dev/trunk/cross
make
make install
cd ../cbuild
../configure --host=i686-mingw32 --build=i686-pc-linux --prefix=/home/basu/dev/trunk/cross --enable-single
make
make install
cd ..
wget "ftp://ftp.santafe.edu/pub/gnu/ffcall-1.10.tar.gz"
tar xvfz ffcall-1.10.tar.gz
cd ffcall-1.10
mkdir build
cd build
../configure --host=i686-mingw32 --build=i686-pc-linux --prefix=/home/basu/dev/trunk/cross
make
make install
i686-mingw32-ranlib /home/basu/dev/trunk/cross/lib/libavcall.a
wget "http://www.cise.ufl.edu/research/sparse/SuiteSparse/SuiteSparse-2.3.1.tar.gz"
tar xvfz SuiteSparse-2.3.1.tar.gz
cd UFconfig
<modify>
cd ../AMD
make
cp Lib/libamd.a /home/basu/dev/trunk/cross/lib/.
