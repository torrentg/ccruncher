# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = COPYING AUTHORS README INSTALL \
             share/ samples/ data/

dist-hook:
	rm -rf `find $(distdir) -name \.svn`
