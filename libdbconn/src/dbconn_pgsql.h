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
*    $Revision: 6424 $
*    $Author: ermold $
*    $Date: 2011-04-26 01:42:06 +0000 (Tue, 26 Apr 2011) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dbconn_pgsql.h
 *  Header file for postgres backend.
 */

#ifndef _DBCONN_PGSQL_H_
#define _DBCONN_PGSQL_H_ 1

#include "dbconn.h"
#include "../config.h"

#ifdef HAVE_POSTGRES

#ifdef HAVE_LIBPQ_FE_H
#include "libpq-fe.h"
#else
#include "pgsql/libpq-fe.h"
#endif

/***************************************
* Connection Functions
*/

DBStatus    pgsql_connect(DBConn *dbconn);
void        pgsql_disconnect(DBConn *dbconn);
int         pgsql_is_connected(DBConn *dbconn);
DBStatus    pgsql_reset(DBConn *dbconn);

/***************************************
* Command Functions
*/

DBStatus    pgsql_exec(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params);

DBStatus    pgsql_query(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                DBResult   **result);

DBStatus    pgsql_query_bool(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                int         *result);

DBStatus    pgsql_query_int(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                int         *result);

DBStatus    pgsql_query_long(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                long        *result);

DBStatus    pgsql_query_float(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                float       *result);

DBStatus    pgsql_query_double(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                double      *result);

DBStatus    pgsql_query_text(
                DBConn      *dbconn,
                const char  *command,
                int          nparams,
                const char **params,
                char       **result);

/***************************************
*  Utility Functions
*/

char       *pgsql_bool_to_text(int bval, char *text);
int        *pgsql_text_to_bool(const char *text, int *bval);

char       *pgsql_time_to_text(time_t time, char *text);
time_t     *pgsql_text_to_time(const char *text, time_t *time);

char       *pgsql_timeval_to_text(const timeval_t *tval, char *text);
timeval_t  *pgsql_text_to_timeval(const char *text, timeval_t *tval);

#endif /* HAVE_LIBPQ_FE_H */
#endif /* _DBCONN_PGSQL_H_ */
