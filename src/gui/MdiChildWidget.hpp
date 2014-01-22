
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

#ifndef _MdiChildWidget_
#define _MdiChildWidget_

#include <QWidget>
#include <QToolBar>
#include <QMainWindow>

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief MDI window with its own toolbar.
 */
class MdiChildWidget: public QWidget
{

  public:

    //! Constructor
    MdiChildWidget(QWidget *parent) : QWidget(parent, Qt::SubWindow) {}
/*
    //! Destructor
    virtual ~MdiChildWidget() {
      QMainWindow *main = dynamic_cast<QMainWindow*>(parent);
      if (main != NULL && getToolBar() != NULL) {
        main->removeToolBar(getToolBar());
      }
    }
*/
    //! Widget toolbar
    virtual QToolBar* getToolBar() { return NULL; }

};

} // namespace

#endif

