SUBDIRS = libs src images tools

DISTCHECK_CONFIGURE_FLAGS = LDFLAGS=-L/usr/lib/atlas CPPFLAGS=-I/usr/include/ufsparse

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

EXTRA_DIST = help toolbox

## nobase_resource_DATA = help/html/index.html toolbox/array/all.m

include $(top_srcdir)/manifest.am

install-exec-hook:
	$(DESTDIR)$(bindir)/FreeMat -i $(DESTDIR)$(resourcedir)

