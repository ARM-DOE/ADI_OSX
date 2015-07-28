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
*    $Revision: 55609 $
*    $Author: ermold $
*    $Date: 2014-07-21 23:29:26 +0000 (Mon, 21 Jul 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file armutils_version.c
 *  libarmutils library version.
 */

#include "armutils.h"

/*******************************************************************************
 *  Private Functions
 */
/** @privatesection */

static char *_VersionTag = "$Version: afl-libarmutils-1.7 $";
static char  _Version[64];

/*******************************************************************************
 *  Public Functions
 */
/** @publicsection */

/**
 *  libarmutils library version.
 *
 *  @return libarmutils library version
 */
const char *armutils_lib_version(void)
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
