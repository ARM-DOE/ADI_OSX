adietcdir = $(sysconfdir)
adietc_DATA = adi-setenv

CLEANFILES = $(adietc_DATA)
EXTRA_DIST = adi-setenv.in

do_subst = sed \
	-e 's,[@]PREFIX[@],$(prefix),g' \
	-e 's,[@]LIBDIR[@],$(libdir),g'

adi-setenv: adi-setenv.in
	$(do_subst) < adi-setenv.in > adi-setenv

MAINTAINERCLEANFILES = Makefile.in
