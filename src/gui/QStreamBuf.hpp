
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include <streambuf>
#include <QObject>
#include <QString>

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief Redirects CCruncher's trace to gui interface.
 *
 * @details Convert streambuf call to signals. It is not performance
 *          optimal because in signal/slot mechanism strings are passed
 *          by value.
 *
 * @see http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html
 * @see http://www.cplusplus.com/reference/streambuf/streambuf/
 */
class QStreamBuf : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT

  public:

    //! Constructor
    QStreamBuf(QObject *parent=nullptr);

  protected:

    //! Inherited from streambuf
    virtual int overflow(int v) override;
    //! Inherited from streambuf
    virtual std::streamsize xsputn(const char *p, std::streamsize n) override;

  signals:

    //! Print data
    void print(const QString &);

};

} // namespace
