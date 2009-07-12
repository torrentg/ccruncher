
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2005 Gerard Torrent
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#
# makefile - a simple makefile to create pdf doc - $Rev$
# --------------------------------------------------------------------------
#
# 2005/09/01 - Gerard Torrent [gerard@mail.generacio.com]
#   . initial release
#
# 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
#   . added Rev (aka LastChangedRevision) svn tag
#
# 2007/07/28 - Gerard Torrent [gerard@mail.generacio.com]
#   . removed makeindex command
#   . replaced ccruncher.aux, etc. by wildcards
#
#***************************************************************************

all:
	latex ccruncher
	bibtex ccruncher
	latex ccruncher
	latex ccruncher
	dvipdfm ccruncher

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.dvi
	rm -f *.idx
	rm -f *.ilg
	rm -f *.ind
	rm -f *.toc
	rm -f *.log
	rm -f *~

install:
	cp ccruncher.pdf ../
