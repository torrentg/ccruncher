#!/bin/sh

# *************************************************************
#
# Create binary distribution package
#
# *************************************************************

NAME="ccruncher"
VERSION="0.1_bin"

# -------------------------------------------------------------
# obtaining repository content
# -------------------------------------------------------------
svn checkout http://www.generacio.com/svn/repos/ccruncher/trunk
mv trunk ccruncher-$VERSION
cd $NAME-$VERSION

# -------------------------------------------------------------
# dropping unused files
# -------------------------------------------------------------
rm configure*
rm Makefile*
rm INSTALL
rm bin/clean.sh
rm bin/makedistbin.sh
rm -rvf src
rm -rvf share
rm -rvf `find . -name \.svn\*`

# -------------------------------------------------------------
# making package
# -------------------------------------------------------------
cd ..
tar -cvf $NAME-$VERSION.tar $NAME-$VERSION
gzip $NAME-$VERSION.tar
rm -rvf $NAME-$VERSION

# -------------------------------------------------------------
# exit
# -------------------------------------------------------------
echo 
echo "done!"
