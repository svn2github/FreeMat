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

CXX=@MPICXX@
CFLAGS=@CFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cflags`
CXXFLAGS=@CXXFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cxxflags`  -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/extlibs/libtecla -I@top_srcdir@/libs/libGraphics  -I@top_srcdir@/extlibs/libffi-2.00-beta/include  -I@top_srcdir@/extlibs/volpack-1.0c2/src

bin_PROGRAMS = FMKernelMPI

FMKernelMPI_SOURCES = KernelMainMPI.cpp \
TeclaInterface.cpp \
TeclaInterface.hpp

LIBS = `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --libs` @LIBS@ 

FMKernelMPI_DEPENDENCIES =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a  @top_builddir@/extlibs/libffi-2.00-beta/libffi.a @top_builddir@/extlibs/libtecla/libtecla.a
FMKernelMPI_LDADD =  @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libFreeMat/libFreeMat.a   @top_builddir@/extlibs/libffi-2.00-beta/libffi.a @top_builddir@/extlibs/libtecla/libtecla.a 

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign

LFLAGS = @LFLAGS@
YFLAGS = @YFLAGS@
