# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = bin share samples data doc

# docdir = $(datadir)/$(distdir)/doc
# samplesdir = $(datadir)/$(distdir)/samples
# scriptsdir = $(datadir)/$(distdir)/scripts

install-hook:
	echo "setting executable flag to scripts ..."
	chmod +x $(distdir)/bin/*

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`
