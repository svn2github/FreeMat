noinst_LIBRARIES = libXP.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex

libXP_a_MOC = KeyManager.moc.cpp \
QTTerm.moc.cpp \
Editor.moc.cpp \
PathTool.moc.cpp \
highlighter.moc.cpp \
VariablesTool.moc.cpp \
FileTool.moc.cpp \
HistoryWidget.moc.cpp \
ToolDock.moc.cpp \
VariablesTool.moc.cpp

libXP_a_UI = findform.ui.h \
replaceform.ui.h

BUILT_SOURCES = $(libXP_a_MOC) $(libXP_a_UI)

libXP_a_HFILES = DynLib.hpp \
KeyManager.hpp \
PathSearch.hpp \
QTTerm.hpp \
System.hpp \
Editor.hpp \
PathTool.hpp \
FileTool.hpp \
highlighter.hpp \
HistoryWidget.hpp \
ToolDock.hpp \
VariablesTool.hpp \
Common.hpp \
$(libXP_a_UI)

nodist_libXP_a_SOURCES = $(libXP_a_MOC) $(libXP_a_UI)

libXP_a_SOURCES = DynLib.cpp \
KeyManager.cpp \
PathSearch.cpp \
QTTerm.cpp \
System.cpp \
Editor.cpp \
PathTool.cpp \
highlighter.cpp \
FileTool.cpp \
Common.cpp \
ToolDock.cpp \
VariablesTool.cpp \
HistoryWidget.cpp \
$(libXP_a_HFILES)


AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libXP_a_MOC)

AM_CPPFLAGS = $(QT_CFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<

%.ui.h: %.ui
	$(QT_UIC) -o $@ $<
