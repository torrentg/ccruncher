%***************************************************************************
%
% CreditCruncher - A portfolio credit risk valorator
% Copyright (C) 2004 Gerard Torrent
%
% This program is free software; you can redistribute it and/or
% modify it under the terms of the GNU General Public License
% as published by the Free Software Foundation; either version 2
% of the License.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
%
%
% user.tex - TeX documentation file
% --------------------------------------------------------------------------
%
% 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
%   . initial release
%
%***************************************************************************

# The majority of stuff to build the PS/PDF files are included in a
# separate Makefile so that they can be invoked directly at "make
# dist" time without needing to generate a real "Makefile" from
# Makefile.am.

include Makefile.latex

# Now ensure that all the relevant files get picked up in the tarball.

EXTRA_DIST      = $(MAIN_TEX) $(OTHER_SRC_FILES) $(FIG_FILES) $(PNG_FILES) Makefile.latex
