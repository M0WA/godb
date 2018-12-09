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
[  --enable-debug          Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_ARG_ENABLE([testbin],
[  --enable-testbin        Build test binary],
[case "${enableval}" in
  yes) testbin=true ;;
  no)  testbin=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-testbin]) ;;
esac],[testbin=false])
AM_CONDITIONAL([TESTBIN], [test x$testbin = xtrue])

AC_ARG_ENABLE([postgre],
[  --disable-postgre       Disables PostgreSQL support],
[case "${enableval}" in
  yes) postgre=false ;;
  no)  postgre=true ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --disable-postgre]) ;;
esac],[postgre=true])
AM_CONDITIONAL([DISABLE_POSTGRES], [test x$postgre = xfalse])
AM_CONDITIONAL([ENABLE_POSTGRESHELPER], [test x$postgre = xtrue])

AC_ARG_ENABLE([mysql],
[  --disable-mysql         Disables MySQL support],
[case "${enableval}" in
  yes) mysql=false ;;
  no)  mysql=true ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --disable-mysql]) ;;
esac],[mysql=true])
AM_CONDITIONAL([DISABLE_MYSQL], [test x$mysql = xfalse])
AM_CONDITIONAL([ENABLE_MYSQLHELPER], [test x$mysql = xtrue])

AC_ARG_ENABLE([dbi],
[  --disable-dbi           Disables DBI support],
[case "${enableval}" in
  yes) dbi=false ;;
  no)  dbi=true ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --disable-dbi]) ;;
esac],[dbi=true])
AM_CONDITIONAL([DISABLE_DBI], [test x$dbi = xfalse])
AM_CONDITIONAL([ENABLE_MYSQLHELPER], [test x$dbi = xtrue])
AM_CONDITIONAL([ENABLE_POSTGRESHELPER], [test x$dbi = xtrue])

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