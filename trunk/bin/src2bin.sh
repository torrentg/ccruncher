#!/bin/sh

#=============================================================
# description: 
#   Converts a ccruncher source distribution to binary 
#   distribution removing autotools files, src directory, etc.
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
numversion="2.2"
svnversion="R957"

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
# copy executables to bin directory (if exists)
#-------------------------------------------------------------
cp `find $CCRUNCHER/build -name ccruncher-\* -type f -executable` $CCRUNCHER/bin/;

#-------------------------------------------------------------
# drops development files
#-------------------------------------------------------------
rm -f  $CCRUNCHER/ccruncher*.pro;
rm -f  $CCRUNCHER/ccruncher*.pro.user;
rm -f  $CCRUNCHER/qrc_*.cpp;
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
