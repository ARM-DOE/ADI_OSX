#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libdsproc3], [2.29], [BUG-REPORT-ADDRESS])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror])

AC_CONFIG_SRCDIR([src/dsproc.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strchr strdup strerror strrchr strstr utime])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/dsproc3.pc])
AC_OUTPUT
