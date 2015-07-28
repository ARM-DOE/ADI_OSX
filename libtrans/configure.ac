#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libtrans], [1.7], [brian.ermold@pnnl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/trans_bin_average.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcds3':
AC_CHECK_LIB([cds3], [main])
# FIXME: Replace `main' with a function in `-lmsngr':
AC_CHECK_LIB([msngr], [main])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memmove sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/trans.pc])
AC_OUTPUT
