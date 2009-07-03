#!/bin/sh

#=============================================================
# description: 
#   Converts a ccruncher source distribution to binary 
#   distribution removing autotools files, src directory, etc.
#
# repository version:
#   $Rev: 485 $
#
# dependencies:
#   shell
#
# retcodes:
#   0    : OK
#   other: KO
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
CCRUNCHER=`dirname $0`/..
progname=src2bin.sh
numversion="1.3"
svnversion="R465M"

#-------------------------------------------------------------
# checking arguments
#-------------------------------------------------------------
if [ "$1" != "-y" ]; then
  echo "This script removes all ccruncher development files";
  echo -n "Do you want to continue? (y/n): ";
  read WISH;
  if [ "$WISH" != "y" ]; then
    exit 1;
  fi
fi

#-------------------------------------------------------------
# drops development files
#-------------------------------------------------------------
if [ -f $CCRUNCHER/build/ccruncher ]; then
  cp $CCRUNCHER/build/ccruncher $CCRUNCHER/bin/;
fi
rm -f  $CCRUNCHER/ccruncher.sln
rm -f  $CCRUNCHER/ccruncher.vcproj
rm -f  $CCRUNCHER/aclocal.m4;
rm -f  $CCRUNCHER/config*;
rm -f  $CCRUNCHER/depcomp;
rm -f  $CCRUNCHER/install-sh;
rm -f  $CCRUNCHER/missing;
rm -rf $CCRUNCHER/autom4te.cache;
rm -f  $CCRUNCHER/Makefile*;
rm -rf $CCRUNCHER/src;
rm -rf $CCRUNCHER/build;
rm -rf $CCRUNCHER/deps;
rm -f  $CCRUNCHER/doc/INSTALL;
rm -f  $CCRUNCHER/bin/src2bin.sh;

