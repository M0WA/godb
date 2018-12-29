#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([GoDB], [0.1], [bugs@go-db.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([include/hooks.h])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],[Turn on debugging (default is no)])],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_ARG_ENABLE([testbin],
[AS_HELP_STRING([--enable-testbin],[Build test binary (default is no)])],
[case "${enableval}" in
  yes) testbin=true ;;
  no)  testbin=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-testbin]) ;;
esac],[testbin=false])
AM_CONDITIONAL([TESTBIN], [test x$testbin = xtrue])

AC_ARG_ENABLE([postgre],
[AS_HELP_STRING([--enable-postgre],[Enables PostgreSQL support (default is yes)])],
[case "${enableval}" in
  yes) postgre=true ;;
  no)  postgre=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-postgre]) ;;
esac],[postgre=true])
AM_CONDITIONAL([DISABLE_POSTGRES], [test x$postgre = xfalse])

AC_ARG_ENABLE([mysql],
[AS_HELP_STRING([--enable-mysql],[Enables MySQL support (default is yes)])],
[case "${enableval}" in
  yes) mysql=true ;;
  no)  mysql=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-mysql]) ;;
esac],[mysql=true])
AM_CONDITIONAL([DISABLE_MYSQL], [test x$mysql = xfalse])

AC_ARG_ENABLE([dbi],
[AS_HELP_STRING([--enable-dbi],[Enables DBI support (default is yes)])],
[case "${enableval}" in
  yes) dbi=true ;;
  no)  dbi=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-dbi]) ;;
esac],[dbi=true])
AM_CONDITIONAL([DISABLE_DBI], [test x$dbi = xfalse])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AC_PROG_LIBTOOL

# Checks for libraries.
LT_INIT

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset select strcasecmp strtol strtoul strtoull snprintf])

AC_OUTPUT