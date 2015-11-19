#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([ADI libdbconn], [1.10], [brian.ermold@pnnl.gov])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror])

AC_CONFIG_SRCDIR([src/dbconn_pgsql.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

m4_include([m4/ax_lib_postgresql.m4])
AX_LIB_POSTGRESQL()

m4_include([m4/ax_compare_version.m4])
m4_include([m4/ax_path_generic.m4])
AX_PATH_GENERIC([curl],,[AC_MSG_ERROR([LIBCURL Not Found])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h malloc.h stddef.h stdint.h stdlib.h string.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/statvfs.h sys/time.h unistd.h utime.h])
#AC_CHECK_HEADERS([libpq-fe.h pgsql/libpq-fe.h], [AC_DEFINE([HAVE_POSTGRES], [1], [Defined if postgres is available])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_INT32_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRTOD
AC_CHECK_FUNCS([clock_gettime dup2 fdatasync getpagesize gettimeofday localtime_r memmove memset strchr strdup strerror strncasecmp strstr strtol utime])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/dbconn.pc])
AC_OUTPUT
