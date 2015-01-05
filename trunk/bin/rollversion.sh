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
gloversion=X.Y.Z
gloversion_short=X.Y
retcode=0
cver=false
csvn=false

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
    -s       update svnversion tag
    -g num   update svnversion and global version identifier tags
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

  while getopts 'sg:h' opt
  do
    case $opt in
      s) csvn=true;;
      g) cver=true;
         gloversion=$OPTARG;
         gloversion_short=$(echo $OPTARG | sed -r 's/([0-9]+\.[0-9]+).*/\1/g');;
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
# getSvnVersion function
#-------------------------------------------------------------
getSvnVersion() {

  #svnversion=R$(svnversion $CCRUNCHERPATH | sed "s/^\([0-9]\+\)[MSP]*\(:.*\)\?/\1/g");
  svnversion=R$(svn info http://www.ccruncher.net/svn/ | awk '/Revision:/ { print $2 }');
 
  if [ $? != 0 ]; then
    echo "error: problems retrieving svnversion";
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

# update svn version tag
if [ "$csvn" = "true" ] || [ "$cver" = "true" ]; then
  getSvnVersion;
  sed -i -e "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/configure.ac
  sed -i -e "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h.in
  sed -i -e "s/\#define\ *SVN_VERSION\ *\".*\"/\#define SVN_VERSION \"$svnversion\"/g" $CCRUNCHERPATH/src/utils/config.h
  sed -i -e "s/\\\def\\\svnversion{.*}/\\\def\\\svnversion{$svnversion}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
fi

# update global version tag
if [ "$cver" = "true" ]; then
  sed -i -e "s/AC_INIT(ccruncher,\([^,]*\),\(.*\))/AC_INIT(ccruncher, $gloversion,\\2)/g" $CCRUNCHERPATH/configure.ac
  sed -i -e "s/\(PROJECT_NUMBER[ ]*=[ ]*\)\(.*\)/\\1$gloversion/g " $CCRUNCHERPATH/Doxyfile
  sed -i -e "s/\\\def\\\numversion{.*}/\\\def\\\numversion{$gloversion_short}/g" $CCRUNCHERPATH/doc/tex/ccruncher.tex
  sed -i -e "s/<span class=\"version\">.*<\/span>/<span class=\"version\">$gloversion<\/span>/g" $CCRUNCHERPATH/doc/html/*.html
  sed -i -e "s/Last modified: .*/Last modified: $curdate/g" $CCRUNCHERPATH/doc/html/*.html $CCRUNCHERPATH/doc/html/.repo.xsl
  sed -i -e "s/version\:.*/version\: $gloversion/g" $CCRUNCHERPATH/doc/html/version
  sed -i -e "s/date\:.*/date\: $curdate/g" $CCRUNCHERPATH/doc/html/version
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$gloversion/g " $CCRUNCHERPATH/ccruncher-cmd.pro
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$gloversion/g " $CCRUNCHERPATH/ccruncher-gui.pro
  sed -i -e "s/^\(VERSION[ ]*=[ ]*\)\(.*\)/\\1$gloversion/g " $CCRUNCHERPATH/ccruncher-tests.pro
  #echo "you need to run autoconf to take effect some changes";
  #echo "check technical document (version log)";
fi

exit $retcode;

