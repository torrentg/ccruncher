#!/bin/sh	

# *************************************************************
#
# Set xerces environement variables
#
# execute next command (type point + space + scriptname)
#   . ./xercesenv.sh
#
# *************************************************************

XERCESROOT=$HOME/temp/xerces-c_2_6_0-redhat_80-gcc_32

export PATH=$PATH:$XERCESROOT/bin
export LD_LIBRARY_PATH=$XERCESROOT/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=$XERCESROOT/lib:$LIBRARY_PATH
export XERCESC_NLS_HOME=$XERCESCROOT/msg
