#!/bin/sh

#=============================================================
# description: 
#   Use this script to roll creditcruncher versions numbers
#   Caution: this is a tool for developers
#
# repository version:
#   $Rev$
#
# dependences:
#   shell, svnversion
#
# retcodes:
#   0    : OK
#   other: KO
#
#-------------------------------------------------------------
#
# 2005/05/29 - Gerard Torrent [gerard@mail.generacio.com]
#   . initial release
#
# 2005/09/04 - Gerard Torrent [gerard@mail.generacio.com]
#   . added script report
#
# 2005/09/06 - Gerard Torrent [gerard@mail.generacio.com]
#   . added tex files
#
# 2005/09/10 - Gerard Torrent [gerard@mail.generacio.com]
#   . removed plotdata and plotmtrans scripts
#
# 2005/09/27 - Gerard Torrent [gerard@mail.generacio.com]
#   . changed CCRUNCHERPATH asignation
#
# 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
#   . added Rev svn:keyword
#   . added dependences paragraph at comments
#
# 2008/10/18 - Gerard Torrent [gerard@mail.generacio.com]
#   . script report renamed to ccreport.sh
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

   $progname is Copyright (C) 2003-2008 Gerard Torrent and licensed
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
         echo "$progname version and arguments at gerard@mail.generacio.com";
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

  CCRUNCHERPATH=`dirname $0`/..

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
  replace "s/\#define\ *SVNVERSION\ *\".*\"/\#define SVNVERSION \"$svnversion\"/g" $CCRUNCHERPATH/configure.in
  replace "s/\#define\ *SVNVERSION\ *\".*\"/\#define SVNVERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h.in
  replace "s/\#define\ *SVNVERSION\ *\".*\"/\#define SVNVERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/ccreport.sh
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/makedist.sh
  replace "s/\\\def\\\svnversion{.*}/\\\def\\\svnversion{$svnversion}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
fi

if [ "$cver" = "true" ]; then
  replace "s/AC_INIT(ccruncher,\(.*\),\ *gerard@mail.generacio.com)/AC_INIT(ccruncher, $gloversion, gerard@mail.generacio.com)/g" $CCRUNCHERPATH/configure.in
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/ccreport.sh
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/makedist.sh
  replace "s/\\\def\\\numversion{.*}/\\\def\\\numversion{$gloversion}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
  echo "you need to run autoconf to take effect some changes";
fi

if [ $retcode != 0 ]; then
  echo "";
  echo "panic: retrieve version from repository now!";
fi

#TODO: add version changes in HTML's

exit $retcode;
