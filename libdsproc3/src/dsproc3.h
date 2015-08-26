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
*    $Revision: 63473 $
*    $Author: ermold $
*    $Date: 2015-08-26 20:44:23 +0000 (Wed, 26 Aug 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc3.h
 *  Header file for libdsproc3
 */

#ifndef _DSPROC3_H
#define _DSPROC3_H

#include "armutils.h"
#include "cds3.h"

/******************************************************************************/
/**
 *  @defgroup DSPROC_MAIN Process Main
 */
/*@{*/

/** Flag specifying that the data retrieval process should be run. */
#define DSP_RETRIEVER           0x001

/** Flag specifying if a retriever definition is required. */
#define DSP_RETRIEVER_REQUIRED  0x002

/** Flag specifying that the data transformation process should be run. */
#define DSP_TRANSFORM           0x004

/** Flag specifying that this is an ingest process. */
#define DSP_INGEST              0x100

/**
 *  Process Models
 */
typedef enum {

    /** Generic VAP process.
     *
     *  The retriever definition will be used if it exists in the database
     *  but it is not required for the process to run. This will also run the
     *  transform logic for any variables that are found in the retrieved
     *  data that have been tagged with a coordinate system name.
     */
    PM_GENERIC          = DSP_RETRIEVER | DSP_TRANSFORM,

    /** Retriever only VAP.
     *
     *  This VAP requires a retriever definition to be specified in the
     *  database, but will bypass the transformation logic.
     */
    PM_RETRIEVER_VAP    = DSP_RETRIEVER | DSP_RETRIEVER_REQUIRED,

    /** Transformation VAP.
     *
     *  This VAP requires a retriever definition to be specified in the
     *  database, and will run the transformation logic.
     */
    PM_TRANSFORM_VAP    = DSP_RETRIEVER | DSP_RETRIEVER_REQUIRED | DSP_TRANSFORM,

    /** Ingest Process.
     *
     *  This is an Ingest process that loops over all raw files in the
     *  input datastream directory.
     */
    PM_INGEST          = DSP_INGEST,

    /** Ingest/VAP Hybrid Process that bypasses the transform logic.
     *
     *  This is an Ingest process that uses the PM_RETRIEVER_VAP processing
     *  model, but is designed to run in real-time like an ingest without the
     *  need for the '-b begin_time' command line argument.  The standard
     *  VAP -b/-e command line options can still be used for reprocessing.
     */
    PM_RETRIEVER_INGEST = DSP_INGEST | DSP_RETRIEVER | DSP_RETRIEVER_REQUIRED,

    /** Ingest/VAP Hybrid Process that bypasses the transform logic.
     *
     *  This is an Ingest process that uses the PM_TRANSFORM_VAP processing
     *  model, but is designed to run in real-time like an ingest without the
     *  need for the '-b begin_time' command line argument.  The standard
     *  VAP -b/-e command line options can still be used for reprocessing.
     */
    PM_TRANSFORM_INGEST = DSP_INGEST | DSP_RETRIEVER | DSP_RETRIEVER_REQUIRED | DSP_TRANSFORM

} ProcModel;

void dsproc_use_nc_extension(void);

int dsproc_main(
        int          argc,
        char       **argv,
        ProcModel    proc_model,
        const char  *proc_version,
        int          nproc_names,
        const char **proc_names);

void dsproc_set_init_process_hook(
    void *(*_init_process_hook)(void));

void dsproc_set_finish_process_hook(
    void (*finish_process_hook)(void *user_data));

void dsproc_set_process_data_hook(
    int (*process_data)(
        void     *user_data,
        time_t    begin_date,
        time_t    end_date,
        CDSGroup *input_data));

void dsproc_set_pre_retrieval_hook(
    int (*pre_retrieval_hook)(
        void     *user_data,
        time_t    begin_date,
        time_t    end_date));

void dsproc_set_post_retrieval_hook(
    int (*post_retrieval_hook)(
        void     *user_data,
        time_t    begin_date,
        time_t    end_date,
        CDSGroup *ret_data));

void dsproc_set_pre_transform_hook(
    int (*pre_transform_hook)(
        void     *user_data,
        time_t    begin_date,
        time_t    end_date,
        CDSGroup *ret_data));

void dsproc_set_post_transform_hook(
    int (*post_transform_hook)(
        void     *user_data,
        time_t    begin_date,
        time_t    end_date,
        CDSGroup *trans_data));

void dsproc_set_process_file_hook(
    int (*process_file_hook)(
        void       *user_data,
        const char *input_dir,
        const char *file_name));

void dsproc_set_custom_qc_hook(
    int (*custom_qc_hook)(
        void       *user_data,
        int         ds_id,
        CDSGroup   *dataset));

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_MESSAGES Process Messages
 */
/*@{*/

/**
 *  Convenience macro for the dsproc_error() function.
 *
 *  Usage: DSPROC_ERROR(const char *status, const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_ERROR(status, ...) \
dsproc_error(__func__, __FILE__, __LINE__, status, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_warning() function.
 *
 *  Usage: DSPROC_WARNING(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_WARNING(...) \
dsproc_warning(__func__, __FILE__, __LINE__, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_log() function.
 *
 *  Usage: DSPROC_LOG(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_LOG(...) \
dsproc_log(__func__, __FILE__, __LINE__, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_bad_file_warning() function.
 *
 *  Usage: DSPROC_BAD_FILE_WARNING(const char *file_name, const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_BAD_FILE_WARNING(file_name, ...) \
    dsproc_bad_file_warning(__func__, __FILE__, __LINE__, file_name, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_bad_line_warning() function.
 *
 *  Usage: DSPROC_BAD_LINE_WARNING(const char *file_name, int line_num, const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_BAD_LINE_WARNING(file_name, line_num, ...) \
    dsproc_bad_line_warning(__func__, __FILE__, __LINE__, file_name, line_num, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_bad_record_warning() function.
 *
 *  Usage: DSPROC_BAD_RECORD_WARNING(const char *file_name, int rec_num, const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_BAD_RECORD_WARNING(file_name, rec_num, ...) \
    dsproc_bad_record_warning(__func__, __FILE__, __LINE__, file_name, rec_num, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_mentor_mail() function.
 *
 *  Usage: DSPROC_MENTOR_MAIL(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_MENTOR_MAIL(...) \
dsproc_mentor_mail(__func__, __FILE__, __LINE__, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_debug() function: level == 1.
 *
 *  Usage: DSPROC_DEBUG_LV1(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_DEBUG_LV1(...) \
if (msngr_debug_level || msngr_provenance_level) \
    dsproc_debug(__func__, __FILE__, __LINE__, 1, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_debug() function: level == 2.
 *
 *  Usage: DSPROC_DEBUG_LV2(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_DEBUG_LV2(...) \
if (msngr_debug_level || msngr_provenance_level) \
    dsproc_debug(__func__, __FILE__, __LINE__, 2, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_debug() function: level == 3.
 *
 *  Usage: DSPROC_DEBUG_LV3(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_DEBUG_LV3(...) \
if (msngr_debug_level || msngr_provenance_level) \
    dsproc_debug(__func__, __FILE__, __LINE__, 3, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_debug() function: level == 4.
 *
 *  Usage: DSPROC_DEBUG_LV4(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_DEBUG_LV4(...) \
if (msngr_debug_level || msngr_provenance_level) \
    dsproc_debug(__func__, __FILE__, __LINE__, 4, __VA_ARGS__)

/**
 *  Convenience macro for the dsproc_debug() function: level == 5.
 *
 *  Usage: DSPROC_DEBUG_LV5(const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_DEBUG_LV5(...) \
if (msngr_debug_level || msngr_provenance_level) \
    dsproc_debug(__func__, __FILE__, __LINE__, 5, __VA_ARGS__)

int dsproc_get_debug_level(void);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_INFO Process Information
 */
/*@{*/

const char *dsproc_get_site(void);
const char *dsproc_get_facility(void);
const char *dsproc_get_name(void);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_CONTROL Process Control
 */
/*@{*/

/**
 *  Convenience macro for the dsproc_abort() function.
 *
 *  Usage: DSPROC_ABORT(const char *status, const char *format, ...)
 *
 *  See printf for a complete description of the format string.
 */
#define DSPROC_ABORT(status, ...) \
dsproc_abort(__func__, __FILE__, __LINE__, status, __VA_ARGS__)

/**
 *  Log File Intervals.
 */
typedef enum {

    LOG_MONTHLY = 0, /**< create monthly log files    */
    LOG_DAILY   = 1, /**< create daily log files      */
    LOG_RUN     = 2, /**< create one log file per run */

} LogInterval;

void dsproc_set_log_interval(LogInterval interval, int use_begin_time);
void dsproc_set_processing_interval_offset(time_t offset);
void dsproc_set_trans_qc_rollup_flag(int flag);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASTREAM DataStreams
 */
/*@{*/

/**
 *  DataStream Roles
 */
typedef enum {

    DSR_INPUT  =  1, /**< input datastream  */
    DSR_OUTPUT =  2  /**< output datastream */

} DSRole;

int     dsproc_get_datastream_id(
            const char *site,
            const char *facility,
            const char *dsc_name,
            const char *dsc_level,
            DSRole      role);

int     dsproc_get_input_datastream_id(
            const char *dsc_name,
            const char *dsc_level);

int     dsproc_get_input_datastream_ids(int **ids);

int     dsproc_get_output_datastream_id(
            const char *dsc_name,
            const char *dsc_level);

int     dsproc_get_output_datastream_ids(int **ids);

const char *dsproc_datastream_name(int ds_id);
const char *dsproc_datastream_site(int ds_id);
const char *dsproc_datastream_facility(int ds_id);
const char *dsproc_datastream_class_name(int ds_id);
const char *dsproc_datastream_class_level(int ds_id);
const char *dsproc_datastream_path(int ds_id);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASTREAM_FILES DataStream Files
 */
/*@{*/

/**
 *  Output File Splitting Mode.
 */
typedef enum {

    SPLIT_ON_STORE  = 0, /**< Always create a new file when data is stored.   */
    SPLIT_ON_HOURS  = 1, /**< Split start is the hour of the day for the first
                              split [0-23], and split interval is in hours.   */
    SPLIT_ON_DAYS   = 2, /**< Split start is the day of the month for the first
                              split [1-31], and split interval is in days.    */
    SPLIT_ON_MONTHS = 3  /**< Split start is the month of the year for the first
                              split [1-12], and split interval is in months.  */
} SplitMode;

int     dsproc_add_datastream_file_patterns(
            int          ds_id,
            int          npatterns,
            const char **patterns,
            int          ignore_case);

void    dsproc_set_datastream_file_extension(
            int         ds_id,
            const char *extension);

void    dsproc_set_datastream_split_mode(
            int       ds_id,
            SplitMode split_mode,
            double    split_start,
            double    split_interval);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASET Datasets
 */
/*@{*/

const char *dsproc_dataset_name(CDSGroup *dataset);

const char *dsproc_get_dataset_version(
                CDSGroup *dataset,
                int      *major,
                int      *minor,
                int      *micro);

CDSGroup   *dsproc_get_output_dataset(
                int ds_id,
                int obs_index);

CDSGroup   *dsproc_get_retrieved_dataset(
                int ds_id,
                int obs_index);

CDSGroup   *dsproc_get_transformed_dataset(
                const char *coordsys_name,
                int         ds_id,
                int         obs_index);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASET_DIMS Dataset Dimensions
 */
/*@{*/

CDSDim *dsproc_get_dim(
            CDSGroup   *dataset,
            const char *name);

size_t  dsproc_get_dim_length(
            CDSGroup   *dataset,
            const char *name);

int     dsproc_set_dim_length(
            CDSGroup    *dataset,
            const char  *name,
            size_t       length);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASET_ATTS Dataset Attributes
 */
/*@{*/

int     dsproc_change_att(
            void        *parent,
            int          overwrite,
            const char  *name,
            CDSDataType  type,
            size_t       length,
            void        *value);

CDSAtt *dsproc_get_att(
            void        *parent,
            const char  *name);

char   *dsproc_get_att_text(
            void       *parent,
            const char *name,
            size_t     *length,
            char       *value);

void   *dsproc_get_att_value(
            void        *parent,
            const char  *name,
            CDSDataType  type,
            size_t      *length,
            void        *value);

int     dsproc_set_att(
            void        *parent,
            int          overwrite,
            const char  *name,
            CDSDataType  type,
            size_t       length,
            void        *value);

int     dsproc_set_att_text(
            void        *parent,
            const char  *name,
            const char *format, ...);

int     dsproc_set_att_value(
            void        *parent,
            const char  *name,
            CDSDataType  type,
            size_t       length,
            void        *value);

int     dsproc_set_att_value_if_null(
            void        *parent,
            const char  *name,
            CDSDataType  type,
            size_t       length,
            void        *value);

int     dsproc_set_att_text_if_null(
            void        *parent,
            const char  *name,
            const char *format, ...);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DATASET_VARS Dataset Variables
 */
/*@{*/

CDSVar *dsproc_clone_var(
            CDSVar       *src_var,
            CDSGroup     *dataset,
            const char   *var_name,
            CDSDataType   data_type,
            const char  **dim_names,
            int           copy_data);

CDSVar *dsproc_define_var(
            CDSGroup    *dataset,
            const char  *name,
            CDSDataType  type,
            int          ndims,
            const char **dim_names,
            const char  *long_name,
            const char  *standard_name,
            const char  *units,
            void        *valid_min,
            void        *valid_max,
            void        *missing_value,
            void        *fill_value);

int     dsproc_delete_var(
            CDSVar *var);

CDSVar *dsproc_get_bounds_var(CDSVar *coord_var);

CDSVar *dsproc_get_coord_var(
            CDSVar *var,
            int     dim_index);

int     dsproc_get_dataset_vars(
            CDSGroup     *dataset,
            const char  **var_names,
            int           required,
            CDSVar     ***vars,
            CDSVar     ***qc_vars,
            CDSVar     ***aqc_vars);

CDSVar *dsproc_get_metric_var(
            CDSVar     *var,
            const char *metric);

CDSVar *dsproc_get_output_var(
            int         ds_id,
            const char *var_name,
            int         obs_index);

CDSVar *dsproc_get_qc_var(
            CDSVar *var);

CDSVar *dsproc_get_retrieved_var(
            const char *var_name,
            int         obs_index);

CDSVar *dsproc_get_transformed_var(
            const char *var_name,
            int         obs_index);

CDSVar *dsproc_get_trans_coordsys_var(
            const char *coordsys_name,
            const char *var_name,
            int         obs_index);

CDSVar *dsproc_get_var(
            CDSGroup   *dataset,
            const char *name);

const char *dsproc_var_name(CDSVar *var);
size_t      dsproc_var_sample_count(CDSVar *var);
size_t      dsproc_var_sample_size(CDSVar *var);

/*

Need to add:

int         dsproc_var_is_unlimited(CDSVar *var);
CDSGroup   *dsproc_var_parent(CDSVar *var);
int         dsproc_var_shape(CDSVar *var, size_t **lengths);

*/

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_VAR_DATA Dataset Variable Data
 */
/*@{*/

void   *dsproc_alloc_var_data(
            CDSVar *var,
            size_t  sample_start,
            size_t  sample_count);

void   *dsproc_alloc_var_data_index(
            CDSVar *var,
            size_t  sample_start,
            size_t  sample_count);

void   *dsproc_get_var_data_index(
            CDSVar *var);

void   *dsproc_get_var_data(
            CDSVar       *var,
            CDSDataType   type,
            size_t        sample_start,
            size_t       *sample_count,
            void         *missing_value,
            void         *data);

int     dsproc_get_var_missing_values(
            CDSVar  *var,
            void   **values);

void   *dsproc_init_var_data(
            CDSVar *var,
            size_t  sample_start,
            size_t  sample_count,
            int     use_missing);

void   *dsproc_init_var_data_index(
            CDSVar *var,
            size_t  sample_start,
            size_t  sample_count,
            int     use_missing);

int     dsproc_set_bounds_data(
            CDSGroup *dataset,
            size_t    sample_start,
            size_t    sample_count);

int     dsproc_set_bounds_var_data(
            CDSVar *coord_var,
            size_t  sample_start,
            size_t  sample_count);

void   *dsproc_set_var_data(
            CDSVar       *var,
            CDSDataType   type,
            size_t        sample_start,
            size_t        sample_count,
            void         *missing_value,
            void         *data);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_TIME_DATA Dataset Time Data
 */
/*@{*/

time_t  dsproc_get_base_time(void *cds_object);

size_t  dsproc_get_time_range(
            void      *cds_object,
            timeval_t *start_time,
            timeval_t *end_time);

CDSVar *dsproc_get_time_var(void *cds_object);

time_t *dsproc_get_sample_times(
            void   *cds_object,
            size_t  sample_start,
            size_t *sample_count,
            time_t *sample_times);

timeval_t *dsproc_get_sample_timevals(
            void      *cds_object,
            size_t     sample_start,
            size_t    *sample_count,
            timeval_t *sample_times);

int     dsproc_set_base_time(
            void       *cds_object,
            const char *long_name,
            time_t      base_time);

int     dsproc_set_sample_times(
            void     *cds_object,
            size_t    sample_start,
            size_t    sample_count,
            time_t   *sample_times);

int     dsproc_set_sample_timevals(
            void      *cds_object,
            size_t     sample_start,
            size_t     sample_count,
            timeval_t *sample_times);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_QC_UTILS QC Utilities
 */
/*@{*/

int dsproc_consolidate_var_qc(
            CDSVar       *in_qc_var,
            unsigned int  bad_mask,
            CDSVar       *out_qc_var,
            unsigned int  bad_flag,
            unsigned int  ind_flag,
            int           reset);

unsigned int dsproc_get_bad_qc_mask(CDSVar *qc_var);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_VARTAGS Variable Tags
 */
/*@{*/

/** Flag instructing the transform logic to ignore this variable. */
#define VAR_SKIP_TRANSFORM   0x1

/** Consolidate the transformation QC bits when mapped to the output dataset. */
#define VAR_ROLLUP_TRANS_QC  0x2

/**
 *  Output Variable Target.
 */
typedef struct {

    int         ds_id;      /**< output datastream ID */
    const char *var_name;   /**< output variable name */

} VarTarget;

int     dsproc_add_var_output_target(
            CDSVar     *var,
            int         ds_id,
            const char *var_name);

int     dsproc_copy_var_tag(
            CDSVar *src_var,
            CDSVar *dest_var);

void    dsproc_delete_var_tag(
            CDSVar *var);

const char *dsproc_get_source_var_name(CDSVar *var);
const char *dsproc_get_source_ds_name(CDSVar *var);
int         dsproc_get_source_ds_id(CDSVar *var);

const char *dsproc_get_var_coordsys_name(CDSVar *var);

int     dsproc_get_var_output_targets(
            CDSVar      *var,
            VarTarget ***targets);

int     dsproc_set_var_coordsys_name(
            CDSVar     *var,
            const char *coordsys_name);

int     dsproc_set_var_flags(CDSVar *var, int flags);

int     dsproc_set_var_output_target(
            CDSVar     *var,
            int         ds_id,
            const char *var_name);

void    dsproc_unset_var_flags(CDSVar *var, int flags);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DQRS Variable DQRs
 */
/*@{*/

/**
 *  Variable DQR.
 *
 *  At the time of this writing the 'code => color => code_desc' values were:
 *
 *    - -1 => None        => Presumed not to exist
 *    -  0 => Black       => Missing
 *    -  1 => White       => Not inspected
 *    -  2 => Green       => Good
 *    -  3 => Yellow      => Suspect
 *    -  4 => Red         => Incorrect
 *    -  5 => Transparent => Does not affect quality
 */
typedef struct {
    const char  *id;          /**< DQR ID                           */
    const char  *desc;        /**< description                      */
    const char  *ds_name;     /**< datastream name                  */
    const char  *var_name;    /**< variable name                    */
    int          code;        /**< code number                      */
    const char  *color;       /**< code color                       */
    const char  *code_desc;   /**< code description                 */
    time_t       start_time;  /**< start time in seconds since 1970 */
    time_t       end_time;    /**< end time in seconds since 1970   */
    size_t       start_index; /**< start time index in dataset      */
    size_t       end_index;   /**< end time index in dataset        */
} VarDQR;

int dsproc_get_var_dqrs(CDSVar *var, VarDQR ***dqrs);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_Print Print Functions
 */
/*@{*/

int dsproc_dump_dataset(
        CDSGroup   *dataset,
        const char *outdir,
        const char *prefix,
        time_t      file_time,
        const char *suffix,
        int         flags);

int dsproc_dump_output_datasets(
        const char *outdir,
        const char *suffix,
        int         flags);

int dsproc_dump_retrieved_datasets(
        const char *outdir,
        const char *suffix,
        int         flags);

int dsproc_dump_transformed_datasets(
        const char *outdir,
        const char *suffix,
        int         flags);

/*
Need functions:

void      dsproc_print_dataset_object(
            const char *file_name,
            void       *object);

*/

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_STATUS Process Status Definitions
 */
/*@{*/

/** Successful */
#define DSPROC_SUCCESS       "Successful"

/** Memory Allocation Error */
#define DSPROC_ENOMEM        "Memory Allocation Error"

/** Could Not Create Fork For New Process */
#define DSPROC_EFORK         "Could Not Create Fork For New Process"

/** No Input Data Found */
#define DSPROC_ENODATA       "No Input Data Found"

/** No Output Data Found */
#define DSPROC_ENOOUTDATA    "No Output Data Created"

/** Could Not Initialize Signal Handlers */
#define DSPROC_EINITSIGS     "Could Not Initialize Signal Handlers"

/** Maximum Run Time Limit Exceeded */
#define DSPROC_ERUNTIME      "Maximum Run Time Limit Exceeded"

/** Could Not Force Process To Continue */
#define DSPROC_EFORCE        "Could Not Force Process To Continue"

/** Could Not Determine Path To Datastream */
#define DSPROC_EDSPATH       "Could Not Determine Path To Datastream"

/** Could Not Determine Path To Logs Directory */
#define DSPROC_ELOGSPATH     "Could Not Determine Path To Logs Directory"

/** Could Not Open Log File */
#define DSPROC_EACCESS       "Could Not Access File or Directory"

/** Could Not Open Log File */
#define DSPROC_ELOGOPEN      "Could Not Open Log File"

/** Could Not Open Provenance Log */
#define DSPROC_EPROVOPEN     "Could Not Open Provenance Log"

/** Could Not Initialize Mail */
#define DSPROC_EMAILINIT     "Could Not Initialize Mail"

/** Database Error (see log file) */
#define DSPROC_EDBERROR      "Database Error (see log file)"

/** Database Connection Error */
#define DSPROC_EDBCONNECT    "Database Connection Error"

/** DQR Database Error (see log file) */
#define DSPROC_EDQRDBERROR   "DQR Database Error (see log file)"

/** DQR Database Connection Error */
#define DSPROC_EDQRDBCONNECT "DQR Database Connection Error"

/** Directory List Error */
#define DSPROC_EDIRLIST      "Could Not Get Directory Listing"

/** Directory List Error */
#define DSPROC_EREGEX        "Regular Expression Error"

/** Invalid Input Datastream Class */
#define DSPROC_EBADINDSC     "Invalid Input Datastream Class"

/** Could Not Find Input Datastream Class In Database */
#define DSPROC_ENOINDSC      "Could Not Find Input Datastream Class In Database"

/** Too Many Input Datastream Classes In Database */
#define DSPROC_ETOOMANYINDSC "Too Many Input Datastreams Defined In Database"

/** Invalid Output Datastream Class */
#define DSPROC_EBADOUTDSC    "Invalid Output Datastream Class"

/** Invalid Datastream ID */
#define DSPROC_EBADDSID      "Invalid Datastream ID"

/** Found Data Time Before Minimum Valid Time */
#define DSPROC_EMINTIME      "Found Data Time Before Minimum Valid Time"

/** Found Data Time In The Future */
#define DSPROC_EFUTURETIME   "Found Data Time In The Future"

/** Invalid Time Order */
#define DSPROC_ETIMEORDER    "Invalid Time Order"

/** Found Overlapping Data Times */
#define DSPROC_ETIMEOVERLAP  "Found Overlapping Data Times"

/** Invalid Base Time */
#define DSPROC_EBASETIME     "Could Not Get Base Time For Time Variable"

/** Invalid Global Attribute Value */
#define DSPROC_EGLOBALATT     "Invalid Global Attribute Value"

/** Invalid Data Type For Time Variable */
#define DSPROC_ETIMEVARTYPE  "Invalid Data Type For Time Variable"

/** Invalid Data Type For QC Variable */
#define DSPROC_EQCVARTYPE    "Invalid Data Type For QC Variable"

/** Invalid QC Variable Sample Size */
#define DSPROC_EQCSAMPLESIZE "Invalid QC Variable Sample Size"

/** Invalid QC Variable Dimensions */
#define DSPROC_EQCVARDIMS    "Invalid QC Variable Dimensions"

/** Invalid Data Type For Variable */
#define DSPROC_EVARTYPE      "Invalid Data Type For Variable"

/** Invalid Variable Sample Size */
#define DSPROC_ESAMPLESIZE   "Invalid Variable Sample Size"

/** Data Attribute Has Invalid Data Type */
#define DSPROC_EDATAATTTYPE  "Data Attribute Has Invalid Data Type"

/** Could Not Copy File */
#define DSPROC_EFILECOPY     "Could Not Copy File"

/** Could Not Move File */
#define DSPROC_EFILEMOVE     "Could Not Move File"

/** Could Not Open File */
#define DSPROC_EFILEOPEN     "Could Not Open File"

/** Could Not Read From File */
#define DSPROC_EFILEREAD     "Could Not Read From File"

/** Could Not Write To File */
#define DSPROC_EFILEWRITE    "Could Not Write To File"

/** Could Not Get File Stats */
#define DSPROC_EFILESTATS    "Could Not Get File Stats"

/** Could Not Delete File */
#define DSPROC_EUNLINK       "Could Not Delete File"

/** Source File Does Not Exist */
#define DSPROC_ENOSRCFILE    "Source File Does Not Exist"

/** Could Not Determine File Time */
#define DSPROC_ENOFILETIME   "Could Not Determine File Time"

/** Could Not Create Destination Directory */
#define DSPROC_EDESTDIRMAKE  "Could Not Create Destination Directory"

/** Time Calculation Error */
#define DSPROC_ETIMECALC     "Time Calculation Error"

/** Could Not Get File MD5 */
#define DSPROC_EFILEMD5      "Could Not Get File MD5"

/** Source And Destination File MD5s Do Not Match */
#define DSPROC_EMD5CHECK     "Source And Destination File MD5s Do Not Match"

/** Could Not Allocate Memory For Dataset Variable */
#define DSPROC_ECDSALLOCVAR  "Could Not Allocate Memory For Dataset Variable"

/** Could Not Copy Dataset Variable */
#define DSPROC_ECDSCOPYVAR   "Could Not Copy Dataset Variable"

/** Could Not Copy Dataset Variable */
#define DSPROC_ECLONEVAR     "Could Not Clone Dataset Variable"

/** Could Not Define Dataset Variable */
#define DSPROC_ECDSDEFVAR    "Could Not Define Dataset Variable"

/** Could Not Delete Dataset Variable */
#define DSPROC_ECDSDELVAR    "Could Not Delete Dataset Variable"

/** Could Not Copy Dataset Metadata */
#define DSPROC_ECDSCOPY      "Could Not Copy Dataset Metadata"

/** Could Not Change Attribute Value In Dataset */
#define DSPROC_ECDSCHANGEATT "Could Not Change Attribute Value In Dataset"

/** Could Not Set Attribute Value In Dataset */
#define DSPROC_ECDSSETATT    "Could Not Set Attribute Value In Dataset"

/** Could Not Set Dimension Length In Dataset */
#define DSPROC_ECDSSETDIM    "Could Not Set Dimension Length In Dataset"

/** Could Not Set Variable Data In Dataset */
#define DSPROC_ECDSSETDATA   "Could Not Set Variable Data In Dataset"

/** Could Not Set Time Values In Dataset */
#define DSPROC_ECDSSETTIME   "Could Not Set Time Values In Dataset"

/** Could Not Get Time Values From Dataset */
#define DSPROC_ECDSGETTIME   "Could Not Get Time Values From Dataset"

/** Could Not Merge Datasets */
#define DSPROC_EMERGE        "Could Not Merge Datasets"

/** Invalid Cell Boundary Variable or Definition */
#define DSPROC_EBOUNDSVAR    "Invalid Cell Boundary Variable or Definition"

/** DOD Not Defined In Database */
#define DSPROC_ENODOD        "DOD Not Defined In Database"

/** Could Not Find Data Retriever Information */
#define DSPROC_ENORETRIEVER  "Could Not Find Retriever Definition In Database"

/** Invalid Retriever Definition */
#define DSPROC_EBADRETRIEVER "Invalid Retriever Definition"

/** Required Variable Missing From Dataset */
#define DSPROC_EREQVAR       "Required Variable Missing From Dataset"

/** Could Not Retrieve Input Data */
#define DSPROC_ERETRIEVER    "Could Not Retrieve Input Data"

/** Could Not Create NetCDF File */
#define DSPROC_ENCCREATE     "Could Not Create NetCDF File"

/** Could Not Open NetCDF File */
#define DSPROC_ENCOPEN       "Could Not Open NetCDF File"

/** Could Not Close NetCDF File */
#define DSPROC_ENCCLOSE      "Could Not Close NetCDF File"

/** Could Not Close NetCDF File */
#define DSPROC_ENCSYNC       "Could Not Sync NetCDF File"

/** NetCDF File Read Error */
#define DSPROC_ENCREAD       "Could Not Read From NetCDF File"

/** NetCDF File Write Error */
#define DSPROC_ENCWRITE      "Could Not Write To NetCDF File"

/** Could Not Find Data Transform Information */
#define DSPROC_ENOTRANSFORM  "Could Not Find Data Transform Information"

/** Could Not Find Data Transform Information */
#define DSPROC_ETRANSFORM    "Could Not Transform Input Data"

/** Could Not Create Consolidated Transformation QC Variable */
#define DSPROC_ETRANSQCVAR   "Could Not Create Consolidated Transformation QC Variable"

/** Could Not Load Transform Parameters File */
#define DSPROC_ETRANSPARAMLOAD "Could Not Load Transform Parameters File"

/** Could Not Map Input Variable To Output Variable */
#define DSPROC_EVARMAP         "Could Not Map Input Variable To Output Variable"

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_INGEST_RENAME_RAW Ingest: Rename Raw
 */
/*@{*/

int     dsproc_rename(
            int         ds_id,
            const char *file_path,
            const char *file_name,
            time_t      begin_time,
            time_t      end_time);

int     dsproc_rename_tv(
            int              ds_id,
            const char      *file_path,
            const char      *file_name,
            const timeval_t *begin_time,
            const timeval_t *end_time);

int     dsproc_rename_bad(
            int         ds_id,
            const char *file_path,
            const char *file_name,
            time_t      file_time);

void    dsproc_set_rename_preserve_dots(int ds_id, int preserve_dots);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_FILE_UTILS File Utilities
 */
/*@{*/

int     dsproc_copy_file(const char *src_file, const char *dest_file);
int     dsproc_move_file(const char *src_file, const char *dest_file);
FILE   *dsproc_open_file(const char *file);

/*@}*/

#include "dsproc3_internal.h"

#endif /* _DSPROC3_H */
