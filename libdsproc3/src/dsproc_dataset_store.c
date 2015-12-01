/*******************************************************************************
*
*  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
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
*    $Revision: 66116 $
*    $Author: ermold $
*    $Date: 2015-11-30 21:55:20 +0000 (Mon, 30 Nov 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_dataset_store.c
 *  Dataset Store Functions.
 */

#include <string.h>
#include <unistd.h>

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */

/*******************************************************************************
 *  Static Functions Visible Only To This Module
 */

/**
 *  Static: Get the next time the output file should be split at.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  ds        - pointer to the DataStream structure
 *  @param  prev_time - time of the previously stored data record
 *
 *  @return
 *    -  next split time (in seconds since 1970)
 *    -  0 if the file should be split now (i.e. SPLIT_ON_STORE is set)
 *    - -1 if an error occurred
 */
static time_t _dsproc_get_next_split_time(
    DataStream *ds,
    time_t      prev_time)
{
    int       split_mode     = ds->split_mode;
    double    split_start    = ds->split_start;
    double    split_interval = ds->split_interval;
    time_t    next_start     = 0;
    time_t    split_time     = 0;
    time_t    interval       = 0;
    struct tm gmt;

    if (split_mode == SPLIT_ON_STORE ||
        split_mode == SPLIT_NONE) {

        return(0);
    }

    /* Get the tm structure for the specified time */

    memset(&gmt, 0, sizeof(struct tm));

    if (!gmtime_r(&prev_time, &gmt)) {

        ERROR( DSPROC_LIB_NAME,
            "Could not get next split time.\n"
            " -> gmtime error: %s\n", strerror(errno));

        dsproc_set_status(DSPROC_ETIMECALC);
        return(-1);
    }

    gmt.tm_hour = 0;
    gmt.tm_min  = 0;
    gmt.tm_sec  = 0;

    /* Get the starting split time */

    if (split_mode == SPLIT_ON_MONTHS) {

        /* get the split interval */

        if (split_interval > 0.0) {
            interval = (time_t)(split_interval + 0.5);
        }
        else {
            interval = 1;
        }

        /* get the starting split time */

        gmt.tm_mon  = (int)(split_start + 0.5) - 1;
        gmt.tm_mday = 1;

        if (gmt.tm_mon < 0 ||
            gmt.tm_mon > 11) {

            gmt.tm_mon = 0;
        }

        split_time = timegm(&gmt);

        if (split_time > prev_time) {

            gmt.tm_year--;

            next_start = split_time;
            split_time = timegm(&gmt);
        }
        else {
            gmt.tm_year++;

            next_start = timegm(&gmt);

            gmt.tm_year--;
        }

        /* find the next split time */

        while (split_time <= prev_time) {

            gmt.tm_mon += interval;

            if (gmt.tm_mon > 11) {

                gmt.tm_mon -= 12;
                gmt.tm_year++;
            }

            split_time = timegm(&gmt);
        }
    }
    else if (split_mode == SPLIT_ON_DAYS) {

        split_start -= 1.0;

        /* get the split interval */

        if (split_interval > 0.0) {
            interval = (time_t)((split_interval * 86400.0) + 0.5);
        }
        else {
            interval = 86400;
        }

        /* get the starting split time */

        gmt.tm_mday = 1;
        split_time  = timegm(&gmt);

        if (split_start > 0.0) {
            split_time += (time_t)((split_start * 86400.0) + 0.5);
        }

        if (split_time > prev_time) {

            if (gmt.tm_mon) {
                gmt.tm_mon--;
            }
            else {
                gmt.tm_mon = 11;
                gmt.tm_year--;
            }

            next_start = split_time;
            split_time = timegm(&gmt);

            if (split_start > 0.0) {
                split_time += (time_t)((split_start * 86400.0) + 0.5);
            }
        }
        else {

            if (gmt.tm_mon < 11) {
                gmt.tm_mon++;
            }
            else {
                gmt.tm_mon = 0;
                gmt.tm_year++;
            }

            next_start = timegm(&gmt);

            if (split_start > 0.0) {
                next_start += (time_t)((split_start * 86400.0) + 0.5);
            }
        }

        /* find the next split time */

        while (split_time <= prev_time) {
            split_time += interval;
        }
    }
    else { /* default: SPLIT_ON_HOURS */

        /* get the split interval */

        if (split_interval > 0.0) {
            interval = (time_t)((split_interval * 3600.0) + 0.5);
        }
        else {
            interval = 86400;
        }

        /* get the starting split time */

        split_time = timegm(&gmt);

        if (split_start > 0.0) {
            split_time += (time_t)((split_start * 3600.0) + 0.5);
        }

        next_start = split_time;

        if (split_time > prev_time) {
            split_time -= 86400;
        }
        else {
            next_start += 86400;;
        }

        /* find the next split time */

        while (split_time <= prev_time) {
            split_time += interval;
        }
    }

    if (next_start < split_time) {
        return(next_start);
    }

    return(split_time);
}

/*******************************************************************************
 *  Private Functions Visible Only To This Library
 */

/**
 *  Private: Convert seconds since 1970 to a timestamp.
 *
 *  This function will create a timestamp of the form:
 *
 *      'YYYYMMDD.hhmmss'.
 *
 *  The timestamp argument must be large enough to hold at least 16 characters
 *  (15 plus the null terminator).
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  secs1970  - seconds since 1970
 *  @param  timestamp - output: timestamp string in the form YYYYMMDD.hhmmss
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int _dsproc_create_timestamp(time_t secs1970, char *timestamp)
{
    struct tm gmt;

    memset(&gmt, 0, sizeof(struct tm));

    if (!gmtime_r(&secs1970, &gmt)) {

        ERROR( DSPROC_LIB_NAME,
            "Could not create timestamp for: %ld\n"
            " -> gmtime error: %s\n",
            (long)secs1970, strerror(errno));

        sprintf(timestamp, "GMTIME.ERROR");
        dsproc_set_status(DSPROC_ETIMECALC);
        return(0);
    }

    sprintf(timestamp,
        "%04d%02d%02d.%02d%02d%02d",
        gmt.tm_year + 1900,
        gmt.tm_mon  + 1,
        gmt.tm_mday,
        gmt.tm_hour,
        gmt.tm_min,
        gmt.tm_sec);

    return(1);
}

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/**
 *  Store all output datasets.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int dsproc_store_output_datasets()
{
    DataStream *ds;
    int         ds_id;

    for (ds_id = 0; ds_id < _DSProc->ndatastreams; ds_id++) {

        ds = _DSProc->datastreams[ds_id];

        if (ds->role == DSR_OUTPUT && ds->out_cds) {

            if (dsproc_store_dataset(ds_id, 0) < 0) {
                return(0);
            }
        }
    }

    return(1);
}

/**
 *  Store an output dataset.
 *
 *  This function will:
 *
 *    - Filter out duplicate records in the dataset, and verify that the
 *      record times are in chronological order. Duplicate records are
 *      defined has having identical times and data values.
 *
 *    - Filter all NaN and Inf values for variables that have a missing value
 *      defined for datastreams that have the DS_FILTER_NANS flag set. This
 *      should only be used if the DS_STANDARD_QC flag is also set, or for
 *      datasets that do not have any QC variables defined. This is the default
 *      for a and b level datastreams.
 *      (see the dsproc_set_datastream_flags() function).
 *
 *    - Apply standard missing value, min, max, and delta QC checks for
 *      datastreams that have the DS_STANDARD_QC flag set. This is the default
 *      for b level datastreams.
 *      (see the dsproc_set_datastream_flags() function).
 *
 *    - Filter out all records that are duplicates of previously stored
 *      data, and verify that the records do not overlap any previously
 *      stored data. This check is currently being skipped if we are in
 *      reprocessing mode, and the file splitting mode is SPLIT_ON_STORE
 *      (the default for VAPs).
 *
 *    - Verify that none of the record times are in the future.
 *
 *    - Merge datasets with existing files and only split on defined intervals
 *      or when metadata values change. The default for VAPs is to create a new
 *      file for every dataset stored, and the default for ingests is to create
 *      daily files that split at midnight UTC
 *      (see the dsproc_set_datastream_split_mode() function).
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  ds_id   - datastream ID
 *  @param  newfile - specifies if a new file should be created
 *
 *  @return
 *    -  number of data samples stored.
 *    -  0 if no data was found in the dataset, or if all the data
 *       samples were duplicates of previously stored data.
 *    - -1 if an error occurred
 */
int dsproc_store_dataset(
    int ds_id,
    int newfile)
{
    DataStream *ds            = _DSProc->datastreams[ds_id];
    const char *ds_path       = ds->dir->path;
    time_t      current_time  = time(NULL);
    int         reproc_mode   = dsproc_get_reprocessing_mode();
    int         force_mode    = dsproc_get_force_mode();

    CDSGroup   *out_dataset   = ds->out_cds;
    timeval_t  *out_times     = (timeval_t *)NULL;
    size_t      out_ntimes;
    timeval_t   out_begin;
    timeval_t   out_end;

    DSFile    **dsfiles       = (DSFile **)NULL;
    DSFile     *dsfile        = (DSFile *)NULL;
    timeval_t   dsfile_end;
    CDSGroup   *dsfile_dod;

    time_t      split_time    = 0;
    timeval_t   split_timeval = { 0, 0 };

    int         filtered_first_sample;
    time_t      base_time;
    time_t      midnight;
    CDSVar     *time_var;
    CDSAtt     *time_desc_att;
    char       *time_desc   = (char *)NULL;

    char        timestamp[32];
    char        full_path[PATH_MAX];
    int         ncid;

    size_t      ds_start;
    size_t      nc_start;
    size_t      count;
    int         si, ei;

    int         last_errno;
    int         status;
    char        current_ts[32], begin_ts[32], end_ts[32];

    /************************************************************
    *  Make sure we have an output dataset with data in it
    *************************************************************/

    if (!out_dataset) {
        DEBUG_LV1( DSPROC_LIB_NAME,
            "%s: Attempt to store NULL dataset\n", ds->name);
        return(0);
    }

    out_ntimes = 0;
    out_times  = dsproc_get_sample_timevals(out_dataset, 0, &out_ntimes, NULL);

    if (!out_times) {

        if (out_ntimes == 0) {

            DEBUG_LV1( DSPROC_LIB_NAME,
                "%s: No data found in output dataset\n", ds->name);

            _dsproc_free_datastream_out_cds(ds);
            return(0);
        }
        else {
            goto ERROR_EXIT;
        }
    }

    out_begin = out_times[0];

    /************************************************************
    * Get the long_name of the time variable. This may be needed
    * later if the base time needs to be adjusted.
    *************************************************************/

    time_desc = (char *)NULL;
    time_var  = cds_get_var(out_dataset, "time");

    if (time_var) {

        time_desc_att = cds_get_att(time_var, "long_name");

        if (time_desc_att &&
            time_desc_att->type == CDS_CHAR) {

            time_desc = strdup(time_desc_att->value.cp);
            if (!time_desc) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not store data for: %s\n"
                    " -> memory allocation error\n",
                    ds->name);

                dsproc_set_status(DSPROC_ENOMEM);
                goto ERROR_EXIT;
            }
        }
    }

    /************************************************************
    *  Filter out duplicate samples in the output dataset, and
    *  verify all samples are in chronological order.
    *************************************************************/

    if (!_dsproc_filter_duplicate_samples(
        &out_ntimes, out_times, out_dataset)) {

        goto ERROR_EXIT;
    }

    /************************************************************
    *  Set cell boundary data variables.
    *************************************************************/

    if (!dsproc_set_bounds_data(out_dataset, 0, 0)) {
        goto ERROR_EXIT;
    }

    /************************************************************
    *  Filter Nan/Inf values
    *************************************************************/

    if (ds->flags & DS_FILTER_NANS) {
        if (!dsproc_filter_dataset_nans(out_dataset, 1)) {
            goto ERROR_EXIT;
        }
    }

    /************************************************************
    *  Apply Standard QC checks
    *************************************************************/

    if (ds->flags & DS_STANDARD_QC) {
        if (!dsproc_standard_qc_checks(ds_id, out_dataset)) {
            goto ERROR_EXIT;
        }
    }

    /************************************************************
    *  Apply Custom QC checks
    *************************************************************/

    status = _dsproc_custom_qc_hook(ds_id, out_dataset);

    if (status < 0) {
        goto ERROR_EXIT;
    }

    if (status == 0) {
        if (out_times) free(out_times);
        if (time_desc) free(time_desc);
        _dsproc_free_datastream_out_cds(ds);
        return(0);
    }

    /************************************************************
    *  Filter out samples in the output dataset that are duplicates
    *  of previously stored data, and verify that the remaining
    *  samples do not overlap any previously stored data.
    *************************************************************/

    /* BDE: I don't really like skipping the check for data overlaps
     * with previously stored data, but it will maintain the behavior
     * the VAP developers are used to. */

    if (!reproc_mode ||
        ds->split_mode != SPLIT_ON_STORE) {

        if (!_dsproc_filter_stored_samples(
            ds, &out_ntimes, out_times, out_dataset)) {

            goto ERROR_EXIT;
        }

        /* Check if all samples were filtered out */

        if (!out_ntimes) {

            WARNING( DSPROC_LIB_NAME,
                "%s: All data was filtered from the dataset\n", ds->name);

            if (out_times) free(out_times);
            if (time_desc) free(time_desc);
            _dsproc_free_datastream_out_cds(ds);
            return(0);
        }
    }

    filtered_first_sample = (TV_EQ(out_times[0], out_begin)) ? 0 : 1;

    out_begin = out_times[0];
    out_end   = out_times[out_ntimes - 1];

    format_timeval(&out_begin, begin_ts);
    format_timeval(&out_end,   end_ts);

    /************************************************************
    *  Validate the begin and end times
    *************************************************************/

    DEBUG_LV1( DSPROC_LIB_NAME,
        "%s: Validating time range ['%s' -> '%s']\n",
        ds->name, begin_ts, end_ts);

    /* Validate first sample time */

    if (!dsproc_validate_datastream_data_time(ds_id, &out_begin)) {
        goto ERROR_EXIT;
    }

    /* Validate last sample time */

    if (out_end.tv_sec > current_time) {

        format_secs1970(current_time, current_ts);

        ERROR( DSPROC_LIB_NAME,
            "Could not store data for: %s\n"
            " -> end time '%s' is in the future (current time is '%s')\n",
            ds->name, end_ts, current_ts);

        dsproc_disable(DSPROC_EFUTURETIME);
        goto ERROR_EXIT;
    }

    /************************************************************
    *  Check if the base time needs to be adjusted.
    *
    *  We only want to do this if the first sample of the dataset
    *  was filtered out, and the time of midnight is being used
    *  as the base_time.
    *************************************************************/

    if (filtered_first_sample) {

        base_time = dsproc_get_base_time(out_dataset);
        midnight  = cds_get_midnight(base_time);

        if (base_time == midnight) {

            midnight = cds_get_midnight(out_begin.tv_sec);

            if (midnight != base_time) {
                dsproc_set_base_time(out_dataset, time_desc, midnight);
            }
        }
    }

    /************************************************************
    * Check if we need to start a new file for this Dataset
    *************************************************************/

    dsfile = (DSFile *)NULL;

    if (!newfile &&
        ds->split_mode != SPLIT_ON_STORE &&
        ds->split_mode != SPLIT_NONE) {

        /************************************************************
        * Check for an existing file we should append this dataset to
        *************************************************************/

        status = _dsproc_find_dsfiles(ds->dir, NULL, &out_begin, &dsfiles);

        if (status < 0) {
            goto ERROR_EXIT;
        }

        if (status > 0) {

            dsfile = dsfiles[0];
            free(dsfiles);

            /************************************************************
            * Make sure the begin time of the output dataset is after
            * the end time of the previous dataset.
            *************************************************************/

            dsfile_end = dsfile->timevals[dsfile->ntimes - 1];

            if (TV_LTEQ(out_begin, dsfile_end)) {

                ERROR( DSPROC_LIB_NAME,
                    "%s: Overlapping records found with previously stored data\n"
                    " -> '%s' to '%s': output dataset overlaps file: %s\n",
                    ds->name, begin_ts, end_ts, dsfile->name);

                dsproc_set_status(DSPROC_ETIMEOVERLAP);
                goto ERROR_EXIT;
            }

            /************************************************************
            * Check for a split interval between the previously
            * stored data and the output dataset.
            *************************************************************/

            split_time = _dsproc_get_next_split_time(ds, dsfile_end.tv_sec);

            if (split_time <= out_begin.tv_sec) {
                dsfile = (DSFile *)NULL;
            }
            else {

                /************************************************************
                * Check for metadata changes between the previously
                * stored data and the output dataset.
                *************************************************************/

                dsfile_dod = _dsproc_fetch_dsfile_dod(dsfile);

                if (!dsfile_dod) {
                    goto ERROR_EXIT;
                }

                DEBUG_LV1( DSPROC_LIB_NAME,
                    "%s: Checking for DOD metadata changes\n",
                    ds->name);

                status = dsproc_compare_dods(dsfile_dod, out_dataset, 1);

                if (status < 0) {
                    goto ERROR_EXIT;
                }

                if (status != 0) {

                    /* We do not want to split files only on dod_version changes,
                     * but we do want to report if the changes found were a result
                     * of a dod_version change. */

                    dsproc_compare_dod_versions(dsfile_dod, out_dataset, 1);

                    dsfile = (DSFile *)NULL;

                    WARNING( DSPROC_LIB_NAME,
                        "%s: Forcing NetCDF file split at: %s\n",
                        ds->name, begin_ts);
                }
            }

        } /* end if found previous datastream file */

    } /* end if (!newfile) */

    /************************************************************
    *  Store the Dataset
    *************************************************************/

    LOG( DSPROC_LIB_NAME,
        "Storing:    %s data from '%s' to '%s': %d records\n",
        ds->name, begin_ts, end_ts, out_ntimes);

    /* Loop over split intervals */

    for (si = 0, ei = 0; si < (int)out_ntimes; si = ei + 1) {

        /************************************************************
        *  Get the time index of the next file split,
        *  and set the sample count accordingly.
        *************************************************************/

        split_time = _dsproc_get_next_split_time(ds, out_times[si].tv_sec);

        if (split_time) {

            split_timeval.tv_sec = split_time;

            ei = cds_find_timeval_index(
                out_ntimes, out_times, split_timeval, CDS_LT);
        }
        else {
            ei = out_ntimes - 1;
        }

        ds_start = si;
        count    = ei - si + 1;

        if (dsfile) {

            /************************************************************
            *  Append this record set to the existing file
            *************************************************************/

            if (msngr_debug_level || msngr_provenance_level) {

                format_timeval(&out_times[si], begin_ts);
                format_timeval(&out_times[ei], end_ts);

                DEBUG_LV1( DSPROC_LIB_NAME,
                    "%s: Appending record set to existing file:\n"
                    " - times:  '%s' to '%s'\n"
                    " - file:   %s",
                    ds->name, begin_ts, end_ts, dsfile->full_path);
            }

            if (!_dsproc_open_dsfile(dsfile, NC_WRITE)) {
                goto ERROR_EXIT;
            }

            ncid     = dsfile->ncid;
            nc_start = dsfile->ntimes;

            if (!_dsproc_update_stored_metadata(out_dataset, ncid)) {
                goto ERROR_EXIT;
            }
        }
        else {

            /************************************************************
            * Check if we need to adjust the base_time metadata
            * for the first time in this output record set.
            *************************************************************/

            if (si != 0) {

                midnight = cds_get_midnight(out_times[si].tv_sec);

                dsproc_set_base_time(out_dataset, time_desc, midnight);
            }

            /************************************************************
            *  Create the new file
            *************************************************************/

            /* Create file name */

            _dsproc_create_timestamp(out_times[si].tv_sec, timestamp);

            snprintf(full_path, PATH_MAX, "%s/%s.%s.%s",
                ds_path, ds->name, timestamp, ds->extension);

            /* Create the file */

            if (msngr_debug_level || msngr_provenance_level) {

                format_timeval(&out_times[si], begin_ts);
                format_timeval(&out_times[ei], end_ts);

                DEBUG_LV1( DSPROC_LIB_NAME,
                    "%s: Creating new file for record set:\n"
                    " - times:  '%s' to '%s'\n"
                    " - file:   %s",
                    ds->name, begin_ts, end_ts, full_path);
            }

            if (reproc_mode) {
                ncid = ncds_create_file(out_dataset, full_path, 0, 0, 1);
            }
            else {
                ncid = ncds_create_file(out_dataset, full_path, NC_NOCLOBBER, 0, 1);
            }

            if (!ncid) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not create file: %s\n",
                    full_path);

                dsproc_set_status(DSPROC_ENCCREATE);
                goto ERROR_EXIT;
            }

            /************************************************************
            *  Write the static data
            *************************************************************/

            if (!ncds_write_static_data(out_dataset, ncid)) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not write static data to file: %s\n",
                    full_path);

                dsproc_set_status(DSPROC_ENCWRITE);
                goto ERROR_EXIT;
            }

            nc_start = 0;
        }

        /************************************************************
        *  Write the data records
        *************************************************************/

        if (!ncds_write_records(out_dataset, ds_start, ncid, nc_start, count)) {

            if (dsfile) {
                ERROR( DSPROC_LIB_NAME,
                    "Could not write data records to file: %s\n",
                    dsfile->full_path);
            }
            else {
                ERROR( DSPROC_LIB_NAME,
                    "Could not write data records to file: %s\n",
                    full_path);
            }

            dsproc_set_status(DSPROC_ENCWRITE);
            goto ERROR_EXIT;
        }

        /************************************************************
        *  Flush data to disk
        *************************************************************/

        if (dsfile) {

            if (!ncds_sync(ncid)) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not sync data to file: %s\n",
                    dsfile->full_path);

                dsproc_set_status(DSPROC_ENCSYNC);
                goto ERROR_EXIT;
            }

            /* Make sure the file times get reloaded
             * if this file is accessed again */

            dsfile->stats.st_mtime = 0;

            /* We are done appending data to this file */

            dsfile = (DSFile *)NULL;
        }
        else {

            if (!ncds_close(ncid)) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not close file: %s\n",
                    full_path);

                dsproc_set_status(DSPROC_ENCCLOSE);
                goto ERROR_EXIT;
            }

            /* Make sure the directory listing gets reloaded
             * if this directory is accessed again */

            ds->dir->stats.st_mtime = 0;
        }

    } /* end loop over split intervals */

    /************************************************************
    *  Update datastream stats and times
    *************************************************************/

    dsproc_update_datastream_data_stats(
        ds_id, out_ntimes, &out_begin, &out_end);

    if (out_times) free(out_times);
    if (time_desc) free(time_desc);
    _dsproc_free_datastream_out_cds(ds);
    return((int)out_ntimes);

ERROR_EXIT:

    last_errno = errno;

    if (out_times) free(out_times);
    if (time_desc) free(time_desc);
    _dsproc_free_datastream_out_cds(ds);

    if (force_mode && !dsproc_is_fatal(last_errno)) {

        LOG( DSPROC_LIB_NAME,
            "FORCE: Forcing ingest to skip output dataset for: %s\n",
            ds->name);

        return(0);
    }

    return(-1);
}

/*******************************************************************************
 *  Public Functions
 */

