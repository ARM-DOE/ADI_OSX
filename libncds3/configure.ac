#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libncds3], [1.12], [brian.ermold@pnnl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/ncds_get.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcds3':
AC_CHECK_LIB([cds3], [main])
# FIXME: Replace `main' with a function in `-lmsngr':
AC_CHECK_LIB([msngr], [main])
# FIXME: Replace `main' with a function in `-lnetcdf':
AC_CHECK_LIB([netcdf], [main])

# Checks for header files.
AC_CHECK_HEADERS([string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset regcomp strdup strerror strrchr strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/ncds3.pc
                 src/test/Makefile])
AC_OUTPUT
