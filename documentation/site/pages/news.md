News
====

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

Latest News - 2008-03-17 - FreeMat 3.6 Released
-----------------------------------------------
We are pleased to announce the release of FreeMat 3.6. This is primarily a bugfix release that adds a few new features,
mostly to the UI. Here is the list of changes:

 * Added ODE45 solver from Sebastien Loisel
 * Added SVG support for exporting figures.
 * Added "recent files" to editor.
 * Added keyboard shortcuts to editor and main application menus.
 * Added increase/decrease indent by tab/shift-tab in editor
 * Added execute button to debug toolbar
 * Added quiver plots
 * Added execute-selected-text button
 * Added directory-change tool to main toolbar, and rearranged windows on the main page.
 * Second pass at the JIT compiler.  Completion of the JIT and it's general activation will wait for FreeMat4.
 * Display of complex numbers
 * Helpwin now accepts an argument.
 * Replaced the toolbox with a tabbed widget.
 * Code clean up for -Wall.
 * Improved the behavior of editor when opening and saving (new)files.
 * Corrected norms of double args on 64 bit machines
 * Interplin1 now works with more argument types.
 * Any, all, min and max handle empties more gracefully.
 * Corrected color display of RGB images
 * Modified bitcmp function for compatibility with matlab
 * Modified fprintf to follow matlab's behavior when no handle is specified.
 * Set default view for 3D plots to view(3).
 * Fixes for anonymous functions and code block leaks.
 * Fixed the "comment" and "uncomment" bugs when applying to the whole
 * Fixed failed feval for local functions
 * Fixed program crash after clear history.
 * Fixed bug in complex matrix assignment operators.


2007-09-23 - FreeMat 3.5 Released
---------------------------------
We are pleased to announce the release of FreeMat 3.5. This is primarily a bugfix release that adds a few new features. Here is the list of changes:

 * Fixed path detection problem on win32 (forward vs backward slashes)
 * Added ability to change the scrollback length via edit->preferences menu
 * Fixed diary command so that commands are echoed as well as their outputs
 * Fixed diary so that the commands are echoed as well as the outputs.
 * Added fflush command.
 * Added ctrl-w to clear the current line
 * Changed sort algorithm to a stable one.
 * Fixed bug in linspace
 * Added the new ctype interface - thanks to Scott McOlash for suggesting it, and GE Global Research for letting us release it under the GPL.
 * Converted FFTW to estimate mode from measure mode - should fix problems with long FFT times for large FFTs
 * Added stack trace on errors, even when not in dbauto on mode.
 * Improved the autoconfig script for mac builds.
 * Moved loadlib and import to external documentation section
 * Added external section to the documentation
 * Changed the default mode for "image" command" to "scaled" instead of "direct". This is less compatible with MATLAB, but I don't think it's unreasonably so.
 * Added "edit" command.
 * Changed default type for zero function to double precision.
 * Added imagesc command. Modified image command to show un-scaled images. 

2007-08-05 - FreeMat 3.3 Released
---------------------------------
We are pleased to announce the release of FreeMat 3.3. Available for download from [SourceForge](http://freemat.sf.net "Download from SourceForge"). Here is a brief summary of the new features in this version:

1. The output format for FreeMat is now closer to MATLABs, and can be adjusted using the format command.
2. The console window has been rewritten, and should be more robust and faster.
3. Contour plots in 2D and 3D are now supported, along with labels.
4. Anonymous functions are now supported.
5. Nested functions are now supported
6. Audio record/playback functionality has been added via the wavplay/wavrecord/etc. functions.
7. Memory usage has been improved. FreeMat should use less memory than previous versions.
8. Performance improvements.
9. Many new functions added. See the detailed change list for the complete list.
10. Bug fixes. See the detailed change list for the complete list.

2007-04-14  FreeMat 3.1 Released
--------------------------------
The latest version of FreeMat (3.1) has just been released. Beyond the usual bug fixes, this version of FreeMat includes:

 * A new Threading API, which allows you to take advantage of multi-core and multi-processor machines from within your FreeMat programs.
 * You can also perform powerful Perl-style regular expression manipulations in 3.1 using the regexp command, and its variants.
 * A new, unified multi-platform regression test suite is now shipped with FreeMat (as the run\_tests command), which allows you to verify the functionality of your installation.
 * Plotting functions have been improved through the use of an explicit double-buffering technique which eliminates flicker during plot updates. Note that to increase compatibility with MATLAB, you must now include explicit drawnow commands to get updates within scripts.
 * FreeMat also has new functions for reading URLs, and parsing HTML and XML files.
 * The help window has a search tool now to help you quickly locate functions and help topics.
 * The memory usage of FreeMat has been significantly improved - so that FreeMat operates with less memory and should run faster than previous versions.
 * Also, you can now specify a script startup.m that is automatically executed at startup time.

2007-01-14  - FreeMat 3.0 Released
----------------------------------
The current release of FreeMat is 3.0. It represents a significant improvement over the prior release (2.0). Here is a partial list of the changes:

 * MATLAB-style GUIs and Widgets
 * Speed improvements (loops and indexing operations are 10 to 100 times faster)
 * A new parser that recognizes more of the MATLAB syntax
 * A debugger that is integrated with the editor
 * Support for 64-bit integers
 * Improved MAT-file support
 * Universal binary support for Mac OS X
 * New graph/plot tools to rotate, zoom and pan plots.
 * Builds are now based on ATLAS for improved matrix performance
 * Various bug fixes. 