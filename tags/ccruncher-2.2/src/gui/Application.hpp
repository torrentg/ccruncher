
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#ifndef _Application_
#define _Application_

#include <QApplication>

using namespace std;

namespace ccruncher_gui {

//===========================================================================
// Application class (used to catch exceptions)
//===========================================================================
class Application : public QApplication
{

  Q_OBJECT

  public:

    // constructor
    explicit Application(int &argc, char** argv);
    // notify method
    bool notify(QObject *rec, QEvent *ev);

};

}

#endif

