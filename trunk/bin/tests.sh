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
numversion="1.7"
svnversion="R653"

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

   $progname is Copyright (C) 2004-2011 Gerard Torrent and licensed
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

  which $CCRUNCHER/bin/ccruncher > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "ccruncher not found in bin/ directory";
    echo "please, execute command 'bin/src2bin.sh'";
    exit 1;
  fi

}

#-------------------------------------------------------------
# runcc function
#-------------------------------------------------------------
runcc() {

  rm -rf data/$1;
  echo "running ccruncher on $1 ...";
  ./build/ccruncher -fq --trace --path=data/$1 samples/$1.xml;
  
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

R --vanilla --slave << _EOF_
    source("./bin/ccreport.R", echo=FALSE);
    source("./bin/tests.R", echo=FALSE);
    options(warn=-1)
    test01();
    test02();
    test03();
    test04();
_EOF_

