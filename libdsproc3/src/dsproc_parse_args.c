/*******************************************************************************
*
*  COPYRIGHT (C) 2012 Battelle Memorial Institute.  All Rights Reserved.
*
********************************************************************************
*
*  Author:
*     name:  Brian Ermold
*     phone: (509) 375-2277
*     email: brian.ermold@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 61404 $
*    $Author: ermold $
*    $Date: 2015-05-07 18:28:10 +0000 (Thu, 07 May 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_parse_args.c
 *  DSProc Parse Arguments.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */

/*******************************************************************************
 *  Static Data and Functions Visible Only To This Module
 */

static char  _CommandLine[PATH_MAX];   /**< command line string              */

/**
 *  Static: Set the command line string.
 *
 *  This function will set the string to use for the command_line global
 *  attribute value when new datasets are created. This value will only
 *  be set in datasets that have the command_line attribute defined.
 *
 *  @param  argc - argc value from the main function
 *  @param  argv - argv pointer from the main function
 */
static void _dsproc_set_command_line(int argc, char **argv)
{
    int   max_length = PATH_MAX - 5;
    char *command;
    int   length;
    int   i;

    if (!argc || !argv || !argv[0]) {
        _CommandLine[0] = '\0';
        return;
    }

    command = strrchr(argv[0], '/');

    if (!command) {
        strncpy(_CommandLine, argv[0], max_length);
    }
    else {
        strncpy(_CommandLine, ++command, max_length);
    }
    _CommandLine[max_length] = '\0';

    length = 0;

    for (i = 1; i < argc; i++) {

        length += strlen(argv[i]) + 1;

        if (length > max_length) {
            strcat(_CommandLine, " ...");
            break;
        }

        strcat(_CommandLine, " ");
        strcat(_CommandLine, argv[i]);
    }
}

/**
 *  Static: Convert an input date string to seconds since 1970.
 *
 *  @param  string - input string in the form YYYYMMDD[hh[mm[ss]]]
 *
 *  @return
 *    - seconds since 1970
 *    - 0 if the string has an invalid format
 */
static time_t _dsproc_input_string_to_time(const char *string)
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int count;

    count = sscanf(string, "%4d%2d%2d%2d%2d%2d",
        &year, &month, &day, &hour, &min, &sec);

    if (count < 6) sec  = 0;
    if (count < 5) min  = 0;
    if (count < 4) hour = 0;

    if (count < 3) {
        return((time_t)0);
    }

    return(get_secs1970(year, month, day, hour, min, sec));
}

/**
 *  Static: Print Ingest Process Usage.
 *
 *  @param  program_name - name of the Ingest program
 *  @param  nproc_names  - number of valid process names
 *  @param  proc_names   - list of valid process names
 *  @param  exit_value   - program exit value
 */
static void _dsproc_ingest_exit_usage(
    const char  *program_name,
    int          nproc_names,
    const char **proc_names,
    int          exit_value)
{
    FILE       *output_stream  = (exit_value) ? stderr : stdout;
    const char *proc_name_arg  = "";
    const char *proc_name_desc = "";
    int i;

    if (nproc_names != 1) {
        proc_name_arg  = " -n proc_name";
        proc_name_desc = "    -n proc_name - Process name.\n";
    }

    fprintf(output_stream,
"\n"
"SYNOPSIS\n"
"\n"
"    %s -s site -f facility%s\n"
"\n"
"ARGUMENTS\n"
"\n"
"%s"
"    -s site      - Site name.\n"
"    -f facility  - Facility name.\n"
"\n"
"OPTIONS\n"
"\n"
"    -a alias     - Specify the .db_connect alias to use (default: dsdb_data).\n"
"    -h           - Display help message.\n"
"    -v           - Display process version.\n"
"\n"
"    -D [level]   - Turn on debug mode.\n"
"    -F           - Force processing to filter out overlapping records and\n"
"                   continue past non-fatal errors.\n"
"    -P [level]   - Turn on provenance logging.\n"
"    -R           - Enable reprocessing/development mode. Enabling this mode\n"
"                   will allow previously processed data to be overwritten.\n"
"\n",
program_name, proc_name_arg, proc_name_desc);

    if (nproc_names > 1) {

        fprintf(output_stream,
            "VALID PROCESS NAMES\n\n");

        for (i = 0; i < nproc_names; i++) {
            fprintf(output_stream,
                "    %s\n", proc_names[i]);
        }

        fprintf(output_stream, "\n");
    }

    exit(exit_value);
}

/**
 *  Static: Print VAP Process Usage.
 *
 *  @param  program_name - name of the VAP program
 *  @param  nproc_names  - number of valid process names
 *  @param  proc_names   - list of valid process names
 *  @param  exit_value   - program exit value
 */
static void _dsproc_vap_exit_usage(
    const char  *program_name,
    int          nproc_names,
    const char **proc_names,
    int          exit_value)
{
    FILE       *output_stream  = (exit_value) ? stderr : stdout;
    const char *proc_name_arg  = "";
    const char *proc_name_desc = "";
    const char *date_args      = "";
    const char *date_args_desc = "";
    const char *date_opts_desc = "";
    int         rt_mode        = dsproc_get_real_time_mode();
    int i;

    if (nproc_names != 1) {
        proc_name_arg  = " -n proc_name";
        proc_name_desc = "    -n proc_name  - Process name.\n";
    }

    date_args      = " -b begin_date -e end_date";
    date_args_desc =
    "    -b begin_date - Begin date in the form YYYYMMDD[hh[mm[ss]]]. Data starting\n"
    "                    from and including this date will be processed.\n"
    "    -e end_date   - End date in the form YYYYMMDD[hh[mm[ss]]]. Data will be\n"
    "                    processed up to but *NOT* including this date.\n";

    if (rt_mode) {
        date_args      = "";
        date_args_desc = "";
        date_opts_desc =
        "\n"
        "    -b begin_date - Begin date in the form YYYYMMDD[hh[mm[ss]]]. Data starting\n"
        "                    from and including this date will be processed.\n"
        "    -e end_date   - End date in the form YYYYMMDD[hh[mm[ss]]]. Data will be\n"
        "                    processed up to but *NOT* including this date.\n";
    }

    fprintf(output_stream,
"\n"
"SYNOPSIS\n"
"\n"
"    %s%s -s site -f facility%s\n"
"\n"
"ARGUMENTS\n"
"\n"
"%s"
"    -s site       - Site name.\n"
"    -f facility   - Facility name.\n"
"%s"
"\n"
"OPTIONS\n"
"\n"
"    -a alias      - Specify the .db_connect alias to use (default: dsdb_data).\n"
"    -h            - Display help message.\n"
"    -v            - Display process version.\n"
"%s"
"\n"
"    -D [level]    - Turn on debug mode.\n"
"    -P [level]    - Turn on provenance logging.\n"
"    -R            - Enable reprocessing/development mode. Enabling this mode\n"
"                    will allow previously processed data to be overwritten.\n"
"\n",
program_name, proc_name_arg, date_args, proc_name_desc,
date_args_desc, date_opts_desc);

    if (nproc_names > 1) {

        fprintf(output_stream,
            "VALID PROCESS NAMES\n\n");

        for (i = 0; i < nproc_names; i++) {
            fprintf(output_stream,
                "    %s\n", proc_names[i]);
        }

        fprintf(output_stream, "\n");
    }

    exit(exit_value);
}

/*******************************************************************************
 *  Private Functions Visible Only To This Library
 */

/**
 *  Get the command line string.
 *
 *  @return
 *    - command line string
 *    - NULL if not set
 */
const char *_dsproc_get_command_line(void)
{
    if (_CommandLine[0]) return((const char *)_CommandLine);
    return((const char *)NULL);
}

/**
 *  Parse the command line arguments for an Ingest process.
 *
 *  If an error occurs in this function the memory used by the internal
 *  _DSProc structure will be freed and the process will exit with value 1.
 *
 *  @param  argc        - command line argc
 *  @param  argv        - command line argv
 *  @param  nproc_names - number of valid process names
 *  @param  proc_names  - list of valid process names
 */
void _dsproc_ingest_parse_args(
    int          argc,
    char       **argv,
    int          nproc_names,
    const char **proc_names)
{
    const char  *program_name = argv[0];
    int          debug_level;
    int          prov_level;
    const char  *switches;
    char         c;
    int          ni;

    _dsproc_set_command_line(argc, argv);

    while(--argc > 0) {
        if ((*++argv)[0] == '-') {

            /* Long options */

            if ((*argv)[1] == '-') {

                if (strcmp(*argv, "--dynamic-dods") == 0) {
                    dsproc_set_dynamic_dods_mode(1);
                }
                else {
                    fprintf(stderr,
                        "%s: Ignoring Invalid Long Option: '%s'\n",
                        program_name, *argv);
                }

                continue;
            }

            /* Short options */

            switches = *argv;
            while ((c = *++switches)) {
                switch(c) {
                case 'a':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->db_alias = strdup(*++argv)) ) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'h':
                    _dsproc_ingest_exit_usage(
                        program_name, nproc_names, proc_names, 0);
                    break;
                case 'f':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->facility = strdup(*++argv))) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'n':
                    if (--argc == 0) goto MISSING_ARG;
                    argv++;
                    if (nproc_names != 1) {
                        if (!(_DSProc->name = strdup(*argv))) {
                            goto MEMORY_ERROR;
                        }
                    }
                    else {
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c %s'\n",
                            program_name, c, *argv);
                    }
                    break;
                case 's':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->site = strdup(*++argv))) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'v':
                    fprintf(stdout,
                        "%s version: %s\n",
                        program_name, _DSProc->version);
                    _dsproc_destroy();
                    exit(0);
                    break;
                case 'd':
                    fprintf(stdout,
                        "\n-d has been depricated, please use -D instead.\n\n");
                case 'D':
                    if (argc > 1 && isdigit(*(argv+1)[0])) {
                        debug_level = atoi(*++argv);
                        argc--;
                    }
                    else {
                        debug_level = 1;
                    }
                    msngr_set_debug_level(debug_level);
                    break;
                case 'F':
                    dsproc_set_force_mode(1);
                    break;
                case 'P':
                    if (argc > 1 && isdigit(*(argv+1)[0])) {
                        prov_level = atoi(*++argv);
                        argc--;
                    }
                    else {
                        prov_level = 1;
                    }
                    msngr_set_provenance_level(prov_level);
                    break;
                case 'R':
                    dsproc_set_reprocessing_mode(1);
                    break;
                default:
                    if (argc > 1 && *(argv+1)[0] != '-') {
                        argc--;
                        argv++;
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c %s'\n",
                            program_name, c, *argv);
                    }
                    else {
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c'\n",
                            program_name, c);
                    }
                }
            }
        }
        else {
            fprintf(stderr,
                "%s: Ignoring Invalid Option: '%s'\n",
                program_name, *argv);
        }
    }

    /************************************************************
    *  Check for required fields
    *************************************************************/

    if (!_DSProc->site     ||
        !_DSProc->facility ||
        !_DSProc->name) {

        _dsproc_ingest_exit_usage(
            program_name, nproc_names, proc_names, 1);
    }

    /************************************************************
    *  Make sure we have a valid process name
    *************************************************************/

    if (nproc_names > 1) {

        for (ni = 0; ni < nproc_names; ni++) {
            if (strcmp(_DSProc->name, proc_names[ni]) == 0) {
                break;
            }
        }

        if (ni == nproc_names) {

            fprintf(stderr,
                "\n%s: Invalid Process Name: '%s'\n\n",
                program_name, _DSProc->name);

            _dsproc_ingest_exit_usage(
                program_name, nproc_names, proc_names, 1);
        }
    }

    return;

MISSING_ARG:

    fprintf(stderr, "\n%s: Missing required argument for -%c option\n\n",
        program_name, c);
    _dsproc_destroy();
    exit(1);

MEMORY_ERROR:

    fprintf(stderr,
        "%s: Memory allocation error initializing process\n",
        program_name);

    _dsproc_destroy();
    exit(1);
}

/**
 *  Parse the command line arguments for a VAP process.
 *
 *  If an error occurs in this function the memory used by the internal
 *  _DSProc structure will be freed and the process will exit with value 1.
 *
 *  @param  argc        - command line argc
 *  @param  argv        - command line argv
 *  @param  nproc_names - number of valid process names
 *  @param  proc_names  - list of valid process names
 */
void _dsproc_vap_parse_args(
    int          argc,
    char       **argv,
    int          nproc_names,
    const char **proc_names)
{
    const char  *program_name = argv[0];
    int          rt_mode;
    int          debug_level;
    float        max_real_time_wait;
    int          prov_level;
    const char  *switches;
    char         c;
    int          ni;

    _dsproc_set_command_line(argc, argv);

    /************************************************************
    *  Parse command line arguments
    *************************************************************/

    while(--argc > 0) {
        if ((*++argv)[0] == '-') {

            /* Long options */

            if ((*argv)[1] == '-') {

                if (strcmp(*argv, "--dynamic-dods") == 0) {
                    dsproc_set_dynamic_dods_mode(1);
                }
                else if (strcmp(*argv, "--real-time") == 0) {

                    if (argc > 1 && isdigit(*(argv+1)[0])) {
                        max_real_time_wait = (float)atof(*++argv);
                        argc--;
                    }
                    else {
                        /* default to three days max wait time */
                        max_real_time_wait = 72.0;
                    }

                    dsproc_set_real_time_mode(1, max_real_time_wait);
                }
                else {
                    fprintf(stderr,
                        "%s: Ignoring Invalid Long Option: '%s'\n",
                        program_name, *argv);
                }

                continue;
            }

            /* Short options */

            switches = *argv;
            while ((c = *++switches)) {
                switch(c) {
                case 'a':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->db_alias = strdup(*++argv)) ) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'b':
                case 'd':
                    if (--argc == 0) goto MISSING_ARG;
                    _DSProc->cmd_line_begin = _dsproc_input_string_to_time(*++argv);
                    if (_DSProc->cmd_line_begin == (time_t)0) {
                        fprintf(stderr, "\n%s: Invalid Begin Date: '%s'\n\n",
                            program_name, *argv);
                        _dsproc_destroy();
                        exit(1);
                    }
                    break;
                case 'e':
                    if (--argc == 0) goto MISSING_ARG;
                    _DSProc->cmd_line_end = _dsproc_input_string_to_time(*++argv);
                    if (_DSProc->cmd_line_end == (time_t)0) {
                        fprintf(stderr, "\n%s: Invalid End Date: '%s'\n\n",
                            program_name, *argv);
                        _dsproc_destroy();
                        exit(1);
                    }
                    break;
                case 'h':
                    _dsproc_vap_exit_usage(
                        program_name, nproc_names, proc_names, 0);
                    break;
                case 'f':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->facility = strdup(*++argv))) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'n':
                    if (--argc == 0) goto MISSING_ARG;
                    argv++;
                    if (nproc_names != 1) {
                        if (!(_DSProc->name = strdup(*argv))) {
                            goto MEMORY_ERROR;
                        }
                    }
                    else if (strcmp(_DSProc->name, *argv) != 0) {
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c %s'\n",
                            program_name, c, *argv);
                    }
                    break;
                case 's':
                    if (--argc == 0) goto MISSING_ARG;
                    if (!(_DSProc->site = strdup(*++argv))) {
                        goto MEMORY_ERROR;
                    }
                    break;
                case 'v':
                    fprintf(stdout,
                        "%s version: %s\n",
                        program_name, _DSProc->version);
                    _dsproc_destroy();
                    exit(0);
                    break;
                case 'D':
                    if (argc > 1 && isdigit(*(argv+1)[0])) {
                        debug_level = atoi(*++argv);
                        argc--;
                    }
                    else {
                        debug_level = 1;
                    }
                    msngr_set_debug_level(debug_level);
                    break;
                case 'P':
                    if (argc > 1 && isdigit(*(argv+1)[0])) {
                        prov_level = atoi(*++argv);
                        argc--;
                    }
                    else {
                        prov_level = 1;
                    }
                    msngr_set_provenance_level(prov_level);
                    break;
                case 'R':
                    dsproc_set_reprocessing_mode(1);
                    break;
                default:
                    if (argc > 1 && *(argv+1)[0] != '-') {
                        argc--;
                        argv++;
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c %s'\n",
                            program_name, c, *argv);
                    }
                    else {
                        fprintf(stderr,
                            "%s: Ignoring Invalid Option: '-%c'\n",
                            program_name, c);
                    }
                }
            }
        }
        else {
            fprintf(stderr,
                "%s: Ignoring Invalid Option: '%s'\n",
                program_name, *argv);
        }
    }

    /************************************************************
    *  Check for required fields
    *************************************************************/

    rt_mode = dsproc_get_real_time_mode();

    if (!_DSProc->site     ||
        !_DSProc->facility ||
        !_DSProc->name     ||
        (!rt_mode && !_DSProc->cmd_line_begin)) {

        _dsproc_vap_exit_usage(
            program_name, nproc_names, proc_names, 1);
    }

    if (_DSProc->cmd_line_begin && _DSProc->cmd_line_end &&
        _DSProc->cmd_line_begin >= _DSProc->cmd_line_end) {

        fprintf(stderr,
            "\n%s: The begin time must be less than the end time\n\n",
            program_name);

        _dsproc_destroy();
        exit(1);
    }

    /************************************************************
    *  Make sure we have a valid process name
    *************************************************************/

    if (nproc_names > 1) {

        for (ni = 0; ni < nproc_names; ni++) {
            if (strcmp(_DSProc->name, proc_names[ni]) == 0) {
                break;
            }
        }

        if (ni == nproc_names) {

            fprintf(stderr,
                "\n%s: Invalid Process Name: '%s'\n\n",
                program_name, _DSProc->name);

            _dsproc_vap_exit_usage(
                program_name, nproc_names, proc_names, 1);
        }
    }

    return;

MISSING_ARG:

    fprintf(stderr, "\n%s: Missing required argument for -%c option\n\n",
        program_name, c);
    _dsproc_destroy();
    exit(1);

MEMORY_ERROR:

    fprintf(stderr,
        "\n%s: Memory allocation error initializing process\n",
        program_name);

    _dsproc_destroy();
    exit(1);
}

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/*******************************************************************************
 *  Public Functions
 */
