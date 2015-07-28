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
 *  Endian Swap Test
 */

int endian_swap_test()
{
    char   *little_endian_file = "test_endian_little.data";
    char   *big_endian_file    = "test_endian_big.data";
    int     fd;
    short   sval;
    int     ival;
    float   fval;
    double  dval;

    LOG( ARMUTILS_TEST,
        "\n============================================================\n"
        "Endian Swap Test\n"
        "============================================================\n");

    /* Read little endian file */

    fd = open(little_endian_file, O_RDONLY);
    if (fd == -1) {

        ERROR( ARMUTILS_TEST,
            "Could not open file: %s\n"
            " -> open error: %s\n",
            little_endian_file, strerror(errno));

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "\n----- Reading Little Endian File -----\n\n");

    if (lton_read_16(fd, &sval, 1) != 1) return(0);
    if (lton_read_32(fd, &ival, 1) != 1) return(0);
    if (lton_read_32(fd, &fval, 1) != 1) return(0);
    if (lton_read_64(fd, &dval, 1) != 1) return(0);

    LOG( ARMUTILS_TEST,
        "short value  = %hd\n"
        "int value    = %d\n"
        "float value  = %g\n"
        "double value = %f\n",
        sval, ival, fval, dval);

    if (sval != 1234      ||
        ival != 123456789 ||
        fval <= 123.45599 ||
        fval >= 123.45601 ||
        dval != 123456789.012345) {

        return(0);
    }

    close(fd);

    /* Read big endian file */

    fd = open(big_endian_file, O_RDONLY);
    if (fd == -1) {

        ERROR( ARMUTILS_TEST,
            "Could not open file: %s\n"
            " -> open error: %s\n",
            big_endian_file, strerror(errno));

        return(0);
    }

    LOG( ARMUTILS_TEST,
        "\n----- Reading Big Endian File -----\n\n");

    if (bton_read_16(fd, &sval, 1) != 1) return(0);
    if (bton_read_32(fd, &ival, 1) != 1) return(0);
    if (bton_read_32(fd, &fval, 1) != 1) return(0);
    if (bton_read_64(fd, &dval, 1) != 1) return(0);

    LOG( ARMUTILS_TEST,
        "short value  = %hd\n"
        "int value    = %d\n"
        "float value  = %g\n"
        "double value = %f\n",
        sval, ival, fval, dval);

    if (sval != 1234      ||
        ival != 123456789 ||
        fval <= 123.45599 ||
        fval >= 123.45601 ||
        dval != 123456789.012345) {

        return(0);
    }

    close(fd);

    return(1);
}
