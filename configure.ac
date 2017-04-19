#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([wye], [0.04], [nosuch@example.org])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/worker.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX

AC_CHECK_LIB([boost_coroutine], [main], [],
      	     [AC_MSG_ERROR(Library boost_coroutine missing.)])
AC_CHECK_LIB([boost_program_options], [main], [],
             [AC_MSG_ERROR(Library boost_program_options.)])
AC_CHECK_LIB([boost_serialization], [main], [],
             [AC_MSG_ERROR(Library boost_serialization missing.)])
AC_CHECK_LIB([boost_system], [main], [],
             [AC_MSG_ERROR(Library boost_system missing.)])
AC_CHECK_LIB([pthread], [main], [],
             [AC_MSG_ERROR(Library pthread missing.)])

# pkg-config
PKG_PROG_PKG_CONFIG

# systemd
AC_MSG_CHECKING([have systemd])
AC_ARG_WITH([systemdsystemunitdir],
  [AS_HELP_STRING([--with-systemdsystemunitdir=DIR],
  [Directory for systemd service files])],,
  [with_systemdsystemunitdir=auto])
     
AS_IF([test "x$with_systemdsystemunitdir" = "xyes" -o "x$with_systemdsystemunitdir" = "xauto"], [
  def_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)
  AS_IF([test "x$def_systemdsystemunitdir" = "x"],
    [AS_IF([test "x$with_systemdsystemunitdir" = "xyes"],
    [AC_MSG_ERROR([systemd support requested but pkg-config unable to query systemd package])])
  with_systemdsystemunitdir=no],
  [with_systemdsystemunitdir="$def_systemdsystemunitdir"])])

AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
  [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])
AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])
AC_MSG_RESULT([$with_systemdsystemunitdir])

# Python
AM_PATH_PYTHON

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 getcwd gettimeofday memset pathconf setenv socket])

AC_CONFIG_FILES([Makefile src/Makefile python/Makefile utils/Makefile
	         init/Makefile examples/Makefile])
AC_OUTPUT

