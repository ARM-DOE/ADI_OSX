/********************************************************************************
*
*  Author:
*     name:
*     phone:
*     email:
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 65934 $
*    $Author: ermold $
*    $Date: 2015-11-18 19:24:30 +0000 (Wed, 18 Nov 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file csv_ingest.c
 *  CSV Ingest Main Functions.
 */

#include "csv_ingest.h"

static char *gVersion = "$State: ingest-csv_ingestor-1.0-alpha $";

/**
 *  Initialize the CSV process.
 *
 *  This function will:
 *
 *    - create the UserData structure
 *    - load the CSV configuration file
 *    - initialize the CSV parser
 *    - add the raw data file patterns
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @retval  data  void pointer to the UserData structure
 *  @retval  NULL  if an error occurred
 */
void *csv_ingest_init(void)
{
    UserData   *data = (UserData *)NULL;
    CSVConf    *conf = (CSVConf *)NULL;
    CSVParser  *csv  = (CSVParser *)NULL;
    int         ndsids;
    int        *dsids;
    const char *level;
    int         status;
    SplitMode   split_mode;
    double      split_start;
    double      split_interval;

    /************************************************************
    *  Create the UserData structure
    *************************************************************/

    DSPROC_DEBUG_LV1(
        "Creating user defined data structure\n");

    data = (UserData *)calloc(1, sizeof(UserData));

    if (!data) {

        DSPROC_ERROR( DSPROC_ENOMEM,
            "Memory allocation error creating user data structure\n");

        goto ERROR_EXIT;
    }

    data->proc_name = dsproc_get_name();
    data->site      = dsproc_get_site();
    data->fac       = dsproc_get_facility();

    /************************************************************
    *  Find and load the CSV Ingest configuration file
    *************************************************************/

    conf = dsproc_init_csv_conf(data->proc_name);
    if (!conf) {
        goto ERROR_EXIT;
    }

    status = dsproc_load_csv_conf(conf, 0, CSV_CHECK_DATA_CONF);
    if (status <= 0) {

        if (status == 0) {

            DSPROC_ERROR( NULL,
                "Could not find required configuration file");
        }

        goto ERROR_EXIT;
    }

    data->conf = conf;

    /************************************************************
    *  Initialize the CSV Parser
    *************************************************************/

    csv = dsproc_init_csv_parser(NULL);
    if (!csv) {
        goto ERROR_EXIT;
    }

    if (!dsproc_configure_csv_parser(conf, csv)) {
        goto ERROR_EXIT;
    }

    data->csv = csv;

    /*********************************************************************
    * Get the input datastream ID.
    **********************************************************************/

    ndsids = dsproc_get_input_datastream_ids(&dsids);

    if (ndsids != 1) {

        if (ndsids > 1) {
            DSPROC_ERROR( NULL,
                "Too Many Input Datastreams Defined For Process: %s"
                " -> found %d, but only 1 was expected",
                data->proc_name, ndsids);
        }
        else if (ndsids == 0) {
            DSPROC_ERROR( NULL,
                "No Input Datastreams Defined For Process: %s",
                data->proc_name);
        }

        free(dsids);
        goto ERROR_EXIT;
    }

    data->in_dsid = dsids[0];
    free(dsids);

    /************************************************************
    *  Add the file patterns to look for to the input datastream
    *************************************************************/

    if (!conf->fn_npatterns) {

        DSPROC_ERROR( NULL,
            "No Input File Name Patterns Found In CSV Configuration File: %s/%s",
            conf->file_path, conf->file_name);

        goto ERROR_EXIT;
    }

    if (!dsproc_add_datastream_file_patterns(
        data->in_dsid, conf->fn_npatterns, conf->fn_patterns, 0)) {

        goto ERROR_EXIT;
    }

    /***********************************************************
    *  Get the output datastream IDs
    **************************************************************/

    ndsids = dsproc_get_output_datastream_ids(&dsids);
    if (ndsids != 2) {

        if (ndsids > 2) {
            DSPROC_ERROR( NULL,
                "Too Many Output Datastreams Defined For Process: %s",
                " -> expected 2 but found %d",
                data->proc_name, ndsids);
        }
        else if (ndsids == 1) {

            level = dsproc_datastream_class_level(dsids[0]);

            if (*level == '0') {

                DSPROC_ERROR( NULL,
                    "Not Enough Output Datastreams Defined For Process: %s"
                    " -> missing output datastream for processed data",
                    data->proc_name);
            }
            else {

                DSPROC_ERROR( NULL,
                    "Not Enough Output Datastreams Defined For Process: %s"
                    " -> missing output datastream for raw data",
                    data->proc_name);
            }
        }
        else if (ndsids == 0) {

            DSPROC_ERROR( NULL,
                "No Output Datastreams Defined For Process: %s",
                data->proc_name);
        }

        free(dsids);
        goto ERROR_EXIT;
    }

    level = dsproc_datastream_class_level(dsids[0]);

    if (*level == '0') {

        data->raw_dsid = dsids[0];

        level = dsproc_datastream_class_level(dsids[1]);

        if (*level != '0') {
            data->out_dsid = dsids[1];
        }
        else {

            DSPROC_ERROR( NULL,
                "Invalid Output Datastream Defined For Process: %s"
                " -> a process can only have one output datastream for raw data",
                data->proc_name);

            free(dsids);
            free(data);
            return((void *)NULL);
        }
    }
    else {

        data->out_dsid = dsids[0];

        level = dsproc_datastream_class_level(dsids[1]);

        if (*level == '0') {
            data->raw_dsid = dsids[1];
        }
        else {

            DSPROC_ERROR( NULL,
                "Invalid Output Datastream Defined For Process: %s"
                " -> a process must have one output datastream for raw data",
                data->proc_name);

            free(dsids);
            free(data);
            return((void *)NULL);
        }
    }

    free(dsids);

    /************************************************************
    *  Check if a split interval was set in the conf file
    *************************************************************/

    if (conf->split_interval) {

        if (strcmp(conf->split_interval, "DAILY") == 0) {

            split_mode     = SPLIT_ON_HOURS;
            split_start    = 0.0;
            split_interval = 24.0;
        }
        else if (strcmp(conf->split_interval, "MONTHLY") == 0) {

            split_mode     = SPLIT_ON_MONTHS;
            split_start    = 1.0;
            split_interval = 1.0;
        }
        else if (strcmp(conf->split_interval, "YEARLY") == 0) {

            split_mode     = SPLIT_ON_MONTHS;
            split_start    = 1.0;
            split_interval = 12.0;
        }
        else if (strcmp(conf->split_interval, "FILE") == 0) {

            split_mode     = SPLIT_ON_STORE;
            split_start    = 0.0;
            split_interval = 0.0;
        }
        else {

            DSPROC_ERROR( NULL,
                "Invalid Split Interval Defined In: %s/%s",
                conf->file_path, conf->file_name);

            free(data);
            return((void *)NULL);
        }

        dsproc_set_datastream_split_mode(
            data->out_dsid, split_mode, split_start, split_interval);
    }

    return((void *)data);

ERROR_EXIT:

    if (data) free(data);
    if (conf) dsproc_free_csv_conf(conf);
    if (csv)  dsproc_free_csv_parser(csv);

    return((void *)NULL);
}

/**
 *  Finish the CSV Ingest process.
 *
 *  This function frees all memory used by the UserData structure.
 *
 *  @param user_data  - void pointer to the UserData structure
 *                      returned by the csv_ingest_init() function
 */
void csv_ingest_finish(void *user_data)
{
    UserData *data = (UserData *)user_data;

    DSPROC_DEBUG_LV1(
        "Cleaning up user defined data structure\n");

    if (data) {

        if (data->conf) dsproc_free_csv_conf(data->conf);
        if (data->csv)  dsproc_free_csv_parser(data->csv);

        free(data);
    }
}

/**
 *  Process a raw CSV data file.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  user_data  void pointer to the UserData structure
 *  @param  input_dir  path to the file
 *  @param  file_name  file name
 *
 *  @retval   1  if successful
 *  @retval   0  if the current file should be skipped
 *  @retval  -1  if a fatal error occurred
 */
int csv_ingest_process_file(
    void       *user_data,
    const char *input_dir,
    const char *file_name)
{
    UserData  *data = (UserData *)user_data;
    CSVConf   *conf = data->conf;
    CSVParser *csv  = data->csv;

    time_t     file_time;
    char       full_path[PATH_MAX];
    int        nrecs_loaded;
    int        nrecs_stored;
    int        status;

    /************************************************************
    *  Initialize data structures for a new file read
    *************************************************************/

    data->input_dir  = input_dir;
    data->file_name  = file_name;
    data->begin_time = 0;
    data->end_time   = 0;

    dsproc_init_csv_parser(csv);

    /* Set the number of dots from the end of the file name to
     * preserve when the file is renamed. */

    if (!dsproc_set_preserve_dots_from_name(data->raw_dsid, file_name)) {
        return(-1);
    }

    /************************************************************
    *  Check for a time varying CSV configuration file
    *************************************************************/

    file_time = dsproc_get_csv_file_name_time(csv, file_name, NULL);
    if (file_time < 0) return(-1);

    status = dsproc_load_csv_conf(conf, file_time, CSV_CHECK_DATA_CONF);
    if (status < 0) return(-1);

    if (status == 1) {

        /* An updated configuration file was found. */

        if (!dsproc_configure_csv_parser(conf, csv)) {
            return(-1);
        }
    }

    /************************************************************
    *  Read in the raw data file
    *************************************************************/

    DSPROC_DEBUG_LV1("Loading file:   %s\n", data->file_name);

    snprintf(full_path, PATH_MAX, "%s/%s", input_dir, file_name);

    nrecs_loaded = csv_ingest_read_data(data);
    if (nrecs_loaded  < 0) return(-1);
    if (nrecs_loaded == 0) {

        DSPROC_BAD_FILE_WARNING(file_name,
            "No valid data records found in file\n"
            " -> marking file bad and continuing\n");

        if (data->begin_time == 0) {
            data->begin_time = file_time;
        }

        if (dsproc_rename_bad(data->raw_dsid,
            input_dir, file_name, data->begin_time)) {

            return(0);
        }
        else {
            return(-1);
        }
    }

    /************************************************************
    *  Store the data
    *************************************************************/

    nrecs_stored = csv_ingest_store_data(data);
    if (nrecs_stored < 0) return(-1);

    /************************************************************
    *  Rename the raw data file
    *************************************************************/

    if (!dsproc_rename(data->raw_dsid,
        input_dir, file_name, data->begin_time, data->end_time)) {

        return(-1);
    }

    return(1);
}

/**
 *  Main CSV Ingest entry function.
 *
 *  @param  argc - number of command line arguments
 *  @param  argv - command line arguments
 *
 *  @retval  0  successful
 *  @retval  1  failure
 */
int main(int argc, char **argv)
{
    int exit_value;

    /* Set output NetCDF file extension to be .nc */

    dsproc_use_nc_extension();

    /* Set Ingest hook functions */

    dsproc_set_init_process_hook(csv_ingest_init);
    dsproc_set_process_file_hook(csv_ingest_process_file);
    dsproc_set_finish_process_hook(csv_ingest_finish);

    /* Run the ingest */

    exit_value = dsproc_main(
        argc, argv,
        PM_INGEST,
        gVersion,
        0, NULL);

    return(exit_value);
}
