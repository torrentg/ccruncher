#!/bin/sh

#=============================================================
# description:
#   Use this script to compute the VaR from a ccruncher
#   output file
#
# dependencies:
#   R, xsltproc
#
# input:
#   ccruncher output data files (*.csv)
#
# output:
#   mean, stddev, VaR results
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
progname=ccreport.sh
numversion="1.8"
svnversion="R678"
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
  usage: $progname [options] <file1> <file2> ...

  description:
    $progname computes the credit risk using the output data 
    files generates by ccruncher using R. The report is 
    stored in an html file in the same directory where output
    data file resides.
    more info at http://www.ccruncher.net
  arguments:
    files    ccruncher output data files
  options
    -h       show this message and exit
    -v       show version and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname portfolio.csv
    $progname *.csv

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

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

  shift `expr $OPTIND - 1`

  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      if [ ! -f $arg ]; then
        echo "file $arg not exist";
        echo "please, try again";
        exit 1;
      fi
    done
  else
    echo "file not specified";
    echo "use -h option for more information";
    exit 1;
  fi

  which R > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "R not found. please report to system administrator";
    echo "R can be found at http://www.r-project.org/";
    exit 1;
  fi

  which xsltproc > /dev/null 2> /dev/null;

  if [ $? != 0 ]; then
    echo "xsltproc not found. please report to system administrator";
    echo "xsltproc can be found at http://xmlsoft.org/XSLT/xsltproc2.html";
    exit 1;
  fi

}

#-------------------------------------------------------------
# create report
#-------------------------------------------------------------
doreport() {

  name=${1%.*};
  extension=${1##*.};

  if [ $extension != "csv" ];  then
    echo "warning: $filename has not csv extension" 1>&2;
    return 1;
  fi

  rm -f $name.xml;
  rm -f $name.png;
  rm -f $name.html;

  R --vanilla --slave > /dev/null << _EOF_
    # load ccruncher script
    source("$CCRUNCHER/bin/ccreport.R", echo=FALSE);
    # create the xml report
    lines <- ccruncher.summary("$filename", format="xml");
    write(lines, file="${name}.xml");
    # create density graphics
    x <- ccruncher.read("$filename");
    for(i in 1:length(x)) {
      png(file="${name}-" %&% names(x)[i] %&% ".png", width=600);
      ccruncher.density(x[,i]);
      dev.off();
    }
    # create piechart (if required)
    if (length(x) > 1) {
      png(file="${name}.png", width=600);
      ccruncher.piechart("$filename");
      dev.off();
    }
_EOF_

  if [ $? != 0 ]; then
    return $?;
  fi

  xsltproc -o $name.html $CCRUNCHER/bin/ccreport.xsl ${name}.xml;

  if [ $? != 0 ]; then
    return $?;
  fi

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

shift `expr $OPTIND - 1`

#copyright;

for filename in "$@"
do
  doreport $filename;
  if [ $? != 0 ]; then
    retcode=`expr $retcode + 1`;
  fi
done

exit $retcode;

