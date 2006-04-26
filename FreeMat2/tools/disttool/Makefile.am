bin_PROGRAMS = disttool

EXTRA_DIST = freemat_nsi.in

disttool_HFILES = disttool.hpp

disttool_MOC = disttool.moc.cpp

BUILT_SOURCES = $(disttool_MOC)

disttool_SOURCES = disttool.cpp \
$(disttool_MOC)

disttool_LDADD =  $(QGL_LIBS) $(QT_LIBS) $(FLIBS)

disttool_LDFLAGS = $(QT_CXXFLAGS)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(FreeMat_MOC)

AM_CPPFLAGS = $(QT_CXXFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<
