SUBDIRS = libs src images tools

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

include $(top_srcdir)/manifest.am

package: all
	chmod +x tools/disttool/makepkg
	tools/disttool/makepkg @platform@ $(top_builddir) $(top_srcdir) $(QT4DIR)

help: all check
	$(top_srcdir)/tools/disttool/makehelp $(top_builddir) $(top_srcdir)


install-data-hook:
	@ln -s $(DESTDIR)$(resourcedir)/help/MFiles $(DESTDIR)$(resourcedir)/mfiles

dist-hook:
	buildnum=`svn info $(top_srcdir)| sed -n -e "/Revision: \([0-9]*\)/p" | sed "s/Revision: //g"`;\
	sed "s/SVN/$$buildnum/g" < $(top_srcdir)/libs/libFreeMat/WalkTree.cpp > $(distdir)/libs/libFreeMat/WalkTree.cpp
