#!/bin/sh

#=============================================================
# description: 
#   Create a CCruncher distribution package
#
# dependencies:
#   shell, tar, gzip, zip, svn, auto-tools, unix2dos
#
# retcodes:
#   0    : OK
#   other: KO
#
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
progname=makedist.sh
numversion="2.1"
svnversion="R957"
disttype="xxx"
PACKAGE="ccruncher"
pathexes="."

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

   $progname is Copyright (C) 2004-2012 Gerard Torrent and licensed
     under the GNU General Public License, version 2. More info at
                   http://www.ccruncher.net

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

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
checkout() {

  svn export http://www.ccruncher.net/svn/trunk;
  chmod -R +w trunk
  mv trunk $1;

}

# -------------------------------------------------------------
# remove developers files
# -------------------------------------------------------------
prepare() {

  # create html doc graphics
  $1/bin/tex2png.sh $1/doc/html/*.html;
  if [ $? != 0 ]; then
    echo "aborting: error found processing images";
    exit 1;
  fi
  
  # set version information
  $1/bin/rollversion.sh -s;
  if [ $? != 0 ]; then
    echo "aborting: error setting version";
    exit 1;
  fi

  # remove developers files
  rm $1/bin/makedist.sh;
  rm $1/bin/rollversion.sh
  rm $1/bin/tests.sh;
  rm $1/bin/tests.R;
  rm $1/bin/tex2png.sh;
  rm $1/bin/tex2png.xsl;
  rm -rvf `find $1/ -name \.svn\*`;
  rm -rvf $1/doc/share;
  rm -vf $1/doc/html/*.gpi;
  rm -vf $1/doc/html/*.vsd;

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
  prepare $workpath;
  cd $workpath;

  # creating tarball
  aclocal;
  autoconf;
  automake -avcf;
  ./configure --prefix=$PWD;
  make -j4 distcheck;

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
  prepare $workpath;
  cd $workpath;

  # removing specific flag
  sed "s/-mtune=native//" configure.in > configure.in.new
  mv configure.in.new configure.in

  # creating binaries
  aclocal;
  autoconf;
  automake -avcf;
  ./configure --prefix=$PWD;
  make -j4;
  qmake-qt4 ccruncher-gui.pro;
  make -j4;
  
  # dropping unused files
  bin/src2bin.sh -y;

  # creating tarball
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
  winfiles="ccruncher-gui.exe
            ccruncher-cmd.exe
            pthreadGC2.dll
            mingwm10.dll
            libgcc_s_dw2-1.dll
            QtCore4.dll
            QtGui4.dll
            QtSvg4.dll
            qwt.dll"

  # obtaining a clean environement
  chmod -R +w $workpath > /dev/null 2> /dev/null;
  rm -rvf $workpath > /dev/null 2> /dev/null;
  checkout $workpath;
  prepare $workpath;
  cd $workpath;

  # copying binaries
  for file in $winfiles
  do
    if [ ! -f $pathexes/$file ]; then 
      echo "error: file $pathexes/$file not found" 
      exit 1;
    else
      cp $pathexes/$file bin/
    fi
  done

  # dropping unused files
  bin/src2bin.sh -y;

  # setting windows end-line
  unix2dos doc/AUTHORS;
  unix2dos doc/README;
  unix2dos doc/TODO;
  unix2dos doc/COPYING;
  unix2dos doc/ChangeLog;
  unix2dos doc/index.html;
  unix2dos samples/*.xml;
  unix2dos samples/*.dtd;
  unix2dos samples/*.xsd;
  unix2dos samples/*.txt;
  unix2dos doc/html/*.html;
  unix2dos doc/html/*.css;
  unix2dos doc/html/*.csv;
  unix2dos doc/html/*.xml;
  unix2dos data/readme.txt;

  # creating tarball
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
         echo "$progname version and arguments at gtorrent@ccruncher.net";
         exit 1;;

esac
