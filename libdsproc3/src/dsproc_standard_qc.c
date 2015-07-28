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
*    $Revision: 52988 $
*    $Author: ermold $
*    $Date: 2014-03-12 20:58:18 +0000 (Wed, 12 Mar 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_standard_qc.c
 *  Standard QC Checks.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */

/*******************************************************************************
 *  Static Data and Functions Visible Only To This Module
 */

/** Array of variable names that should be excluded from the QC checks. */
static char **ExQcVars;

/** Number of variables that should be excluded from the QC checks. */
static int    NumExQcVars;

/**
 *  Static: Check if a variable has been exculded from the QC checks.
 *
 *  @param  var_name - the variable name
 *
 *  @return
 *    - 1 if the variable has been excluded from the QC checks
 *    - 0 if the variable has not been excluded
 */
static int _dsproc_is_excluded_from_standard_qc_checks(const char *var_name)
{
    int vi;

    for (vi = 0; vi < NumExQcVars; ++vi) {
        if (strcmp(var_name, ExQcVars[vi]) == 0) {
            return(1);
        }
    }

    return(0);
}

/**
 *  Static: Get a data attribute.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  var      - pointer to the variable
 *  @param  att_name - the attribute name
 *  @param  attp     - output: pointer to the attribute
 *
 *  @return
 *    -  1 if found
 *    -  0 if not found
 *    - -1 if an error occurred
 */
static int _dsproc_get_data_att(
    CDSVar      *var,
    const char  *att_name,
    CDSAtt     **attp)
{
    CDSAtt *att;

    *attp = (CDSAtt *)NULL;

    att = cds_get_att(var, att_name);

    if (att && att->length && att->value.vp) {

        if (att->type != var->type) {

            ERROR( DSPROC_LIB_NAME,
                "Attribute type does not match variable type:\n"
                " - variable  %s has type: %s\n"
                " - attribute %s has type: %s\n",
                cds_get_object_path(var),
                cds_data_type_name(var->type),
                cds_get_object_path(att),
                cds_data_type_name(att->type));

            dsproc_set_status(DSPROC_EDATAATTTYPE);
            return(-1);
        }

        *attp = att;

        return(1);
    }

    return(0);
}

/**
 *  Static: Get previous DSFile and time index.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  ds         - pointer to the DataStream structure
 *  @param  cds_object - pointer to the CDS Object to get the time from
 *  @param  dsfile     - output: pointer to the DSFile
 *  @param  index      - output: index of the time in the DSFile that is just
 *                       prior to the first time in the specified CDS Object.
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
static int _dsproc_get_prev_dsfile_time_index(
    DataStream *ds,
    void       *cds_object,
    DSFile    **dsfile,
    int        *index)
{
    size_t     count         = 1;
    timeval_t  start_timeval = { 0, 0 };
    int        ndsfiles      = 0;
    DSFile   **dsfiles       = (DSFile **)NULL;

    *dsfile = (DSFile *)NULL;
    *index  = -1;

    if (!dsproc_get_sample_timevals(cds_object, 0, &count, &start_timeval)) {
        return((count == 0) ? 1 : 0);
    }

    ndsfiles = _dsproc_find_dsfiles(ds->dir, NULL, &start_timeval, &dsfiles);

    if (ndsfiles <  0) return(0);
    if (ndsfiles == 0) return(1);

    *dsfile = dsfiles[0];
    free(dsfiles);

    *index = cds_find_timeval_index(
        (*dsfile)->ntimes, (*dsfile)->timevals, start_timeval, CDS_LT);

    return(1);
}

/*******************************************************************************
 *  Private Functions Visible Only To This Library
 */

/**
 *  Private: Free all memory used by the interval ExQcVars list.
 */
void _dsproc_free_excluded_qc_vars(void)
{
    int vi;

    if (ExQcVars) {

        for (vi = 0; vi < NumExQcVars; ++vi) {
            if (ExQcVars[vi]) free(ExQcVars[vi]);
        }

        free(ExQcVars);
    }

    ExQcVars    = (char **)NULL;
    NumExQcVars = 0;
}

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/**
 *  Exclude a variable from the standard QC checks.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  var_name - the name of the variable to exclude
 *
 *  @return
 *    - 1 if successful
 *    - 0 if a memory allocation error occurred
 */
int dsproc_exclude_from_standard_qc_checks(const char *var_name)
{
    const char *xvar;
    char      **new_xvars;
    int         new_nxvars;

    xvar = var_name;

    if ((strlen(var_name) > 3) &&
        (strncmp(var_name, "qc_", 3) == 0)) {

         xvar = &(var_name[3]);
    }

    if (_dsproc_is_excluded_from_standard_qc_checks(xvar)) {
        return(1);
    }

    new_nxvars = NumExQcVars + 1;
    new_xvars  = (char **)realloc(
        ExQcVars, new_nxvars * sizeof(char *));

    if (!new_xvars) goto MEMORY_ERROR;

    ExQcVars = new_xvars;

    if (!(ExQcVars[NumExQcVars] = strdup(xvar))) {
        goto MEMORY_ERROR;
    }

    NumExQcVars += 1;

    return(1);

MEMORY_ERROR:

    ERROR( DSPROC_LIB_NAME,
        "Could not exclude variable from standard QC checks: %s\n"
        " -> memory allocation error\n", var_name);

    dsproc_set_status(DSPROC_ENOMEM);
    return(0);
}

/**
 *  Perform all standard QC checks.
 *
 *  If a qc_time variable is found in the dataset, this function will use the
 *  following time variable attributes to determine the lower and upper delta
 *  time limits, and if the previously store sample time should be used for the
 *  delta check on the first sample:
 *
 *     - delta_t_lower_limit
 *     - delta_t_upper_limit
 *     - prior_sample_flag
 *
 *  The QC variable bit values used by these checks are:
 *
 *    - bit 1 = "Delta time between current and previous samples is zero."
 *    - bit 2 = "Delta time between current and previous samples is less than the delta_t_lower_limit field attribute."
 *    - bit 3 = "Delta time between current and previous samples is greater than the delta_t_upper_limit field attribute."
 *
 *  For all variables that have a companion QC variable this function will use
 *  the following variable attributes to determine the missing_value, and min,
 *  max, delta limits:
 *
 *    - missing_value, or _FillValue, or default NetCDF fill value
 *    - valid_min
 *    - valid_max
 *    - valid_delta
 *
 *  The QC variable bit values used by these checks are:
 *
 *    - bit 1 = "Value is equal to missing_value."
 *    - bit 2 = "Value is less than the valid_min."
 *    - bit 3 = "Value is greater than the valid_max."
 *    - bit 4 = "Difference between current and previous values exceeds valid_delta."
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  ds_id   - datastream ID used to get the previous dataset
 *                    if it is needed for the QC time and delta checks.
 *  @param  dataset - pointer to the dataset
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int dsproc_standard_qc_checks(
    int         ds_id,
    CDSGroup   *dataset)
{
    DataStream *ds           = _DSProc->datastreams[ds_id];
    timeval_t   prev_timeval = { 0, 0 };
    DSFile     *dsfile       = (DSFile *)NULL;
    int         index        = -1;

    CDSVar     *var;
    CDSVar     *qc_var;
    CDSAtt     *att;
    int         prior_sample_flag = 0;
    int         is_base_time;
    size_t      length;
    int         found;

    int         dc_nvars     = 0;
    CDSVar    **dc_vars      = (CDSVar **)NULL;
    CDSVar    **dc_qc_vars   = (CDSVar **)NULL;
    char      **dc_var_names = (char **)NULL;
    CDSGroup   *dc_dataset   = (CDSGroup *)NULL;

    CDSVar     *prev_var;
    CDSVar     *prev_qc_var;
    int         bad_flags    = 0x1 | 0x2 | 0x4;
    int         vi;

    DEBUG_LV1( DSPROC_LIB_NAME,
        "%s: Applying standard QC checks\n",
        dataset->name);

    /************************************************************
    * Apply the QC time checks
    *************************************************************/

    /* Check if we have a qc_time variable */

    if ((var    = dsproc_get_time_var(dataset)) &&
        (qc_var = dsproc_get_qc_var(var))       &&
        var->sample_count) {

        /* Check if we need the time of the previously stored sample */

        att = cds_get_att(qc_var, "prior_sample_flag");
        if (att) {

            length = 1;
            cds_get_att_value(att, CDS_INT, &length, &prior_sample_flag);

            if (length && prior_sample_flag) {

                /* Get the time of the previously stored sample */

                if (!dsfile) {

                    if (!_dsproc_get_prev_dsfile_time_index(
                        ds, var, &dsfile, &index)) {

                        return(0);
                    }
                }

                if (dsfile && index >= 0) {
                    prev_timeval = dsfile->timevals[index];
                }
            }
        }

        /* Apply the QC time checks */

        if (!dsproc_qc_time_checks(
            var, qc_var, &prev_timeval, 0x1, 0x2, 0x4)) {
            return(0);
        }
    }

    /************************************************************
    * Loop over all variables, applying the QC limit checks and
    * looking for variables that have delta checks defined.
    *************************************************************/

    dc_nvars = 0;

    for (vi = 0; vi < dataset->nvars; ++vi) {

        var = dataset->vars[vi];

        /* Skip the time variables */

        if (cds_is_time_var(var, &is_base_time)) {
            continue;
        }

        /* Check for a companion QC variable */

        if (!(qc_var = dsproc_get_qc_var(var))) {
            continue;
        }

        /* Check if this variable has been excluded from the QC checks */

        if (_dsproc_is_excluded_from_standard_qc_checks(var->name)) {
            continue;
        }

        /* Do the QC limit checks */

        if (!dsproc_qc_limit_checks(var, qc_var, 0x1, 0x2, 0x4)) {
            return(0);
        }

        /* Check for a valid delta attribute */

        found = _dsproc_get_data_att(var, "valid_delta", &att);

        if (found < 0) {
            return(0);
        }

        if (found) {

            /* Check if we need to allocate memory for the delta check lists */

            if (dc_nvars == 0) {

                dc_vars      = (CDSVar **)calloc(dataset->nvars, sizeof(CDSVar *));
                dc_qc_vars   = (CDSVar **)calloc(dataset->nvars, sizeof(CDSVar *));
                dc_var_names = (char **)calloc(dataset->nvars, sizeof(char *));

                if (!dc_vars || !dc_var_names) {

                    ERROR( DSPROC_LIB_NAME,
                        "Could not create list of variables that require delta checks in dataset: %s\n"
                        " -> memory allocation error\n",
                        dataset->name);

                    if (dc_vars)      free(dc_vars);
                    if (dc_var_names) free(dc_var_names);

                    dsproc_set_status(DSPROC_ENOMEM);
                    return(0);
                }
            }

            dc_vars[dc_nvars]          = var;
            dc_qc_vars[dc_nvars]       = qc_var;
            dc_var_names[2*dc_nvars]   = var->name;
            dc_var_names[2*dc_nvars+1] = qc_var->name;

            dc_nvars += 1;
        }
    }

    /************************************************************
    * Check if any delta checks were found
    *************************************************************/

    if (dc_nvars) {

        if (prior_sample_flag) {

            /* Get the previously stored values for all
             * variables that have a delta check */

            if (!dsfile) {

                if (!_dsproc_get_prev_dsfile_time_index(
                    ds, dataset, &dsfile, &index)) {

                    return(0);
                }
            }

            if (dsfile && index >= 0) {
                dc_dataset = _dsproc_fetch_dsfile_dataset(
                    dsfile, (size_t)index, 1,
                    2 * dc_nvars, (const char **)dc_var_names, NULL);
            }
        }

        /* Loop over all variables that need delta checks */

        for (vi = 0; vi < dc_nvars; ++vi) {

            var    = dc_vars[vi];
            qc_var = dc_qc_vars[vi];

            if (dc_dataset) {
                prev_var    = dsproc_get_var(dc_dataset, var->name);
                prev_qc_var = (prev_var) ? dsproc_get_qc_var(prev_var) : NULL;
            }
            else {
                prev_var    = (CDSVar *)NULL;
                prev_qc_var = (CDSVar *)NULL;
            }

            if (!dsproc_qc_delta_checks(
                var,
                qc_var,
                prev_var,
                prev_qc_var,
                0x8,
                bad_flags)) {

                return(0);
            }
        }

        /* Free up the memory allocated for the delta checks */

        if (dc_vars)      free(dc_vars);
        if (dc_qc_vars)   free(dc_qc_vars);
        if (dc_var_names) free(dc_var_names);
        if (dc_dataset)   cds_delete_group(dc_dataset);
    }

    return(1);
}

/**
 *  Perform QC delta checks.
 *
 *  This function uses the following variable attribute to determine the
 *  delta limits:
 *
 *    - valid_delta
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  var         - pointer to the variable
 *  @param  qc_var      - pointer to the QC variable
 *  @param  prev_var    - pointer to the variable from the previous dataset
 *                        if delta checks need to be done on the first sample,
 *                        or NULL to skip the delta checks on the first sample.
 *  @param  prev_qc_var - pointer to the QC variable from the previous dataset
 *                        if delta checks need to be done on the first sample,
 *                        or NULL to skip the delta checks on the first sample.
 *  @param  delta_flag  - QC flag to use for failed delta checks.
 *  @param  bad_flags   - QC flags used to determine bad or missing values
 *                        that should not be used for delta checks.
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int dsproc_qc_delta_checks(
    CDSVar *var,
    CDSVar *qc_var,
    CDSVar *prev_var,
    CDSVar *prev_qc_var,
    int     delta_flag,
    int     bad_flags)
{
    size_t  sample_size;
    int     found;
    CDSAtt *att;
    int     ndeltas;
    void   *deltas_vp;
    int    *delta_flags;
    size_t *dim_lengths;
    void   *prev_sample_vp;
    int    *prev_qc_flags;
    int     free_prev_qc = 0;
    size_t  sample_start;
    int     retval;
    int     di;

    /* Make sure the QC variable has an integer data type */

    if (qc_var->type != CDS_INT) {

        ERROR( DSPROC_LIB_NAME,
            "Invalid data type for QC variable: %s\n",
            cds_get_object_path(qc_var));

        dsproc_set_status(DSPROC_EQCVARTYPE);
        return(0);
    }

    /* Make sure the sample sizes match */

    sample_size = dsproc_var_sample_size(var);
    if (!sample_size) {

        ERROR( DSPROC_LIB_NAME,
            "Found zero length dimension for variable: %s\n",
            cds_get_object_path(var));

        dsproc_set_status(DSPROC_ESAMPLESIZE);
        return(0);
    }

    if (dsproc_var_sample_size(qc_var) != sample_size) {

        ERROR( DSPROC_LIB_NAME,
            "QC variable dimensions do not match variable dimensions:\n"
            " - variable    %s has sample size: %d\n"
            " - qc variable %s has sample size: %d\n",
            cds_get_object_path(var), sample_size,
            cds_get_object_path(qc_var), dsproc_var_sample_size(qc_var));

        dsproc_set_status(DSPROC_EQCVARDIMS);
        return(0);
    }

    /* Check if we need to initialize memory for the QC flags */

    if (qc_var->sample_count < var->sample_count) {

        if (!dsproc_init_var_data(
            qc_var, qc_var->sample_count,
            (var->sample_count - qc_var->sample_count), 0)) {

            return(0);
        }
    }

    /* Check for a valid_delta attribute */

    found = _dsproc_get_data_att(var, "valid_delta", &att);

    if (found  < 0) return(0);
    if (found == 0) return(1);

    ndeltas   = att->length;
    deltas_vp = att->value.vp;

    if (!ndeltas || !deltas_vp) {
        return(1);
    }

    /* Make sure we actually have data in the variable */

    if (var->sample_count == 0) {
        return(1);
    }

    /* Create the array of dimension lengths */

    dim_lengths = (size_t *)NULL;

    if (var->ndims) {

        dim_lengths = (size_t *)malloc(var->ndims * sizeof(size_t));

        if (!dim_lengths) {

            ERROR( DSPROC_LIB_NAME,
                "Could not perform standard QC delta checks\n"
                " -> memory allocation error\n");

            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }

        dim_lengths[0] = var->sample_count;
        for (di = 1; di < var->ndims; di++) {
            dim_lengths[di] = var->dims[di]->length;
        }
    }

    /* Create the array of delta flags */

    delta_flags = (int *)malloc(ndeltas * sizeof(int));

    if (!delta_flags) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform standard QC delta checks\n"
            " -> memory allocation error\n");

        dsproc_set_status(DSPROC_ENOMEM);
        if (dim_lengths) free(dim_lengths);
        return(0);
    }

    for (di = 0; di < ndeltas; di++) {
        delta_flags[di] = delta_flag;
    }

    /* Check if a previous variable was specified */

    prev_sample_vp = (void *)NULL;
    prev_qc_flags  = (void *)NULL;
    free_prev_qc   = 0;

    if (prev_var &&
        dsproc_var_sample_size(prev_var) == sample_size) {

        sample_start = (prev_var->sample_count - 1) * sample_size;

        if (prev_qc_var &&
            prev_qc_var->type == CDS_INT &&
            prev_qc_var->sample_count >= prev_var->sample_count &&
            dsproc_var_sample_size(prev_qc_var) == sample_size) {

            prev_qc_flags = &(prev_qc_var->data.ip[sample_start]);
        }
        else {

            prev_qc_flags = (int *)calloc(sample_size, sizeof(int));

            if (prev_qc_flags) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not perform standard QC delta checks\n"
                    " -> memory allocation error\n");

                dsproc_set_status(DSPROC_ENOMEM);
                free(delta_flags);
                if (dim_lengths) free(dim_lengths);
                return(0);
            }

            free_prev_qc = 1;
        }

        sample_start  *= cds_data_type_size(prev_var->type);
        prev_sample_vp = (void *)(prev_var->data.bp + sample_start);
    }

    /* Do the QC checks */

    retval = 1;

    if (!cds_qc_delta_checks(
        var->type,
        var->ndims,
        dim_lengths,
        var->data.vp,
        ndeltas,
        deltas_vp,
        delta_flags,
        prev_sample_vp,
        prev_qc_flags,
        bad_flags,
        qc_var->data.ip)) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform standard QC delta checks\n"
            " -> memory allocation error\n");

        dsproc_set_status(DSPROC_ENOMEM);
        retval = 0;
    }

    free(delta_flags);
    if (dim_lengths)  free(dim_lengths);
    if (free_prev_qc) free(prev_qc_flags);

    return(retval);
}

/**
 *  Perform QC limit checks.
 *
 *  This function uses the following variable attributes to determine the
 *  missing_value, and min, max limits:
 *
 *    - missing_value, or _FillValue, or default NetCDF fill value
 *    - valid_min
 *    - valid_max
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  var          - pointer to the variable
 *  @param  qc_var       - pointer to the QC variable
 *  @param  missing_flag - QC flag to use for missing values
 *  @param  min_flag     - QC flag to use for values below the minimum
 *  @param  max_flag     - QC flag to use for values above the maximum
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int dsproc_qc_limit_checks(
    CDSVar *var,
    CDSVar *qc_var,
    int     missing_flag,
    int     min_flag,
    int     max_flag)
{
    size_t  sample_size;
    size_t  nvalues;
    int     nmissings;
    void   *missings_vp;
    int    *missing_flags;
    CDSAtt *att;
    void   *min_vp;
    void   *max_vp;
    int     found;
    int     mi;

    /* Make sure the QC variable has integer data type */

    if (qc_var->type != CDS_INT) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform QC limit checks\n"
            " -> invalid data type for QC variable: %s\n",
            cds_get_object_path(qc_var));

        dsproc_set_status(DSPROC_EQCVARTYPE);
        return(0);
    }

    /* Make sure the sample sizes match */

    sample_size = dsproc_var_sample_size(var);
    if (!sample_size) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform QC limit checks\n"
            " -> found zero length dimension for variable: %s\n",
            cds_get_object_path(var));

        dsproc_set_status(DSPROC_ESAMPLESIZE);
        return(0);
    }

    if (dsproc_var_sample_size(qc_var) != sample_size) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform QC limit checks\n"
            " -> QC variable dimensions do not match variable dimensions:\n"
            " -> variable    %s has sample size: %d\n"
            " -> qc variable %s has sample size: %d\n",
            cds_get_object_path(var), sample_size,
            cds_get_object_path(qc_var), dsproc_var_sample_size(qc_var));

        dsproc_set_status(DSPROC_EQCVARDIMS);
        return(0);
    }

    /* Check if we need to initialize memory for the QC flags */

    if (qc_var->sample_count < var->sample_count) {

        if (!dsproc_init_var_data(
            qc_var, qc_var->sample_count,
            (var->sample_count - qc_var->sample_count), 0)) {

            return(0);
        }
    }

    /* Check for a valid_min attribute */

    found = _dsproc_get_data_att(var, "valid_min", &att);

    if (found < 0) {
        return(0);
    }

    min_vp = (found) ? att->value.vp : (void *)NULL;

    /* Check for a valid_max attribute */

    found = _dsproc_get_data_att(var, "valid_max", &att);

    if (found < 0) {
        return(0);
    }

    max_vp = (found) ? att->value.vp : (void *)NULL;

    /* Make sure we actually have data in the variable */

    if (var->sample_count == 0) {
        return(1);
    }

    /* Get the missing values used by this variable */

    missings_vp = (void *)NULL;
    nmissings   = dsproc_get_var_missing_values(var, &missings_vp);

    if (nmissings < 0) {
        return(0);
    }
    else if (nmissings == 0) {
        missing_flags = (int *)NULL;
    }
    else {
        missing_flags = (int *)malloc(nmissings * sizeof(int));

        if (!missing_flags) {

            ERROR( DSPROC_LIB_NAME,
                "Could not perform QC limit checks\n"
                " -> memory allocation error\n");

            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }

        for (mi = 0; mi < nmissings; mi++) {
            missing_flags[mi] = missing_flag;
        }
    }

    if (msngr_debug_level || msngr_provenance_level) {

        char   string_value[128];
        size_t string_length;

        DEBUG_LV2( DSPROC_LIB_NAME,
            " - %s\n",
            var->name);

        if (min_vp) {
            string_length = 128;
            cds_array_to_string(var->type, 1, min_vp, &string_length, string_value);
            DEBUG_LV2( DSPROC_LIB_NAME, "    - min:     %s\n", string_value);
        }

        if (max_vp) {
            string_length = 128;
            cds_array_to_string(var->type, 1, max_vp, &string_length, string_value);
            DEBUG_LV2( DSPROC_LIB_NAME, "    - max:     %s\n", string_value);
        }

        if (missings_vp) {
            string_length = 128;
            cds_array_to_string(var->type, nmissings, missings_vp, &string_length, string_value);
            DEBUG_LV2( DSPROC_LIB_NAME, "    - missing: %s\n", string_value);
        }
    }

    /* Do the QC checks */

    nvalues = var->sample_count * sample_size;

    cds_qc_limit_checks(
            var->type,
            nvalues,
            var->data.vp,
            nmissings,
            missings_vp,
            missing_flags,
            min_vp,
            min_flag,
            max_vp,
            max_flag,
            qc_var->data.ip);

    if (missings_vp)   free(missings_vp);
    if (missing_flags) free(missing_flags);

    return(1);
}

/**
 *  Perform QC time checks.
 *
 *  This function uses the following time variable attributes to determine
 *  the lower and upper delta time limits:
 *
 *     - delta_t_lower_limit
 *     - delta_t_upper_limit
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  time_var       - pointer to the time variable
 *  @param  qc_time_var    - pointer to the qc_time variable
 *  @param  prev_timeval   - the timeval of the previous sample in seconds
 *                           since 1970-1-1 00:00:00, or NULL to skip the
 *                           checks for the first sample time.
 *
 *  @param  lteq_zero_flag - QC flag to use for time deltas <= zero
 *  @param  min_delta_flag - QC flag to use for time deltas below the minimum
 *  @param  max_delta_flag - QC flag to use for time deltas above the maximum
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int dsproc_qc_time_checks(
    CDSVar    *time_var,
    CDSVar    *qc_time_var,
    timeval_t *prev_timeval,
    int        lteq_zero_flag,
    int        min_delta_flag,
    int        max_delta_flag)
{
    CDSAtt     *att;
    size_t      length;
    CDSData     min_delta;
    char        min_delta_buff[16];
    CDSData     max_delta;
    char        max_delta_buff[16];

    time_t      base_time;
    time_t      prev_tv_sec;
    CDSData     prev_offset;
    char        prev_offset_buff[16];

    /* Make sure the QC variable has integer data type */

    if (qc_time_var->type != CDS_INT) {

        ERROR( DSPROC_LIB_NAME,
            "Could not perform QC time checks\n"
            " -> invalid data type for QC time variable: %s\n",
            cds_get_object_path(qc_time_var));

        dsproc_set_status(DSPROC_EQCVARTYPE);
        return(0);
    }

    /* Check if we need to initialize memory for the QC flags */

    if (qc_time_var->sample_count < time_var->sample_count) {

        if (!dsproc_init_var_data(
            qc_time_var, qc_time_var->sample_count,
            (time_var->sample_count - qc_time_var->sample_count), 0)) {

            return(0);
        }
    }

    /* Get the delta_t_lower_limit attribute value */

    att = cds_get_att(qc_time_var, "delta_t_lower_limit");
    if (att) {
        length       = 1;
        min_delta.cp = min_delta_buff;
        cds_get_att_value(att, time_var->type, &length, min_delta.vp);
    }
    else {
        min_delta.vp = (void *)NULL;
    }

    /* Get the delta_t_upper_limit attribute value */

    att = cds_get_att(qc_time_var, "delta_t_upper_limit");
    if (att) {
        length       = 1;
        max_delta.cp = max_delta_buff;
        cds_get_att_value(att, time_var->type, &length, max_delta.vp);
    }
    else {
        max_delta.vp = (void *)NULL;
    }

    /* Make sure we have data in the time variable  */

    if (time_var->sample_count == 0) {
        return(1);
    }

    /* Check if a previous time was specified */

    prev_offset.vp = (void *)NULL;

    if (prev_timeval &&
        prev_timeval->tv_sec > 0) {

        base_time = cds_get_base_time(time_var);
        if (!base_time) {

            ERROR( DSPROC_LIB_NAME,
                "Could not perform QC time checks\n"
                " -> could not get base_time for variable: %s\n",
                cds_get_object_path(time_var));

            dsproc_set_status(DSPROC_EBASETIME);
            return(0);
        }

        prev_tv_sec    = prev_timeval->tv_sec - base_time;
        prev_offset.cp = prev_offset_buff;

        switch (time_var->type) {
            case CDS_DOUBLE: *prev_offset.dp = (double)prev_tv_sec + (double)prev_timeval->tv_usec * 1E-6; break;
            case CDS_FLOAT:  *prev_offset.fp = (float)prev_tv_sec  + (float)prev_timeval->tv_usec * 1E-6;  break;
            case CDS_INT:    *prev_offset.ip = (int)prev_tv_sec;           break;
            case CDS_SHORT:  *prev_offset.sp = (short)prev_tv_sec;         break;
            case CDS_BYTE:   *prev_offset.bp = (signed char)prev_tv_sec;   break;
            case CDS_CHAR:   *prev_offset.cp = (unsigned char)prev_tv_sec; break;
            default:
                break;
        }
    }

    /* Do the QC checks */

    cds_qc_time_offset_checks(
        time_var->type,
        time_var->sample_count,
        time_var->data.vp,
        prev_offset.vp,
        lteq_zero_flag,
        min_delta.vp,
        min_delta_flag,
        max_delta.vp,
        max_delta_flag,
        qc_time_var->data.ip);

    return(1);
}
