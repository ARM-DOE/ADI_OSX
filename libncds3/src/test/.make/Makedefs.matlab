################################################################################
# RCS INFORMATION:
#   $Author: younkin $
#   $Locker:  $
#   $Revision: 1.5 $
#   $Date: 2008/05/22 18:46:06 $
#   $Name: armcm-swawtdev-2#4-0 $
#   $State: armcm-swawtdev-2.4-0 $
#
# PURPOSE:
#   Define template information for the makefile generator for those who
#   need to use Matlab software.
#
# DESCRIPTION:
#   The variables below define alternate locations for the files that matlab
#   looks for when doing mex and mcc.  If these values are not specified, 
#   matlab looks in a user's home directory in directory called .matlab, if no
#   files are found there, then the system ones are used.  By using the -f
#   option on the mex and mcc command lines, matlab looks specifically for these
#   files.  Since they are used by all matlab developers, they are specified
#   here once.
################################################################################

#MEXOPTS = -f /home/sbeus/.matlab/R14SP3/mexopts.sh
#MCCOPTS = -f /home/sbeus/.matlab/R14SP3/mbuildopts.sh
MEXOPTS = -f $(MATLAB_DIR)/bin/mexopts.sh
MCCOPTS = -f $(MATLAB_DIR)/bin/mbuildopts.sh -R -nojvm -R -nodisplay
