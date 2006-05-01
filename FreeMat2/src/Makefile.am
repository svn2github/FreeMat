bin_PROGRAMS = FreeMat

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/libs/libFN -I@top_srcdir@/src

EXTRA_DIST = appIcon.icns freemat.ico freemat.rc freematv2.ico

FreeMat_HFILES = \
FuncMode.hpp \
FuncTerminal.hpp \
MainApp.hpp \
SocketCB.hpp 

if USECLI
FreeMat_HFILES += Terminal.hpp \
 DumbTerminal.hpp
endif

FreeMat_MOC = application.moc.cpp \
FuncMode.moc.cpp \
MainApp.moc.cpp \
SocketCB.moc.cpp 

if USECLI
FreeMat_MOC += Terminal.moc.cpp \
DumbTerminal.moc.cpp 
endif

FreeMat_QRC = FreeMat.qrc.cpp

BUILT_SOURCES = $(FreeMat_MOC) $(FreeMat_QRC)

FreeMat_SOURCES = application.cpp \
FuncMode.cpp \
FuncTerminal.cpp \
MainApp.cpp \
main.cpp \
SocketCB.cpp \
$(FreeMat_MOC) \
$(FreeMat_QRC)

if USECLI
FreeMat_SOURCES += DumbTerminal.cpp \
Terminal.cpp 
endif

FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libMex/libMex.a $(QGL_LIBS) $(QT_LIBS) $(FLIBS)
FreeMat_LDFLAGS = $(QT_CXXFLAGS)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(FreeMat_MOC)

AM_CPPFLAGS = $(QT_CXXFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<

%.qrc.cpp: %.qrc
	$(QT_RCC) -o $@ $<



