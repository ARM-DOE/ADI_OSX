check_PROGRAMS = libcds3.test

AM_CPPFLAGS = $(GLIB_CFLAGS)

libcds3_test_SOURCES = libcds3_test.c libcds3_test.h
libcds3_test_LDFLAGS = -no-undefined -avoid-version -L${libdir} -lcds3 -lmsngr

EXTRA_DIST = ref.conversion_tests ref.copy_tests ref.data_index_tests ref.define_tests ref.delete_tests ref.error_tests ref.get_set_tests ref.rename_tests ref.string_array_tests ref.time_tests ref.trans_params_tests ref.units_tests transform_params.cfg transform_params.include

