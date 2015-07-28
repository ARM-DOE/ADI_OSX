check_PROGRAMS = libarmutils.test

libarmutils_test_SOURCES = test_dir_utils.c test_dsenv.c test_endian_swap.c test_file_utils.c test_libarmutils.c test_regex_time.c test_regex_utils.c test_string_utils.c test_time_utils.c

libarmutils_test_CFLAGS  = -I../src -larmutils -I${includedir}
libarmutils_test_LDFLAGS = -L../src/.libs -larmutils -L${libdir}

EXTRA_DIST = ref.dir_utils_test ref.dsenv_test ref.endian_swap_test ref.file_utils_test ref.regex_utils_test ref.string_utils_test ref.time_utils_test

MAINTAINERCLEANFILES = Makefile.in
TESTS = libarmutils.test

distclean-local:
	-rm -rf test_dir
