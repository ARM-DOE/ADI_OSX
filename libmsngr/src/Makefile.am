lib_LTLIBRARIES     = libmsngr.la
libmsngr_la_SOURCES = msngr.c msngr_lockfile.c msngr_log.c msngr_mail.c msngr_procstats.c msngr_utils.c msngr_version.c
include_HEADERS     = messenger.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = msngr.pc

libmsngr_la_LDFLAGS = -no-undefined -avoid-version
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
