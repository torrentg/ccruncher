#!/bin/sh

#=============================================================
# description: 
#   Use this script to roll creditcruncher versions numbers
#   Caution: this is a tool for developers
#
# retcodes:
#   0    : OK
#   other: KO
#
#-------------------------------------------------------------
#
# 2005/05/29 - Gerard Torrent [gerard@fobos.generacio.com]
#   . initial release
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
progname=rollversion.sh
gloversion=X.X
retcode=0
cver=false
csvn=false

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options] <file>

  description:
    $progname is a shell script to roll version numbers in
    CreditCruncher project. This script is only used by 
    developers.
  options
    -s       update svnversion tag
    -g num   update global version identifier
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -s
    $progname -g "1.0"
    $progname -s -g "1.0"

_EOF_

}

#-------------------------------------------------------------
# copyright function
#-------------------------------------------------------------
copyright() {

  cat << _EOF_

   $progname is Copyright (C) 2003-2005 Gerard Torrent and licensed
     under the GNU General Public License, version 2. more info at
               http://www.generacio.com/ccruncher

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

  while getopts 'sg:h' opt
  do
    case $opt in
      s) csvn=true;;
      g) cver=true;
         gloversion=$OPTARG;;
      h) usage; 
         exit 0;;
     \?) echo "unknow option. use -h for more information"; 
         exit 1;;
      *) echo "unexpected error parsing arguments. Please report this bug sending";
         echo "$progname version and arguments at gerard@fobos.generacio.com";
         exit 1;;
    esac
  done

  shift `expr $OPTIND - 1`

  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      echo "unexpected argument: $arg";
      echo "please, try again";
      exit 1;
    done
  fi

}

#-------------------------------------------------------------
# replace function
# $1: regexp string to apply
# $2: file to be replaced
#-------------------------------------------------------------
replace() {

  sed -e "$1" $2 > /tmp/$progname.aux

  if [ $? != 0 ]; then
    echo "problems replacing file $2";
    retcode=`expr $retcode + 1`;
  else
    cat /tmp/$progname.aux > $2;
    echo "$2 updated";
  fi

}

#-------------------------------------------------------------
# getPath
#-------------------------------------------------------------
getPath() {

  CCRUNCHERPATH=.

  #TODO; retrieve path from directori script
}

#-------------------------------------------------------------
# getSvnVersion function
#-------------------------------------------------------------
getSvnVersion() {

  svnversion=R$(svnversion $CCRUNCHERPATH);

  if [ $? != 0 ]; then
    echo "problems retrieving svnversion";
    exit 1;
  fi

  #TODO; check that is a pure version (non exist ':' or 'M' or 'S')
}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

copyright;

getPath;

if [ "$csvn" = "true" ]; then
  getSvnVersion;
  replace "s/\#define\ *SVNVERSION\ *\".*\"/\#define SVNVERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h.in
  replace "s/\#define\ *SVNVERSION\ *\".*\"/\#define SVNVERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/plotdata
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/plotmtrans
fi

if [ "$cver" = "true" ]; then
  replace "s/AC_INIT(ccruncher,\(.*\),\ *gerard@mail.generacio.com)/AC_INIT(ccruncher, $gloversion, gerard@mail.generacio.com)/g" $CCRUNCHERPATH/configure.in
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/plotdata
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/plotmtrans
  replace "s/VERSION=\"\(.*\)\"/VERSION=\"$gloversion\"/g" $CCRUNCHERPATH/bin/makedistbin.sh
  echo "you need to run autoconf to take effect some changes";
fi

if [ $retcode != 0 ]; then
  echo "";
  echo "panic: retrieve version from repository now!";
fi

#TODO: add version changes in HTML's

exit $retcode;
