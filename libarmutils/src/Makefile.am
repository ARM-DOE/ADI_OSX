SUBDIRS = armutils

lib_LTLIBRARIES        = libarmutils.la
libarmutils_la_SOURCES = armutils_version.c benchmark.c dir_utils.c dsenv.c endian_swap.c file_utils.c regex_time.c regex_utils.c string_utils.c time_utils.c
include_HEADERS        = armutils.h

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = armutils.pc

libarmutils_la_CFLAGS  = -I${includedir}
libarmutils_la_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lmsngr -L/usr/lib -lcrypto

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
