noinst_LIBRARIES = libXP.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex

libXP_a_MOC = KeyManager.moc.cpp \
QTTerm.moc.cpp \
Editor.moc.cpp \
PathTool.moc.cpp \
highlighter.moc.cpp

BUILT_SOURCES = $(libXP_a_MOC)

libXP_a_HFILES = DynLib.hpp \
KeyManager.hpp \
PathSearch.hpp \
QTTerm.hpp \
System.hpp \
Editor.hpp \
PathTool.hpp \
highlighter.hpp \
Common.hpp

libXP_a_SOURCES = DynLib.cpp \
KeyManager.cpp \
PathSearch.cpp \
QTTerm.cpp \
System.cpp \
Editor.cpp \
PathTool.cpp \
highlighter.cpp \
Common.cpp \
$(libXP_a_MOC) \
$(libXP_a_HFILES)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libXP_a_MOC)

AM_CPPFLAGS = $(QT_CXXFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<
