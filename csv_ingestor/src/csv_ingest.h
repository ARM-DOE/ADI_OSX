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
 *  UserData structure passed to all user defined hook functions.
 */
typedef struct
{
    const char *proc_name;   /**< process name                          */
    const char *site;        /**< process site                          */
    const char *fac;         /**< process facility                      */

    CSVConf    *conf;        /**< CSV configuration structure           */
    CSVParser  *csv;         /**< pointer to the CSV Parser             */

    int         in_dsid;     /**< input datastream ID                   */
    int         out_dsid;    /**< output datastream ID                  */
    int         raw_dsid;    /**< raw output datastream ID              */

    const char *input_dir;   /**< raw file input directory              */
    const char *file_name;   /**< name of the file being processed      */

    time_t      begin_time;  /**< first record time in the current file */
    time_t      end_time;    /**< last record time in the current file  */

} UserData;

/* Main Ingest Functions */

void   *csv_ingest_init(void);
void    csv_ingest_finish(void *user_data);
int     csv_ingest_process_file(
            void       *user_data,
            const char *input_dir,
            const char *file_name);

int     main(int argc, char *argv[]);

/*@}*/

/**
 *  @defgroup CSV2NC_READ_DATA CSV2NC Read Data
 */
/*@{*/

int     csv_ingest_read_data(UserData *data);

/*@}*/

/**
 *  @defgroup CSV2NC_STORE_DATA CSV2NC Store Data
 */
/*@{*/

int     csv_ingest_store_data(UserData *data);

/*@}*/

#endif /* _CSV_INGEST_H_ */
