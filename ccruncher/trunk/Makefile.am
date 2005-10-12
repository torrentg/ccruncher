# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = bin share samples data doc

docdir = $(datadir)/$(distdir)/doc
samplesdir = $(datadir)/$(distdir)/samples

doc_DATA = doc
samples_DATA = samples
bin_SCRIPTS = bin/report bin/report.R

install-hook:
	echo "setting executable flag to scripts ..."
	chmod +x $(distdir)/bin/*

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`
