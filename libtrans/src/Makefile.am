lib_LTLIBRARIES = libtrans.la
libtrans_la_SOURCES = cds_transform_driver.c trans_bin_average.c trans_subsample.c trans_interpolate.c trans_utils.c trans_passthrough.c trans_version.c trans_quadrature.c
include_HEADERS = trans.h

libtrans_la_CFLAGS = -I${includedir}
libtrans_la_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lcds3 -lmsngr

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = trans.pc

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
