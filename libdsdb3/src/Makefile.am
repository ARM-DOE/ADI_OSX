SUBPACKAGE = libdsdb3

lib_LTLIBRARIES = libdsdb3.la

libdsdb3_la_SOURCES = datastream_config.c dbog_dod.c dbog_dsdb.c dbog_retriever.c dqrdb.c ds_class.c dsdb.c dsdb_version.c ds_dod.c ds_properties.c ds_times.c facility_location.c family_process.c process_config.c process_location.c process_state.c process_status.c retriever.c dbog_dod.h dbog_dsdb.h dbog_retriever.h
include_HEADERS = dsdb3.h


libdsdb3_la_CFLAGS = -I${includedir}
libdsdb3_la_LDFLAGS = --no-undefined -avoid-version -L${libdir} -ldbconn -lcds3 -lmsngr

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dsdb3.pc
