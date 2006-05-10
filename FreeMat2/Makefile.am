SUBDIRS = libs src images tools

DESTDIR = src

dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -rf `find $(distdir) -name *~`

distclean-local:
	rm -rf extern
	rm -rf tools/disttool/makepkg

help-mingw: @abs_top_builddir@/tools/helpgen/helpgen
	declare -x PATH="$(PATH):@QT4DIR@/bin" && @abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help-unix: @abs_top_builddir@/tools/helpgen/helpgen

# Regenerate all of the help files
help: @help_target@

htmlhelpfiles = `find help/html -type f -printf "%p "`
texthelpfiles = `find help/text -type f -printf "%p "`
mfiles = `find help/MFiles -type f -printf "%p "`
mfiledirs = `find help/MFiles -type d -printf "%p "`

resourcedir = $(datadir)/$(PACKAGE)-$(VERSION)

# Rebuild the help files in the source directory
help-dir: 
	rm -rf $(srcdir)/help
	mkdir -p $(top_srcdir)/help/html
	mkdir -p $(top_srcdir)/help/text
	mkdir -p $(top_srcdir)/help/MFiles
	echo "EXTRA_DIST = MFiles extern/Packages extern/patches $(htmlhelpfiles) $(texthelpfiles) $(mfiles) " > $(srcdir)/help/manifest.am 
	echo "nobase_resource_DATA = $(htmlhelpfiles) $(texthelpfiles) $(mfiles) " >> $(srcdir)/help/manifest.am 
	for fname in $(mfiledirs); do \
	mkdir -p $(top_srcdir)/$$fname ;\
	done;
	for fname in $(htmlhelpfiles) $(texthelpfiles) $(mfiles); do \
	cp $(top_builddir)/$$fname $(top_srcdir)/$$fname ;\
	done;
	touch help.stamp


#install-data-local:
#	@for f in `find help -type f -print`; do \
### Compute the install directory at runtime.
#	d="echo $$f | sed -e s,/[^/]*$$,,'"; \
### Make the install directory.
#	$(mkinstalldirs) $(DESTDIR)$(includedir)/$$d; \
### Find the header file -- in our case it might be in srcdir or
### it might be in the build directory.  "p" is the variable that
### names the actual file we will install.
#	if test -f $(srcdir)/$$f; then p=$(srcdir)/$$f; else p=$$f; fi; \
### Actually install the file.
#	$(INSTALL_DATA) $$p $(DESTDIR)$(includedir)/$$f; \
#	done
#
#foodir = $(datadir)/foo
#foo_DATA = help/html
#
#mfiledir_set = `cd $(top_builddir)/help/MFiles && find . -type d -print`
#mfile_set = `cd $(top_builddir)/help/MFiles && find . -name '*.m' -print`
#help_set = `cd $(top_builddir)/help/html && find . -type f -print`
#text_set = `cd $(top_builddir)/help/text && find . -type f -print`
#
#install-data-local: 
#	$(mkinstalldirs) $(DESTDIR)$(datadir)/FreeMat-$(VERSION)
#	$(mkinstalldirs) $(DESTDIR)$(datadir)/FreeMat-$(VERSION)/MFiles
#	for dirname in $(mfiledir_set); do \
#	  $(mkinstalldirs) $(DESTDIR)$(datadir)/FreeMat-$(VERSION)/MFiles/$$dirname;\
#	done;
#	for scriptname in $(mfile_set); do \
#	  $(INSTALL_DATA) $(top_builddir)/help/MFiles/$$scriptname $(DESTDIR)$(datadir)/FreeMat-$(VERSION)/MFiles/$$scriptname;\
#	done;
#	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help
#	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/pdf
#	$(INSTALL_DATA) $(top_builddir)/help/latex/main.pdf $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/pdf/FreeMat-Manual-$(VERSION).pdf
#	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/html
#	for fname in $(help_set); do \
#	  $(INSTALL_DATA) $(top_builddir)/help/html/$$fname $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/html/$$fname;\
#	done;
#	$(mkinstalldirs) $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/text
#	for fname in $(text_set); do \
#	  $(INSTALL_DATA) $(top_builddir)/help/text/$$fname $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/text/$$fname;\
#	done;
#
#
#uninstall-local:
#	for scriptname in $(mfile_set); do \
#	  rm -f $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/MFiles/$$scriptname;\
#	done;
#	rm -f $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/pdf/FreeMat-Manual-$(VERSION).pdf
#	for fname in $(help_set); do					\
#	  rm -f $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/html/$$fname;	\
#	done;
#	for fname in $(text_set); do \
#	  rm -f $(DESTDIR)$(prefix)/share/FreeMat-$(VERSION)/help/text/$$fname;\
#	done;

include $(top_srcdir)/help/manifest.am
