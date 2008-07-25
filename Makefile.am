# *************************************************************
#
# Process this file with automake to produce Makefile.in
#
# *************************************************************

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = COPYING AUTHORS README INSTALL TODO NEWS\
             share samples data

install-hook:
	echo "setting executable flag to scripts ..."
	chmod +x `find $(distdir) -name \*\.sh`

dist-hook:
	echo "dropping subversion stuff ..."
	rm -rf `find $(distdir) -name \.svn`
	echo "setting executable flag to scripts ..."
	chmod +x `find $(distdir) -name \*\.sh`

check-hook:
	echo "running tests ..."
	$(distdir)/tests
	
