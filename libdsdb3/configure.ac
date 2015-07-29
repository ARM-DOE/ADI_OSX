#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libdsdb3], [1.9], [brian.ermold@pnnl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/process_status.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcds3':
AC_CHECK_LIB([cds3], [main])
# FIXME: Replace `main' with a function in `-ldbconn':
AC_CHECK_LIB([dbconn], [main])
# FIXME: Replace `main' with a function in `-ldbcore3':
AC_CHECK_LIB([dbcore3], [main])
# FIXME: Replace `main' with a function in `-lmsngr':
AC_CHECK_LIB([msngr], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 strchr strdup])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/dsdb3.pc])
AC_OUTPUT
