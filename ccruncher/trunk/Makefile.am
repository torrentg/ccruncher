# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = COPYING AUTHORS README INSTALL TODO \
             share samples data \
             bin/xercesenv.sh

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`
