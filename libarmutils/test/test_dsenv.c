/*******************************************************************************
*
*  COPYRIGHT (C) 2013 Battelle Memorial Institute.  All Rights Reserved.
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
*    $Revision: 18221 $
*    $Author: ermold $
*    $Date: 2013-07-17 17:24:05 +0000 (Wed, 17 Jul 2013) $
*    $Version: $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  DSENV Tests
 */

typedef struct EnvVar {
    const char *name;
    const char *value;
} EnvVar;

EnvVar RootDirs[] = {
    { "COLLECTION_DATA", "/data/collection" },
    { "CONF_DATA",       "/data/conf"       },
    { "DATASTREAM_DATA", "/data/datastream" },
    { "LOGS_DATA",       "/data/logs"       },
    { "QUICKLOOK_DATA",  "/data/quicklook"  },
    { "TMP_DATA",        "/data/tmp"        },
    { "INGEST_HOME",     "/apps/process"    },
    { "VAP_HOME",        "/apps/process"    }
};

int nRootDirs = sizeof(RootDirs)/sizeof(EnvVar);

int dsenv_test()
{
    const char *proc_name = "mfrsr";
    const char *proc_type = "Ingest";
    const char *site      = "sgp";
    const char *facility  = "C1";
    const char *name      = "mfrsr";
    const char *level     = "b1";
    char       *value;
    char       *path;
    int         status;
    int         i;

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "DSENV Tests\n"
        "============================================================\n");

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "Setting environment variables\n"
        "------------------------------------------------------------\n\n");

    for (i = 0; i < nRootDirs; ++i) {

        LOG( ARMUTILS_TEST, "%s = %s\n", RootDirs[i].name, RootDirs[i].value);

        if (!dsenv_setenv(RootDirs[i].name, RootDirs[i].value)) {
            return(0);
        }
    }

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "Getting environment variables\n"
        "------------------------------------------------------------\n\n");

    for (i = 0; i < nRootDirs; ++i) {

        status = dsenv_getenv(RootDirs[i].name, &value);

        if (status <= 0) {
            if (status == 0) {
                ERROR( ARMUTILS_TEST,
                    "Environment variable does not exist: %s\n",
                    RootDirs[i].name);
            }
            return(0);
        }

        LOG( ARMUTILS_TEST, "%s = %s\n", RootDirs[i].name, value);
        free(value);
    }

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "Testing Functions\n"
        "------------------------------------------------------------\n\n");

    /*************************************************************************
    *  dsenv_get_apps_conf_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_apps_conf_root(%s, %s)\n", proc_name, proc_type);

    status = dsenv_get_apps_conf_root(proc_name, proc_type, &path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_apps_conf_dir
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_apps_conf_dir(%s, %s, %s, %s, %s, %s)\n",
        proc_name, proc_type, site, facility, name, level);

    status = dsenv_get_apps_conf_dir(
        proc_name, proc_type, site, facility, name, level, &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_apps_conf_dir (NULL data level)
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_apps_conf_dir(%s, %s, %s, %s, %s, NULL)\n",
        proc_name, proc_type, site, facility, name);

    status = dsenv_get_apps_conf_dir(
        proc_name, proc_type, site, facility, name, NULL, &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_collection_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_collection_root()\n");

    status = dsenv_get_collection_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_collection_dir
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_collection_dir(%s, %s, %s, 00)\n",
        site, facility, name, level);

    status = dsenv_get_collection_dir(
        site, facility, name, "00", &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_data_conf_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_data_conf_root()\n");

    status = dsenv_get_data_conf_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_data_conf_dir
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_data_conf_dir(%s, %s, %s, %s)\n",
        site, facility, name, level);

    status = dsenv_get_data_conf_dir(
        site, facility, name, level, &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_data_conf_dir (NULL data level)
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_data_conf_dir(%s, %s, %s, NULL)\n",
        site, facility, name, level);

    status = dsenv_get_data_conf_dir(
        site, facility, name, NULL, &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_datastream_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_datastream_root()\n");

    status = dsenv_get_datastream_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_datastream_dir
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_datastream_dir(%s, %s, %s, %s)\n",
        site, facility, name, level);

    status = dsenv_get_datastream_dir(
        site, facility, name, level, &path);

    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_tmp_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_tmp_root()\n");

    status = dsenv_get_tmp_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_logs_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_logs_root()\n");

    status = dsenv_get_logs_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    /*************************************************************************
    *  dsenv_get_quicklook_root
    */

    LOG( ARMUTILS_TEST,
        "\ndsenv_get_quicklook_root()\n");

    status = dsenv_get_quicklook_root(&path);
    if (status <= 0) {
        if (status == 0) {
            ERROR( ARMUTILS_TEST,"Missing environment variable\n");
        }
        return(0);
    }

    LOG( ARMUTILS_TEST, " = %s\n", path);
    free(path);

    return(1);
}
