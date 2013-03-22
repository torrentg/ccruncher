#!/bin/sh

#=============================================================
# description: 
#   Use this script to roll CCruncher version numbers
#   Caution: this is a tool for developers
#
# dependences:
#   shell, svnversion
#
# retcodes:
#   0    : OK
#   other: KO
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
    CCruncher project. This script is only used by developers.
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

   $progname is Copyright (C) 2004-2013 Gerard Torrent and licensed
     under the GNU General Public License, version 2. More info at
                   http://www.ccruncher.net

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
         echo "$progname version and arguments at gtorrent@ccruncher.net";
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

  #svnversion=R$(svnversion $CCRUNCHERPATH | sed "s/^\([0-9]\+\)[MSP]*\(:.*\)\?/\1/g");
  svnversion=R$(svn info http://www.ccruncher.net/svn/ | awk '/Revision:/ { print $2 }');
 
  if [ $? != 0 ]; then
    echo "problems retrieving svnversion";
    exit 1;
  fi
}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

copyright;

getPath;

if [ "$csvn" = "true" ]; then
  getSvnVersion;
  replace "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/configure.in
  replace "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h.in
  replace "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/makedist.sh
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/tex2png.sh
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/src2bin.sh
  replace "s/svnversion=\".*\"/svnversion=\"$svnversion\"/g" $CCRUNCHERPATH/bin/tests.sh
fi

if [ "$cver" = "true" ]; then
  replace "s/AC_INIT(ccruncher,\(.*\),\ *gtorrent@ccruncher.net)/AC_INIT(ccruncher, $gloversion, gtorrent@ccruncher.net)/g" $CCRUNCHERPATH/configure.in
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/makedist.sh
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/tex2png.sh
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/src2bin.sh
  replace "s/numversion=\"\(.*\)\"/numversion=\"$gloversion\"/g" $CCRUNCHERPATH/bin/tests.sh
  echo "you need to run autoconf to take effect some changes";
fi

#TODO: add version changes in HTML's

exit $retcode;
