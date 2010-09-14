
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2010 Gerard Torrent
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
#***************************************************************************

all: pdf

pdf: ccruncher.tex refs.bib
	latex ccruncher
	bibtex ccruncher
	latex ccruncher
	latex ccruncher
	dvipdfm ccruncher

rtf: ccruncher.tex refs.bib
	iconv -f utf8 -t iso88591 < ccruncher.tex > ccruncher-iso88591.tex
	latex ccruncher-iso88591
	bibtex ccruncher-iso88591
	latex2rtf -Z9 -S -M12 ccruncher-iso88591.tex
	mv ccruncher-iso88591.rtf ccruncher.rtf
	rm -f ccruncher-iso88591.tex

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

