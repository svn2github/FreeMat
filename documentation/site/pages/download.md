Download
========

The following links point to the latest version of FreeMat. For older versions (or a complete list), go to the SourceForge download page [here](http://sourceforge.net/project/showfiles.php?group_id=91526).

 * [Click to download FreeMat for Windows](http://downloads.sourceforge.net/freemat/FreeMat-3.6_Setup.exe) !["ico-win"](assets/ico-win.png)
 * [Click to download FreeMat for Mac OS X 10.4 (Universal)](http://downloads.sourceforge.net/freemat/FreeMat-3.6.dmg) !["ico-osx-uni"](assets/ico-osx-uni.png)
 * [Click to download FreeMat for Fedora Core 7 Linux (RPM)](http://downloads.sourceforge.net/freemat/FreeMat-3.6.i386.fc7.rpm) !["ico-tux"](assets/ico-tux.png)

Note that you will need the ffcall and arpack libraries which are not available in Fedora Core 7. They are available at Dries RPM Repository: [ffcall](http://dries.studentenweb.org/rpm/packages/ffcall/info.html) and ATrpms: [arpack](http://www.atrpms.net/dist/fc6/arpack).

 * [Click to download FreeMat for Fedora Core 6 Linux on x86\_64 architecture (RPM)](http://downloads.sourceforge.net/freemat/FreeMat-3.4-1.fc6.x86_64.rpm) currently 3.4 !["ico-tux"](assets/ico-tux.png)
 * [Click to download FreeMat Source Code (All platforms)](http://downloads.sourceforge.net/freemat/FreeMat-3.6.tar.gz)
 * [Click to download FreeMat Manual in PDF](http://freemat.sourceforge.net/FreeMat-3.6.pdf) !["ico-pdf"](assets/ico-pdf.gif)

For Gentoo users, Andrey Grozin has indicated that FreeMat is in the mainline. So, you should be able to do:

> root@linux:~$ emerge freemat

Note that if you want to build FreeMat on your Linux system, but do not have the required dependencies (or root access), you can try to use this Perl script [buildtool](http://downloads.sourceforge.net/freemat/buildtool). Then just run it as:

> root@linux:~$ buildtool /your/home/dir native-freemat

This will download sources for all missing dependencies, configure and build them (including FreeMat). It is still experimental, so please report problems (see the help menu on the side). 