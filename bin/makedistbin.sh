#!/bin/sh

# *************************************************************
# 
# Create binary distribution package
#
# *************************************************************

NAME="ccruncher"
VERSION="0.2"

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
svn checkout http://www.generacio.com/svn/repos/ccruncher/trunk
mv trunk $NAME-$VERSION
cd $NAME-$VERSION

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
cd doc
rm -rvf `find . ! -name \*.pdf`
cd ..

# -------------------------------------------------------------
# making package
# -------------------------------------------------------------
cd ..
tar -cvf $NAME-${VERSION}_bin.tar $NAME-$VERSION
gzip $NAME-${VERSION}_bin.tar
rm -rvf $NAME-$VERSION

# -------------------------------------------------------------
# exit
# -------------------------------------------------------------
echo 
echo "done!"
