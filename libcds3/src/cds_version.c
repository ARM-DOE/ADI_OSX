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
*    $Revision: 60277 $
*    $Author: ermold $
*    $Date: 2015-02-15 00:42:57 +0000 (Sun, 15 Feb 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file cds_version.c
 *  libcds3 library version.
 */

/** @privatesection */

static char *_VersionTag = "$Version: afl-libcds3-1.16 $";
static char  _Version[64];

/** @publicsection */

/**
 *  libcds3 library version.
 *
 *  @return libcds3 library version
 */
const char *cds_lib_version(void)
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
