# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

#EXTRA_DIST = AUTHORS ChangeLog NEWS README
EXTRA_DIST = share/ samples/ data/

dist-hook:
	rm -rf `find $(distdir) -name \.svn`
