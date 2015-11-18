sysconf_DATA = adi-setenv

CLEANFILES = $(sysconf_DATA)
EXTRA_DIST = adi-setenv.in

do_subst = sed \
	-e 's,[@]PREFIX[@],$(prefix),g' \
	-e 's,[@]LIBDIR[@],$(libdir),g'

$(sysconf_DATA): %: %.in
	$(do_subst) < $< > $@

MAINTAINERCLEANFILES = Makefile.in
