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
#   other: KO
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
CCRUNCHER=`dirname $0`/..
progname=tests.sh
numversion="1.4"
svnversion="R454"
retcode=0
options=""

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

   $progname is Copyright (C) 2003-2010 Gerard Torrent and licensed
     under the GNU General Public License, version 2. more info at
               http://www.generacio.com/ccruncher

_EOF_

}

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options] <file1> <file2>

  description:
    $progname computes the credit risk using the output data 
    files generates by ccruncher using R. The report is 
    stored in an html file in the same directory where output
    data file resides.
    more info at http://www.generacio.com/ccruncher
  arguments:
    file     ccruncher output data file
  options
    -h       show this message and exit
    -v       show version and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname portfolio-rest.out
    $progname *.out

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  while getopts 'hvx' opt
  do
    case $opt in
      v) version;
         exit 0;;
      h) usage;
         exit 0;;
     \?) echo "unknow option. use -h for more information";
         exit 1;;
      *) echo "unexpected error parsing arguments. Please report this bug sending";
         echo "$progname version and arguments at gerard@mail.generacio.com";
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
    echo "please, execute command 'make install'";
    exit 1;
  fi

}

#-------------------------------------------------------------
# performs test 1
#-------------------------------------------------------------
test01() {

  $CCRUNCHER/bin/ccruncher -f --lcopula --path=data $CCRUNCHER/samples/test01.xml > /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher has reported an error executing file samples/test01.xml";
    return $?;
  fi

  R --vanilla --slave << _EOF_
    source("$CCRUNCHER/bin/ccreport.R", echo=FALSE);
    data <- ccruncher.read("$CCRUNCHER/data/portfolio.csv");
    #computes observed frequencies
    obs <- tabulate(data[,1]+1, 2);
    #fixes theoretical probabilities
    prob <- c(0.9, 0.1);
    #performs chi square test
    cst <- chisq.test(obs, p=prob)
    #if p-value > 0.01 than distribution is correct
    if (cst\$p.value > 0.01) { 
      cat("test01 passed\n");
      quit(save="no", status=0);
    }
    else {
      cat("test01 failed\n");
      quit(save="no", status=1);
    }
_EOF_

  return $?;

}

#-------------------------------------------------------------
# performs test 2
#-------------------------------------------------------------
test02() {

  $CCRUNCHER/bin/ccruncher -f --lcopula --path=data $CCRUNCHER/samples/test02.xml > /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher has reported an error executing file samples/test02.xml";
    return $?;
  fi

  R --vanilla --slave << _EOF_
    source("$CCRUNCHER/bin/ccreport.R", echo=FALSE);
    data <- ccruncher.read("$CCRUNCHER/data/portfolio.csv");
    #computes observed frequencies
    obs <- tabulate(data[,1]+1, 3);
    #fixes theoretical probabilities
    prob <- c(0.9*0.9, 0.9*0.1+0.1*0.9, 0.1*0.1);
    #performs chi square test
    cst <- chisq.test(obs, p=prob)
    #if p-value > 0.01 than distribution is correct
    if (cst\$p.value <= 0.01) { 
      cat("test02 failed (chisq test)\n");
      quit(save="no", status=1);
    }
    #load copula data
    data <- read.csv("$CCRUNCHER/data/copula.csv", header=TRUE)
    correl <- cor(data[,1],data[,2])
    if (abs(correl) > 0.05) { 
      cat("test02 failed (correlation test)\n");
      quit(save="no", status=1);
    }
    #exit
    cat("test02 passed\n");
    quit(save="no", status=0);
_EOF_

  return $?;

}

#-------------------------------------------------------------
# performs test 3
#-------------------------------------------------------------
test03() {

  $CCRUNCHER/bin/ccruncher -f --lcopula --path=data $CCRUNCHER/samples/test03.xml > /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher has reported an error executing file samples/test03.xml";
    return $?;
  fi

  R --vanilla --slave << _EOF_
    options(warn=-1)
    source("$CCRUNCHER/bin/ccreport.R", echo=FALSE);
    data <- ccruncher.read("$CCRUNCHER/data/portfolio.csv");
    #computes observed frequencies
    obs <- tabulate(data[,1]+1, 101);
    #fixes theoretical probabilities
    prob <- dbinom(0:100, 100, 0.1);
    #performs chi square test
    cst <- chisq.test(obs, p=prob);
    #if p-value > 0.01 than distribution is correct
    if (cst\$p.value <= 0.01) { 
      cat("test03 failed (chisq test)\n");
      quit(save="no", status=1);
    }
    #load copula data
    data <- read.csv("$CCRUNCHER/data/copula.csv", header=TRUE)
    correl <- cor(data)
    for(i in 1:length(data))
    {
       for(j in 1:i)
       {
         if (i!=j && abs(correl[i,j]) > 0.05) { 
           cat("test03 failed (correlation test)\n");
           quit(save="no", status=1);
         }
       }
    }
    #exit
    cat("test03 passed\n");
    quit(save="no", status=0);
_EOF_

  return $?;

}

#-------------------------------------------------------------
# performs test 4
#-------------------------------------------------------------
test04() {

  $CCRUNCHER/bin/ccruncher -f --lcopula --path=data $CCRUNCHER/samples/test04.xml > /dev/null;

  if [ $? != 0 ]; then
    echo "error: ccruncher has reported an error executing file samples/test04.xml";
    return $?;
  fi

  R --vanilla --slave << _EOF_
    #load copula data
    data <- read.csv("$CCRUNCHER/data/copula.csv", header=TRUE)
    correl <- cor(data)
    for(i in 1:50)
    {
       for(j in 1:50)
       {
         if (i!=j && abs(correl[i,j]-0.1) > 0.025) { 
           cat("test04 failed (correlation test)\n");
           quit(save="no", status=1);
         }
       }
    }
    for(i in 51:100)
    {
       for(j in 51:100)
       {
         if (i!=j && abs(correl[i,j]-0.15) > 0.025) { 
           cat("test04 failed (correlation test)\n");
           quit(save="no", status=1);
         }
       }
    }
    for(i in 1:50)
    {
       for(j in 51:100)
       {
         if (i!=j && abs(correl[i,j]-0.05) > 0.025) { 
           cat("test04 failed (correlation test)\n");
           quit(save="no", status=1);
         }
       }
    }
    #exit
    cat("test04 passed\n");
    quit(save="no", status=0);
_EOF_

  return $?;

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

#copyright;

test01;
if [ $? != 0 ]; then
  retcode=`expr $retcode + 1`;
fi

test02;
if [ $? != 0 ]; then
  retcode=`expr $retcode + 1`;
fi

test03;
if [ $? != 0 ]; then
  retcode=`expr $retcode + 1`;
fi

test04;
if [ $? != 0 ]; then
  retcode=`expr $retcode + 1`;
fi

exit $retcode;

