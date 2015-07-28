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
*    $Revision: 16166 $
*    $Author: ermold $
*    $Date: 2012-12-07 22:39:18 +0000 (Fri, 07 Dec 2012) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  String Utils Test
 */

// $
// Release_1_6_1
// smor_ingest.c,v 1.19 2003/10/07 12:25:02 koontz process-ingest-smor_ingest-7.11-0 $
// smor_ingest.c,v 2.11 1997/06/04 14:42:49 d34863 Exp d34863 $

typedef struct TestVersion {
    int major;
    int minor;
    int micro;
    const char *string;
} TestVersion;

int test_parse_version_string()
{
    TestVersion versions[] = {
        { 1,  1,  2, "$State: vap-rlprof_dep-1.1-2.sol5_10 $" },
        { 9,  3,  0, "ingest-rwp-9.3-0.el5" },
        { 1,  5,  0, "ConVVAP-1.5" },
        { 0,  1,  0, "Evaluation 0.1" },
        { 2,  5,  0, "Version: ver2.5p6" },
        { 3,  0,  0, "V3.0" },
        { 3, 10, 12, "V3.10.12" },

        { 1,  3,  0, "915rwp_2_1.3_v" },

        { 0,  0,  0, "$State: Release_2_1 $" },
        { 0,  0,  0, "$State: Exp $" },
        { 0,  0,  0, "$" },
    };

    size_t nversions = sizeof(versions)/sizeof(TestVersion);
    int    retval    = 1;
    int    nfound;
    int    major;
    int    minor;
    int    micro;
    size_t i;

    for (i = 0; i < nversions; i++) {

        nfound = parse_version_string(
            versions[i].string, &major, &minor, &micro);

        if (versions[i].major == major &&
            versions[i].minor == minor &&
            versions[i].micro == micro) {

            LOG( ARMUTILS_TEST,
                "passed: parse_version_string test (found %d): '%d.%d-%d' == '%s'\n",
                nfound, versions[i].major, versions[i].minor, versions[i].micro,
                versions[i].string);

        }
        else {
            LOG( ARMUTILS_TEST,
                "FAILED: parse_version_string test (found %d): '%d.%d-%d' != '%d.%d-%d' == '%s'\n",
                nfound, major, minor, micro,
                versions[i].major, versions[i].minor, versions[i].micro,
                versions[i].string);

            retval = 0;
        }
    }

    return(retval);
}

int string_utils_test()
{
    char  *string;
    char  *copy;
    int    retval;
    int    nvals;
    double dbl_buffer[16];
    int    int_buffer[16];
    char   log_buffer[256];
    char   tmp_buffer[256];
    int    i;

    retval = 1;

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "String Utils Test\n"
        "============================================================\n");

    /* Test parse_version_string */

    if (!test_parse_version_string()) {
        retval = 0;
    }

    /* Test string_copy */

    string = "test string copy";
    copy   = string_copy(string);

    if (!copy) {
        return(0);
    }

    if (strcmp(string, copy) == 0) {
        LOG( ARMUTILS_TEST,
            "passed: string_copy test: '%s' == '%s'\n", string, copy);
    }
    else {
        LOG( ARMUTILS_TEST,
            "FAILED: string_copy test: '%s' != '%s'\n", string, copy);
        retval = 0;
    }

    free(copy);

    /* Test string_create */

    string = string_create("%s (%d), %s (%d), %s (%d)",
        "one", 1, "two", 2, "three", 3);

    if (strcmp(string, "one (1), two (2), three (3)") == 0) {
        LOG( ARMUTILS_TEST,
            "passed: string_create test: '%s'\n", string);
    }
    else {
        LOG( ARMUTILS_TEST,
            "FAILED: string_create test: '%s'\n", string);
        retval = 0;
    }

    free(string);

    /* Test string_to_double */

    string = "1234.56789 1.2, 34.56, 78.9 1011";

    nvals = string_to_doubles(string, 0, dbl_buffer);

    if (nvals == 5) {
        nvals = string_to_doubles(string, nvals, dbl_buffer);
    }

    if (nvals != 5) {
        LOG( ARMUTILS_TEST,
            "FAILED: string_to_doubles test: nvals (%d) != 5\n", nvals);
    }
    else {

        sprintf(log_buffer, "%.9g", dbl_buffer[0]);

        for (i = 1; i < nvals; i++) {
            sprintf(tmp_buffer, ", %.9g", dbl_buffer[i]);
            strcat(log_buffer, tmp_buffer);
        }

        LOG( ARMUTILS_TEST,
            "passed: string_to_doubles test: %s\n", log_buffer);
    }

    /* Test string_to_double with ****'s*/

    string = "**** 1234.56789 **** 1.2, 34.56, 78.9 1011 *****";

    nvals = string_to_doubles(string, 0, dbl_buffer);

    if (nvals == 8) {
        nvals = string_to_doubles(string, nvals, dbl_buffer);
    }

    if (nvals != 8) {
        LOG( ARMUTILS_TEST,
            "FAILED: string_to_doubles test: nvals (%d) != 8\n", nvals);
    }
    else {

        sprintf(log_buffer, "%.9g", dbl_buffer[0]);

        for (i = 1; i < nvals; i++) {
            sprintf(tmp_buffer, ", %.9g", dbl_buffer[i]);
            strcat(log_buffer, tmp_buffer);
        }

        LOG( ARMUTILS_TEST,
            "passed: string_to_doubles test: %s\n", log_buffer);
    }

    /* Test string_to_int */

    string = "1234.56789 1.2, 34.56, 78.9 1011";

    nvals = string_to_ints(string, 0, int_buffer);

    if (nvals == 9) {
        nvals = string_to_ints(string, nvals, int_buffer);
    }

    if (nvals != 9) {
        LOG( ARMUTILS_TEST,
            "FAILED: string_to_ints test: nvals (%d) != 9\n", nvals);
    }
    else {

        sprintf(log_buffer, "%d", int_buffer[0]);

        for (i = 1; i < nvals; i++) {
            sprintf(tmp_buffer, ", %d", int_buffer[i]);
            strcat(log_buffer, tmp_buffer);
        }

        LOG( ARMUTILS_TEST,
            "passed: string_to_ints test: %s\n", log_buffer);
    }

    /* Test string_to_int with ****'s */

    string = "**** 1234.56789 **** 1.2, 34.56, 78.9 1011 *****";

    nvals = string_to_ints(string, 0, int_buffer);

    if (nvals == 12) {
        nvals = string_to_ints(string, nvals, int_buffer);
    }

    if (nvals != 12) {
        LOG( ARMUTILS_TEST,
            "FAILED: string_to_ints test: nvals (%d) != 12\n", nvals);
    }
    else {

        sprintf(log_buffer, "%d", int_buffer[0]);

        for (i = 1; i < nvals; i++) {
            sprintf(tmp_buffer, ", %d", int_buffer[i]);
            strcat(log_buffer, tmp_buffer);
        }

        LOG( ARMUTILS_TEST,
            "passed: string_to_ints test: %s\n", log_buffer);
    }

    /* Test trim_trailing_spaces */

    string = strdup("ten spaces including newline and line feed:         \r\n");
    trim_trailing_spaces(string);

    if (strcmp(string, "ten spaces including newline and line feed:") == 0) {
        LOG( ARMUTILS_TEST,
            "passed: trim_trailing_spaces test: '%s'\n", string);
    }
    else {
        LOG( ARMUTILS_TEST,
            "FAILED: trim_trailing_spaces test: '%s'\n", string);
        retval = 0;
    }

    free(string);

    return(retval);
}
