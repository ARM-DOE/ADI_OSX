sysconf_DATA = dsdb.cshrc dsdb.bashrc

CLEANFILES = $(sysconf_DATA)
EXTRA_DIST = dsdb.cshrc.in dsdb.bashrc.in

do_subst = sed \
	-e 's,[@]PREFIX[@],$(prefix),g' \
	-e 's,[@]LIBDIR[@],$(libdir),g'

$(sysconf_DATA): %: %.in
	$(do_subst) < $< > $@

MAINTAINERCLEANFILES = Makefile.in
