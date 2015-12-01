bin_PROGRAMS = data_consolidator

data_consolidator_SOURCES = \
	data_consolidator.c

data_consolidator_CFLAGS  = -I${includedir} -Wall -Wextra
data_consolidator_LDFLAGS = -rpath ${libdir} -L${libdir} -ldsproc3

MAINTAINERCLEANFILES = Makefile.in
