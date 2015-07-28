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
*    $Revision: 5133 $
*    $Author: ermold $
*    $Date: 2011-02-19 04:31:46 +0000 (Sat, 19 Feb 2011) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  File Utils Test
 */

int file_utils_test()
{
    char    *src_dir  = "./test_files";
    char    *dest_dir = "./test_dir";
    char     src_file[PATH_MAX];
    char     dest_file[PATH_MAX];
    DirList *dirlist;
    char   **file_list;
    int      nfiles;
    int      i;

    const char *patterns[] = { "^AEROSL\\_" };
    int         npatterns  = sizeof(patterns) / sizeof(char *);

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "File Utils Test\n"
        "============================================================\n");

    /* Test file copy */

    dirlist = dirlist_create(src_dir, 0);
    if (!dirlist) {
        return(0);
    }

    if (!dirlist_add_patterns(dirlist, npatterns, patterns, 0)) {
        return(0);
    }

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "File copy test\n"
        "------------------------------------------------------------\n");

    nfiles = dirlist_get_file_list(dirlist, &file_list);
    if (nfiles < 0) {
        return(0);
    }

    for (i = 0; i < nfiles; i++) {

        sprintf(src_file,  "%s/%s", src_dir,  file_list[i]);
        sprintf(dest_file, "%s/%s", dest_dir, file_list[i]);

        LOG( ARMUTILS_TEST, "%s -> %s\n", src_file, dest_file);

        if (!file_copy(src_file, dest_file, FC_CHECK_MD5)) {
            return(0);
        }

        if (!file_exists(dest_file)) {

            ERROR( ARMUTILS_TEST,
                "file_copy failed but reported success\n");

            return(0);
        }
    }

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "File move test\n"
        "------------------------------------------------------------\n");

    for (i = 0; i < nfiles; i++) {

        sprintf(src_file,  "%s/%s", dest_dir,  file_list[i]);
        sprintf(dest_file, "/tmp/%s", file_list[i]);

        LOG( ARMUTILS_TEST, "%s -> %s\n", src_file, dest_file);

        if (!file_move(src_file, dest_file, FC_CHECK_MD5)) {
            return(0);
        }

        if (file_exists(src_file) || !file_exists(dest_file)) {

            ERROR( ARMUTILS_TEST,
                "file_move failed but reported success\n");

            return(0);
        }

        unlink(dest_file);
    }

    dirlist_free(dirlist);

    return(1);
}
