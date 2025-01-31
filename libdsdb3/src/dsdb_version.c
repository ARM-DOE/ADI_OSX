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
*    $Revision: 54547 $
*    $Author: ermold $
*    $Date: 2014-05-27 19:37:27 +0000 (Tue, 27 May 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsdb_version.c
 *  libdsdb3 library version.
 */

/**
 *  @defgroup DSDB_VERSION Library Version
 */
/*@{*/

/** @privatesection */

static char *_VersionTag = "$Version: dsdb-libdsdb3-1.9-0$";
static char  _Version[64];

/** @publicsection */

/**
 *  libdsdb3 library version.
 *
 *  @return libdsdb3 library version
 */
const char *dsdb_lib_version(void)
{
    char *tp = _VersionTag;
    char *vp = _Version;

    if (!*vp) {

        /* find colon */

        for (; *tp != ':'; tp++)
            if (*tp == '\0') return((const char *)_Version);

        /* skip spaces */

        for (tp++; *tp == ' '; tp++);

        /* extract version from tag */

        while (*tp != ' ' && *tp != '$' && *tp != '\0')
            *vp++ = *tp++;

        *vp = '\0';
    }

    return((const char *)_Version);
}

/*@}*/
