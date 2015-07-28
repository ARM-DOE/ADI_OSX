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
*    $Revision: 60948 $
*    $Author: ermold $
*    $Date: 2015-04-02 18:52:36 +0000 (Thu, 02 Apr 2015) $
*    $Version: afl-libarmutils-1.0-0 $
*
*******************************************************************************/

#include "test_libarmutils.h"

const char *gProgramName;
LogFile    *gLog;

/*******************************************************************************
 *  File Compare
 */

int compare_files(char *file1, char *file2)
{
    char        *files[2];
    struct stat  stats[2];
    int          fds[2]      = { -1, -1 };
    char        *buffers[2]  = { NULL,  NULL };
    int          retval      = 1;
    int          i;

    files[0] = file1;
    files[1] = file2;

    /* Compare file sizes */

    for (i = 0; i < 2; i++) {

        if (stat(files[i], &stats[i]) < 0) {

            ERROR( gProgramName,
                "Could not stat file: %s\n -> %s\n",
                files[i], strerror(errno));

            return(0);
        }
    }

    if (stats[0].st_size != stats[1].st_size) {
        return(0);
    }

    /* Read in the files */

    for (i = 0; i < 2; i++) {

        buffers[i] = (char *)malloc(stats[i].st_size * sizeof(char));

        if (!buffers[i]) {

            ERROR( gProgramName,
                "Memory allocation error\n");

            retval = 0;
            break;
        }

        fds[i] = open(files[i], O_RDONLY);

        if (fds[i] < 0) {

            ERROR( gProgramName,
                "Could not open file: %s\n -> %s\n",
                files[i], strerror(errno));

            retval = 0;
            break;
        }

        if (read(fds[i], buffers[i], stats[i].st_size) != stats[i].st_size) {

            ERROR( gProgramName,
                "Could not read file: %s\n -> %s\n",
                files[i], strerror(errno));

            retval = 0;
            break;
        }
    }

    if (retval) {
        if (memcmp(buffers[0], buffers[1], stats[0].st_size) != 0) {
            retval = 0;
        }
    }

    for (i = 0; i < 2; i++) {
        if (buffers[i])  free(buffers[i]);
        if (fds[i] > -1) close(fds[i]);
    }

    return(retval);
}

/*******************************************************************************
 *  Logging Functions
 */

int open_log(const char *log_file)
{
    char errstr[MAX_LOG_ERROR];

    unlink(log_file);

    if (!msngr_init_log(".", log_file, 0, MAX_LOG_ERROR, errstr)) {
        fprintf(stderr, errstr);
        return(0);
    }

    gLog = msngr_get_log_file();

    return(1);
}

void close_log(void)
{
    msngr_finish_log();
    gLog = (LogFile *)NULL;
}

/*******************************************************************************
 *  Main
 */

int main(int argc, char **argv)
{
    int status;
    int pass_count;
    int fail_count;

    gProgramName = argv[0];
    pass_count   = 0;
    fail_count   = 0;

    fprintf(stdout, "\nTesting build for libarmutils version: %s\n\n",
        armutils_lib_version());

    /************************************************************
    *  Regex Time Test
    ************************************************************/

    if (!open_log("out.regex_time_test")) {
        return(1);
    }

    status = regex_time_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.regex_time_test", "out.regex_time_test");
    }

    if (status == 0) {
        fprintf(stdout, "Regex Time Test........FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "Regex Time Test........pass\n");
        pass_count++;
    }

    /************************************************************
    *  Regex Utils Test
    ************************************************************/

    if (!open_log("out.regex_utils_test")) {
        return(1);
    }

    status = regex_utils_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.regex_utils_test", "out.regex_utils_test");
    }

    if (status == 0) {
        fprintf(stdout, "Regex Utils Test.......FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "Regex Utils Test.......pass\n");
        pass_count++;
    }

    /************************************************************
    *  Directory Utils Test
    ************************************************************/

    if (!open_log("out.dir_utils_test")) {
        return(1);
    }

    status = dir_utils_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.dir_utils_test", "out.dir_utils_test");
    }

    if (status == 0) {
        fprintf(stdout, "Directory Utils Test...FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "Directory Utils Test...pass\n");
        pass_count++;
    }

    /************************************************************
    *  DSENV Test
    ************************************************************/

    if (!open_log("out.dsenv_test")) {
        return(1);
    }

    status = dsenv_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.dsenv_test", "out.dsenv_test");
    }

    if (status == 0) {
        fprintf(stdout, "DSENV Test.............FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "DSENV Test.............pass\n");
        pass_count++;
    }

    /************************************************************
    *  Endian Swap Test
    ************************************************************/

    if (!open_log("out.endian_swap_test")) {
        return(1);
    }

    status = endian_swap_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.endian_swap_test", "out.endian_swap_test");
    }

    if (status == 0) {
        fprintf(stdout, "Endian Swap Test.......FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "Endian Swap Test.......pass\n");
        pass_count++;
    }

    /************************************************************
    *  File Utils Test
    ************************************************************/

    if (!open_log("out.file_utils_test")) {
        return(1);
    }

    status = file_utils_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.file_utils_test", "out.file_utils_test");
    }

    if (status == 0) {
        fprintf(stdout, "File Utils Test........FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "File Utils Test........pass\n");
        pass_count++;
    }

    /************************************************************
    *  String Utils Test
    ************************************************************/

    if (!open_log("out.string_utils_test")) {
        return(1);
    }

    status = string_utils_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.string_utils_test", "out.string_utils_test");
    }

    if (status == 0) {
        fprintf(stdout, "String Utils Test......FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "String Utils Test......pass\n");
        pass_count++;
    }

    /************************************************************
    *  Time Utils Test
    ************************************************************/

    if (!open_log("out.time_utils_test")) {
        return(1);
    }

    status = time_utils_test();

    close_log();

    if (status != 0) {
        status = compare_files("ref.time_utils_test", "out.time_utils_test");
    }

    if (status == 0) {
        fprintf(stdout, "Time Utils Test........FAIL\n");
        fail_count++;
    }
    else {
        fprintf(stdout, "Time Utils Test........pass\n");
        pass_count++;
    }

    /************************************************************
    *  Cleanup and exit
    ************************************************************/

    if (fail_count) {
        return(1);
    }

    return(0);
}
