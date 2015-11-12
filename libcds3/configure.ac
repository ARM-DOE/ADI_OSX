#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libcds3], [1.16], [brian.ermold@pnnl.gov])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign -Wall -Werror])

AC_CONFIG_SRCDIR([src/cds_print.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ludunits2':
#AC_CHECK_LIB([udunits2], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h unistd.h])

AC_CHECK_HEADER([udunits2.h], [AC_DEFINE([UDUNITS_INCLUDE],[<udunits2.h>], [udunits location])])
AC_CHECK_HEADER([udunits2/udunits2.h], [AC_DEFINE([UDUNITS_INCLUDE],[<udunits2/udunits2.h>], [udunits location])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset strchr strdup strerror strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/cds3.pc])
AC_OUTPUT
