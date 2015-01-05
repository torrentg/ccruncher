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
winexes="xxx"
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
    -w       windows files directory
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -w /c:/temp/ccruncher/bin

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

  while getopts 'w:h' opt
  do
    case $opt in
      w) winexes=$OPTARG;;
      h) usage; 
         exit 0;;
     \?) echo "error: unknow option."; 
         echo "use -h for more information"; 
         exit 1;;
      *) echo "error: unexpected error parsing arguments.";
         echo "use -h for more information"; 
         exit 1;;
    esac
  done

  if [ $winexes = "xxx" ]; then
    echo "error: -w argument not set";
    echo "use -h for more information"; 
    exit 1;
  fi

  shift `expr $OPTIND - 1`

  # more arguments?
  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      echo "error: unexpected argument '$arg'";
      echo "use -h for more information"; 
      exit 1;
    done
  fi

  # check winfiles
  for file in $winfiles
  do
    if [ ! -f $winexes/$file ]; then 
      echo "error: file $winexes/$file not found" 
      exit 1;
    fi
  done

}

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
checkout() {

  svn export http://www.ccruncher.net/svn/trunk > /dev/null;
  if [ $? -ne 0 ]; then
    echo "error retrieving project from svn";
    exit 1;
  fi
  chmod -R +w trunk
  mv trunk $1;

  # get current version
  numversion=$(grep AC_INIT $1/configure.ac | cut -d"," -f2 | tr -d ' ');

  # update subversion revision info
  $1/bin/rollversion.sh -g "$numversion";
  if [ $? != 0 ]; then
    echo "error setting current svn version";
    exit 1;
  fi

  # compile pdf documentation
  make -C $1/doc/tex > /dev/null
  if [ $? -ne 0 ]; then
    echo "error creating pdf technical document";
    exit 1;
  fi
  cp $1/doc/tex/ccruncher.pdf $1/doc/

}

# -------------------------------------------------------------
# remove developers files
# -------------------------------------------------------------
remDevFiles() {

  # redirect html document
  sed -i -e 's/href="ccruncher\.pdf"/href="\.\.\/ccruncher\.pdf"/g' $1/doc/html/*.html
  
  # remove developers files
  rm $1/bin/makedist.sh;
  rm $1/bin/rollversion.sh;
  rm $1/bin/tests.sh;
  rm $1/bin/tests.R;
  rm $1/doc/html/version;
  rm $1/doc/html/favicon.ico;
  rm $1/doc/html/robots.txt;
  rm $1/doc/html/.repo.xsl;
  rm -rf $1/doc/tex;
  rm -rf `find $1/ -name \.svn\*`;

}

# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  cd $1;

  # creating tarball
  aclocal;
  autoconf;
  automake -acf 2> /dev/null;
  ./configure -q --prefix=$PWD;
  make -j4 distcheck > /dev/null;

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
  automake -acf 2> /dev/null;
  ./configure -q --prefix=$PWD;
  make -j4 > /dev/null;
  qmake-qt4 ccruncher-gui.pro;
  make -j4 > /dev/null;
  
  # dropping unused files
  bin/src2bin.sh -y;

  # creating and moving tarball
  cd ..;
  mv $1 $PACKAGE-${numversion};
  tar -czf $PACKAGE-${numversion}_bin.tgz $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_bin.tgz $currpath;
  mv $PACKAGE-${numversion} $1;

}

# -------------------------------------------------------------
# make win dist
# -------------------------------------------------------------
makeWinDist() {

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
  unix2dos -q doc/AUTHORS;
  unix2dos -q doc/README;
  unix2dos -q doc/TODO;
  unix2dos -q doc/COPYING;
  unix2dos -q doc/ChangeLog;
  unix2dos -q doc/index.html;
  unix2dos -q samples/*.xml;
  unix2dos -q samples/*.dtd;
  unix2dos -q samples/*.xsd;
  unix2dos -q samples/*.txt;
  unix2dos -q doc/html/*.html;
  unix2dos -q doc/html/*.css;
  unix2dos -q doc/html/*.js;
  unix2dos -q doc/html/*.xml;
  unix2dos -q data/readme.txt;

  # creating tarball
  cd ..;
  mv $1 $PACKAGE-${numversion};
  zip -q -r $PACKAGE-${numversion}_win.zip $PACKAGE-${numversion};
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

#create web package
cp -rf $workpath/base/doc/html $workpath/web;
cp -f $workpath/base/doc/ccruncher.pdf $workpath/web/;
tar --directory=$workpath/web/ -czf $currpath/$PACKAGE-${numversion}_web.tgz .;

# remove development files
remDevFiles $workpath/base;

#create win package
cp -rf $workpath/base $workpath/win;
makeWinDist $workpath/win;

# create bin package
cp -rf $workpath/base $workpath/bin;
makeBinDist $workpath/bin;

# create src package
cp -rf $workpath/base $workpath/src;
makeSrcDist $workpath/src;

# remove build files
chmod -R +w $workpath > /dev/null 2> /dev/null;
rm -rvf $workpath > /dev/null 2> /dev/null;

cd $currpath;
echo "1. check $PACKAGE-${numversion}_* packages";
echo "2. check TODO and CHANGELOG files";
echo "3. check pdf technical document";
echo "2. check html files";

