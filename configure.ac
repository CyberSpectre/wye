#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([wye], [0.01], [nosuch@example.org])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/worker.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX

AC_CHECK_LIB([boost_coroutine], [main], [],
	     [AC_MSG_ERROR(Library boost_coroutine missing.)])
AC_CHECK_LIB([boost_serialization], [main], [],
             [AC_MSG_ERROR(Library boost_serialization missing.)])
AC_CHECK_LIB([boost_system], [main], [],
             [AC_MSG_ERROR(Library boost_system missing.)])
AC_CHECK_LIB([pthread], [main], [], [AC_MSG_ERROR(Library pthread missing.)])

# Python
AM_PATH_PYTHON

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
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

AC_CONFIG_FILES([Makefile src/Makefile python/Makefile utils/Makefile])
AC_OUTPUT

