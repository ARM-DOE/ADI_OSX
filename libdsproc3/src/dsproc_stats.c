/*******************************************************************************
*
*  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
*
********************************************************************************
*
*  Author:
*     name:  Krista Gaustad
*     phone: (509) 375-5950
*     email: krista.gaustad@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 9121 $
*    $Author: gaustad $
*    $Date: 2011-09-25 21:07:41 +0000 (Sun, 25 Sep 2011) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_transform.c
 *  Tansformation Functions.
 */

#include "dsproc3.h"
#include "dsproc_private.h"
#include <math.h>

/*******************************************************************************
 *  Private Functions
 */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */


/***************************************************************************************
 *  This function calculates the mean and standard deviation 
 *  for given sample of a CDSVar around a window of either (1) time around 
 *  the sample, or (2) for a number of good samples around the sample.
 *  
 *
 *  @param  input:data - an array of values for which to calculate mean and stdev
 *  @param  input:nsamples - number of samples in data array
 *  @param  input:missing_value - misisng value for variable
 *  @param  input:time - time array for data values
 *  @param  input:current_sample - current sample for which to calcualte stdev and mean
 *  @param  input:window - number of seconds or seconds before and after current sample
 *                   whether value is seconds or samples is indicated by flg_window_time
 *  @param  input:flg_window_time - flag to indicate whether window is of time = 1, or good samples = 0
 *  @param  input:flg_include_current_sample - flag to indicate whether or not to include the current 
                     sample in determine mean and stdev. = 1 to include, = 0 to not. 
 *  @param  input:min_samples - minimum number of samples needed to calculate mean, stdev
 *  @param  output:mean - the mean of window of values around current sample
 *  @param  output:stdev - the stdev of window of values around current sample
 *
 *  @return
 *    The number of samples in the window that were used to calcaulate the mean and stdev
 *    0 
 ****************************************************************************************/
int get_mean_and_stdev(double *data, int nsamples, double missing_value, time_t *time, 
          int current_sample, int window, int flg_window_time, 
          int flg_include_current_sample, int min_samples, double *mean, double *stdev)
{  
  int count = 0, scount = 0, i = 1;
  double sum = 0, dummy = 0;

  /* include current sample in sum if indicated  */
  if(flg_include_current_sample == 1 && (data[current_sample] != missing_value)) {
    sum = sum + data[current_sample];
    count ++;
  }
 
  /* If window is in seconds find sum of value from time falling from minus */ 
  /* the window to time of current sample */
  if(flg_window_time == 1) {
    while (((current_sample - i) >= 0) && ((time[current_sample] - time[current_sample-i]) <= window)) {
      if(data[current_sample-i] != missing_value) {
        sum = sum + data[current_sample -i];
        count++;
      }
      i++;
    } 
  }
  /* if window is in samples then find sum from window good samples before current sample */
  /* to current sample */
  if(flg_window_time == 0) { 

    while( ((current_sample - i) >= 0) && (scount < window ) ) {
      if(data[current_sample - i] != missing_value) {
        sum = sum + data[current_sample - i];
        count ++;
        scount ++;
      }
      i++;
    }
  }


  /* If window is in seconds find sum of values from time of current sample forward */ 
  /* window number of seconds  */
  i = 1;
  if(flg_window_time == 1) {
    while (((current_sample + i) >= (nsamples - 1)) && ((time[current_sample + i] - time[current_sample]) <= window)) {
      if(data[current_sample + i] != missing_value) {
        sum = sum + data[current_sample + i];
        count++;
      }
      i++;
    } 
  }
  /* if window is in samples thne find sum of current sample forward window samples of good data */
  if(flg_window_time == 0) { 

    scount = 0;
    while( ((current_sample + i) <= (nsamples - 1)) && (scount < window ) ) {
      if(data[current_sample + i] != missing_value) {
        sum = sum + data[current_sample + i];
        count ++;
        scount ++;
      }
      i++;
    }
  }

  if(count <=  min_samples) {
    *mean = missing_value;
    *stdev = missing_value;
    return(count);
  }
  else if(count == 1) {
    *mean = sum;
    *stdev = 0;  
    return(count);
  }
  else {
      /* There are enough samples to calculate mean and stdev */
      *mean = sum/count;
    
      sum = 0;
      count = 0;
      scount = 0;

     /* Calculate standard deviation */
     if(flg_include_current_sample == 1 && (data[current_sample] != missing_value)) {
        dummy = data[current_sample] - *mean; 
        sum += dummy*dummy;
        count ++;
     }
  
     /* If window is in seconds find sum of value from time falling from minus */ 
     /* the window to time of current sample */
     i = i;
     if(flg_window_time == 1) {
       while (((current_sample - i) >= 0) && ((time[current_sample] - time[current_sample-i]) <= window)) {
         if(data[current_sample-i] != missing_value) {
           dummy = data[current_sample - i] - *mean;
           sum += dummy*dummy;
           count++;
         }
         i++;
       } 
     }
     /* if window is in samples then find sum from window good samples before current sample */
     /* to current sample */
     if(flg_window_time == 0) { 

       while( ((current_sample - i) >= 0) && (scount < window ) ) {
         if(data[current_sample - i] != missing_value) {
           dummy = data[current_sample - i] - *mean;
           sum += dummy*dummy;
           count ++;
           scount ++;
         }
         i++;
       }
     }

     /* If window is in seconds find sum of values from time of current sample forward */ 
     /* window number of seconds  */
     i = 1;
     if(flg_window_time == 1) {
       while (((current_sample + i) >= (nsamples - 1)) && ((time[current_sample + i] - time[current_sample]) <= window)) {
         if(data[current_sample + i] != missing_value) {
           dummy = data[current_sample + i] - *mean;
           sum += dummy*dummy;
           count++;
         }
         i++;
       } 
     }
     /* if window is in samples thne find sum of current sample forward window samples of good data */
     if(flg_window_time == 0) { 

       scount = 0;
       while( ((current_sample + i) <= (nsamples - 1)) && (scount < window ) ) {
         if(data[current_sample + i] != missing_value) {
           dummy = data[current_sample + i] - *mean;
           sum += dummy*dummy;
           count ++;
           scount ++;
         }
         i++;
       }
     }
  
     if(sum == 0) *stdev = 0; 
     else *stdev = sqrt(sum / (count - 1));
     return(count);
  } /* end more than 1 sample */


}


/***************************************************************************************
 *  Given a CDSVar structure, this function loops across its values three tims applying a 
 *  time variaiblity test to each sample in each of the three iterations.
 *  The time variability test finds the stdev for each sample about a window of good samples around that sample.
 *  It then repeats the calculation of stdev for the sample about a window of good samples, 
 *  but excludes the sample itself from the calculation.
 *  If the absolute value of the stdev of all with the current smaple minus the stdev of all samples without the 
 *  current samples is less than then provided min_value, then the current sample is set to -9999.

 *  @param  input:var - a structure of the input variable
 *  @param  input:qc_var - a structure of the input qc variable
 *  @param  input:half_window - number of samples of good data that must be found before and after current sample.
 *  @param  input:time_variability_min- min value for time variabilty test 
 *  @param  input:qc_time_variability_bit - the qc bit for this var that documents the time_variabilty check
 *
 *  @return
 *    -  1 if the test is successfully applied and variable data updated
 *    -  0 if an error occurred

 ****************************************************************************************/
int stat_time_variability_check(CDSVar *var, CDSVar *qc_var, int half_window, 
                           int time_variability_min, int qc_time_variability_bit)
{  
  int iter, samp, nsamples; 
  size_t att_length;
  int *qc_data, nsamples_stats;
  double *data, *tmp_missing, missing_value, stdev_all, mean_all, stdev, mean; 
  CDSVar *time_var;
  CDSAtt *missing_att;
  time_t *time;


/* KLG Check all return values and insert error handling */

  qc_data = NULL;
  /* Get the time var */
  time_var = dsproc_get_time_var((void *)var);
  time = cds_copy_array(time_var->type, time_var->sample_count, time_var->data.vp, CDS_DOUBLE, NULL,
                        0, NULL, NULL, NULL, NULL, NULL,NULL); 

  /* Copy the variable's data to an array of doubles */
  /* use the time var length to ensure proper value independent of number of dimensions*/
  nsamples = time_var->sample_count;
  data = cds_copy_array(var->type, nsamples, var->data.vp, CDS_DOUBLE, NULL,  
                        0, NULL, NULL, NULL, NULL, NULL,NULL); 

  /* Get the qc variable data*/
  if(qc_var) qc_data = cds_copy_array(qc_var->type, nsamples, qc_var->data.vp, CDS_INT, NULL,  
                        0, NULL, NULL, NULL, NULL, NULL,NULL); 

  /* get the missing value of the variable as type double*/
  missing_att = cds_get_att(var, "missing_value");

  if(!missing_att) missing_value = -9999.0; 
  else { 
    tmp_missing = (double *)cds_get_att_value(missing_att, CDS_DOUBLE, &att_length, NULL); 
    missing_value = *tmp_missing;
    free(tmp_missing);
  }

  for(iter = 0; iter < 3; iter++) {

     /* Loop over samples */
     for(samp = 0; samp < nsamples; samp++) {

       /* Only test good samples */
       if(data[samp] != missing_value) {
         /* find mean and stdev for this sample, including this sample */
         /* for this function the window is in number of good samples */   
         nsamples_stats = get_mean_and_stdev(data, nsamples, missing_value, time, 
                   samp, half_window,0, 1, 3, &mean_all, &stdev_all);

         /* find mean and stdev for this sample, including this sample */
         if(nsamples_stats > 3) { 
           nsamples_stats = get_mean_and_stdev(data, nsamples, missing_value, time, 
                    samp, half_window,0, 0, 3, &mean, &stdev);
         }

  /* KLG make sure this works for cases when stdev is returned as zero */
         if(nsamples_stats >= 2 * (half_window)) {
            /* If absolute value of stdev of all samples minus stdev excluding current sample */
            /* is greater than or equal to the minimum time variability allowed for this variable */
            /* then set value of the data to  missing and update its qc flag*/
            if(stdev_all != missing_value && stdev != missing_value && (fabs(stdev_all - stdev) >= time_variability_min) ) {
                data[samp] = missing_value;
                if(qc_var)qc_data[samp] = qc_data[samp] | qc_time_variability_bit;
            }
         }
      } /* end sample not missing */

     }
  } /* end 3 iteration loop */

  /* Load the updated data to var and qc_var */
/* KLG make sure data and qc_data are not null bvefore calling cds_put_var_data */
/* KLG also update to use set_var_data instead of put_var_data */
  if(cds_put_var_data(var, 0, nsamples, CDS_DOUBLE, data) == NULL) { 
         ERROR(DSPROC_LIB_NAME, "Problem updating var %s with time variability results\n", var->name);
         return(0);
  }
  if(cds_put_var_data(qc_var, 0, nsamples, CDS_INT, qc_data) == NULL) {
         ERROR(DSPROC_LIB_NAME, "Problem updating qc_var %s with time variability results\n", qc_var->name);
         return(0);
  }

  free(time);
  free(data);
  free(qc_data);

  return(1);
}

/***************************************************************************************
 *  Given a CDSVar structure, this function determines whether any of its samples are 
 *  outliers, and if so these values are set to missing.
 *  Outliers are identifed by cases where the absolute value of the actual value minus its mean  
 *  is greater than 3*stdev.

 *  @param  input:var - a structure of the input variable
 *  @param  input:qc_var - a structure of the input's qc variable
 *  @param  input:min_samples - minimum number of samples needed to calculate mean 
 *  @param  input:qc_outlier_bit - the qc bit for this var that documents the outlier check
 *  @param  input:n_stdev_outlier - the number of standard deviations that the sample value may 
 *                deviate from the variable's mean
 *
 *  @return
 *    -  1 if the test is successfully applied and variable data updated
 *    -  0 if the test could not be performed because of too few good samples
 *    -  -1 if an error occurred

 ****************************************************************************************/
int stat_outlier_check(CDSVar *var, CDSVar *qc_var, int min_samples, int qc_outlier_bit, int n_stdev_outlier)
{  
  int  samp, nsamples, *qc_data, count=0, status;
  size_t att_length;
  double *data, missing_value, *tmp_missing, diff, stdev=0, mean=0, sum=0; 
  CDSVar *time_var;
  CDSAtt *missing_att;
  double var_missing;
  int qc_missing;

  /* Get the time var */
  time_var = dsproc_get_time_var((void *)var);
  if(!time_var) {
     WARNING( DSPROC_LIB_NAME,
            "Could not get time variable for varaible %s= %d\n", var->name);
    return(-1);
  }

  /* get number of samples based on time var */
  nsamples = time_var->sample_count;

  /* Copy the variable's data and qc_data to an array of doubles and int respectively */
/*  status = cds_get_var_missing_values(var, (void *)&missing); */
  data = cds_get_var_data(var, CDS_DOUBLE, 0, NULL, &var_missing, NULL);
  if(!data) {
     WARNING( DSPROC_LIB_NAME,
            "Could not data for varaible %s= %d\n", var->name);
    return(-1);
  }

/*  status = cds_get_var_missing_values(var, (void *)&missing); */
  if(qc_var) {
     qc_data = cds_get_var_data(qc_var, CDS_INT, 0, NULL, &qc_missing, NULL);
    if(!qc_data) {
       WARNING( DSPROC_LIB_NAME,
            "Could not qc data for varaible %s= %d\n", var->name);
      return(-1);
     }
  }

  /* get the missing value of the variable as type double*/
/* KLG not working 
  if(var) {
    status = cds_get_var_missing_values(var,(void *)&tmp_missing); 
    if(status < 0) {
       WARNING( DSPROC_LIB_NAME,
           "Error in getting missing value for variable %s= %d\n", var->name);
       return(-1);
    }
    else if(status == 0) missing_value = -9999;
    else missing_value = (double) *tmp_missing;
  }
*/

  missing_att = cds_get_att(var, "missing_value");

  if(!missing_att) missing_value = -9999.0;
  else { 
    tmp_missing = (double *)cds_get_att_value(missing_att, CDS_DOUBLE, &att_length, NULL); 
    missing_value = *tmp_missing;
    free(tmp_missing);
  }

  /* Find mean */
  count = 0;
  for(samp = 0; samp < nsamples; samp++) {

    /* Only use good samples */
    if(data[samp] != missing_value) {
      sum = sum + data[samp];
      count ++;
    }
  }

  if(count <=  min_samples) {
    mean = missing_value;
    stdev = missing_value;
    return(0);
  }
  else if(count == 1) {
    mean = sum;
    stdev = 0;
  }
  else {
    mean = sum/count;

    /* Loop over samples finding the standard deviation*/
    diff = 0;
    count = 0;
    sum = 0;
    for(samp = 0; samp < nsamples; samp++) {

       /* Only test good samples */
       if(data[samp] != missing_value) {
          diff = fabs(data[samp] - mean);
          sum += diff * diff;
          count ++;
        }
    }
    stdev = sqrt(sum/((double)count-1.));

  } /* end else count > 1 */
  /* Loop over the samples again and test whether the actual minus mean is */
  /* greather than 'x' times the standard deviation. If it is replace the value*/  
  /* with missing and set the appropriate qc bit */
  count = 0;
  for(samp = 0; samp < nsamples; samp++) {
     if(data[samp] != missing_value) {
        if( fabs(data[samp] - mean) > n_stdev_outlier*stdev) {
/* KLG debug */
printf("var %s fails outlier test %f, on sample %d\n", var->name, data[samp], samp);
           data[samp] = missing_value; 
           qc_data[samp] = qc_data[samp] | qc_outlier_bit;
           count ++;
 
        }
     }
  } /* end samp < nsamples */

  /* Load the updated data to var and qc_var */
/*KLG update  make sure not null before calling put_var_data also update to use set_var_data */
  if(count > 0) { 
    if(cds_put_var_data(var, 0, nsamples, CDS_DOUBLE, data) == NULL) { 
         ERROR(DSPROC_LIB_NAME, "Problem updating var %s with outlier test results\n", var->name);
         return(-1);
    }
    if(cds_put_var_data(qc_var, 0, nsamples, CDS_INT, qc_data) == NULL) {
         ERROR(DSPROC_LIB_NAME, "Problem updating qc_var %s with outlier test results\n", qc_var->name);
         return(-1);
    }
  }

  free(data);
  free(qc_data);

  return(1);

}

