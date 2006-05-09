SUBDIRS = libs src images tools

DESTDIR = src

EXTRA_DIST = MFiles extern/Packages extern/patches

dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -rf `find $(distdir) -name *~`

distclean-local:
	rm -rf extern
	rm -rf tools/disttool/makepkg

help-mingw: 
	declare -x PATH="$(PATH):@QT4DIR@/bin" && @abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help-unix: 
	@abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help: @help_target@


#Make DVI builds the documentation and runs the regression tests
dvi: help

mfiledir_set = `cd $(top_builddir)/help/MFiles && find . -type d -print`
mfile_set = `cd $(top_builddir)/help/MFiles && find . -name '*.m' -print`
help_set = `cd $(top_builddir)/help/html && find . -type f -print`
text_set = `cd $(top_builddir)/help/text && find . -type f -print`

install-data-local: 
	$(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/MFiles
	for dirname in $(mfiledir_set); do \
	  $(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/MFiles/$$dirname;\
	done;
	for scriptname in $(mfile_set); do \
	  $(INSTALL) $(srcdir)/MFiles/$$scriptname $(prefix)/share/FreeMat-$(VERSION)/MFiles/$$scriptname;\
	done;
	$(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/help
	$(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/help/pdf
	$(INSTALL) $(top_builddir)/help/latex/main.pdf $(prefix)/share/FreeMat-$(VERSION)/help/pdf/FreeMat-Manual-$(VERSION).pdf
	$(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/help/html
	for fname in $(help_set); do \
	  $(INSTALL) $(top_builddir)/help/html/$$fname $(prefix)/share/FreeMat-$(VERSION)/help/html/$$fname;\
	done;
	$(mkinstalldirs) $(prefix)/share/FreeMat-$(VERSION)/help/text
	for fname in $(text_set); do \
	  $(INSTALL) $(top_builddir)/help/text/$$fname $(prefix)/share/FreeMat-$(VERSION)/help/text/$$fname;\
	done;


uninstall-local:
	for scriptname in $(mfile_set); do \
	  rm -f $(prefix)/share/FreeMat-$(VERSION)/MFiles/$$scriptname;\
	done;
	rm -f $(prefix)/share/FreeMat-$(VERSION)/help/pdf/FreeMat-Manual-$(VERSION).pdf
	for fname in $(help_set); do					\
	  rm -f $(prefix)/share/FreeMat-$(VERSION)/help/html/$$fname;	\
	done;
	for fname in $(text_set); do \
	  rm -f $(prefix)/share/FreeMat-$(VERSION)/help/text/$$fname;\
	done;
