SUBDIRS = extern libs src images tools

DESTDIR = src

EXTRA_DIST = MFiles

dist-hook:
	rm -rf `find $(distdir)/MFiles -name .svn`
	rm -rf `find $(distdir)/MFiles -name '*~'`

help-mingw: FORCE
	declare -x PATH="$(PATH):@QT4DIR@/bin" && @abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help-unix: FORCE
	@abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help: @help_target@

FORCE:
