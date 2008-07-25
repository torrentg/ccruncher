# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = build

EXTRA_DIST = bin share data doc samples

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`
