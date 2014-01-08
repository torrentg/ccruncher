
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

#ifndef _QStreamBuf_
#define _QStreamBuf_

#include <streambuf>
#include <QObject>
#include <QString>

//---------------------------------------------------------------------------

namespace ccruncher_gui {

/*
  convert streambuf call to signals
  based on http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
  caution: it is not performance optimal
  causes: string usage + string by value as argument
*/

class QStreamBuf : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

  public:

    // constructor
    QStreamBuf(QObject *parent=NULL);

  protected:

    // inherited from streambuf
    virtual int overflow(int v);
    // inherited from streambuf
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

  signals:

    // print
    void print(const QString &);

};

}

#endif

