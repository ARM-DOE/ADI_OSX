lib_LTLIBRARIES     = libmsngr.la

include_HEADERS     = messenger.h
libmsngr_la_SOURCES = msngr.c msngr_lockfile.c msngr_log.c msngr_mail.c msngr_procstats.c msngr_utils.c msngr_version.c

libmsngr_la_CFLAGS  = -I${includedir} -Wall -Wextra
libmsngr_la_LDFLAGS = -no-undefined -avoid-version

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = msngr.pc

MAINTAINERCLEANFILES = Makefile.in
