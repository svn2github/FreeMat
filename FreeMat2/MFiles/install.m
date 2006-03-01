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
%\item Unpack the source code: @|tar xvfz FreeMat-<VERSION_NUMBER>-src.tar.gz|
%\item Configure the source (not required on Windows): @|./configure|  For help with
%configuring the source, do @|./configure -help|.
%\item Build the source: @|make|
%\item Build the help files: @|make help|.  You will need a functional installation of
%@|LaTeX| and @|dvipng| for the help files to build properly.
%\item Test the result @|make check|
%\item Build a package: @|make install|
%\item The result is a directory @|FreeMat-<VERSION_NUMBER>| that contains the newly built
%installation.  Follow the directions listed above to install the new version.
%\end{enumerate}
%Note that currently a number of prerequisites exist. These prerequisites are currently
%bundled with FreeMat in the @|extern| subdirectory.  Ideally, in the future, the packages
%listed there would be contained in the major Linux distros.
%!
