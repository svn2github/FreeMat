bin_PROGRAMS = FreeMat

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/libs/libFN -I@top_srcdir@/src

EXTRA_DIST = appIcon.icns freemat.ico freemat.rc FreeMat.qrc freematv2.ico

FreeMat_HFILES = DumbTerminal.hpp \
FileTool.hpp \
FuncMode.hpp \
FuncTerminal.hpp \
helpgen.hpp \
HistoryWidget.hpp \
MainApp.hpp \
SocketCB.hpp \
Terminal.hpp \
ToolDock.hpp \
VariablesTool.hpp

FreeMat_MOC = application.moc.cpp \
DumbTerminal.moc.cpp \
FileTool.moc.cpp \
FuncMode.moc.cpp \
HistoryWidget.moc.cpp \
MainApp.moc.cpp \
SocketCB.moc.cpp \
Terminal.moc.cpp \
ToolDock.moc.cpp \
VariablesTool.moc.cpp \
helpgen.moc.cpp

BUILT_SOURCES = $(FreeMat_MOC)

FreeMat_SOURCES = application.cpp \
DumbTerminal.cpp \
FileTool.cpp \
FuncMode.cpp \
FuncTerminal.cpp \
helpgen.cpp \
HistoryWidget.cpp \
MainApp.cpp \
main.cpp \
SocketCB.cpp \
Terminal.cpp \
ToolDock.cpp \
VariablesTool.cpp \
$(FreeMat_MOC)

FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libMex/libMex.a $(QGL_LIBS) $(QT_LIBS) $(FLIBS)
FreeMat_LDFLAGS = $(QT_CXXFLAGS)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(FreeMat_MOC)

AM_CPPFLAGS = $(QT_CXXFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<



