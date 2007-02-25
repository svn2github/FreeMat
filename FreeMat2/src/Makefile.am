bin_PROGRAMS = FreeMat

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/libs/libFN -I@top_srcdir@/src -I@top_builddir@/libs/libXP

EXTRA_DIST = appIcon.icns freemat.ico freematv2.ico

FreeMat_HFILES = \
FuncMode.hpp \
FuncTerminal.hpp \
MainApp.hpp \
Terminal.hpp \
DumbTerminal.hpp \
application.hpp 

FreeMat_MOC = application.moc.cpp \
FuncMode.moc.cpp \
MainApp.moc.cpp \
Terminal.moc.cpp \
DumbTerminal.moc.cpp 

FreeMat_QRC = FreeMat_qrc.cpp

BUILT_SOURCES = $(FreeMat_MOC) $(FreeMat_QRC)

SUFFIXES = .rc
.rc.o :
	$(WINDRES) -I@top_srcdir@/src $< $@

FreeMat_SOURCES = application.cpp \
FuncMode.cpp \
FuncTerminal.cpp \
MainApp.cpp \
main.cpp \
DumbTerminal.cpp \
Terminal.cpp \
$(FreeMat_HFILES) \
FreeMat.qrc

nodist_FreeMat_SOURCES = $(FreeMat_MOC) $(FreeMat_QRC) 

if WIN32
FreeMat_SOURCES += freemat.rc
endif

FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libMex/libMex.a $(QGL_LIBS) $(QT_LIBS)
FreeMat_LDFLAGS = $(QT_CXXFLAGS)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(FreeMat_MOC) $(FreeMat_QRC)

AM_CPPFLAGS = $(QT_CFLAGS) 

application.moc.cpp : application.hpp
	$(QT_MOC) -o application.moc.cpp application.hpp

FuncMode.moc.cpp : FuncMode.hpp
	$(QT_MOC) -o FuncMode.moc.cpp FuncMode.hpp

MainApp.moc.cpp : MainApp.hpp
	$(QT_MOC) -o MainApp.moc.cpp MainApp.hpp

Terminal.moc.cpp : Terminal.hpp
	$(QT_MOC) -o Terminal.moc.cpp Terminal.hpp

DumbTerminal.moc.cpp : DumbTerminal.hpp
	$(QT_MOC) -o DumbTerminal.moc.cpp DumbTerminal.hpp

FreeMat_qrc.cpp : FreeMat.qrc
	$(QT_RCC) -o FreeMat_qrc.cpp FreeMat.qrc

