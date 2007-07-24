#!/bin/sh

#=============================================================
# description: 
#   Create a CreditCruncher distribution package
#
# repository version:
#   $Rev$
#
# dependences:
#   shell, tar, gzip, zip, svn, auto-tools, unix2dos
#
# retcodes:
#   0    : OK
#   other: KO
#
#-------------------------------------------------------------
#
# 2005/06/07 - Gerard Torrent [gerard@fobos.generacio.com]
#   . initial release
#
# 2005/09/10 - Gerard Torrent [gerard@fobos.generacio.com]
#   . added oblivion directory management
#   . 'svn checkout' replaced by 'svn export'
#
# 2005/10/12 - Gerard Torrent [gerard@fobos.generacio.com]
#   . moved $CCRUNCHER/ doc files to directory $CCRUNCHER/doc
#   . changed argument flag -l by -b (binary distribution)
#   . added file data/readme.txt
#
# 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
#   . added support for build/ directory
#   . removed unused checkVersion() function
#   . check that a distribution option is filled (xxx)
#   . added Rev svn:keyword
#   . added dependences paragraph at comments
#
# 2005/11/08 - Gerard Torrent [gerard@fobos.generacio.com]
#   . solved minor bugs related to windows installation
#
# 2007/07/24 - Gerard Torrent [gerard@mail.generacio.com]
#   . added doc/other directory
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
progname=makedist.sh
numversion="1.1"
svnversion="R381"
disttype="xxx"
PACKAGE="ccruncher"
pathexes=""
retcode=0

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options] <file>

  description:
    $progname is a shell script to create the distribution
    packages in CreditCruncher project. This script is only 
    used by developers.
  options
    -s       make source package (default)
    -b       make binary linux package
    -w       make binary windows package
    -d       directory where resides exe files (only win dist)
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -s
    $progname -b
    $progname -w -d /c:/temp/ccruncher/bin

_EOF_

}

#-------------------------------------------------------------
# copyright function
#-------------------------------------------------------------
copyright() {

  cat << _EOF_

   $progname is Copyright (C) 2003-2007 Gerard Torrent and licensed
     under the GNU General Public License, version 2. more info at
               http://www.generacio.com/ccruncher

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

  while getopts 'sbwhd:' opt
  do
    case $opt in
      s) disttype="src";;
      b) disttype="bin";;
      w) disttype="win";;
      d) pathexes=$OPTARG;;
      h) usage; 
         exit 0;;
     \?) echo "unknow option. use -h for more information"; 
         exit 1;;
      *) echo "unexpected error parsing arguments. Please report this bug sending";
         echo "$progname version and arguments at gerard@fobos.generacio.com";
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

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
checkout() {

  svn export http://www.generacio.com/svn/repos/ccruncher/trunk;
  chmod -R +w trunk
  mv trunk $1;

}

# -------------------------------------------------------------
# remove developers files
# -------------------------------------------------------------
rmDevFiles() {

  rm $1/bin/clean.sh;
  rm $1/bin/makedist.sh;
  rm $1/bin/rollversion.sh
  rm -rvf `find $1/ -name \.svn\*`;
  rm -rvf $1/doc/share;
  rm -rvf $1/doc/tex;
  rm -rvf $1/doc/other;

}

# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  # local variables
  currpath=$(pwd);
  # automake don't add missing files if a parent dir content them
  workpath=/tmp/$PACKAGE-${numversion}_src

  # obtaining a clean environement
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;
  checkout $workpath;
  rmDevFiles $workpath;
  cd $workpath;

  # creating tarball
  aclocal;
  autoconf;
  automake -avcf;
  ./configure --prefix=$PWD;
  make distcheck;

  # cleaning
  mv $PACKAGE-$numversion.tar.gz $currpath/$PACKAGE-${numversion}_src.tgz;
  cd $currpath;
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;

}

# -------------------------------------------------------------
# make bin dist
# -------------------------------------------------------------
makeBinDist() {

  # local variables
  currpath=$(pwd);
  workpath=/tmp/$PACKAGE-${numversion}

  # obtaining a clean environement
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;
  checkout $workpath;
  rmDevFiles $workpath;
  cd $workpath;

  #creating binaries
  aclocal;
  autoconf;
  automake -avcf;
  ./configure --prefix=$PWD;
  make;
  make install;

  #dropping unused files
  rm ccruncher.sln
  rm ccruncher.vcproj
  rm aclocal.m4;
  rm -rvf autom4te.cache;
  rm config*;
  rm depcomp install-sh missing;
  rm doc/INSTALL;
  rm Makefile*;
  rm -rvf src;
  rm -rvf build;
  rm -rvf share;
  rm -rvf oblivion;

  #creating tarball
  cd /tmp/;
  tar -cvzf $PACKAGE-${numversion}_bin.tgz $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_bin.tgz $currpath;
  cd $currpath;
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;

}

# -------------------------------------------------------------
# make win dist
# -------------------------------------------------------------
makeWinDist() {

  # local variables
  currpath=$(pwd);
  workpath=/tmp/$PACKAGE-${numversion}

  # obtaining a clean environement
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;
  checkout $workpath;
  rmDevFiles $workpath;
  cd $workpath;

  #creating binaries
  cp $pathexes/ccruncher.exe bin/
  cp $pathexes/libexpat.dll bin/
  cp $pathexes/zlib1.dll bin/

  #dropping unused files
  rm bin/report
  rm ccruncher.sln
  rm ccruncher.vcproj
  rm aclocal.m4;
  rm -rvf autom4te.cache;
  rm config*;
  rm depcomp install-sh missing;
  rm doc/INSTALL;
  rm Makefile*;
  rm -rvf src;
  rm -rvf build;
  rm -rvf share;
  rm -rvf oblivion;

  #setting windows end-line
  unix2dos doc/AUTHORS;
  unix2dos doc/README;
  unix2dos doc/TODO;
  unix2dos doc/COPYING;
  unix2dos doc/NEWS;
  unix2dos doc/index.html;
  unix2dos bin/report.R;
  unix2dos samples/*.xml;
  unix2dos samples/*.dtd;
  unix2dos samples/*.xsd;
  unix2dos samples/*.txt;
  unix2dos doc/html/*.html;
  unix2dos doc/html/*.css;
  unix2dos data/readme.txt;

  #creating tarball
  cd /tmp/;
  zip -r  $PACKAGE-${numversion}_win.zip $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_win.zip $currpath;
  cd $currpath;
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;
shift `expr $OPTIND - 1`

case $disttype in

  'src') copyright;
         makeSrcDist;;
  'bin') copyright;
         makeBinDist;;
  'win') copyright;
         makeWinDist;;
  'xxx') echo "please, specify the distribution type";
         echo "use -h option for more information";
         exit 1;;
      *) echo "unexpected error. Please report this bug sending";
         echo "$progname version and arguments at gerard@fobos.generacio.com";
         exit 1;;

esac

if [ $retcode = 0 ]; then
  echo "done!";
else
  echo "finished with problems";
fi

exit $retcode;
