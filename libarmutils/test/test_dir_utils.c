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
*    $Revision: 5283 $
*    $Author: ermold $
*    $Date: 2011-03-04 07:28:09 +0000 (Fri, 04 Mar 2011) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  Directory Utils Test
 */

int dirlist_test(
    const char  *test_name,
    DirList     *dirlist)
{
    char **file_list;
    int    nfiles;
    int    i;

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "%s\n"
        "------------------------------------------------------------\n",
        test_name);

    nfiles = dirlist_get_file_list(dirlist, &file_list);
    if (nfiles < 0) {
        return(0);
    }

    for (i = 0; i < nfiles; i++) {
        LOG( ARMUTILS_TEST, "%d - %s\n", i, file_list[i]);
    }

    return(1);
}

int dir_utils_test()
{
    char    *test_files = strdup("./test_files");
    char    *test_dir   = strdup("./test_dir/test_sub_dir");
    DirList *dirlist;

    const char *patterns[] = {
        "^AEROSL\\_",
        "^CPDMON\\_"
    };
    int npatterns  = sizeof(patterns) / sizeof(char *);

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "Directory Utils Test\n"
        "============================================================\n");

    /* Test make_path */

    if (access(test_dir, F_OK) == 0) {
        if (rmdir(test_dir) == -1) {
            ERROR( ARMUTILS_TEST,
                "Could not remove test directory: %s\n", test_dir);
        }

        test_dir[10] = '\0';

        if (rmdir(test_dir) == -1) {
            ERROR( ARMUTILS_TEST,
                "Could not remove test directory: %s\n", test_dir);
        }

        test_dir[10] = '/';

    }

    if (!make_path(test_dir, 0775)) {
        return(0);
    }

    /* List all files using the default alphanumeric sort */

    dirlist = dirlist_create(test_files, 0);
    if (!dirlist) {
        return(0);
    }

    if (!dirlist_test("List of all files (alphanumeric):", dirlist)) {
        return(0);
    }

    /* List all files using a numeric sort */

    dirlist_set_qsort_compare(dirlist, qsort_numeric_strcmp);

    if (!dirlist_test("List of all files (numeric):", dirlist)) {
        return(0);
    }

    /* List only the AEROSL and CPDMON files */

    if (!dirlist_add_patterns(dirlist, npatterns, patterns, 0)) {
        return(0);
    }

    if (!dirlist_test("List of AEROSL and CPDMON files:", dirlist)) {
        return(0);
    }

    dirlist_free(dirlist);
    free(test_files);
    free(test_dir);

    return(1);
}
