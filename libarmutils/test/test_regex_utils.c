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
*    $Revision: 4881 $
*    $Author: ermold $
*    $Date: 2011-01-17 09:33:00 +0000 (Mon, 17 Jan 2011) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  Regex Utils Test
 */

void log_relist_patterns(REList *re_list)
{
    int i;

    LOG( ARMUTILS_TEST, "\nRegular expression patterns:\n");

    for (i = 0; i < re_list->nregs; i++) {
        LOG( ARMUTILS_TEST, " - %s\n", re_list->patterns[i]);
    }
}

void log_relist_results(
    REList      *re_list,
    int          status,
    int          mindex,
    size_t       nsubs,
    regmatch_t  *pmatch,
    char       **substrings)
{
    int   i;
    char  so[8];
    char  eo[8];
    char *ss;

    LOG( ARMUTILS_TEST,
        "\nResults for string: %s\n", re_list->string);

    if (status == 0) {
        LOG( ARMUTILS_TEST,
            " - no match\n");
    }
    else {
        LOG( ARMUTILS_TEST,
            " - mindex = %d\n"
            " - nsubs  = %u\n",
            mindex, nsubs);

        for (i = 0; i <= nsubs; i++) {

            if (pmatch) {
                sprintf(so, "%d", (int)pmatch[i].rm_so);
                sprintf(eo, "%d", (int)pmatch[i].rm_eo);
            }
            else {
                strcpy(so, "null");
                strcpy(eo, "null");
            }

            if (substrings && substrings[i]) {
                ss = substrings[i];
            }
            else {
                ss = "NULL";
            }

            LOG( ARMUTILS_TEST,
                " - match[%d]: so = %s, eo = %s: '%s'\n",
                i, so, eo, ss);
        }
    }
}

int regex_check_strings(
    REList      *re_list,
    int          nstrings,
    const char **strings)
{
    int          mindex;
    size_t       nsubs;
    regmatch_t  *pmatch;
    char       **substrings;
    int          status;
    int          i;

    for (i = 0; i < nstrings; i++) {

        status = relist_execute(
            re_list, strings[i], 0,
            &mindex, &nsubs, &pmatch, &substrings);

        if (status < 0) {
            return(0);
        }

        log_relist_results(re_list, status, mindex, nsubs, pmatch, substrings);
    }

    return(1);
}

int regex_run_test(
    const char  *test_name,
    int          cflags,
    int          npatterns,
    const char **patterns,
    int          nstrings,
    const char **strings)
{
    REList *re_list;

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "%s\n"
        "------------------------------------------------------------\n",
        test_name);

    re_list = relist_compile(NULL, npatterns, patterns, cflags);
    if (!re_list) {
        return(0);
    }

    log_relist_patterns(re_list);

    if (!regex_check_strings(re_list, nstrings, strings)) {
        return(0);
    }

    relist_free(re_list);

    return(1);
}

int regex_utils_test()
{
    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "Regex Utils Test\n"
        "============================================================\n");
    {
        const char *patterns[] = {
            "\\.csv$",
            "\\.dat$"
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "file_name.csv",
            "file_name.dat",
            "file_name.CSV"
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!regex_run_test(
            "Patterns with no parenthesised subexpressions",
            REG_EXTENDED,
            npatterns, patterns, nstrings, strings)) {

            return(0);
        }

        if (!regex_run_test(
            "Patterns with no parenthesised subexpressions (ignore case)",
            REG_EXTENDED | REG_ICASE,
            npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    {
        const char *patterns[] = {
            "[^0-9]?([0-9]{9,10})\\.",
            "[^0-9]?([0-9]{4})([0-9]{2})([0-9]{2})\\.([0-9]{2})([0-9]{2})([0-9]{2})\\."
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "1286840328.icm",
            "prefix.1286840328.icm",
            "20101011.233848.cdf",
            "prefix.20101011.233848.cdf"
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!regex_run_test(
            "Numeric patterns with parenthesised subexpressions",
            REG_EXTENDED,
            npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    return(1);
}
