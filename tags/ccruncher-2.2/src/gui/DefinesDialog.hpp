
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

#ifndef _DefinesDialog_
#define _DefinesDialog_

#include <map>
#include <string>
#include <QDialog>

namespace ccruncher_gui {

namespace Ui {
class DefinesDialog;
}

class DefinesDialog : public QDialog
{
    Q_OBJECT

  private:

    // dialog widgets
    Ui::DefinesDialog *ui;
    // defines
    std::map<std::string,std::string> defines;

  public:

    // constructor
    explicit DefinesDialog(QWidget *parent, const std::map<std::string, std::string> &);
    // destructor
    ~DefinesDialog();
    // return content
    const std::map<std::string,std::string> & getDefines() const;

  public slots:

    // cell content changed
    void cellChanged(int, int);
    // submit dialog content
    void submit();

};

}

#endif
