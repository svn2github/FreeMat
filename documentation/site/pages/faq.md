#FAQ#

Contents:
--------

 1. [What is FreeMat?](faq.html#1)
 2. [Why GPL?](faq.html#2)
 3. [Why another MATLAB clone? Have you heard of Octave, Scilab, etc.?](faq.html#3)
 4. [FreeMat 100% compatible with MATLAB? What about IDL?](faq.html#4)
 5. [What platforms are supported?](faq.html#5)
 6. [How do I get it?](faq.html#6)
 7. [I found a bug! What now?](faq.html#7)
 8. [Where is function xyz?](faq.html#8)
 9. [Who wrote FreeMat and why?](faq.html#9)
 10. [Are there any development related pages?](faq.html#10)

<span id="1"/>
1. What is FreeMat?
-------------------
FreeMat is an environment for rapid engineering and scientific processing. It is similar to commercial systems such as MATLAB from Mathworks and IDL from Research Systems, but is Open Source. It is free as in speech and free as in beer.

<span id="2"/>
2. Why GPL?
-----------
Previous versions of FreeMat were released under MIT licenses. The current version is released under GPL. There are a number of great tools that are available to GPL-ed code (e.g., Qt, FFTW, FFCALL), and FreeMat is now one of them.

<span id="3"/>
3. Why another MATLAB clone? Have you heard of Octave, Scilab, etc.?
--------------------------------------------------------------------
Yes! FreeMat is chartered to go beyond MATLAB to include features such as a codeless interface to external C/C++/FORTRAN code, parallel/distributed algorithm development (via MPI), and advanced volume and 3D visualization capabilities. As for the open source alternatives, try them out and decide for yourself. Who said choice was a bad thing?

<span id="4"/>
4. Is FreeMat 100% compatible with MATLAB? What about IDL?
----------------------------------------------------------
No. FreeMat supports roughly 95% (a made up statistic) of the features in MATLAB. The following table summarizes how FreeMat stacks up against MATLAB and IDL. Because we like to lead with the positive, here are the features in that are supported:

 * N-dimensional array manipulation (by default, N is limited to 6)
 * Support for 8,16, 32, and 64 bit integer types (signed and unsigned), 32 and 64 bit floating point types, and 64 and 128 bit complex types.
 * Built in arithmetic for manipulation of all supported data types.
 * Support for solving linear systems of equations via the divide operators.
 * Eigenvalue and singular value decompositions
 * Full control structure support (including, for, while, break, continue, etc.)
 * 2D plotting and image display
 * Heterogeneous array types (called "cell arrays" in MATLAB-speak) fully supported
 * Full support for dynamic structure arrays
 * Arbitrary-size FFT support
 * Pass-by-reference support (an IDL feature)
 * Keyword support (an IDL feature)
 * Codeless interface to external C/C++/FORTRAN code
 * Native Windows support
 * Native sparse matrix support
 * Native support for Mac OS X (no X11 server required).
 * Function pointers (eval and feval are fully supported)
 * Classes, operator overloading
 * 3D Plotting and visualization via OpenGL
 * Parallel processing with MPI (in the FreeMat development versions)
 * Handle-based graphics 

Here are the list of major MATLAB features not currently supported:

 * GUI/Widgets 

Finally the list of features that are in progress (meaning they are in the development version or are planned for the near future):

 * Widgets/GUI building
 * 3D volume rendering capability (in the FreeMat development versions)
 * FreeMat-to-MEX interface for porting MATLAB MEX files. 


If you feel very strongly that one or more MATLAB features are missing that would be useful to have in FreeMat, you can either add it yourself or try and convince someone else (e.g., me) to add it for you. As for IDL, FreeMat is not compatible at all with IDL (the syntax is MATLAB-based), but a few critical concepts from IDL are implemented, including pass by reference and keywords.

<span id="5"/>
5. What platforms are supported?
--------------------------------
Currently, Windows 2000/XP, Linux and Mac OS X are supported platforms. Other UNIX environments (such as IRIX/SOLARIS) may work. FreeMat essentially requires GNU gcc/g++ and gfortran to build. The Win32 build requires MINGW32. I don't know if FreeMat will work with Windows 98/95/ME or NT4 as I don't have access to any of these platforms. A native port to Mac OS X is now available.

<span id="6"/>
6. How do I get it?
-------------------
Click on the [Downloads](download.html) link (or on the navigation bar on the left). Installers are available for Windows and Mac OS X, and source and binary packages are available for Linux.

<span id="7"/>
7. I found a bug! What now?
---------------------------
Congratulations! Please [file a bug report](http://sourceforge.net/tracker/?group_id=91526&atid=597446) . FreeMat is a fairly complicated program. Simply saying "it crashed" is not particularly helpful. If possible, please provide a short function or script that reproduces the problem. That will go a long way towards helping us figure out the problem. Also, the bug tracking feature of SourceForge will allow you to put in bugs anonymously, but please don't! Anonymous bug reports are difficult to follow up on.

<span id="8"/>
8. Where is function xyz?
-------------------------
There are a number of basic functions that are missing from FreeMat's repertoire. They will be added as time goes on. If there is a particular function you would like to see, either write it yourself or put in an RFE (Request For Enhancement) here.

<span id="9"/>
9. Who wrote FreeMat and why?
-----------------------------
FreeMat has been in development by myself (Samit Basu) over a period of four years with help from a number of contributers distributed throughout the globe. Why not? Seriously, my main reason is to provide a free implementation of a reasonable part of the Matlab API, and add some extensions along the way.

<span id="10"/>
10. Are there any development related pages?
-------------------------------------------
Yes. Please see our [WIKI based Development Pages](http://code.google.com/p/freemat/wiki/Development)

