#!/bin/sh

#=============================================================
# description:
#   Executes ccruncher functional tests
#
# dependencies:
#   ccruncher, R
#
# input:
#   none
#
# output:
#   tests results in stdout
#
# retcodes:
#   0    : OK
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
CCRUNCHER=`dirname $0`/..
progname=tests.sh
numversion="2.1"
svnversion="R945"

#-------------------------------------------------------------
# version function
#-------------------------------------------------------------
version() {

  echo $progname ccruncher-$numversion \($svnversion\)

}

#-------------------------------------------------------------
# copyright function
#-------------------------------------------------------------
copyright() {

  cat << _EOF_

   $progname is Copyright (C) 2004-2012 Gerard Torrent and licensed
     under the GNU General Public License, version 2. More info at
                   http://www.ccruncher.net

_EOF_

}

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options]

  description:
    $progname runs some functional tests to check that ccruncher
    runs fine. More info at http://www.ccruncher.net
  options
    -h       show this message and exit
    -v       show version and exit
  return code:
    val      number of failed tests (0 = OK)
  examples:
    $progname

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  while getopts 'hv' opt
  do
    case $opt in
      v) version;
         exit 0;;
      h) usage;
         exit 0;;
     \?) echo "unknow option. use -h for more information";
         exit 1;;
      *) echo "unexpected error parsing arguments. Please report this bug sending";
         echo "$progname version and arguments at gtorrent@ccruncher.net";
         exit 1;;
    esac
  done

  which R > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "R not found. please report to system administrator";
    echo "R can be found at http://www.r-project.org/";
    exit 1;
  fi

  which $CCRUNCHER/build/ccruncher > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher not found in build/ directory";
    exit 1;
  fi

}

#-------------------------------------------------------------
# runcc function
#-------------------------------------------------------------
runcc() {

  mkdir -p data/$1;
  rm -f data/$1/*;
  echo "running ccruncher on $1 ...";
  ./build/ccruncher-cmd -w --path=data/$1 samples/$1.xml;

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

#copyright;

cd $CCRUNCHER;

runcc test01;
runcc test02;
runcc test03;
runcc test04;
runcc test05;

R --vanilla --slave << _EOF_
    source("./bin/tests.R", echo=FALSE);
    options(warn=-1)
    test01();
    test02();
    test03();
    test04();
    test05();
_EOF_
