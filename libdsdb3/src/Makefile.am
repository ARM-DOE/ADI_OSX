lib_LTLIBRARIES = libdsdb3.la

include_HEADERS     = dsdb3.h
libdsdb3_la_SOURCES = datastream_config.c dbog_dod.c dbog_dsdb.c dbog_retriever.c dqrdb.c ds_class.c dsdb.c dsdb_version.c ds_dod.c ds_properties.c ds_times.c facility_location.c family_process.c process_config.c process_location.c process_state.c process_status.c retriever.c dbog_dod.h dbog_dsdb.h dbog_retriever.h

libdsdb3_la_CFLAGS = -I${includedir} -Wall -Wextra
libdsdb3_la_LDFLAGS = --no-undefined -avoid-version -L${libdir} -ldbconn -lcds3 -lmsngr

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dsdb3.pc

MAINTAINERCLEANFILES = Makefile.in
