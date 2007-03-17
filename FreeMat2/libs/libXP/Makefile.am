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
$(libXP_a_HFILES) \
findform.ui \
replaceform.ui


AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libXP_a_MOC) $(libXP_a_UI)

AM_CPPFLAGS = $(QT_CFLAGS) 

KeyManager.moc.cpp : KeyManager.hpp
	$(QT_MOC) -o $@ $<

QTTerm.moc.cpp : QTTerm.hpp
	$(QT_MOC) -o $@ $<

Editor.moc.cpp : Editor.hpp
	$(QT_MOC) -o $@ $<

PathTool.moc.cpp : PathTool.hpp
	$(QT_MOC) -o $@ $<

highlighter.moc.cpp : highlighter.hpp
	$(QT_MOC) -o $@ $<

VariablesTool.moc.cpp : VariablesTool.hpp
	$(QT_MOC) -o $@ $<

FileTool.moc.cpp : FileTool.hpp
	$(QT_MOC) -o $@ $<

HistoryWidget.moc.cpp : HistoryWidget.hpp
	$(QT_MOC) -o $@ $<

ToolDock.moc.cpp : ToolDock.hpp
	$(QT_MOC) -o $@ $<

findform.ui.h : findform.ui
	$(QT_UIC) -o $@ $<

replaceform.ui.h : replaceform.ui
	$(QT_UIC) -o $@ $<
