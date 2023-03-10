#!/bin/sh

#=============================================================
# description:
#   Executes ccruncher functional tests
#
# dependencies:
#   ccruncher-cmd, R
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
TESTS="test01 test02 test03 test04 test05 test06"

#-------------------------------------------------------------
# check installed apps
#-------------------------------------------------------------
checkapps() {

  which R > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "R not found. please report to system administrator";
    echo "R can be found at http://www.r-project.org/";
    exit 1;
  fi

  which $CCRUNCHER/build/ccruncher-cmd > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher-cmd not found in build/ directory";
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
  ./build/ccruncher-cmd -w -o data/$1 -Dantithetic=true samples/$1.xml > /dev/null;

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

checkapps;

cd $CCRUNCHER;

if [ $# = 0 ]; then
  INPUT=$TESTS
else
  INPUT=$*
fi

for i in $INPUT; do
  if [[ $TESTS =~ $i ]]; then
    runcc $i;
    FUNCS="$FUNCS $i();"
  else
    echo "error: test '"$i"' not found"
    exit 1;
  fi
done

R --vanilla --slave << _EOF_
  source("./bin/tests.R", echo=FALSE);
  options(warn=-1)
  $FUNCS
_EOF_

