AC_DEFUN([SCATS_INIT],
[
AC_PREREQ(2.54)
AC_CANONICAL_HOST
scats_var=`echo $1`_PLATFORM
AC_DEFINE_UNQUOTED($scats_var,"${host}")
scats_var=`echo $1`_MAJOR_VERSION
AC_DEFINE_UNQUOTED($scats_var,$2)
scats_var=`echo $1`_MINOR_VERSION
AC_DEFINE_UNQUOTED($scats_var,$3)
scats_var=`echo $1`_RELEASE_LEVEL
AC_DEFINE_UNQUOTED($scats_var,$4)

AC_ARG_WITH(debug,
[  --with-debug=yes	turn on debugging],
debug=$withval,
debug=no)

AC_MSG_CHECKING(debug mode)
if test $debug = yes
then
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(DEBUG)
else
    AC_MSG_RESULT(no)
    AC_DEFINE_UNQUOTED(NODEBUG)
fi

AC_ARG_WITH(docs,
[  --with-docs=yes	install documentation],
docs_install=$withval,
docs_install=no)

if test "$withval" = "yes"
then
    AC_CHECK_PROG(have_doxygen,doxygen,yes,no)
    if test "$have_doxygen" = "yes"
    then
        docs_install=yes
    else
        docs_install=no
    fi
fi

AC_MSG_CHECKING(install docs)
if test "$docs_install" = "yes"
then
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(DOCS_INSTALL, test "x$docs_install" = "xyes")
])
