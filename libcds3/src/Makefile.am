AM_CPPFLAGS = $(MSNGR_CFLAGS) $(GLIB_CFLAGS)

lib_LTLIBRARIES = libcds3.la
libcds3_la_SOURCES = cds_atts.c cds_converter.c cds_copy.c cds_data_types.c cds_dims.c cds_groups.c cds_objects.c cds_print.c cds_private.h cds_times.c cds_transform_params.c cds_units.c cds_utils.c cds_vararrays.c cds_var_data.c cds_vargroups.c cds_vars.c cds_version.c
include_HEADERS = cds3.h

libcds3_la_CFLAGS = -I${includedir}
libcds3_la_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lmsngr -ludunits2

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cds3.pc

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

MAINTAINERCLEANFILES = Makefile.in
