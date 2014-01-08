
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include "QStreamBuf.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher_gui::QStreamBuf::QStreamBuf(QObject *parent) : QObject(parent), std::basic_streambuf<char>()
{
  // nothing to do
}

//===========================================================================
// inherited from streambuf
//===========================================================================
int ccruncher_gui::QStreamBuf::overflow(int v)
{
  emit print(QString(QChar(v)));
  return v;
}

//===========================================================================
// inherited from streambuf
//===========================================================================
std::streamsize ccruncher_gui::QStreamBuf::xsputn(const char *p, std::streamsize n)
{
  emit print(QString::fromUtf8(p,n));
  return n;
}

