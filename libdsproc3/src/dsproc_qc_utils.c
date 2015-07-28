/*******************************************************************************
*
*  COPYRIGHT (C) 2014 Battelle Memorial Institute.  All Rights Reserved.
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
*    $Revision: 57300 $
*    $Author: ermold $
*    $Date: 2014-10-06 20:03:42 +0000 (Mon, 06 Oct 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_qc_utils.c
 *  QC Utilities.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */

/*******************************************************************************
 *  Static Data and Functions Visible Only To This Module
 */

/**
 *  Get the QC mask for a specific QC assessment.
 *
 *  @param  assessment  - the QC assessment to get the mask for
 *                        (i.e. "Bad" or "Indeterminate")
 *  @param  prefix      - attribute name prefix before the bit number
 *  @param  natts       - number of attributes in the list
 *  @param  atts        - list of attributes
 *  @param  nfound      - output: total number of qc assessment attributes found
 *  @param  max_bit_num - output: highest QC bit number used
 *
 *  @retval  qc_mask  The QC mask for the specified QC assessment.
 */
static unsigned int _dsproc_get_qc_assessment_mask(
    const char  *assessment,
    const char  *prefix,
    int          natts,
    CDSAtt     **atts,
    int         *nfound,
    int         *max_bit_num)
{
    CDSAtt      *att;
    size_t       prefix_length;
    size_t       name_length;
    unsigned int qc_mask;
    int          bit_num;
    int          ai;

    if (nfound)      *nfound      = 0;
    if (max_bit_num) *max_bit_num = 0;

    qc_mask       = 0;
    prefix_length = strlen(prefix);

    for (ai = 0; ai < natts; ++ai) {

        att = atts[ai];

        /* Check if this is a QC assessment attribute */

        if ((att->type != CDS_CHAR) ||
            (strncmp(att->name, prefix, prefix_length) != 0)) {

            continue;
        }

        name_length = strlen(att->name);
        if (name_length < prefix_length + 12 ||
            strcmp( (att->name + name_length - 10), "assessment") != 0) {

            continue;
        }

        bit_num = atoi(att->name + prefix_length);
        if (!bit_num) continue;

        /* Keep track of the number of assessment attributes found,
         * and the highest bit number used if requested */

        if (nfound) *nfound += 1;

        if (max_bit_num &&
            *max_bit_num < bit_num) {

            *max_bit_num = bit_num;
        }

        /* Update the qc_mask if this is the assessment we are looking for */

        if (strcmp(att->value.cp, assessment) != 0) {
            continue;
        }

        qc_mask |= 1 << (bit_num-1);
    }

    return(qc_mask);
}

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/**
 *  Get the QC mask for a specific QC assessment.
 *
 *  This function will use the bit assessment attributes to create a mask
 *  with all bits set for the specified assessment value.  It will first
 *  check for field level bit assessment attributes, and then for the global
 *  attributes if they are not found.
 *
 *  @param  qc_var      - pointer to the QC variable
 *  @param  assessment  - the QC assessment to get the mask for
 *                        (i.e. "Bad" or "Indeterminate")
 *  @param  nfound      - output: total number of qc assessment attributes found
 *  @param  max_bit_num - output: highest QC bit number used
 *
 *  @retval  qc_mask  The QC mask for the specified QC assessment.
 */
unsigned int dsproc_get_qc_assessment_mask(
    CDSVar     *qc_var,
    const char *assessment,
    int        *nfound,
    int        *max_bit_num)
{
    unsigned int qc_mask;
    CDSGroup    *dataset;
    int          _nfound;

    if (!nfound) nfound = &_nfound;

    qc_mask = _dsproc_get_qc_assessment_mask(
        assessment,
        "bit_",
        qc_var->natts,
        qc_var->atts,
        nfound,
        max_bit_num);

    if (!nfound) {

        dataset = (CDSGroup *)qc_var->parent;

        qc_mask = _dsproc_get_qc_assessment_mask(
            assessment,
            "qc_bit_",
            dataset->natts,
            dataset->atts,
            nfound,
            max_bit_num);
    }

    return(qc_mask);
}

/*******************************************************************************
 *  Public Functions
 */

/**
 *  Consolidate all QC bits for a variable into bad or indeterminate.
 *
 *  This function will consolidate all the bad and indeterminate QC bits in
 *  the input QC variable into a single bad or indeterminate bit in the output
 *  QC variable. By default (reset == 0) the bit values of the output variable
 *  will be updated without affecting any of the bits that may have already been
 *  set. The output QC values can be reset to zero before setting the bad or
 *  indeterminate bit by specifying 1 for the reset parameter.
 *
 *  The bad_mask can be determined using the bit assessment attributes by
 *  passing in 0 for it's value (see dsproc_get_bad_qc_mask()).
 *
 *  The bad and indeteminate flag values are *not* the bit numbers. They are
 *  the actual numeric values of the bits. For example:
 *
 *    - bit_1 = pow(2,0) = (1 << 0) = 0x01 =   1
 *    - bit_2 = pow(2,1) = (1 << 1) = 0x02 =   2
 *    - bit_3 = pow(2,2) = (1 << 2) = 0x04 =   4
 *    - bit_4 = pow(2,3) = (1 << 3) = 0x08 =   8
 *    - bit_5 = pow(2,4) = (1 << 4) = 0x10 =  16
 *    - bit_6 = pow(2,5) = (1 << 5) = 0x20 =  32
 *    - bit_7 = pow(2,6) = (1 << 6) = 0x40 =  64
 *    - bit_8 = pow(2,7) = (1 << 7) = 0x80 = 128
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  in_qc_var  - pointer to the input QC variable
 *  @param  bad_mask   - mask used to determine bad QC values in the input QC,
 *                       or 0 to indicate that the bit assessment attributes
 *                       should be used to determine it.
 *  @param  out_qc_var - pointer to the output QC variable
 *  @param  bad_flag   - QC flag to use for bad values
 *  @param  ind_flag   - QC flag to use for indeterminate values
 *  @param  reset      - specifies if the output QC values should be reset
 *                       to zero (0 = false, 1 = true)
 *
 *  @retval  1 if successful
 *  @retval  0 if a fatal error occurred
 */
int dsproc_consolidate_var_qc(
    CDSVar       *in_qc_var,
    unsigned int  bad_mask,
    CDSVar       *out_qc_var,
    unsigned int  bad_flag,
    unsigned int  ind_flag,
    int           reset)
{
    size_t  sample_size;
    size_t  sample_count;
    int     count;
    int    *inp;
    int    *outp;

    if (!bad_mask) bad_mask = dsproc_get_bad_qc_mask(in_qc_var);

    /* Make sure the QC variables have interger data types */

    if (in_qc_var->type != CDS_INT) {

        ERROR( DSPROC_LIB_NAME,
            "Could not consolidate QC bits\n"
            " -> invalid data type for input QC variable: %s\n",
            cds_get_object_path(in_qc_var));

        dsproc_set_status(DSPROC_EQCVARTYPE);
        return(0);
    }

    if (out_qc_var->type != CDS_INT) {

        ERROR( DSPROC_LIB_NAME,
            "Could not consolidate QC bits\n"
            " -> invalid data type for output QC variable: %s\n",
            cds_get_object_path(out_qc_var));

        dsproc_set_status(DSPROC_EQCVARTYPE);
        return(0);
    }

    /* Allocate memory for the output variable if necessary */

    sample_size  = dsproc_var_sample_size(in_qc_var);
    sample_count = in_qc_var->sample_count;

    if (out_qc_var != in_qc_var) {

        /* Make sure the output QC variable has the correct shape */

        if (dsproc_var_sample_size(out_qc_var) != sample_size) {

            ERROR( DSPROC_LIB_NAME,
                "Could not consolidate QC bits\n"
                " -> QC variables do not have the same shape\n"
                " -> input:  %s\n"
                " -> output: %s\n",
                cds_get_object_path(in_qc_var),
                cds_get_object_path(out_qc_var));

            dsproc_set_status(DSPROC_EQCVARDIMS);
            return(0);
        }

        /* Allocate memory for the output variable's data values
         * if necessary. */

        if (out_qc_var->sample_count < sample_count) {

            if (!dsproc_init_var_data(out_qc_var,
                out_qc_var->sample_count, sample_count, 0)) {
                return(0);
            }
        }
    }

    /* Map all QC bits to bad or indeterminate */

    count = (int)(sample_count * sample_size) + 1;
    inp   = in_qc_var->data.ip;
    outp  = out_qc_var->data.ip;

    if (reset) {
        for ( ; --count; ++inp, ++outp) {
            if (*inp) *outp = (*inp & bad_mask) ? bad_flag : ind_flag;
            else      *outp = 0;
        }
    }
    else {
        for ( ; --count; ++inp, ++outp) {
            if (*inp) *outp |= (*inp & bad_mask) ? bad_flag : ind_flag;
        }
    }

    return(1);
}

/**
 *  Get the QC mask used to determine bad QC values.
 *
 *  This function will use the bit assessment attributes to create a mask
 *  with all bits set for bad assessment values. It will first check for
 *  field level bit assessment attributes, and then for the global attributes
 *  if they are not found.
 *
 *  @param   qc_var   pointer to the QC variable
 *
 *  @retval  qc_mask  the QC mask with all bad bits set.
 */
unsigned int dsproc_get_bad_qc_mask(CDSVar *qc_var)
{
    return(dsproc_get_qc_assessment_mask(qc_var, "Bad", NULL, NULL));
}
