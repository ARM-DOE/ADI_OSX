adietcdir = $(sysconfdir)/adi
adietc_DATA = .cshrc

CLEANFILES = $(adietc_DATA)
EXTRA_DIST = .cshr.in

do_subst = sed \
	-e 's,[@]PREFIX[@],$(prefix),g' \
	-e 's,[@]LIBDIR[@],$(libdir),g'

.cshrc: .cshrc.in
	$(do_subst) < .cshrc.in > .cshrc

