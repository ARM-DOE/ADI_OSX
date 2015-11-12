bin_SCRIPTS = \
	db_check_state \
	db_delete_dod \
	db_delete_dsatts \
	db_delete_dsdods \
	db_export_dod \
	db_export_process \
	db_get_proc_stats \
	db_import_process \
	db_load_data \
	db_load_dod \
	db_load_dsatts \
	db_load_dsdods \
	db_process_config \
	db_query \
	db_set_proc_states \
	db_update_dstimes \
	db_update

CLEANFILES = $(bin_SCRIPTS)

EXTRA_DIST = \
	db_check_state.in \
	db_delete_dod.in \
	db_delete_dsatts.in \
	db_delete_dsdods.in \
	db_export_dod.in \
	db_export_process.in \
	db_get_proc_stats.in \
	db_import_process.in \
	db_load_data.in \
	db_load_dod.in \
	db_load_dsatts.in \
	db_load_dsdods.in \
	db_process_config.in \
	db_query.in \
	db_set_proc_states.in \
	db_update_dstimes.in \
	db_update.in

do_subst = sed -e 's,[@]PERL[@],$(PERL),g'

$(bin_SCRIPTS): %: %.in
	$(do_subst) < $< > $@

MAINTAINERCLEANFILES = Makefile.in
