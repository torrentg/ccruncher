#!/bin/sh

#=============================================================
# description:
#   Use this script to compute the VaR from a ccruncher
#   output file
#
# repository version:
#   $Rev$
#
# dependencies:
#   R, xsltproc
#
# input:
#   ccruncher output data files (*.out)
#
# output:
#   mean, stddev, VaR results
#
# retcodes:
#   0    : OK
#   other: KO
#
#-------------------------------------------------------------
#
# 2005/08/28 - Gerard Torrent [gerard@mail.generacio.com]
#   . initial release
#
# 2005/09/04 - Gerard Torrent [gerard@mail.generacio.com]
#   . added xml format argument
#   . added portfolio value|loss argument
#
# 2005/09/10 - Gerard Torrent [gerard@mail.generacio.com]
#   . removed unused options
#   . CCRUNCHER var filled with dirname
#
# 2005/10/12 - Gerard Torrent [gerard@mail.generacio.com]
#   . shows a message when no files found (and return KO)
#
# 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
#   . added Rev svn:keyword
#
# 2008/10/18 - Gerard Torrent [gerard@mail.generacio.com]
#   . xml and txt output replaced by html output
#   . renamed to ccreport.sh
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
CCRUNCHER=`dirname $0`/..
progname=ccreport.sh
numversion="1.4"
svnversion="R494"
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

   $progname is Copyright (C) 2003-2008 Gerard Torrent and licensed
     under the GNU General Public License, version 2. more info at
               http://www.generacio.com/ccruncher

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
    more info at http://www.generacio.com/ccruncher
  arguments:
    files    ccruncher output data files
  options
    -h       show this message and exit
    -v       show version and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname portfolio.out
    $progname *.out

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

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
    source("$CCRUNCHER/bin/ccreport.R", echo=FALSE);
    lines <- ccruncher.summary("$filename", format="xml");
    write(lines, file="${name}.xml");
    png(file="${name}.png", width=400);
    ccruncher.graphic("$filename");
    dev.off();
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

