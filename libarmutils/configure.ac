#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libarmutils], [1.7], [brian.ermold@pnnl.gov])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror])

AC_CONFIG_SRCDIR([src/dir_utils.c])

AC_SUBST(PACKAGE_VERSION)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

m4_include([m4/ax_check_openssl.m4])
AX_CHECK_OPENSSL([], [AC_MSG_ERROR([OPENSSL Not Found])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h sys/time.h unistd.h utime.h openssl/md5.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gethostname getpagesize memset munmap regcomp rmdir setenv strchr strdup strerror strrchr strtol utime])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/armutils.pc
                 src/armutils/Makefile])
AC_OUTPUT
