/*******************************************************************************
*
*  Author:
*     name:
*     phone:
*     email:
*
* ********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 65933 $
*    $Author: ermold $
*    $Date: 2015-11-18 19:18:57 +0000 (Wed, 18 Nov 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file csv2nc_store_data.c
 *  CSV2NC Store Data Functions.
 */

#include "csv_ingest.h"

/**
 *  Store metadata in the output dataset.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param   data   pointer to the UserData structure
 *
 *  @retval  1  if successful
 *  @retval  0  if a fatal error occurred
 */
int csv_ingest_set_metadata(UserData *data)
{
    // Prevent unused parameter warnings, these
    // can be removed if this function is ever used
    data = data;

    // Add code to set any extra metadata here

    return(1);
}

/**
 *  Store data.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param   data   pointer to the UserData structure
 *
 *  @retval  nrecs  number of records stored
 *  @retval  -1     if a fatal error occurred
 */
int csv_ingest_store_data(UserData *data)
{
    int         dsid  = data->out_dsid;
    CSVConf    *conf  = data->conf;
    CSVParser  *csv   = data->csv;
    timeval_t  *times = csv->tvs;
    int         nrecs = csv->nrecs;

    CDSGroup   *dataset;
    CSV2CDSMap *map;
    int         nstored;

    /************************************************************
    * Create the output dataset
    *************************************************************/

    dataset = dsproc_create_output_dataset(dsid, times[0].tv_sec, 1);
    if (!dataset) return(-1);

    /* Set the times in the output dataset */

    if (!dsproc_set_sample_timevals(dataset, 0, nrecs, times)) {
        return(-1);
    }

    /* Set the metadata in the output dataset */

    if (!csv_ingest_set_metadata(data)) {
        return(-1);
    }

    /************************************************************
    * Map the CSV fields to the output dataset variables
    *************************************************************/

    map = dsproc_create_csv_to_cds_map(conf, csv, dataset, 0);

    if (!dsproc_map_csv_to_cds(csv, 0, 0, map, dataset, 0, 0)) {
        return(-1);
    }

    dsproc_free_csv_to_cds_map(map);

    /************************************************************
    * Store the output dataset
    *************************************************************/

    /* Dump the output dataset to a file if debug level is > 3 */

    if (dsproc_get_debug_level() > 3) {

        dsproc_dump_output_datasets(
            "./debug_dumps", "before_store.debug", 0);
    }

    nstored = dsproc_store_dataset(dsid, 0);

    return(nstored);
}
