/*******************************************************************************
*
*  COPYRIGHT (C) 2014 Battelle Memorial Institute.  All Rights Reserved.
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
*    $Revision: 57302 $
*    $Author: ermold $
*    $Date: 2014-10-06 20:09:37 +0000 (Mon, 06 Oct 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_trans_params.c
 *  Transformation Parameter Functions.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */


/*******************************************************************************
 *  Static Functions Visible Only To This Module
 */

/*******************************************************************************
 *  Private Functions Visible Only To This Library
 */

/**
 *  Private: Set the transformation parameters using the bounds variable.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  dim - pointer to the CDSDim
 *
 *  @return
 *    -  1 if successful
 *    -  0 if a boundary variable does not exist
 *    - -1 if an error occurred
 */
int _dsproc_set_trans_params_from_bounds_var(CDSDim *dim)
{
    CDSGroup   *dataset    = (CDSGroup *)dim->parent;
    const char *dim_name   = dim->name;
    double     *front_edge = (double *)NULL;
    double     *back_edge  = (double *)NULL;
    CDSVar     *coord_var;
    CDSVar     *bounds_var;
    CDSAtt     *att;
    size_t      length;
    CDSData     data;
    double      dbl_val;
    size_t      bi;

    /* Check if there is a coordinate variable for this dimension */

    coord_var = cds_get_var(dataset, dim_name);

    if (!coord_var ||
        coord_var->ndims   != 1 ||
        coord_var->dims[0] != dim) {

        return(0);
    }

    /* Check for a valid bounds variable */

    bounds_var = cds_get_bounds_var(coord_var);
    if (bounds_var) {

        if (bounds_var->ndims   != 2   ||
            bounds_var->dims[0] != dim ||
            bounds_var->dims[1]->length != 2) {

            bounds_var = (CDSVar *)NULL;
        }
    }

    if (bounds_var) {

        /* Set front and back edge using bounds variable */

        length = bounds_var->dims[0]->length;

        if (!(front_edge = malloc(length * sizeof(double))) ||
            !(back_edge  = malloc(length * sizeof(double)))) {

            goto MEMORY_ERROR;
        }

        data.vp = bounds_var->data.vp;

        switch (bounds_var->type) {

            case CDS_BYTE:
                for (bi = 0; bi < length; ++bi) {
                    front_edge[bi] = (double)*data.bp++;
                    back_edge[bi]  = (double)*data.bp++;
                }
                break;

            case CDS_SHORT:
                for (bi = 0; bi < length; ++bi) {
                    front_edge[bi] = (double)*data.sp++;
                    back_edge[bi]  = (double)*data.sp++;
                }
                break;

            case CDS_INT:
                for (bi = 0; bi < length; ++bi) {
                    front_edge[bi] = (double)*data.ip++;
                    back_edge[bi]  = (double)*data.ip++;
                }
                break;

            case CDS_FLOAT:
                for (bi = 0; bi < length; ++bi) {
                    front_edge[bi] = (double)*data.fp++;
                    back_edge[bi]  = (double)*data.fp++;
                }
                break;

            case CDS_DOUBLE:
                for (bi = 0; bi < length; ++bi) {
                    front_edge[bi] = *data.dp++;
                    back_edge[bi]  = *data.dp++;
                }
                break;

            default:

                ERROR( DSPROC_LIB_NAME,
                    "Invalid data type '%d' for bounds variable: %s\n",
                    bounds_var->type, cds_get_object_path(bounds_var));

                dsproc_set_status(DSPROC_EBOUNDSVAR);

                free(front_edge);
                free(back_edge);
                return(-1);
        }

        if (!cds_set_transform_param(dataset, dim_name,
            "front_edge", CDS_DOUBLE, length, front_edge)) {

            goto MEMORY_ERROR;
        }

        if (!cds_set_transform_param(dataset, dim_name,
            "back_edge", CDS_DOUBLE, length, back_edge)) {

            goto MEMORY_ERROR;
        }

        free(front_edge);
        free(back_edge);

        return(1);
    }
    else {

        /* Check for the Conventions attribute.  If this has been
         * defined and specifies the ARM or CF convention we can
         * assume the values are instantaneous. */

        att = cds_get_att(dataset, "Conventions");
        if (att && att->type == CDS_CHAR && att->length && att->value.vp) {

            if (strstr(att->value.cp, "ARM") ||
                strstr(att->value.cp, "CF")) {

                dbl_val = 0.0;

                if (!cds_set_transform_param(dataset, dim_name,
                    "width", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                    goto MEMORY_ERROR;
                }

                dbl_val = 0.5;

                if (!cds_set_transform_param(dataset, dim->name,
                    "alignment", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                    goto MEMORY_ERROR;
                }

                return(1);
            }
        }
    }

    return(0);

MEMORY_ERROR:

    if (front_edge) free(front_edge);
    if (back_edge)  free(back_edge);

    ERROR( DSPROC_LIB_NAME,
        "Could not set transformation parameters for: %s\n"
        " -> memory allocation error\n",
        cds_get_object_path(dim));

    dsproc_set_status(DSPROC_ENOMEM);
    return(-1);
}

/**
 *  Private: Set the transformation parameters using the DSDB.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  dsid - datastream ID
 *  @param  dim  - pointer to the CDSDim
 *
 *  @return
 *    -  1 if successful
 *    -  0 if both width and alignment were not found in the DSDB
 *    - -1 if an error occurred
 */
int _dsproc_set_trans_params_from_dsdb(int dsid, CDSDim *dim)
{
    CDSGroup   *dataset   = (CDSGroup *)dim->parent;
    const char *dim_name  = dim->name;
    timeval_t   data_time = { 0, 0 };
    size_t      length;
    size_t      found_width;
    size_t      found_alignment;
    int         status;
    double      dbl_val;
    const char *str_val;

    /* Get the time of the first sample in the dataset. */

    length = 1;
    if (!dsproc_get_sample_timevals(dataset, 0, &length, &data_time)) {
        if (length != 0) return(-1);
    }

    /* Set the width from the datastream properties table. */

    status = dsproc_get_datastream_property(
        dsid, dim_name, "trans_bin_width", data_time.tv_sec, &str_val);

    if (status  < 0) return(-1);
    if (status == 1) {

        dbl_val = atof(str_val);

        if (!cds_set_transform_param(dataset, dim_name,
            "width", CDS_DOUBLE, 1, (void *)&dbl_val)) {

            goto MEMORY_ERROR;
        }
    }

    found_width = status;

    /* Set the alignment from the datastream properties table. */

    status = dsproc_get_datastream_property(
        dsid, dim_name, "trans_bin_alignment", data_time.tv_sec, &str_val);

    if (status  < 0) return(0);
    if (status == 1) {

        dbl_val = atof(str_val);

        if (!cds_set_transform_param(dataset, dim_name,
            "alignment", CDS_DOUBLE, 1, (void *)&dbl_val)) {

            goto MEMORY_ERROR;
        }

    }

    found_alignment = status;

    if (found_width || found_alignment)
        return(1);

    return(0);

MEMORY_ERROR:

    ERROR( DSPROC_LIB_NAME,
        "Could not set transformation parameters for: %s\n"
        " -> memory allocation error\n",
        cds_get_object_path(dim));

    dsproc_set_status(DSPROC_ENOMEM);
    return(-1);
}

/**
 *  Private: Set the transformation parameters for a retrieved dimension.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  dsid - datastream ID
 *  @param  dim  - pointer to the retrieved dimension
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int _dsproc_set_ret_dim_trans_params(int dsid, CDSDim *dim)
{
    CDSGroup   *dataset      = (CDSGroup *)dim->parent;
    const char *dim_name     = dim->name;
    double     *front_edge   = (double *)NULL;
    double     *back_edge    = (double *)NULL;
    size_t      length;
    size_t      bi;

    double      dbl_val;
    int         status;

    double      width;

    size_t      found_front_edge;
    size_t      found_back_edge;
    size_t      found_width;
    size_t      found_alignment;
    int         found_bounds;

    /* Check for parameter values that have already been set,
     * we do not want to overwrite them if they have. */

    found_front_edge = 1;
    cds_get_transform_param(dim,
        "front_edge", CDS_DOUBLE, &found_front_edge, &dbl_val);

    found_back_edge = 1;
    cds_get_transform_param(dim,
        "back_edge", CDS_DOUBLE, &found_back_edge, &dbl_val);

    found_width = 1;
    cds_get_transform_param(dim,
        "width", CDS_DOUBLE, &found_width, &width);

    found_alignment = 1;
    cds_get_transform_param(dim,
        "alignment", CDS_DOUBLE, &found_alignment, &dbl_val);

    /* Check if a width has been defined. */

    if (found_width) {

        if (found_front_edge && !found_back_edge) {

            /* Compute back_edge from front_edge and width */

            front_edge = cds_get_transform_param(dim,
                "front_edge", CDS_DOUBLE, &length, NULL);

            if (!front_edge) goto MEMORY_ERROR;

            back_edge = (double *)malloc(length * sizeof(double));
            if (!back_edge) goto MEMORY_ERROR;

            for (bi = 0; bi < length; ++bi) {
                back_edge[bi] = front_edge[bi] + width;
            }

            if (!cds_set_transform_param(dataset, dim->name,
                "back_edge", CDS_DOUBLE, length, back_edge)) {

                goto MEMORY_ERROR;
            }

            found_back_edge = 1;
        }
        else if (found_back_edge && !found_front_edge) {

            /* Compute front_edge from back_edge and width */

            back_edge = cds_get_transform_param(dim,
                "back_edge", CDS_DOUBLE, &length, NULL);

            if (!back_edge) goto MEMORY_ERROR;

            front_edge = (double *)malloc(length * sizeof(double));
            if (!front_edge) goto MEMORY_ERROR;

            for (bi = 0; bi < length; ++bi) {
                front_edge[bi] = back_edge[bi] - width;
            }

            if (!cds_set_transform_param(dataset, dim_name,
                "front_edge", CDS_DOUBLE, length, front_edge)) {

                goto MEMORY_ERROR;
            }

            found_front_edge = 1;
        }
        else if (!found_alignment) {

            /* Assume center bin alignment */

            dbl_val = 0.5;

            if (!cds_set_transform_param(dataset, dim_name,
                "alignment", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                goto MEMORY_ERROR;
            }

            found_alignment = 1;
        }
    }

    /* If front_edge/back_edge or width/alignment information hasn't
     * been defined we need to check for a boundary variable, or
     * if the Conventions attribute specifies ARM or CF. */

    if ((!found_front_edge || !found_back_edge) &&
        (!found_width || !found_alignment)) {

        found_bounds = _dsproc_set_trans_params_from_bounds_var(dim);
        if (found_bounds < 0) return(0);

        /* If the bounds variable was not found we need to get the
         * information from the datastream properties table in the database. */

        if (!found_bounds) {
            status = _dsproc_set_trans_params_from_dsdb(dsid, dim);
            if (status < 0) return(0);
        }
    }

    return(1);

MEMORY_ERROR:

    ERROR( DSPROC_LIB_NAME,
        "Could not set input transformation parameters for: %s\n"
        " -> memory allocation error\n",
        cds_get_object_path(dim));

    dsproc_set_status(DSPROC_ENOMEM);
    return(0);
}

/**
 *  Private: Set the transform parameters for a retrieved coordinate variable.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  dsid - datastream ID
 *  @param  obs  - pointer to the retrieved observation
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int _dsproc_set_ret_obs_params(int dsid, CDSGroup *obs)
{
    int di;

    for (di = 0; di < obs->ndims; ++di) {

        if (!_dsproc_set_ret_dim_trans_params(dsid, obs->dims[di])) {
            return(0);
        }
    }

    return(1);
}

/**
 *  Private: Set Transform paramters for a trans_coord_var.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param trans_coord_var - pointer to the trans_coord_var
 *  @param ret_dsid        - ID of the retrieved datastream
 *                           or -1 if this isn't a direct mapping
 *  @param ret_coord_dim   - pointer to the RetCoordDim structure
 *
 *  @return
 *    - 1 if successful
 *    - 0 if an error occurred
 */
int _dsproc_set_trans_coord_var_params(
    CDSVar      *trans_coord_var,
    int          ret_dsid,
    RetCoordDim *ret_coord_dim)
{
    CDSGroup   *trans_group     = (CDSGroup *)trans_coord_var->parent;
    CDSDim     *trans_coord_dim = (CDSDim *)NULL;
    size_t      found_width     = 0;
    size_t      found_alignment = 0;
    int         found_bounds    = 0;
    const char *dim_name;
    size_t      length;
    double      dbl_val;
    int         int_val;
    int         status;

    if (trans_coord_var->ndims == 1) {
        trans_coord_dim = trans_coord_var->dims[0];
        dim_name = trans_coord_dim->name;
    }
    else {
        dim_name = trans_coord_var->name;
    }

    found_width = 1;
    cds_get_transform_param(trans_coord_dim,
        "width", CDS_DOUBLE, &found_width, &dbl_val);

    found_alignment = 1;
    cds_get_transform_param(trans_coord_dim,
        "alignment", CDS_DOUBLE, &found_alignment, &dbl_val);

    if (ret_coord_dim) {

        /* Set transformation type */

        if (ret_coord_dim->trans_type) {

            length = strlen(ret_coord_dim->trans_type) + 1;

            if (!cds_set_transform_param(trans_group, dim_name,
                "transform", CDS_CHAR, length, (void *)ret_coord_dim->trans_type)) {

                goto MEMORY_ERROR;
            }
        }

        /* Set range */

        if (ret_coord_dim->trans_range) {

            dbl_val = atof(ret_coord_dim->trans_range);

            if (!cds_set_transform_param(trans_group, dim_name,
                "range", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                goto MEMORY_ERROR;
            }
        }

        /* Set width */

        if (ret_coord_dim->interval) {

            dbl_val = atof(ret_coord_dim->interval);

            if (!cds_set_transform_param(trans_group, dim_name,
                "interval", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                goto MEMORY_ERROR;
            }

            if (!found_width) {

                if (!cds_set_transform_param(trans_group, dim_name,
                    "width", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                    goto MEMORY_ERROR;
                }

                found_width = 1;
            }
        }

        /* Set alignment */

        if (ret_coord_dim->trans_align) {

            int_val = atoi(ret_coord_dim->trans_align);

            if      (int_val == -1) dbl_val = 0.0;
            else if (int_val ==  0) dbl_val = 0.5;
            else                    dbl_val = 1.0;

            if (!cds_set_transform_param(trans_group, dim_name,
                "alignment", CDS_DOUBLE, 1, (void *)&dbl_val)) {

                goto MEMORY_ERROR;
            }

            found_alignment = 1;
        }
    }

    if (trans_coord_dim) {

        /* If width/alignment information hasn't been defined we need to check
         * for a boundary variable, or if the Conventions attribute specifies
         * ARM or CF. */

        if (!found_width || !found_alignment) {

            found_bounds = _dsproc_set_trans_params_from_bounds_var(
                trans_coord_dim);

            if (found_bounds < 0) return(0);

            /* If a boundary variable was not found we need to get the
             * information from the datastream properties table in the database. */

            if (!found_bounds && ret_dsid >= 0) {

                status = _dsproc_set_trans_params_from_dsdb(
                    ret_dsid, trans_coord_dim);

                if (status < 0) return(0);
            }
        }
    }

    /* Print the transform parameters if we are in debug mode */

    if (msngr_debug_level) {

        fprintf(stdout,
        "\n"
        "--------------------------------------------------------------------\n"
        "Transformation Parameters for: %s->%s\n"
        "\n", trans_group->name, trans_coord_var->name);

        cds_print_transform_params(stdout, "    ",
            trans_group, trans_coord_var->name);

        fprintf(stdout,
        "\n"
        "--------------------------------------------------------------------\n"
        "\n");
    }

    return(1);

MEMORY_ERROR:

    ERROR( DSPROC_LIB_NAME,
        "Could not set transformation parameters for: %s->%s\n"
        " -> memory allocation error\n",
        trans_group->name, trans_coord_var->name);

    dsproc_set_status(DSPROC_ENOMEM);
    return(0);
}

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/**
 *  Internal: Load a transformation parameters file.
 *
 *  This function will look for a transformation parameters file in the
 *  following directories in the order specified:
 *
 *      "dsenv_get_data_conf_root()/transform/<proc_name>"
 *      "dsenv_get_apps_conf_root()/transform"
 *
 *  The first file found in the following search order will be loaded:
 *
 *    - {site}{name}{facility}.{level}
 *    - {site}{name}.{level}
 *    - {name}.{level}
 *    - {name}
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  group    - pointer to the CDSGroup to store the transformation
 *                     parameters in
 *  @param  site     - site name or
 *                     NULL to skip search patterns containing the site name.
 *  @param  facility - facility name or
 *                     NULL to skip search patterns containing the facility name.
 *  @param  name     - base name of the file to look for
 *  @param  level    - data level or
 *                     NULL to skip search patterns containing the data level.
 *
 *  @return
 *    -  1 if successful
 *    -  0 no transformation parameters file found
 *    - -1 if an error occurred
 */
int dsproc_load_transform_params_file(
    CDSGroup   *group,
    const char *site,
    const char *facility,
    const char *name,
    const char *level)
{
    char *conf_root;
    char  conf_path[PATH_MAX];
    char  file_name[PATH_MAX];
    int   dir_index;
    int   file_index;
    int   check;
    int   status;

    status = 0;

    for (dir_index = 0; dir_index < 2; ++dir_index) {

        /* Get the full path to the transformation params files. */

        switch (dir_index) {

            case 0:

                status = dsenv_get_data_conf_root(&conf_root);

                if (status < 0) {
                    dsproc_set_status(DSPROC_ENOMEM);
                    return(-1);
                }

                if (status == 0) {
                    continue;
                }

                snprintf(conf_path, PATH_MAX, "%s/transform/%s",
                    conf_root, _DSProc->name);

                break;

            case 1:

                status = dsenv_get_apps_conf_root(
                    _DSProc->name, _DSProc->type, &conf_root);

                if (status < 0) {
                    dsproc_set_status(DSPROC_ENOMEM);
                    return(-1);
                }

                if (status == 0) {
                    continue;
                }

                snprintf(conf_path, PATH_MAX, "%s/transform",
                    conf_root);

                break;
        }

        free(conf_root);
        status = 0;

        DEBUG_LV1( DSPROC_LIB_NAME,
            "Checking for transformation parameters file in: %s\n", conf_path);

        if (access(conf_path, F_OK) != 0) {

            if (errno != ENOENT) {

                ERROR( DSPROC_LIB_NAME,
                    "Could not access directory: %s\n"
                    " -> %s\n", conf_path, strerror(errno));

                dsproc_set_status(DSPROC_EACCESS);
                return(-1);
            }

            DEBUG_LV1( DSPROC_LIB_NAME,
                " - directory not found\n");

            continue;
        }

        /* Search for the transform parameter file */

        for (file_index = 0; file_index < 4; file_index++) {

            check  = 0;

            switch (file_index) {

                case 0:
                    /* <site><name><facility>.<level> */

                    if (site && facility && name && level) {

                        snprintf(file_name, PATH_MAX, "%s%s%s.%s",
                            site, name, facility, level);

                        check = 1;
                    }
                    break;
                case 1:
                    /* <site><name>.<level> */

                    if (site && name && level) {
                        snprintf(file_name, PATH_MAX, "%s%s.%s", site, name, level);
                        check = 1;
                    }
                    break;
                case 2:
                    /* <name>.<level> */

                    if (name && level) {
                        snprintf(file_name, PATH_MAX, "%s.%s", name, level);
                        check = 1;
                    }
                    break;
                case 3:
                    /* <name> */

                    if (name) {
                        snprintf(file_name, PATH_MAX, "%s", name);
                        check = 1;
                    }
                    break;
            }

            if (check) {

                DEBUG_LV1( DSPROC_LIB_NAME,
                    " - checking for: %s\n", file_name);

                status = cds_load_transform_params_file(
                    group, conf_path, file_name);
            }

            if (status != 0) {
                break;
            }
        }

        if (status != 0) {
            break;
        }
    }

    /* Cleanup and return */

    if (status < 0) {
        dsproc_set_status(DSPROC_ETRANSPARAMLOAD);
        return(-1);
    }

    if (status == 0) {
        return(0);
    }

    /* Print the transform parameters if we are in debug mode */

    if (msngr_debug_level) {

        fprintf(stdout,
        "\n"
        "--------------------------------------------------------------------\n"
        "Loaded Transformation Parameters File: %s\n"
        "\n", file_name);

        cds_print_transform_params(stdout, "    ", group, NULL);

        fprintf(stdout,
        "\n"
        "--------------------------------------------------------------------\n"
        "\n");
    }

    return(1);
}
