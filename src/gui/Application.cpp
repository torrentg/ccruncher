
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

#include <iostream>
#include <exception>
#include "Application.hpp"

using namespace std;

/**************************************************************************//**
 * @see http://qt-project.org/doc/qt-4.8/qapplication.html
 */
ccruncher_gui::Application::Application(int &argc, char** argv) : QApplication(argc, argv)
{
  // nothing to do
}

/**************************************************************************//**
 * @details Prints exception info and close application.
 * @see http://qt-project.org/doc/qt-4.8/qapplication.html#notify
 */
bool ccruncher_gui::Application::notify(QObject *rec, QEvent *ev)
{
  try
  {
    return QApplication::notify(rec, ev);
  }
  catch (std::exception &e)
  {
    cerr << "error: " << e.what() << endl;
    this->exit(1);
  }
  catch(...)
  {
    cerr <<
      "panic: unexpected error. Please report this bug sending input\n"
      "file, ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
    this->exit(1);
  }
  return false;
}

