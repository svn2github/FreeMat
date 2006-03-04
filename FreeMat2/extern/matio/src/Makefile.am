# automake file for scats
# $Revision$ $State$
# $Date$

AUTOMAKE_OPTIONS = foreign 1.8 no-dependencies
VPATH = @srcdir@


AM_CFLAGS  = -I. $(ZLIB_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS)

if HAVE_ZLIB
    ZLIB_SRC = inflate.c
endif

include_HEADERS     = matio.h matioConfig.h
noinst_LIBRARIES    = libmatio.a
libmatio_a_SOURCES = snprintf.c endian.c io.c $(ZLIB_SRC) read_data.c \
                      mat5.c mat4.c mat.c

