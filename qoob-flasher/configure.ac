AC_INIT(gqoob, 0.7, jvaltane@kapsi.fi)
AC_CONFIG_SRCDIR(Makefile.am)

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE
AC_PROG_CC

GQOOB_CFLAGS="-Werror -Wall"
AC_SUBST(GQOOB_CFLAGS)

PKG_CHECK_MODULES(libqoob, [libqoob >= 0.7], , AC_MSG_ERROR( libqoob >= 0.7 is required ))
AC_SUBST(libqoob_CFLAGS)
AC_SUBST(libqoob_LIBS)

dnl debug
AC_ARG_ENABLE(debug,
[  --enable-debug          turn debugging on],
[case "${enableval}" in
  yes) debug=yes ;;
  no)  debug=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=no])

if test x$debug = xyes; then
   CFLAGS+=" -g -O0 -DDEBUG"
fi
AC_SUBST(debug_CFLAGS)

AC_OUTPUT(
Makefile
src/Makefile
)

