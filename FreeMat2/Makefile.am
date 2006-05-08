SUBDIRS = libs src images tools

DESTDIR = src

EXTRA_DIST = MFiles extern/Packages extern/patches

dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -rf `find $(distdir) -name *~`

help-mingw: FORCE
	declare -x PATH="$(PATH):@QT4DIR@/bin" && @abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help-unix: FORCE
	@abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help: @help_target@

FORCE:
