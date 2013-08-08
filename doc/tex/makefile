
#***************************************************************************
#
# CreditCruncher - A portfolio credit risk valorator
# Copyright (C) 2004-2013 Gerard Torrent
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

pdf: ccruncher.tex structure.tex bibliography.bib
	pdflatex ccruncher
	bibtex ccruncher
	pdflatex ccruncher
	pdflatex ccruncher

rtf: ccruncher.tex bibliography.bib
	# convert to iso8859
	iconv -f utf8 -t iso88591 < ccruncher.tex > ccruncher-iso88591.tex
	iconv -f utf8 -t iso88591 < structure.tex > structure-iso88591.tex
	# include structure-iso88591
	sed -i -e "s/\input{structure}/\input{structure-iso88591}/g" ccruncher-iso88591.tex
	# remove box environment in structure
	sed -i -e "s/\\\\\(begin\|end\){.Box}//g" structure-iso88591.tex
	# removes \z@ errors
	sed -i -e "s/\\\z@/0pt/g" structure-iso88591.tex
	# remove watermark
	sed -i -e "s/^.*[Ww]atermark/%REMOVED/g" ccruncher-iso88591.tex
	# recreate proof environment
	sed -i -e "s/\\\begin{proof}/\\\emph{Proof. }/g" ccruncher-iso88591.tex
	sed -i -e "s/\\\end{proof}/\\\qedsymbol/g" ccruncher-iso88591.tex
	# recreate listings
	sed -i -e "s/\\\begin{lstlisting}\\[.*\\]/\\\begin{verbatim}/g" ccruncher-iso88591.tex
	sed -i -e "s/\\\end{lstlisting}/\\\end{verbatim}/g" ccruncher-iso88591.tex
	# replace \text by \textnormal
	sed -i -e "s/\\\text{/\\\textnormal{/g" ccruncher-iso88591.tex
	# replace \upsilon by v
	sed -i -e "s/\\\upsilon/v/g" ccruncher-iso88591.tex
	# create bibtex entries
	pdflatex ccruncher-iso88591
	bibtex ccruncher-iso88591
	# create rtf document
	latex2rtf -Z9 -S -M2 -M4 -t2 -f0 -E2 ccruncher-iso88591 2>&1 | tee ccruncher-iso88591.out
	mv ccruncher-iso88591.rtf ccruncher.rtf
	rm -f ccruncher-iso88591.tex

clean:
	rm -f *.aux *.bbl *.bcf *.blg *.dvi *.idx
	rm -f *.ilg *.ind *.ptc *.toc *.log
	rm -f *.run.xml
	rm -f *~

#
#Developers tips & tricks
#--------------------------------
# yum install "tex(wrapfig.sty)"
# yum install "tex(bbm.sty)"
# miktex package manager -> mpm.exe
# set 'draft' in \documentclass properties
# check tex file using : chktex ccruncher.tex
#
