SUBDIRS = libs src images tools

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

include $(top_srcdir)/manifest.am

package: all
	chmod +x tools/disttool/makepkg
	tools/disttool/makepkg @platform@ $(top_builddir) $(top_srcdir) $(QTDIR)

help: all check
	$(top_srcdir)/tools/disttool/makehelp $(top_builddir) $(top_srcdir)

