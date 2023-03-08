#!/bin/sh

#=============================================================
# description: 
#   Use this script to roll CCruncher version numbers
#   Caution: this is a tool for developers
#
# dependences:
#   git, awk, sed, help2man
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
version=X.Y.Z
retcode=0
cman=false
cver=false
cgit=false

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options]

  description:
    $progname is a shell script to roll version numbers in
    CCruncher project. This script is only used by developers.
  options
    -m       update manual pages
    -s       update gitrev tag
    -g num   update gitrev and global version identifier tags
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -s
    $progname -g "2.5.0"

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  if [ $# -eq 0 ]; then
    echo "error: no arguments supplied.";
    echo "Use -h for more information";
    exit 1;
  fi

  OPTIND=0

  while getopts 'msg:h' opt
  do
    case $opt in
      m) cman=true;;
      s) cgit=true;;
      g) cver=true;
         version=$OPTARG;;
      h) usage; 
         exit 0;;
     \?) echo "error: unknow option.";
         echo "Use -h for more information"; 
         exit 1;;
      *) echo "panic: unexpected error parsing arguments.";
         exit 1;;
    esac
  done

  shift `expr $OPTIND - 1`

  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      echo "error: unexpected argument '$arg'";
      echo "Please, try again";
      exit 1;
    done
  fi

}

#-------------------------------------------------------------
# getPath
#-------------------------------------------------------------
getPath() {

  CCRUNCHERPATH=`dirname $0`/..

}

#-------------------------------------------------------------
# getGitRev function
#-------------------------------------------------------------
getGitRev() {

  gitrev=$(git rev-parse --short HEAD)
 
  if [ $? != 0 ]; then
    echo "error: problems retrieving gitrev";
    exit 1;
  fi
}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

getPath;

# date en format dd-mmm-yyyy (eg. 01-Jan-2014)
# observe month 1st letter capitalized
LANG=en_US.utf8
DAY=$(date +%d);
MONTH=$(date +%b);
MONTH=${MONTH^}
YEAR=$(date +%Y);
curdate=$DAY-$MONTH-$YEAR

# update gitrev tag
if [ "$cgit" = "true" ] || [ "$cver" = "true" ]; then
  getGitRev;
  sed -i -e "s/\#define\ *GIT_VERSION\ *\".*\"/\#define GIT_VERSION \"$gitrev\"/g" $CCRUNCHERPATH/configure.ac
  sed -i -e "s/\#define\ *GIT_VERSION\ *\".*\"/\#define GIT_VERSION \"$gitrev\"/g" $CCRUNCHERPATH/src/utils/config.h.in
  sed -i -e "s/\#define\ *GIT_VERSION\ *\".*\"/\#define GIT_VERSION \"$gitrev\"/g" $CCRUNCHERPATH/src/utils/config.h
  sed -i -e "s/\\\def\\\gitrev{.*}/\\\def\\\gitrev{$gitrev}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
fi

# update global version tag
if [ "$cver" = "true" ]; then
  sed -i -e "s/AC_INIT(ccruncher,\([^,]*\),\(.*\))/AC_INIT(ccruncher, $version,\\2)/g" $CCRUNCHERPATH/configure.ac
  sed -i -e "s/\(PROJECT_NUMBER[ ]*=[ ]*\)\(.*\)/\\1$version/g " $CCRUNCHERPATH/Doxyfile
  sed -i -e "s/\\\def\\\numversion{.*}/\\\def\\\numversion{$version}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
  sed -i -e "s/\(ccruncher\)-[0-9]\\+\\.[0-9]\\+\\.[0-9]\\+/\\1-$version/gI" $CCRUNCHERPATH/doc/html/*.html
  sed -i -e "s/Last modified: .*/Last modified: $curdate/g" $CCRUNCHERPATH/doc/html/*.html
  sed -i -e "s/version\:.*/version\: $version/g" $CCRUNCHERPATH/doc/html/version
  sed -i -e "s/date\:.*/date\: $curdate/g" $CCRUNCHERPATH/doc/html/version
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$version/g " $CCRUNCHERPATH/ccruncher-cmd.pro
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$version/g " $CCRUNCHERPATH/ccruncher-gui.pro
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$version/g " $CCRUNCHERPATH/ccruncher-tests.pro
  sed -i -e "s/^Version:[ ]*\(.*\)/Version: $version/g" $CCRUNCHERPATH/build/ccruncher.spec
  #echo "you need to run autoconf to take effect some changes";
  #echo "check technical document (version log)";
fi

# update manual pages
if [ "$cman" = "true" ]; then

  if [ ! -f build/ccruncher-cmd ]; then 
    echo "error: file build/ccruncher-cmd not found" 
    exit 1;
  fi

  if [ ! -f build/ccruncher-gui ]; then 
    echo "error: file build/ccruncher-gui not found" 
    exit 1;
  fi

  if [ ! -f build/ccruncher-inf ]; then 
    echo "error: file build/ccruncher-inf not found" 
    exit 1;
  fi

  help2man --no-info \
    -n "simule the loss distribution of a credit portfolio using the Monte Carlo method" \
    -o doc/ccruncher-cmd.1 build/ccruncher-cmd

  help2man --no-info \
    -n "simule the loss distribution of a credit portfolio using the Monte Carlo method" \
    -o doc/ccruncher-gui.1 build/ccruncher-gui

  help2man --no-info \
    -n "infer the dependency parameters of the multivariate default times using the Metropolis-Hastings method" \
    -o doc/ccruncher-inf.1 build/ccruncher-inf

fi

exit $retcode;
