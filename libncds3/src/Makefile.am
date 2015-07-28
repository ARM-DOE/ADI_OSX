SUBDIRS = test

SUBPACKAGE = libncds3

lib_LTLIBRARIES = libncds3.la
libncds3_la_SOURCES = ncds_data_types.c ncds_datastreams.c ncds_find_files.c ncds_get.c ncds_private.h ncds_read.c ncds_utils.c ncds_version.c ncds_write.c ncwrap_dataset.c ncwrap_inquire.c
include_HEADERS = ncds3.h

libncds3_la_CFLAGS  = -I${includedir}
libncds3_la_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lnetcdf -lcds3 -lmsngr

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ncds3.pc

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
