/*******************************************************************************
*
*  COPYRIGHT (C) 2015 Battelle Memorial Institute.  All Rights Reserved.
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
*    $Revision: 61068 $
*    $Author: ermold $
*    $Date: 2015-04-14 21:59:50 +0000 (Tue, 14 Apr 2015) $
*
*******************************************************************************/

#include "test_libarmutils.h"

/*******************************************************************************
 *  Regex Time Test
 */

int retime_run_test(
    const char  *test_name,
    int          npatterns,
    const char **patterns,
    int          nstrings,
    const char **strings)
{
    RETimeList *retlist;
    RETime     *retime;
    RETimeRes   res;
    int         status;
    time_t      secs1970;
    timeval_t   tv;
    char        ts[32];

    int pi, si;

    LOG( ARMUTILS_TEST,
        "\n------------------------------------------------------------\n"
        "%s\n"
        "------------------------------------------------------------\n",
        test_name);

    LOG( ARMUTILS_TEST, "\nCompiling Time String Patterns:\n");

    retlist = retime_list_compile(npatterns, patterns, 0);

    for (pi = 0; pi < npatterns; ++pi) {

        retime = retlist->retimes[pi];

        LOG( ARMUTILS_TEST,
            "\n"
            " - pattern: '%s'\n"
            "   regex:   '%s'\n",
            retime->tspattern, retime->pattern);
    }

    LOG( ARMUTILS_TEST, "\nChecking Strings For Pattern Match:\n");

    for (si = 0; si < nstrings; ++si) {

        LOG( ARMUTILS_TEST, "\n - string: '%s'\n", strings[si]);

        status = retime_list_execute(retlist, strings[si], &res);
        if (status < 0) return(0);
        if (status > 0) {

            secs1970 = retime_get_secs1970(&res);
            tv       = retime_get_timeval(&res);

            format_timeval(&tv, ts);

            LOG( ARMUTILS_TEST,
                " - match:  '%s'\n"
                "    - formated   = %s\n"
                "    - year       = %d\n"
                "    - month      = %d\n"
                "    - mday       = %d\n"
                "    - hour       = %d\n"
                "    - min        = %d\n"
                "    - sec        = %d\n"
                "    - usec       = %d\n"
                "    - century    = %d\n"
                "    - yy         = %d\n"
                "    - yday       = %d\n"
                "    - hhmm       = %d\n"
                "    - secs1970   = %d\n"
                "    - tv.tv_sec  = %d\n"
                "    - tv.tv_usec = %d\n",
                res.retime->tspattern,
                ts,
                res.year,
                res.month,
                res.mday,
                res.hour,
                res.min,
                res.sec,
                res.usec,
                res.century,
                res.yy,
                res.yday,
                res.hhmm,
                res.secs1970,
                res.tv.tv_sec,
                res.tv.tv_usec);
        }
    }

    retime_list_free(retlist);

    return(1);
}

int regex_time_test()
{
    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "Regex Time Tests\n"
        "============================================================\n");

    {
        const char *test_name = "Range Tests";
        const char *patterns[] = {
            "%Y-%m-%d %H:%M:%S"
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "2015-04-01 01:59:00",
            "2015-4-1 1:59:00",
            "2015-04-01 01:59:60",
            "2015-04-01 01:60:61",
            "2015-04-01 01:60:00",
            "2015-04-01 24:59:00",
            "2015-04-32 01:59:00",
            "2015-13-01 01:59:00",
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!retime_run_test(
            test_name, npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    {
        const char *test_name = "Standard Date/Time Formats";
        const char *patterns[] = {
            "%Y[-/]%m[-/]%d +%H:%M:%S",
            "%Y[-/]%m[-/]%dT%H:%M:%S",
            "%m/%d/%Y +%H:%M:%S",
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "2014-09-23 08:23:13",
            "begin2014-09-23 08:23:13end",
            "2014/09/23 08:23:13",
            "2014-09-23T08:23:13",
            "2014-09-23T08:23:13Z",
            "09/23/2014 08:23:13",
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!retime_run_test(
            test_name, npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    {
        const char *test_name = "Fractional Seconds";
        const char *patterns[] = {
            "%Y[-/]%m[-/]%d +%H:%M:%S",
            "%Y[-/]%m[-/]%dT%H:%M:%S",
            "%m/%d/%Y +%H:%M:%S",
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "2014-09-23 08:23:13.789",
            "begin2014-09-23T08:23:13.0789end",
            "2014/09/23 08:23:13.00789",
            "2014/09/23T08:23:13.000789",
            "2014/09/23T08:23:13.0000789",
            "09/23/2014 08:23:13.123",
            "09/23/2014 08:23:13.0123",
            "09/23/2014 08:23:13.00123",
            "09/23/2014 08:23:13.000123",
            "09/23/2014 08:23:13.0000123",
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!retime_run_test(
            test_name, npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    {
        const char *test_name = "CDL Time";
        const char *patterns[] = {
            "%Y *, *%j *, *%h",
            "%Y +%j +%h",
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "2012,60,1",
            "2013,60,1",
            "2014,266,12",
            "2014,266,123",
            "2014,266,1234",

            "2012,366,1234",
            "2014,366,1234",
            "2014,367,1234",
            "2014,266,2400",
            "2014,266,2401",
            "2014,266,2360",
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!retime_run_test(
            test_name, npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    {
        const char *test_name = "Standard File Name Timestamp Formats";
        const char *patterns[] = {
            "%Y%0m%0d[._T-]%0H%0M%0S"
        };
        int npatterns  = sizeof(patterns) / sizeof(char *);

        const char *strings[]  = {
            "20140923.082313",
            "20140923_082313",
            "20140923-082313",
            "20140923T082313",
            "20140923T082313Z",
            "20140923x082313",
            "201409230082313",

            "2014092.082313",
            "20140923.82313",
        };
        int nstrings = sizeof(strings) / sizeof(char *);

        if (!retime_run_test(
            test_name, npatterns, patterns, nstrings, strings)) {

            return(0);
        }
    }

    return(1);
}
