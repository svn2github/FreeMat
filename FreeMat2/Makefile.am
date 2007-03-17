SUBDIRS = libs src images tools

DISTCHECK_CONFIGURE_FLAGS = LDFLAGS=-L/usr/lib/atlas CPPFLAGS=-I/usr/include/ufsparse

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

EXTRA_DIST = $(top_srcdir)/help

nobase_resource_DATA = $(top_srcdir)/help

clean-manifest:
	rm -rf $(top_srcdir)/manifest.am
	touch $(top_srcdir)/manifest.am

package: all
	chmod +x tools/disttool/makepkg
	tools/disttool/makepkg @platform@ $(top_builddir) $(top_srcdir) $(QT4DIR)

install-data-hook:
	cd $(DESTDIR)$(resourcedir); rm -f toolbox; $(LN_S) help/toolbox

install-exec-hook:
	$(DESTDIR)$(bindir)/FreeMat -i $(DESTDIR)$(resourcedir)


