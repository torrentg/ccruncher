# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = build

EXTRA_DIST = bin data doc samples

dist_man_MANS = ccruncher-cmd.1

ccruncher-cmd.1: src/ccruncher-cmd.cpp $(top_srcdir)/configure.ac
	$(MAKE) $(AM_MAKEFLAGS) ccruncher-cmd$(EXEEXT)
	$(HELP2MAN) --no-info \
	    -n "simule the loss distribution of a credit portfolio using the Monte Carlo method" \
	    -o doc/ccruncher-cmd.1 build/ccruncher-cmd

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`

