noinst_LIBRARIES = libMatC.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libMex 

libMatC_a_HFILES = MCScanner.hpp \
MCParser.hpp \
JIT.hpp \
CodeGen.hpp

libMatC_a_SOURCES = MCScanner.cpp \
MCParser.cpp \
JIT.cpp \
CodeGen.cpp

AUTOMAKE_OPTIONS 	= foreign

AM_CPPFLAGS = $(QT_CFLAGS) 
