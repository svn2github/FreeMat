SUBDIRS = libs src images tools

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

include $(top_srcdir)/manifest.am

clean-manifest:
	rm -rf $(top_srcdir)/manifest.am
	touch $(top_srcdir)/manifest.am

package: all
	chmod +x tools/disttool/makepkg
	tools/disttool/makepkg @platform@ $(top_builddir) $(top_srcdir) $(QT4DIR)

help: all check
	$(top_srcdir)/tools/disttool/makehelp $(top_builddir) $(top_srcdir)


install-data-hook:
	cd $(DESTDIR)$(resourcedir); rm -f toolbox; $(LN_S) $(DESTDIR)/help/toolbox

install-exec-hook:
	$(DESTDIR)$(bindir)/FreeMat -i $(DESTDIR)$(resourcedir)


