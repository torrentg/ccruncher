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
progname=makedist.sh
numversion="0.3"
svnversion="R157:162M"
disttype="src"
PACKAGE="ccruncher"
pathexes=""

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
    -l       make binary linux package
    -w       make binary windows package
    -d       directory where resides exe files (only win dist)
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

  while getopts 'slwhd:' opt
  do
    case $opt in
      s) disttype="src";;
      l) disttype="bin";;
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

  #TODO: check that if win -> pathexes is full

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
  mv trunk $1;
  
}

# -------------------------------------------------------------
# check version numbers
# -------------------------------------------------------------
checkVersion() {

  aux=R$(svnversion $1);
  
  if [ "$aux" != "$svnversion" ]; then
    echo "conflict with version numbers";
    echo "run rollversion.sh + svn commit and try again";
    exit 1;
  fi

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

}

# -------------------------------------------------------------
# make src dist
# -------------------------------------------------------------
makeSrcDist() {

  # local variables
  workpath=$PACKAGE-$numversion
  
  # obtaining a clean environement
  rm -rvf $workpath;
  checkout $workpath;
  checkVersion $workpath;
  rmDevFile $workpath;
  cd $workpath;
  
  # creating tarball
  aclocal;
  autoconf;
  automake -a -v -c -f;
  ./configure --prefix=$PWD;
  make distcheck;

  # cleaning   
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
  rm ccruncher.sln
  rm ccruncher.vcproj
  rm aclocal.m4;
  rm -rvf autom4te.cache;
  rm config*;
  rm Makefile*;
  rm depcomp install-sh missing;
  rm INSTALL;
  rm -rvf src;
  rm -rvf share;
  
  #creating tarball
  cd ..;
  tar -cvf $PACKAGE-$numversion_bin.tar $PACKAGE-$numversion;
  gzip $PACKAGE-$numversion_bin.tar;
  chmod -R +w $PACKAGE-$numversion;
  rm -rvf $PACKAGE-$numversion;
  
}

# -------------------------------------------------------------
# make win dist
# -------------------------------------------------------------
makeWinDist() {
   echo "implementation pending";
}

#-------------------------------------------------------------
# main function
#-------------------------------------------------------------

readconf $@;
shift `expr $OPTIND - 1`

copyright;

case $disttype in

  'src') makeSrcDist;;
  'bin') makeBinDist;;
  'win') makeWinDist;;
      *) echo "unexpected error. Please report this bug sending";
         echo "$progname version and arguments at gerard@fobos.generacio.com";
         exit 1;;

esac

if [ $retcode != 0 ]; then
  echo "finished with problems";
else
  echo "done!";
fi

exit $retcode;


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
