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
*    $Revision: 65236 $
*    $Author: ermold $
*    $Date: 2015-10-15 20:53:29 +0000 (Thu, 15 Oct 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_version.c
 *  libdsproc3 library version.
 */

#include <string.h>
#include "dsproc3.h"
#include "trans.h"

/** @privatesection */

static char *_VersionTag = "$Version: dslib-libdsproc3-2.29 $";
static char  _Version[64];

static char  _ADI_Version[128];

/**
 *  Private: Trim repository tag from version string if necessary.
 *
 *  @param version - version string
 */
void _dsproc_trim_version(char *version)
{
    char *vp;
    char *cp;

    if (!version) return;

    /* find colon */

    if ((*version != '$') || !(cp = strchr(version, ':'))) {
        return;
    }

    /* skip spaces */

    for (cp++; *cp == ' '; cp++);

    if (*cp == '\0') {
        *version = '\0';
        return;
    }

    /* trim leading portion of repository tag */

    vp = version;
    while (*cp != '\0') *vp++ = *cp++;

    /* trim trailing portion of repository tag */

    for (vp--; (*vp == ' ') || (*vp == '$'); vp--) {
        if (vp == version) break;
    }

    *++vp = '\0';
}

/** @publicsection */

/**
 *  libdsproc3 library version.
 *
 *  @return libdsproc3 library version
 */
const char *dsproc_lib_version(void)
{
    if (*_Version == '\0') {
        strncpy(_Version, _VersionTag, 64);
        _dsproc_trim_version(_Version);
    }

    return((const char *)_Version);
}

/**
 *  ADI version.
 *
 *  @return Full ADI version string
 */
const char *adi_version(void)
{
    const char *string;
    char        version[16];
    char       *strp;
    int         nfound, major, minor, micro, i;

    const char *(*version_functions[])(void) = {
        dsproc_lib_version,
        dsdb_lib_version,
        trans_lib_version,
        cds_lib_version,
        ncds_lib_version,
        armutils_lib_version,
        dbconn_lib_version,
        msngr_lib_version,
        NULL
    };

    if (*_ADI_Version == '\0') {
    
        strp = _ADI_Version;

        for (i = 0; version_functions[i]; ++i) {

            micro  = 0;
            string = version_functions[i]();
            nfound = parse_version_string(string, &major, &minor, &micro);

            if (nfound < 2) {
                strcpy(version, "x.x");
            }
            else {
                sprintf(version, "%d.%d", major, minor);
            }
        
            if (i == 0) {
                strp += sprintf(strp, "%s", version);
            }
            else {
                strp += sprintf(strp, "-%s", version);
            }
        }
    }

    return((const char *)_ADI_Version);
}
