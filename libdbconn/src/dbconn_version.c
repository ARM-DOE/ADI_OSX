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
*    $Revision: 65910 $
*    $Author: ermold $
*    $Date: 2015-11-17 00:12:02 +0000 (Tue, 17 Nov 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dbconn_version.c
 *  libdbconn library version.
 */

/**
 *  @defgroup DBCONN_VERSION Library Version
 */
/*@{*/

/** @privatesection */

static char *_VersionTag = "$Version: afl-libdbconn-1.10-0 $";
static char  _Version[64];

/** @publicsection */

/**
 *  libdbconn library version.
 *
 *  @return libdbconn library version
 */
const char *dbconn_lib_version(void)
{
    char *tp = _VersionTag;
    char *vp = _Version;

    if (!*vp) {

        /* find colon */

        for (; *tp != ':'; tp++)
            if (*tp == '\0') return((const char *)_Version);

        /* skip spaces */

        for (tp++; *tp == ' '; tp++)
            if (*tp == '\0') return((const char *)_Version);

        /* extract version from tag */

        while (*tp != ' ' && *tp != '$' && *tp != '\0')
            *vp++ = *tp++;

        *vp = '\0';
    }

    return((const char *)_Version);
}

/*@}*/
