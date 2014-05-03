
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

#ifndef _TextBrowser_
#define _TextBrowser_

#include <QTextBrowser>

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief CCruncher trace widget.
 *
 * @details Referenced in SimulationWidget.ui.
 *          Overrides setSource() method to avoid unwanted behavior when
 *          clicking in links.
 */
class TextBrowser : public QTextBrowser
{

  protected:

    //! Method overrided
    void setSource(const QUrl &) {}

  public:

    //! Constructor
    TextBrowser(QWidget *parent=0) : QTextBrowser(parent) {}

};

} // namespace

#endif
