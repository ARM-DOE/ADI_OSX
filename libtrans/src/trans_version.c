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
*    $Revision: 54690 $
*    $Author: shippert $
*    $Date: 2014-05-29 15:39:29 +0000 (Thu, 29 May 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file trans_version.c
 *  libtrans library version.
 */

/** @privatesection */

static char *_VersionTag = "$Version: $";
static char  _Version[64];

/** @publicsection */

/**
 *  libtrans library version.
 *
 *  @return libtrans library version
 */
const char *trans_lib_version(void)
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
