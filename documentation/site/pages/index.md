Home
====

FreeMat is a free environment for rapid engineering and scientific prototyping and data processing. It is similar to commercial systems such as MATLAB from Mathworks, and IDL from Research Systems, but is Open Source. FreeMat is available under the GPL license.


[!["Screen"](assets/screencap_composite_small.png "Screen")](screenshots.html) [!["Download"](assets/download.png "Download")](download.html)


Latest News - 2009-10-09 - FreeMat 4.0 Released
----------------------------------------------
We are pleased to announce the release of FreeMat 4.0 . This version brings major feature improvements and changes to the internals of FreeMat. Here is a  list of changes:

 * Improved Editor with integrated debugger
 * Improved Main Application UI with dockable workspace browser and command history
 * Just In Time compiler (enabled by default)
 * Greatly improved compatibility with Matlab (over 366 compatibility tests pass)
 * Dynamic linking with BLAS (users can use custom optimized BLAS libraries)
 * Vectorized fprintf, sprintf, fscanf, sscanf functions
 * Added patch handle graphics object
 * Much faster figure drawing
 * Ability to handle huge arrays (more than 2GB) when compiled under 64 bit OS
 * Code profiler

Internal changes:

 * New array class implementation
 * Compatible type handling
 * JIT compiler
 * CMake build system  
