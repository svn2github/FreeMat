## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2000 Gary V. Vaughan
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

CFLAGS=@CFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cflags`
CXXFLAGS=@CXXFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cxxflags`  -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/extlibs/libtecla -I@top_srcdir@/libs/libGraphics  -I@top_srcdir@/extlibs/libffi-2.00-beta/include  -I@top_srcdir@/extlibs/volpack-1.0c2/src

bin_PROGRAMS = FreeMatGUI FreeMat FMKernel
##bin_PROGRAMS = FreeMat

FMKernel_SOURCES = KernelMain.cpp \
TeclaInterface.hpp \
TeclaInterface.cpp 

FreeMat_SOURCES =  AppNoGUI.cpp \
AppNoGUI.hpp \
TeclaInterface.hpp \
TeclaInterface.cpp \
CLIThread.cpp \
CLIThread.hpp

FreeMatGUI_SOURCES = App.cpp \
App.hpp \
GUIInterface.hpp \
GUIInterface.cpp \
CLIBuffer.hpp \
CLIBuffer.cpp \
SessionFrame.cpp \
SessionFrame.hpp \
CLIThread.cpp \
CLIThread.hpp \
wxCLI2.cpp \
wxCLI2.hpp 

FreeMat_DEPENDENCIES = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/extlibs/libtecla/libtecla.a  @top_builddir@/extlibs/libffi-2.00-beta/libffi.a @top_builddir@/extlibs/volpack-1.0c2/src/libvolpack.a
FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/extlibs/libffi-2.00-beta/libffi.a @top_builddir@/extlibs/libtecla/libtecla.a `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --libs` @top_builddir@/extlibs/volpack-1.0c2/src/libvolpack.a

FreeMatGUI_DEPENDENCIES =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a  @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a   @top_builddir@/extlibs/libffi-2.00-beta/libffi.a  @top_builddir@/extlibs/volpack-1.0c2/src/libvolpack.a
FreeMatGUI_LDADD =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a   @top_builddir@/extlibs/libffi-2.00-beta/libffi.a `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --libs`  @top_builddir@/extlibs/volpack-1.0c2/src/libvolpack.a

FMKernel_DEPENDENCIES =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a  @top_builddir@/extlibs/libffi-2.00-beta/libffi.a @top_builddir@/extlibs/libtecla/libtecla.a
FMKernel_LDADD =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a   @top_builddir@/extlibs/libffi-2.00-beta/libffi.a `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --libs` @top_builddir@/extlibs/libtecla/libtecla.a

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign

LFLAGS = @LFLAGS@
YFLAGS = @YFLAGS@
