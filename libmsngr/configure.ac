#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libmsngr], [1.7], [brian.ermold@pnnl.gov])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror])

AC_CONFIG_SRCDIR([src/msngr_utils.c])

AC_SUBST(PACKAGE_VERSION)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 gethostname memset mkdir strchr strdup strerror strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/msngr.pc])
AC_OUTPUT
