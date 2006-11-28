#! /bin/bash -e

BASE=$PWD
TARGET=i686-mingw32
PREFIX=$PWD

# find the archives
cd $BASE
BINUTILS=`ls binutils-*-src.tar.gz`
GCC_CORE=`ls gcc-core-*-src.tar.gz`
GCC_GXX=`ls gcc-g++-*-src.tar.gz`
GCC_G77=`ls gcc-g77-*-src.tar.gz`
MINGW_RUNTIME=`ls mingw-runtime-*-src.tar.gz`
W32API=`ls w32api-*-src.tar.gz`

PATH=$PATH:$PREFIX/bin

# unpack the archives
for i in $BINUTILS $GCC_CORE $GCC_GXX $GCC_G77 $MINGW_RUNTIME $W32API
do
   tar xzf $i
done

# get the source directory names
CHOP="sed 's/-src\.tar\.gz//'"

BINUTILS=$BASE/`basename $BINUTILS -src.tar.gz`
GCC=$BASE/`basename $GCC_CORE -src.tar.gz | sed 's/-core//'`
MINGW_RUNTIME=$BASE/`basename $MINGW_RUNTIME -src.tar.gz`
W32API=$BASE/`basename $W32API -src.tar.gz`

# build binutils
cd $BINUTILS
./configure --target=$TARGET --prefix=$PREFIX
make
make install

# make mingw and w32api includes available
mkdir -vp $PREFIX/include
ln -s $PREFIX/include $PREFIX/$TARGET/include
cp -r $MINGW_RUNTIME/include/* $PREFIX/include
cp -r $W32API/include/* $PREFIX/include

# build gcc for C only
cd $GCC
./configure --prefix=$PREFIX --target=$TARGET --enable-threads --enable-languages=c
make
make install

# build w32api
cd $W32API
./configure --prefix=$PREFIX --target=$TARGET --host=$TARGET --build=$(./config.guess)
make
make install

# put libs where mingw runtime expects them
ln -s $W32API $BASE/w32api
mv $PREFIX/$TARGET/lib/* $PREFIX/lib
rmdir $PREFIX/$TARGET/lib
ln -s $PREFIX/lib $PREFIX/$TARGET/lib

# build mingw runtime
cd $MINGW_RUNTIME
find . -name configure -exec dos2unix \{\} \;
dos2unix config.guess config.sub mkinstalldirs
./configure --prefix=$PREFIX --target=$TARGET --host=$TARGET --build=$(./config.guess)
make
make install

# rebuild gcc for both C and C++
cd $GCC
./configure --prefix=$PREFIX --target=$TARGET --enable-threads --enable-languages=c,c++,f77
make
make install

