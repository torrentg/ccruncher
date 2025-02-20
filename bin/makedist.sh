#!/bin/bash

#=============================================================
# description: 
#   Create a CCruncher distribution package
#
# dependencies:
#   tar, gzip, zip, git, auto-tools, unix2dos
#   make, pdflatex, bibtex, iconv, rpmbuild,
#   dirname
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
windir="xxx"

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
    $progname -w win32/

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0;

  while getopts 'w:h' opt
  do
    case $opt in
      w) windir=$OPTARG;;
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

  if [ $windir = "xxx" ]; then
    echo "error: -w argument not set";
    echo "use -h for more information"; 
    exit 1;
  else
    windir=$(readlink -f "$windir");
  fi

  shift `expr $OPTIND - 1`;

  # more arguments?
  if [ "$*" != "" ]; then
    for arg in "$@"
    do
      echo "error: unexpected argument '$arg'";
      echo "use -h for more information"; 
      exit 1;
    done
  fi

  # check windows files
  if [[ ! -d "$windir" ]]; then
    echo "error: directory '$windir' not found" 
    exit 1;
  fi

}

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
checkout() {

  echo "retrieving project from GIT";
  git clone --depth 1 "git@github.com:torrentg/ccruncher.git" "$1" > /dev/null;
  if [ $? -ne 0 ]; then
    echo "error retrieving project from git";
    exit 1;
  fi
  chmod -R +w "$1";

  # get current version
  numversion=$(grep AC_INIT $1/configure.ac | cut -d, -f2 | tr -d ' []');
  echo "project version = $numversion";

  # update git revision info
  echo "rolling version numbers";
  $1/bin/rollversion.sh -g "$numversion";
  if [ $? != 0 ]; then
    echo "error setting current git version";
    exit 1;
  fi

  # compile pdf documentation
  echo "creating the pdf technical document";
  make -C "$1/doc/tex" > /dev/null
  if [ $? -ne 0 ]; then
    echo "error creating pdf technical document";
    exit 1;
  fi
  cp "$1/doc/tex/ccruncher.pdf" "$1/doc/";

}

# -------------------------------------------------------------
# remove developers files
# -------------------------------------------------------------
remDevFiles() {

  # redirect html document
  sed -i -e 's/href="ccruncher\.pdf"/href="\.\.\/ccruncher\.pdf"/g' "$1/doc/html/"*.html;
  
  # remove developers files
  rm "$1/bin/makedist.sh";
  rm "$1/bin/rollversion.sh";
  rm "$1/bin/tests.sh";
  rm "$1/bin/tests.R";
  rm "$1/doc/html/version";
  rm "$1/doc/html/favicon.ico";
  rm -rf "$1/doc/tex";
  rm -rf $(find "$1/" -name \.git\*);

}

# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  echo "creating src package";
  cd "$1";

  # creating tarball
  aclocal;
  autoconf;
  # automake don't add missing files if a parent dir content them
  automake -acf 2> /dev/null;
  ./configure -q --prefix="$PWD";
  make -j distcheck > /dev/null;

  # moving tarball
  mv $PACKAGE-*.tar.gz "$currpath/$PACKAGE-${numversion}_src.tgz";

}

# -------------------------------------------------------------
# make bin dist
# -------------------------------------------------------------
makeBinDist() {

  echo "creating bin package";
  cd "$1";

  # creating binaries
  aclocal;
  autoconf;
  # automake don't add missing files if a parent dir content them
  automake -acf 2> /dev/null;
  ./configure -q --prefix=$PWD;
  make -j > /dev/null;
  qmake-qt5 ccruncher-gui.pro;
  make -j > /dev/null;
  
  # dropping unused files
  bin/src2bin.sh -y;

  # creating and moving tarball
  cd ..;
  mv "$1" $PACKAGE-${numversion};
  tar -czf $PACKAGE-${numversion}_bin.tgz $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_bin.tgz "$currpath";
  mv $PACKAGE-${numversion} "$1";

}

# -------------------------------------------------------------
# make win dist
# -------------------------------------------------------------
makeWinDist() {

  echo "creating win package";
  cd "$1";

  # copying binaries
  mkdir -p bin;
  cp -r $windir/* bin;

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
  unix2dos -q doc/html/*.xml;
  unix2dos -q data/readme.txt;

  # creating tarball
  cd ..;
  mv "$1" $PACKAGE-${numversion};
  zip -q -r $PACKAGE-${numversion}_win.zip $PACKAGE-${numversion};
  mv $PACKAGE-${numversion}_win.zip $currpath;
  mv $PACKAGE-${numversion} "$1";

}

# -------------------------------------------------------------
# make rpm dist
# -------------------------------------------------------------
makeRpmDist() {

  echo "creating rpm packages";

  # create RPM build tree within user's home directory 
  rpmdev-setuptree;
  cd ~/rpmbuild/;
  
  # create the rpms packages
  cp "$1/build/ccruncher.spec" SPECS/;
  cp "$currpath/$PACKAGE-${numversion}_src.tgz" SOURCES/;
  rpmbuild --define "debug_package %{nil}" -ba SPECS/ccruncher.spec > /dev/null 2> /dev/null;
  
  # copy rpms to current directory
  cp $(find RPMS/ -name $PACKAGE-${numversion}-\*.rpm) "$currpath";
  cp $(find SRPMS/ -name $PACKAGE-${numversion}-\*.rpm) "$currpath";
  
}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;

currpath=$(pwd);
workpath=$(mktemp -d -p /tmp $PACKAGE-XXX);

# obtaining a clean environement
checkout "$workpath/base";

#create web package
cp -rf "$workpath/base/doc/html" "$workpath/web";
cp -f "$workpath/base/doc/ccruncher.pdf" "$workpath/web/";
tar --directory="$workpath/web/" -czf "$currpath/$PACKAGE-${numversion}_web.tgz" .;

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

# create rpm package
makeRpmDist $workpath/base;

# remove build files
chmod -R +w $workpath > /dev/null 2> /dev/null;
rm -rvf $workpath > /dev/null 2> /dev/null;

cd $currpath;
echo "1. check $PACKAGE-${numversion}_* packages";
echo "2. check TODO and CHANGELOG files";
echo "3. check pdf technical document";
echo "4. check html files";
echo "5. check man pages";
echo "6. run functional tests";
