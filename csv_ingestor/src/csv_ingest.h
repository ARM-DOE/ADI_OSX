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
*    $Revision: 66394 $
*    $Author: ermold $
*    $Date: 2015-12-10 23:49:40 +0000 (Thu, 10 Dec 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file csv_ingest.h
 *  CSV Ingest Header File.
 */

#ifndef _CSV_INGEST_H_
#define _CSV_INGEST_H_

#include "dsproc3.h"

/**
 *  @defgroup CSV_INGEST CSV Ingest
 */
/*@{*/

/**
 *  Datastream Data structure
 *
 *  Structure used to read in an input file and map it to an output dataset.
 */
typedef struct
{
    int         dsid;      /**< output datastream ID                        */
    CSVConf    *conf;      /**< CSV configuration structure                 */
    CSVParser  *csv;       /**< pointer to the CSV Parser                   */
    CSV2CDSMap *map;       /**< pointer to the CSV to CDS mapping structure */
    REList     *fn_relist; /**< compiled file name patterns                 */

} DsData;

/**
 *  UserData structure passed to all user defined hook functions.
 */
typedef struct
{
    const char *proc_name;    /**< process name                          */
    const char *site;         /**< process site                          */
    const char *fac;          /**< process facility                      */

    int         raw_in_dsid;  /**< raw data input datastream ID          */
    int         raw_out_dsid; /**< raw data output datastream ID         */

    const char *input_dir;    /**< raw file input directory              */
    const char *file_name;    /**< name of the file being processed      */

    time_t      begin_time;   /**< first record time in the current file */
    time_t      end_time;     /**< last record time in the current file  */

    DsData    **dsp;          /**< array of DsData pointers              */
    int         ndsp;         /**< number of DsData entries              */

} UserData;

/* Main CSV Ingest Functions */

void   *csv_ingest_init(void);
void    csv_ingest_finish(void *user_data);
int     csv_ingest_process_file(
            void       *user_data,
            const char *input_dir,
            const char *file_name);

void    csv_ingest_free_dsdata(DsData *ds);

DsData *csv_ingest_init_dsdata(
            const char *dsname,
            const char *dslevel);

int     main(int argc, char *argv[]);

/*@}*/

/**
 *  @defgroup CSV_INGESTOR_READ_DATA CSV Ingest Read Data
 */
/*@{*/

int     csv_ingest_read_data(UserData *data, DsData *ds);

/*@}*/

/**
 *  @defgroup CSV_INGESTOR_STORE_DATA CSV Ingest Store Data
 */
/*@{*/

int     csv_ingest_store_data(UserData *data, DsData *ds);

/*@}*/

#endif /* _CSV_INGEST_H_ */
