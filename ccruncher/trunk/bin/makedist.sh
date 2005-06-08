#!/bin/sh

#=============================================================
# description: 
#   Create a CreditCruncher distribution package
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
#=============================================================

#-------------------------------------------------------------
# variables declaration
#-------------------------------------------------------------
progname=makedist
numversion="0.3"
svnversion="R157"
disttype="src"
PACKAGE="ccruncher"


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
    -s       make source package
    -l       make binary linux package
    -w       make binary windows package
    -h       show this message and exit
  return codes:
    0        OK. finished without errors
    1        KO. finished with errors
  examples:
    $progname -s
    $progname -l
    $progname -w

_EOF_

}

#-------------------------------------------------------------
# copyright function
#-------------------------------------------------------------
copyright() {

  cat << _EOF_

   $progname is Copyright (C) 2003-2005 Gerard Torrent and licensed
     under the GNU General Public License, version 2. more info at
               http://www.generacio.com/ccruncher

_EOF_

}

#-------------------------------------------------------------
# readconf function
#-------------------------------------------------------------
readconf() {

  OPTIND=0

  while getopts 'slwh' opt
  do
    case $opt in
      s) disttype="src";;
      l) disttype="bin";;
      w) disttype="win";;
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

  svn checkout http://www.generacio.com/svn/repos/ccruncher/trunk;
  chmod -R +w trunk
  mv trunk $PACKAGE-$numversion;
  cd $PACKAGE-$numversion;
  
}

# -------------------------------------------------------------
# check version numbers
# -------------------------------------------------------------
checkVersion() {

  aux=R$(svnversion $1);
  
  if [ "$aux" != "$svnversion" ]; then
    echo "conflict with version numbers";
    exit 1;
  fi

}

# -------------------------------------------------------------
# remove developers files
# -------------------------------------------------------------
rmDevFiles() {

  rm bin/clean.sh;
  rm bin/makedist.sh;
  rm bin/rollversion.sh
  rm -rvf `find . -name \.svn\*`;
  rm -rvf doc/share;
  rm -rvf doc/tex;

}


# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  aclocal;
  autoconf;
  automake -a -v -c -f;
  ./configure --prefix=$PWD;
  make distcheck;
  mv $PACKAGE-$numversion.tar.gz ../$PACKAGE-$numversion_src.tar.gz;
  rm -rvf $PACKAGE-$numversion;

}

# -------------------------------------------------------------
# make bin dist
# -------------------------------------------------------------
makeBinDist() {

  #creating binaries
  aclocal;
  autoconf;
  automake -a -v -c -f;
  ./configure --prefix=$PWD;
  make;
  make install;

  #dropping unused files
  
  #creating tarball
  
}

# -------------------------------------------------------------
# creating binaries
# -------------------------------------------------------------
aclocal
autoconf
automake -avc --warnings=all
./configure --prefix=$PWD
make
make install

# -------------------------------------------------------------
# dropping unused files
# -------------------------------------------------------------
rm aclocal.m4
rm -rvf autom4te.cache
chmod -R +w $NAME-$VERSION
rm -rvf $NAME-$VERSION
rm config*
rm Makefile*
rm depcomp install-sh missing
rm INSTALL
rm bin/clean.sh
rm bin/makedistbin.sh
rm -rvf src
rm -rvf share
rm -rvf `find . -name \.svn\*`
rm -rvf doc
#cd doc
#rm -rvf `find . ! -name \*.pdf`
#cd ..

# -------------------------------------------------------------
# making package
# -------------------------------------------------------------
cd ..
tar -cvf $NAME-${VERSION}_bin.tar $NAME-$VERSION
gzip $NAME-${VERSION}_bin.tar
chmod -R +w $NAME-$VERSION
rm -rvf $NAME-$VERSION

# -------------------------------------------------------------
# exit
# -------------------------------------------------------------
echo 
echo "done!"
