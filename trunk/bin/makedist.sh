#!/bin/sh

#=============================================================
# description: 
#   Create a CCruncher distribution package
#
# dependencies:
#   shell, tar, gzip, zip, svn, auto-tools, unix2dos
#   make, pdflatex, bibtex, iconv
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
numversion="xxx"
PACKAGE="ccruncher"
winexes="."

#-------------------------------------------------------------
# usage function
#-------------------------------------------------------------
usage() {

  cat << _EOF_
  usage: $progname [options] <file>

  description:
    $progname is a shell script to create the distribution
    packages in CCruncher project. This script is only 
    used by developers.
  options
    -n       set version identifier (eg. "2.3")
    -w       windows files directory
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -n 2.3 -w /c:/temp/ccruncher/bin

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

  while getopts 'n:w:h' opt
  do
    case $opt in
      n) numversion=$OPTARG;;
      w) winexes=$OPTARG;;
      h) usage; 
         exit 0;;
     \?) echo "unknow option. use -h for more information"; 
         exit 1;;
      *) echo "unexpected error parsing arguments.";
         exit 1;;
    esac
  done

  shift `expr $OPTIND - 1`

  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      echo "unexpected argument: $arg";
      echo "use -h for more information"; 
      exit 1;
    done
  fi

  #TODO: check win dir
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

  # get current version
  numversion=$(grep AC_INIT $1/configure.ac | cut -d"," -f2 | tr -d ' ');

  # update subversion revision info
  $1/bin/rollversion.sh -s;
  if [ $? != 0 ]; then
    echo "aborting: error setting version";
    exit 1;
  fi

  # compile pdf documentation
  make -C $1/doc/tex
  cp $1/doc/tex/ccruncher.pdf $1/doc/

  # redirect html document
  sed -i -e 's/href="ccruncher\.pdf"/href="\.\.\/ccruncher\.pdf"/g' $1/doc/html/*.html
  
  # remove developers files
  rm $1/bin/makedist.sh;
  rm $1/bin/rollversion.sh
  rm $1/bin/tests.sh;
  rm $1/bin/tests.R;
  rm $1/doc/html/robots.txt;
  rm $1/doc/html/.repo.xsl;
  rm -rvf $1/doc/tex;
  rm -rvf `find $1/ -name \.svn\*`;

}

# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  cd $1;

  # creating tarball
  aclocal;
  autoconf;
  automake -avcf;
  ./configure --prefix=$PWD;
  make -j4 distcheck;

  # moving tarball
  mv $PACKAGE-*.tar.gz $currpath/$PACKAGE-${numversion}_src.tgz;

}

# -------------------------------------------------------------
# make bin dist
# -------------------------------------------------------------
makeBinDist() {

  cd $1;

  # removing specific flag
  sed "s/-mtune=native//" configure.ac > configure.ac.new
  mv configure.ac.new configure.ac

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

  # creating and moving tarball
  cd ..;
  mv $1 $PACKAGE-${numversion};
  tar -cvzf $PACKAGE-${numversion}_bin.tgz $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_bin.tgz $currpath;
  mv $PACKAGE-${numversion} $1;

}

# -------------------------------------------------------------
# make win dist
# -------------------------------------------------------------
makeWinDist() {

  # local variables
  winfiles="ccruncher-gui.exe
            ccruncher-cmd.exe
            pthreadGC2.dll
            mingwm10.dll
            libgcc_s_dw2-1.dll
            QtCore4.dll
            QtGui4.dll
            QtNetwork4.dll
            QtOpenGL4.dll
            QtSvg4.dll
            qwt.dll"

  # obtaining a clean environement
  cd $1;

  # copying binaries
  for file in $winfiles
  do
    if [ ! -f $winexes/$file ]; then 
      echo "error: file $winexes/$file not found" 
      exit 1;
    else
      cp $winexes/$file bin/
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
  unix2dos doc/html/*.js;
  unix2dos doc/html/*.xml;
  unix2dos data/readme.txt;

  # creating tarball
  cd ..;
  mv $1 $PACKAGE-${numversion};
  zip -r $PACKAGE-${numversion}_win.zip $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_win.zip $currpath;
  mv $PACKAGE-${numversion} $1;

}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

currpath=$(pwd);
# automake don't add missing files if a parent dir content them
workpath=/tmp/$PACKAGE-${numversion}

# obtaining a clean environement
chmod -R +w $workpath > /dev/null 2> /dev/null;
rm -rvf $workpath > /dev/null 2> /dev/null;
mkdir $workpath;
checkout $workpath/base;
prepare $workpath/base;

#create win package
cp -rvf $workpath/base $workpath/win;
makeWinDist $workpath/win;

# create src package
cp -rvf $workpath/base $workpath/src;
makeSrcDist $workpath/src;

# create bin package
cp -rvf $workpath/base $workpath/bin;
makeBinDist $workpath/bin;

# remove build files
chmod -R +w $workpath > /dev/null 2> /dev/null;
rm -rvf $workpath > /dev/null 2> /dev/null;

cd $currpath;
