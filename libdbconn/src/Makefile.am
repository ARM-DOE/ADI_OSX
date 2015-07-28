SUBPACKAGE=libdbconn

lib_LTLIBRARIES = libdbconn.la
libdbconn_la_SOURCES = dbconn.c dbconn_pgsql.c dbconn_pgsql.h dbconn_private.h dbconn_sqlite.c dbconn_sqlite.h dbconn_version.c dbconn_wspc.c dbconn_wspc.h sqlite3.c sqlite3.h 
include_HEADERS = dbconn.h

libdbconn_la_CFLAGS = -I${includedir}
libdbconn_la_LDFLAGS = $(POSTGRESQL_LDFLAGS) $(CURL_LIBS) -lmsngr

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dbconn.pc
