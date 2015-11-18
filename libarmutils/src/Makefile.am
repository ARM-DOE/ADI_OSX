SUBDIRS = armutils

lib_LTLIBRARIES        = libarmutils.la

include_HEADERS        = armutils.h
libarmutils_la_SOURCES = armutils_version.c benchmark.c dir_utils.c dsenv.c endian_swap.c file_utils.c regex_time.c regex_utils.c string_utils.c time_utils.c

libarmutils_la_CFLAGS  = -I${includedir} $(OPENSSL_INCLUDES) -Wall -Wextra
libarmutils_la_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lmsngr $(OPENSSL_LDFLAGS) $(OPENSSL_LIBS)

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = armutils.pc

MAINTAINERCLEANFILES = Makefile.in
