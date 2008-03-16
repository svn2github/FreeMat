Home
====

FreeMat is a free environment for rapid engineering and scientific prototyping and data processing. It is similar to commercial systems such as MATLAB from Mathworks, and IDL from Research Systems, but is Open Source. FreeMat is available under the GPL license.


[!["Screen"](assets/screencap_composite_small.png "Screen")](screenshots.html) [!["Download"](assets/download.png "Download")](download.html)


Latest News - 2007-09-23 - FreeMat 3.5 Released
-----------------------------------------------
We are pleased to announce the release of FreeMat 3.5. This is primarily a bugfix release that adds a few new features. Here is the list of changes.

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
