AC_DEFUN([MATIO_CHECK_ZLIB],
[
AC_ARG_WITH(zlib,AS_HELP_STRING([--with-zlib=DIR],
            [Prefix where zlib Library is installed]))
if test "x$with_zlib" != "xno"
then
    saved_LIBS="$LIBS"
    saved_CFLAGS="$CFLAGS"

    AC_MSG_CHECKING([for zlib Library])

    if test "x$with_zlib" = "x" -o "x$with_zlib" = "xyes"
    then
        ZLIB_LDOPTS=""
        ZLIB_CFLAGS=""
        LIBS="$saved_LIBS -lz"
        CFLAGS="$saved_CFLAGS $ZLIB_CFLAGS"
    else
        ZLIB_LDOPTS="$with_zlib/$acl_libdirstem"
        ZLIB_CFLAGS="-I$with_zlib/include"
        LIBS="$saved_LIBS -L$ZLIB_LDOPTS -lz"
        CFLAGS="$saved_CFLAGS $ZLIB_CFLAGS"
    fi

    AC_TRY_LINK( [
#include <stdlib.h>
#include <zlib.h>
                  ],
[inflateCopy(NULL,NULL);], ac_have_zlib=yes, ac_have_zlib=no)

    LIBS="$saved_LIBS"
    CFLAGS="$saved_CFLAGS"

    if test "$ac_have_zlib" = "yes"
    then
        if test -z "$ZLIB_LDOPTS"
        then
            ZLIB_LIBS="-lz"
        else
            ZLIB_LIBS="$ZLIB_LDOPTS/libz.a"
        fi
        AC_DEFINE_UNQUOTED(HAVE_ZLIB)
        AC_SUBST(ZLIB_LIBS)
        AC_SUBST(ZLIB_CFLAGS)
        AC_MSG_RESULT([$ZLIB_LIBS])
    else
        AC_MSG_NOTICE($ac_have_zlib)
    fi
else
    ac_have_zlib=no
fi
AM_CONDITIONAL(HAVE_ZLIB, test "$ac_have_zlib" = "yes" )
])
