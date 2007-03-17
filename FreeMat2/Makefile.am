SUBDIRS = libs src images tools

DISTCHECK_CONFIGURE_FLAGS = LDFLAGS=-L/usr/lib/atlas CPPFLAGS=-I/usr/include/ufsparse

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

EXTRA_DIST = $(top_srcdir)/help $(top_srcdir)/toolbox

nobase_resource_DATA = $(top_srcdir)/help $(top_srcdir)/toolbox

install-exec-hook:
	$(DESTDIR)$(bindir)/FreeMat -i $(DESTDIR)$(resourcedir)


