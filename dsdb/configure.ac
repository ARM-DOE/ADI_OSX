#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([DSDB Tools], [1.0], [brian.ermold@arm.gov])

AC_CONFIG_AUX_DIR([build-aux])
#AC_CONFIG_LIBOBJ_DIR([portable])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror])
#AM_INIT_AUTOMAKE([1.11 foreign -Wall -Werror])
#AM_MAINTAINER_MODE

# Checks for programs.

AC_PATH_PROG(PERL, perl, /usr/bin/perl)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 bin/Makefile
                 conf/Makefile
                 etc/Makefile
                 lib/Makefile])

AC_OUTPUT
