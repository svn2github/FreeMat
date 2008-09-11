AC_DEFUN([MP_WITH_CURSES],
  [AC_ARG_WITH(ncurses, [  --with-ncurses          Force the use of ncurses over curses],,)
   mp_save_LIBS="$LIBS"
   CURSES_LIB=""
   if test "$with_ncurses" != yes
   then
     AC_CACHE_CHECK([for working curses], mp_cv_curses,
       [LIBS="$LIBS -lcurses"
        AC_TRY_LINK(
          [#include <curses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_curses=yes, mp_cv_curses=no)])
     if test "$mp_cv_curses" = yes
     then
       AC_DEFINE(HAVE_CURSES_H)
       CURSES_LIB="-lcurses"
     fi
   fi
   if test ! "$CURSES_LIB"
   then
     AC_CACHE_CHECK([for working ncurses], mp_cv_ncurses,
       [LIBS="$mp_save_LIBS -lncurses"
        AC_TRY_LINK(
          [#include <ncurses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_ncurses=yes, mp_cv_ncurses=no)])
     if test "$mp_cv_ncurses" = yes
     then
       AC_DEFINE(HAVE_NCURSES_H)
       CURSES_LIB="-lncurses"
     fi
   fi
   LIBS="$mp_save_LIBS"
])dnl

AC_DEFUN([ACX_BLAS], [
AC_PREREQ(2.50)
AC_REQUIRE([AC_F77_LIBRARY_LDFLAGS])
acx_blas_ok=no

AC_ARG_WITH(blas,
	[AC_HELP_STRING([--with-blas=<lib>], [use BLAS library <lib>])])
case $with_blas in
	yes | "") ;;
	no) acx_blas_ok=disable ;;
	-* | */* | *.a | *.so | *.so.* | *.o) BLAS_LIBS="$with_blas" ;;
	*) BLAS_LIBS="-l$with_blas" ;;
esac

# Get fortran linker names of BLAS functions to check for.
AC_F77_FUNC(sgemm)
AC_F77_FUNC(dgemm)

acx_blas_save_LIBS="$LIBS"
LIBS="$LIBS $FLIBS"

# First, check BLAS_LIBS environment variable
if test $acx_blas_ok = no; then
if test "x$BLAS_LIBS" != x; then
	save_LIBS="$LIBS"; LIBS="$BLAS_LIBS $LIBS"
	AC_MSG_CHECKING([for $sgemm in $BLAS_LIBS])
	AC_TRY_LINK_FUNC($sgemm, [acx_blas_ok=yes], [BLAS_LIBS=""])
	AC_MSG_RESULT($acx_blas_ok)
	LIBS="$save_LIBS"
fi
fi

# BLAS linked to by default?  (happens on some supercomputers)
if test $acx_blas_ok = no; then
	save_LIBS="$LIBS"; LIBS="$LIBS"
	AC_CHECK_FUNC($sgemm, [acx_blas_ok=yes])
	LIBS="$save_LIBS"
fi

# BLAS in ATLAS library? (http://math-atlas.sourceforge.net/)
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(atlas, ATL_xerbla,
		[AC_CHECK_LIB(f77blas, $sgemm,
		[AC_CHECK_LIB(cblas, cblas_dgemm,
			[acx_blas_ok=yes
			 BLAS_LIBS="-lcblas -lf77blas -latlas"],
			[], [-lf77blas -latlas])],
			[], [-latlas])])
fi

if test $acx_blas_ok = no; then
	unset ac_cv_lib_atlas_ATL_xerbla
	save_LIBS="$LIBS";
	LIBS="-L/usr/lib/atlas $LIBS"
	AC_CHECK_LIB(atlas, ATL_xerbla,
		[AC_CHECK_LIB(f77blas, $sgemm,
		[AC_CHECK_LIB(cblas, cblas_dgemm,
			[acx_blas_ok=yes
			 BLAS_LIBS="-lcblas -lf77blas -latlas"],
			[], [-lf77blas -latlas])],
			[], [-latlas])])
	LIBS="$save_LIBS";
	if test $acx_blas_ok = yes; then
	  BLAS_LIBS="-L/usr/lib/atlas $BLAS_LIBS"
	fi	
fi

# BLAS in PhiPACK libraries? (requires generic BLAS lib, too)
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(blas, $sgemm,
		[AC_CHECK_LIB(dgemm, $dgemm,
		[AC_CHECK_LIB(sgemm, $sgemm,
			[acx_blas_ok=yes; BLAS_LIBS="-lsgemm -ldgemm -lblas"],
			[], [-lblas])],
			[], [-lblas])])
fi

# BLAS in Alpha CXML library?
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(cxml, $sgemm, [acx_blas_ok=yes;BLAS_LIBS="-lcxml"])
fi

# BLAS in Alpha DXML library? (now called CXML, see above)
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(dxml, $sgemm, [acx_blas_ok=yes;BLAS_LIBS="-ldxml"])
fi

# BLAS in Sun Performance library?
if test $acx_blas_ok = no; then
	if test "x$GCC" != xyes; then # only works with Sun CC
		AC_CHECK_LIB(sunmath, acosp,
			[AC_CHECK_LIB(sunperf, $sgemm,
        			[BLAS_LIBS="-xlic_lib=sunperf -lsunmath"
                                 acx_blas_ok=yes],[],[-lsunmath])])
	fi
fi

# BLAS in SCSL library?  (SGI/Cray Scientific Library)
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(scs, $sgemm, [acx_blas_ok=yes; BLAS_LIBS="-lscs"])
fi

# BLAS in SGIMATH library?
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(complib.sgimath, $sgemm,
		     [acx_blas_ok=yes; BLAS_LIBS="-lcomplib.sgimath"])
fi

# BLAS in IBM ESSL library? (requires generic BLAS lib, too)
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(blas, $sgemm,
		[AC_CHECK_LIB(essl, $sgemm,
			[acx_blas_ok=yes; BLAS_LIBS="-lessl -lblas"],
			[], [-lblas $FLIBS])])
fi

# BLAS in vector framework?
if test $acx_blas_ok = no; then
   save_LIBS="$LIBS"; LIBS="$LIBS -framework vecLib"
   AC_CHECK_LIB(m, $sgemm, [acx_blas_ok=yes
			  BLAS_LIBS="-framework vecLib"])
   LIBS="$save_LIBS"
fi

# Generic BLAS library?
if test $acx_blas_ok = no; then
	AC_CHECK_LIB(blas, $sgemm, [acx_blas_ok=yes; BLAS_LIBS="-lblas"])
fi

AC_SUBST(BLAS_LIBS)

LIBS="$acx_blas_save_LIBS"

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_blas_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_BLAS,1,[Define if you have a BLAS library.]),[$1])
        :
else
        acx_blas_ok=no
        $2
fi
])dnl ACX_BLAS

AC_DEFUN([ACX_LAPACK], [
AC_REQUIRE([ACX_BLAS])
acx_lapack_ok=no

AC_ARG_WITH(lapack,
        [AC_HELP_STRING([--with-lapack=<lib>], [use LAPACK library <lib>])])
case $with_lapack in
        yes | "") ;;
        no) acx_lapack_ok=disable ;;
        -* | */* | *.a | *.so | *.so.* | *.o) LAPACK_LIBS="$with_lapack" ;;
        *) LAPACK_LIBS="-l$with_lapack" ;;
esac

# Get fortran linker name of LAPACK function to check for.
AC_F77_FUNC(cheev)

# We cannot use LAPACK if BLAS is not found
if test "x$acx_blas_ok" != xyes; then
        acx_lapack_ok=noblas
fi

# First, check LAPACK_LIBS environment variable
if test "x$LAPACK_LIBS" != x; then
        save_LIBS="$LIBS"; LIBS="$LAPACK_LIBS $BLAS_LIBS $LIBS $FLIBS"
        AC_MSG_CHECKING([for $cheev in $LAPACK_LIBS])
        AC_TRY_LINK_FUNC($cheev, [acx_lapack_ok=yes], [LAPACK_LIBS=""])
        AC_MSG_RESULT($acx_lapack_ok)
        LIBS="$save_LIBS"
        if test acx_lapack_ok = no; then
                LAPACK_LIBS=""
        fi
fi

# LAPACK linked to by default?  (is sometimes included in BLAS lib)
if test $acx_lapack_ok = no; then
        save_LIBS="$LIBS"; LIBS="$LIBS $BLAS_LIBS $FLIBS"
        AC_CHECK_FUNC($cheev, [acx_lapack_ok=yes])
        LIBS="$save_LIBS"
fi

# Generic LAPACK library?
for lapack in lapack lapack_rs6k; do
        if test $acx_lapack_ok = no; then
                save_LIBS="$LIBS"; LIBS="$LIBS $FLIBS"
                AC_CHECK_LIB($lapack, $cheev,
                    [acx_lapack_ok=yes; LAPACK_LIBS="-l$lapack"], [], [$FLIBS])
                LIBS="$save_LIBS"
        fi
done

AC_SUBST(LAPACK_LIBS)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_lapack_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_LAPACK,1,[Define if you have LAPACK library.]),[$1])
        :
else
        acx_lapack_ok=no
        $2
fi
])dnl ACX_LAPACK


dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN([PKG_CHECK_MODULES], [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     succeeded=no
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])

AC_DEFUN([AC_LIB_FREEMAT_DEPEND_PRINT], [
	if $3; then
		echo "$1       $2          <Found>"
	else
		echo "$1       $2          <Not found>"
	fi
])

AC_DEFUN([AC_LIB_FREEMAT_CHECK], [
extern_flags=""
need_extern="no"
LIBS="$LIBS $FLIBS"

AC_CHECK_LIB(portaudio,Pa_GetDefaultInputDevice,found_portaudio19="yes",found_portaudio19="no")
AC_CHECK_HEADER(portaudio.h,[],found_portaudio19="no")

if test x"$found_portaudio19" == xyes; then
  LIBS="-lportaudio $LIBS"
  CFLAGS="$CFLAGS"
  AC_DEFINE(HAVE_PORTAUDIO19, 1, [Set to 1 if you have libportaudio (v19 API)])
fi

if test x"$found_portaudio19" != xyes; then
AC_CHECK_LIB(portaudio,Pa_OpenStream,found_portaudio18="yes",found_portaudio18="no")
AC_CHECK_HEADER(portaudio.h,[],found_portaudio18="no")

if test x"$found_portaudio18" == xyes; then
  LIBS="-lportaudio $LIBS"
  CFLAGS="$CFLAGS"
  AC_DEFINE(HAVE_PORTAUDIO18, 1, [Set to 1 if you have libportaudio (v18 API)])
fi

fi

if (test x"$found_portaudio19" == xyes) || (test x"$found_portaudio18" == xyes); then
  found_portaudio="yes";
else
  found_portaudio="no";
fi

AC_CHECK_LIB(pcre,pcre_compile,found_pcre="yes",found_pcre="no")
AC_CHECK_HEADER(pcre.h,[],found_pcre="no")

if test x"$found_pcre" == xyes; then
  LIBS="-lpcre $LIBS"
  AC_DEFINE(HAVE_PCRE, 1, [Set to 1 if you have libpcre])
fi

AC_CHECK_LIB(avcall,__structcpy,found_avcall="yes",found_avcall="no")
AC_CHECK_HEADER(avcall.h,[],found_avcall="no")

if test x"$found_avcall" == xyes; then
  LIBS="-lavcall $LIBS"
  AC_DEFINE(HAVE_AVCALL, 1, [Set to 1 if you have libavcall])
fi    

if test x"$is_osx" == xyes; then
   LIBS="$LIBS -framework vecLib"
   found_blas="yes"
   found_lapack="yes"
else
   ACX_BLAS(found_blas="yes",found_blas="no")
   if test x"$found_blas" == xyes; then
	LIBS="$BLAS_LIBS $LIBS"
   fi
   ACX_LAPACK(found_lapack="yes",found_lapack="no")
   if test x"$found_lapack" == xyes; then
      LIBS="$LAPACK_LIBS $LIBS"
   fi
fi

AC_CHECK_LIB(amd,amd_postorder,found_amd="yes",found_amd="no")
AC_CHECK_HEADER(amd.h,found_amdh="yes",found_amdh="no")
if test x"$found_amdh" == xno; then
  AC_CHECK_HEADER(amd/amd.h,found_amdh="yes",found_amdh="no")
  if test x"$found_amdh" == xyes; then
    CFLAGS="$CFLAGS -I/usr/include/amd"
    CXXFLAGS="$CXXFLAGS -I/usr/include/amd"
  fi
fi
if test x"$found_amdh" == xno; then
  AC_CHECK_HEADER(ufsparse/amd.h,found_amdh="yes",found_amdh="no")
  if test x"$found_amdh" == xyes; then
   CFLAGS="$CFLAGS -I/usr/include/ufsparse"
   CXXFLAGS="$CXXFLAGS -I/usr/include/ufsparse"
 fi
fi

if (test x"$found_amdh" == xyes) && (test x"$found_amd" == xyes); then
    LIBS="-lamd $LIBS"
fi

AC_CHECK_LIB(umfpack,umfpack_zl_solve,found_umfpack="yes",found_umfpack="no")
AC_CHECK_HEADER(umfpack.h,found_umfpackh="yes",found_umfpackh="no")
if test x"$found_umfpackh" == xno; then
  AC_CHECK_HEADER(umfpack/umfpack.h,found_umfpackh="yes",found_umfpackh="no")
  if test x"$found_umfpackh" == xyes; then
    CFLAGS="$CFLAGS -I/usr/include/umfpack"
    CXXFLAGS="$CXXFLAGS -I/usr/include/umfpack"
  fi
fi
if test x"$found_umfpackh" == xno; then
  AC_CHECK_HEADER(ufsparse/umfpack.h,found_umfpackh="yes",found_umfpackh="no")
  if test x"$found_umfpackh" == xyes; then
    CFLAGS="$CFLAGS -I/usr/include/ufsparse"
    CXXFLAGS="$CXXFLAGS -I/usr/include/ufsparse"
  fi
fi
if (test x"$found_umfpackh" == xyes) && (test x"$found_umfpack" == xyes); then
    LIBS="-lumfpack $LIBS"
fi
if (test x"$found_amdh" == xyes) && (test x"$found_amd" == xyes) && (test x"$found_umfpackh" == xyes) && (test x"$found_umfpack" == xyes); then
    AC_DEFINE(HAVE_UMFPACK, 1, [Set to 1 if you have UMFPACK])
fi

dnl PKG_CHECK_MODULES(fftw3, fftw3 >= 3.1.0, found_fftw3="yes", found_fftw3="no")
dnl PKG_CHECK_MODULES(fftw3f, fftw3f >= 3.1.0, found_fftw3f="yes", found_fftw3f="no")

AC_CHECK_LIB(fftw3f,fftwf_malloc,found_fftw3f="yes",found_fftw3f="no")
AC_CHECK_LIB(fftw3,fftw_malloc,found_fftw3="yes",found_fftw3="no")
AC_CHECK_HEADER(fftw3.h,[],[found_fftw3="no";found_fftw3f="no"])

if test x"$found_fftw3f" == xyes; then
	LIBS="-lfftw3f $LIBS"
	AC_DEFINE(HAVE_FFTWF, 1, [Set to 1 if you have the single precision version of FFTW installed])
fi

if test x"$found_fftw3" == xyes; then
	LIBS="-lfftw3 $LIBS"
  	AC_DEFINE(HAVE_FFTW, 1, [Set to 1 if you have the double precision version of FFTW installed])
fi

AC_F77_FUNC(znaupd)
if test x"$znaupd" == x"unknown"; then
  znaupd="znaupd_"
fi

AC_CHECK_LIB(arpack,$znaupd,found_arpack="yes",found_arpack="no",[$FLIBS])
if test x"$found_arpack" == xyes; then
	LIBS="-larpack $LIBS"
	AC_DEFINE(HAVE_ARPACK, 1, [Set to 1 if you have ARPACK installed])
fi

if test x"$found_arpack" == xno; then
   AC_CHECK_LIB(ARPACK,$znaupd,found_arpack="yes",found_arpack="no",[$FLIBS])
   if test x"$found_arpack" == xyes; then
    	   LIBS="-lARPACK $LIBS"
	   AC_DEFINE(HAVE_ARPACK, 1, [Set to 1 if you have ARPACK installed])
   fi
fi

AC_CHECK_LIB(z,inflate,found_z="yes",found_z="no")
if test x"$found_z" == xyes; then
   LIBS="-lz $LIBS"
fi

AC_CHECK_LIB(hdf5,inflate,found_hdf5="yes",found_hdf5="no")
if test x"$found_hdf5" == xyes; then
  LIBS="$LIBS -lhdf5_hl -lhdf5"
  AC_DEFINE(HAVE_HDF5, 1, [Set to 1 if you have HDF5 installed])
fi

AC_MSG_CHECKING(for LLVM)
found_llvm=no
if test -z "$LLVM_CONFIG"; then
  AC_PATH_PROG(LLVM_CONFIG, llvm-config, no)
fi

if test "$LLVM_CONFIG" = "no" ; then
  found_llvm=no
else
  AC_MSG_CHECKING(for LLVM >= 2.1)
  LLVM_VERSION=`$LLVM_CONFIG --version`
  VERSION_CHECK=`expr $LLVM_VERSION \>\= 2.1`
  if test "$VERSION_CHECK" == "1" ; then
    AC_MSG_RESULT(yes)
    found_llvm=yes;
    CXXFLAGS="$CXXFLAGS `$LLVM_CONFIG --cxxflags`"
    LDFLAGS="$LDFLAGS `$LLVM_CONFIG --ldflags` -Wl,--export-dynamic"
    LIBS="$LIBS `$LLVM_CONFIG --libs all`"
    AC_DEFINE(HAVE_LLVM, 1, [Set to 1 if you have LLVM installed])
  fi
fi


AC_CHECK_HEADERS([libintl.h malloc.h stddef.h stdlib.h string.h strings.h sys/time.h unistd.h limits.h c_asm.h intrinsics.h stdint.h mach/mach_time.h sys/sysctl.h])
AC_CHECK_FUNCS([BSDgettimeofday gettimeofday gethrtime read_real_time time_base_to_time drand48 sqrt memset posix_memalign memalign _mm_malloc _mm_free clock_gettime mach_absolute_time sysctl abort])
AC_MSG_CHECKING([for _rtc intrinsic])
rtc_ok=yes
AC_TRY_LINK([#ifdef HAVE_INTRINSICS_H
#include <intrinsics.h>
#endif], [_rtc()], [AC_DEFINE(HAVE__RTC,1,[Define if you have the UNICOS _rtc() intrinsic.])], [rtc_ok=no])
AC_MSG_RESULT($rtc_ok)
AC_MSG_CHECKING([whether a cycle counter is available])
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I$srcdir/libs/libFreeMat"
AC_TRY_CPP([#include "fftw_cycle.h"
#ifndef HAVE_TICK_COUNTER
#  error No cycle counter
#endif], [ok=yes], [ok=no])
CPPFLAGS=$save_CPPFLAGS
AC_MSG_RESULT($ok)
found_cycle=$ok
if test x"$found_cycle" == xyes; then
   AC_DEFINE(HAVE_PROFILE, 1, [Set to 1 if you have a cycle counter])
fi

])
