# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = COPYING AUTHORS README INSTALL TODO NEWS \
             bin share samples data doc

install-hook:
	echo "setting executable flag to scripts ..."
	chmod +x $(distdir)/bin/*

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`

