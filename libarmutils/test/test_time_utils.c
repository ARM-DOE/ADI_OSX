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
*    $Revision: 5093 $
*    $Author: ermold $
*    $Date: 2011-02-15 02:32:36 +0000 (Tue, 15 Feb 2011) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  Time Utils Test
 */

int test_days_in_month(int year, int month, int exp_ndays)
{
    int ndays = days_in_month(year, month);

    if (ndays != exp_ndays) {

        LOG( ARMUTILS_TEST,
            "FAILED: days_in_month test: %d-%02d == %d days (expected %d)\n",
            year, month, ndays, exp_ndays);

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "passed: days_in_month test: %d-%02d == %d days\n",
        year, month, ndays);

    return(1);
}

int test_four_digit_year(int year, int exp_year)
{
    int yyyy = four_digit_year(year);

    if (yyyy != exp_year) {

        LOG( ARMUTILS_TEST,
            "FAILED: four_digit_year test: %d == %d (expected %d)\n",
            year, yyyy, exp_year);

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "passed: four_digit_year test: %d == %d\n",
        year, yyyy);

    return(1);
}

int test_normalize_time_values(
    int year, int mon, int day, int hour, int min, int sec, int usec,
    time_t exp_secs1970, const char *exp_string)
{
    char time_string[32];

    time_t secs1970 = normalize_time_values(
        &year, &mon, &day, &hour, &min, &sec, &usec);

    format_time_values(
        year, mon, day, hour, min, sec, usec, time_string);

    if (secs1970 != exp_secs1970) {

        LOG( ARMUTILS_TEST,
            "FAILED: normalize_time_values test: %d != %d\n",
            secs1970, exp_secs1970);

        return(0);
    }

    if (strcmp(time_string, exp_string) != 0) {

        LOG( ARMUTILS_TEST,
            "FAILED: normalize_time_values test: ‘%s’ != ‘%s’\n",
            time_string, exp_string);

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "passed: normalize_time_values test: ‘%s’\n",
        time_string);

    return(1);
}

int test_normalize_timeval(
    timeval_t tval, const char *exp_string)
{
    char time_string[32];

    normalize_timeval(&tval);
    format_timeval(&tval, time_string);

    if (strcmp(time_string, exp_string) != 0) {

        LOG( ARMUTILS_TEST,
            "FAILED: normalize_timeval test: ‘%s’ != ‘%s’\n",
            time_string, exp_string);

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "passed: normalize_timeval test: ‘%s’\n",
        time_string);

    return(1);
}

int time_utils_test()
{
    char        time_string[32];
    const char *exp_string;
    int         exp_secs1970;
    timeval_t   tval;
    char        retval;
    time_t      secs1970;
    struct tm   tm_time;

    retval = 1;

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "Time Utils Test\n"
        "============================================================\n");

    /* Test days_in_month, this also tests IS_LEAP_YEAR */

    if (!test_days_in_month(2007, 2, 28)) retval = 0;
    if (!test_days_in_month(2008, 2, 29)) retval = 0;

    /* test format_secs1970, this also tests format_time_values */

    secs1970   = 1297737066;
    exp_string = "2011-02-15 02:31:06";

    format_secs1970(secs1970, time_string);

    if (strcmp(time_string, exp_string) == 0) {

        LOG( ARMUTILS_TEST,
            "passed: format_secs1970 test: '%s'\n",
            time_string);
    }
    else {

        LOG( ARMUTILS_TEST,
            "FAILED: format_secs1970 test: '%s' (expected ‘%s’)\n",
            time_string, exp_string);

        retval = 0;
    }

    /* test format_timeval, this also tests format_time_values */

    tval.tv_sec  = 1297737066;
    tval.tv_usec = 123456;
    exp_string   = "2011-02-15 02:31:06.123456";

    format_timeval(&tval, time_string);

    if (strcmp(time_string, exp_string) == 0) {

        LOG( ARMUTILS_TEST,
            "passed: format_timeval test: '%s'\n",
            time_string);
    }
    else {

        LOG( ARMUTILS_TEST,
            "FAILED: format_secs1970 test: '%s' != ‘%s’\n",
            time_string, exp_string);

        retval = 0;
    }

    /* test four_digit_year */

    if (!test_four_digit_year(92,   1992)) retval = 0;
    if (!test_four_digit_year(11,   2011)) retval = 0;
    if (!test_four_digit_year(108,  2008)) retval = 0;
    if (!test_four_digit_year(2008, 2008)) retval = 0;

    /* test get_secs1970 */

    exp_secs1970 = 1297737066;

    secs1970 = get_secs1970(2011, 2, 15, 2, 31, 6);

    format_secs1970(secs1970, time_string);

    if (secs1970 == exp_secs1970) {

        LOG( ARMUTILS_TEST,
            "passed: get_secs1970 test: %d\n",
            secs1970);
    }
    else {

        LOG( ARMUTILS_TEST,
            "FAILED: get_secs1970 test: %d (expected %d)\n",
            secs1970, exp_secs1970);

        retval = 0;
    }

    /* test normalize_time_values */

    exp_secs1970 = 1297737066;
    exp_string   = "2011-02-15 02:31:06.123456";

    if (!test_normalize_time_values(
        2011, 2, 15, 2, 31, 6, 123456, exp_secs1970, exp_string)) {

        retval = 0;
    }

    if (!test_normalize_time_values(
        2011, 2, 15, 2, 31, 5, 1123456, exp_secs1970, exp_string)) {

        retval = 0;
    }

    if (!test_normalize_time_values(
        2011, 2, 15, 2, 31, 7, -876544, exp_secs1970, exp_string)) {

        retval = 0;
    }

    if (!test_normalize_time_values(
        2011, 2, 15, 2, 30, 66, 123456, exp_secs1970, exp_string)) {

        retval = 0;
    }

    if (!test_normalize_time_values(
        2010, 14, 15, 2, 31, 6, 123456, exp_secs1970, exp_string)) {

        retval = 0;
    }

    if (!test_normalize_time_values(
        2010, 2, 380, 2, 31, 6, 123456, exp_secs1970, exp_string)) {

        retval = 0;
    }

    /* test normalize_timeval */

    tval.tv_sec  = 1297737066;
    tval.tv_usec = 123456;
    exp_string   = "2011-02-15 02:31:06.123456";

    tval.tv_sec  += 1;
    tval.tv_usec -= 1000000;

    if (!test_normalize_timeval(tval, exp_string)) retval = 0;

    tval.tv_sec  -= 2;
    tval.tv_usec += 2000000;

    if (!test_normalize_timeval(tval, exp_string)) retval = 0;

    /* test timegm */

    secs1970 = 1297737066;

    gmtime_r(&secs1970, &tm_time);
    secs1970 = timegm(&tm_time);

    if (secs1970 == 1297737066) {

        LOG( ARMUTILS_TEST,
            "passed: timegm test\n");
    }
    else {

        LOG( ARMUTILS_TEST,
            "FAILED: timegm test\n");

        retval = 0;
    }

    return(retval);
}
