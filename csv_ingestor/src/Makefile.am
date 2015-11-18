bin_PROGRAMS = csv_ingestor

csv_ingestor_SOURCES = \
	csv_ingest.c \
	csv_ingest.h \
	csv_ingest_read_data.c \
	csv_ingest_store_data.c

csv_ingestor_CFLAGS  = -I${includedir} -Wall -Wextra
csv_ingestor_LDFLAGS = -rpath ${libdir} -L${libdir} -ldsproc3

MAINTAINERCLEANFILES = Makefile.in
