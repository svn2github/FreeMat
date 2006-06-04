%!
%@Module INSTALL Installing FreeMat
%@@Section INTRODUCTION
%@@General Instructions
%Here are the general instructions for installing FreeMat.  First, follow the 
%instructions listed below for the platform of interest.  Then, run the
%@[
%-->pathtool
%@]
%which brings up the path setup tool.  More documentation on the GUI elements
%(and how to use them) will be forthcoming.  
%@@Linux
%For Linux, FreeMat is now provided as a binary installation.  To install it
%simply download the binary using your web browser, and then unpack it
%@[
%  tar xvfz FreeMat-<VERSION_NUMBER>-Linux-Binary.tar.gz
%@]
%You can then run FreeMat directly without any additional effort
%@[
%  FreeMat-<VERSION_NUMBER>-Linux-Binary/Contents/bin/FreeMat
%@]
%will start up FreeMat as an X application.  If you want to run it
%as a command line application (to run from within an xterm), use
%the @|nogui| flag
%@[
%  FreeMat-<VERSION_NUMBER>-Linux-Binary/Contents/bin/FreeMat -nogui
%@]
%If you do not want FreeMat to use X at all (no graphics at all), use
%the @|noX| flag
%@[
%  FreeMat-<VERSION_NUMBER>-Linux-Binary/Contents/bin/FreeMat -noX
%@]
%For convenience, you may want to add FreeMat to your path.  The exact
%mechanism for doing this depends on your shell.  Assume that you have
%unpacked @|FreeMat-<VERSION_NUMBER>-Linux-Binary.tar.gz| into the directory
%@|/home/myname|.  Then if you use @|csh| or its derivatives (like @|tcsh|)
%you should add the following line to your @|.cshrc| file:
%@[
%  set path=($path /home/myname/FreeMat-<VERSION_NUMBER>-Linux/Binary/Contents/bin)
%@]
%If you use @|bash|, then add the following line to your @|.bash_profile|
%@[
%  PATH=$PATH:/home/myname/FreeMat-<VERSION_NUMBER>-Linux/Binary/Contents/bin
%@]
%If the prebuilt binary package does not work for your Linux distribution, you
%will need to build FreeMat from source (see the source section below).  When
%you have FreeMat running, you can setup your path using the @|pathtool|.  Note
%that the @|FREEMAT_PATH| is no longer used by FreeMat.  You must use the @|pathtool|
%to adjust the path.
%@@Windows
%For Windows, FreeMat is installed via a binary installer program.  To use it,
%simply download the setup program @|FreeMat-<VERSION_NUMBER>-Setup.exe|, and double
%click it.  Follow the instructions to do the installation, then setup your path
%using @|pathtool|.
%@@Mac OS X
%For Mac OS X, FreeMat is distributed as an application bundle.  To install it,
%simply download the compressed disk image file @|FreeMat-<VERSION_NUMBER>.dmg|, double
%click to mount the disk image, and then copy the application @|FreeMat-<VERSION_NUMBER>| to
%some convenient place.  To run FreeMat, simply double click on the application.  Run
%@|pathtool| to setup your FreeMat path.
%@@Source Code
%The source code build is a little more complicated than previous versions of FreeMat.  Here
%are the current build instructions for all platforms.
%\begin{enumerate}
%\item Build and install Qt 4.1 or later - @|http://www.trolltech.com/download/opensource.html|
%\item Install g77 or gfortran (use fink for Mac OS X, use @|gcc-g77| package for MinGW)
%\item Download the source code @|FreeMat-<VERSION_NUMBER>-src.tar.gz|.
%\item Unpack the source code: @|tar xvfz FreeMat-<VERSION_NUMBER>-src.tar.gz|.
%\item For Windows, you will need to install MSYS as well as MINGW to
%build FreeMat.  You will also need unzip to unpack the enclosed
%matio.zip archive
%\item If you are extraordinarily lucky (or prepared), you can issue the
%usual @|configure && make && make install|.  This is not likely to work
%because of the somewhat esoteric dependencies of FreeMat.  The configure
%step will probably fail and indicate what external dependencies are
%still needed.  It will also create a script that you can run to build
%the missing dependencies.  
%\end{enumerate}
%For example, on my machine, a @|configure|
%step yields the following result:
%@[
%checking for amd_postorder in -lamd... no
%checking for umfpack_zl_solve in -lumfpack... no
%checking for fftwf_malloc in -lfftw3f... no
%checking for fftw_malloc in -lfftw3... no
%checking for sgemm_... no
%checking for ATL_xerbla in -latlas... no
%checking for sgemm_ in -lblas... no
%checking for sgemm_ in -lcxml... no
%checking for sgemm_ in -ldxml... no
%checking for sgemm_ in -lscs... no
%checking for sgemm_ in -lcomplib.sgimath... no
%checking for sgemm_ in -lblas... (cached) no
%checking for sgemm_ in -lm... no
%checking for sgemm_ in -lblas... (cached) no
%checking for znaupd_ in -larpack... no
%checking for inflate in -lz... yes
%checking for Mat_Open in -lmatio... no
%configure: creating ./config.status
%config.status: creating tools/disttool/builddeps
%config.status: executing depfiles commands
%configure: error:
%**********************************************************************
%One or more of the following external dependencies was not
%found:
%
%  AMD                       no
%  UMFPACK                   no
%  FFTW3 (Single Precision)  no
%  FFTW3 (Double Precision)  no
%  BLAS                      no
%  LAPACK                    no
%  ARPACK                    no
%  ZLIB                      yes
%  MATIO                     no
%**********************************************************************
%A script to build these external dependencies has been created
%in the current directory.  To build the missing dependencies,
%run the script via:
%
%./builddeps  --with-ffcall --with-umfpack --with-umfpack --with-fftw --with-fftw   --with-blas --with-lapack --with-arpack --with-matio
%
%Note that this will attempt to build and install the libraries
%and header files in extern/Root/lib and extern/Root/include
%(respectively).  Once the required libraries have been successfully
%built, rerun configure.
%@]
%After you run the @|builddeps| script, the configure succeeds, and the
%usual @|configure && make && make install| should work.
%Note that for Linux, the location of Qt4 is highly system dependent.  For
% @|configure| to find the whereabouts of Qt4, you need to make sure
%that @|pkg-config| can find Qt4.  For example, if you installed Qt4
%yourself, you would set
%@[
%declare -x PKG_CONFIG_PATH=/usr/local/Trolltech/Qt-4.1.0/lib
%@]
%Also, to build a binary distributable (app bundle on the Mac, setup
%installer on win32, and a binary distribution on Linux), you will
%need to run @|make package| instead of @|make install|.
%!
