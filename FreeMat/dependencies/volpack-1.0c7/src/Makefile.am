# automake file for libvolpack, process with automake
# Andy Loening 2001.09.29/2003.05.24

lib_LTLIBRARIES = libvolpack.la

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

if OS_WIN32
install-libtool-import-lib:
	$(INSTALL) .libs/libvolpack.dll.a $(DESTDIR)$(libdir)

uninstall-libtool-import-lib:
	-rm $(DESTDIR)$(libdir)/libvolpack.dll.a
else
install-libtool-import-lib:
uninstall-libtool-import-lib:
endif


libvolpack_la_LDFLAGS = -version-info $(LIBVERSION) $(no_undefined)
AM_CFLAGS = $(OPT_FLAGS)
include_HEADERS = \
	volpack.h

libvolpack_la_SOURCES = \
	vp_check.c \
	vp_context.c \
	vp_extract.c \
	vp_file.c \
	vp_linalg.c \
	vp_octree.c \
	vp_renderA.c \
	vp_renderB.c \
	vp_renderC.c \
	vp_renderR.c \
	vp_resample.c \
	vp_rle.c \
	vp_shade.c \
	vp_transpose.c \
	vp_util.c \
	vp_view.c \
	vp_warp.c \
	vp_global.h \
	patchlevel.h \
	$(VP_OPTS_SRCS) \
	$(COMP_SRCS) \
	$(WARP_SRCS)

# C define flags corresponding to COMP_SRCS; these tell the dispatch
# routines in the library which object files are available
OPT_FLAGS = -DCOMP_AC11B   -DCOMP_AC31B   -DCOMP_AC32B -DCOMP_AR11B   -DCOMP_AR31B   -DCOMP_AR32B

OPT_SRCS = \
	vp_compAC11B.c \
	vp_compAC31B.c \
	vp_compAC32B.c \
	vp_compAR11B.c \
	vp_compAR31B.c \
	vp_compAR32B.c

DEFAULT_HEADER = no_edit_header
COMP_IN = vp_compA.m4
WARP_IN = vp_warpA.m4

VP_OPTS_SRCS = vp_opts.c

COMP_SRCS = \
	vp_compAC1NB.c \
	vp_compAC3NB.c \
	vp_compAR1NB.c \
	vp_compAR3NB.c \
	vp_compAC1PB.c \
	vp_compAC3PB.c \
	vp_compAR1PB.c \
	vp_compAR3PB.c \
	vp_compAC00G.c \
	vp_compAR00G.c \
	vp_compAC1NS.c \
	vp_compAC3NS.c \
	vp_compAR1NS.c \
	vp_compAR3NS.c \
	$(OPT_SRCS)

WARP_SRCS = \
	vp_warpA101N.c \
	vp_warpA301N.c \
	vp_warpA110N.c \
	vp_warpA111N.c \
	vp_warpA330N.c \
	vp_warpA331N.c \
	vp_warpA330R.c \
	vp_warpA331R.c

makeopts: makeopts.c
	$(CC) -o makeopts makeopts.c

$(VP_OPTS_SRCS): makeopts Makefile
	./makeopts $@ $(CFLAGS)

$(COMP_SRCS): $(DEFAULT_HEADER) $(COMP_IN)
	@for i in $(COMP_SRCS) ; \
	    do \
		rm -f $$i ;\
		echo "$(M4) -DSourceFile=$$i $(DEFAULT_HEADER) $(COMP_IN) > $$i" ;\
		$(M4) -DSourceFile=$$i $(DEFAULT_HEADER) $(COMP_IN) > $$i; \
		chmod -w $$i; \
	    done

$(WARP_SRCS): $(DEFAULT_HEADER) $(WARP_IN)
	@for i in $(WARP_SRCS) ; \
	    do \
		rm -f $$i ;\
		echo "$(M4) -DSourceFile=$$i $(DEFAULT_HEADER) $(WARP_IN) > $$i" ;\
		$(M4) -DSourceFile=$$i $(DEFAULT_HEADER) $(WARP_IN) > $$i; \
		chmod -w $$i; \
	    done



MOSTLYCLEANFILES = $(COMP_SRCS) $(WARP_SRCS) $(VP_OPTS_SRCS) makeopts
DISTCLEANFILES = *~


install-data-local: install-libtool-import-lib
uninstall-local: uninstall-libtool-import-lib
