
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include <map>
#include <string>
#include <QDialog>

namespace ccruncher_gui {

namespace Ui {
class DefinesDialog;
}

/**************************************************************************//**
 * @brief Dialog to manage user-defined macros.
 */
class DefinesDialog : public QDialog
{
    Q_OBJECT

  private:

    //! Dialog widgets
    Ui::DefinesDialog *ui;
    //! Defines
    std::map<std::string,std::string> defines;

  public:

    //! Constructor
    DefinesDialog(QWidget *parent, const std::map<std::string, std::string> &);
    //! Destructor
    virtual ~DefinesDialog() override;
    //! Return content
    const std::map<std::string,std::string> & getDefines() const;

  public slots:

    //! Cell content changed
    void cellChanged(int, int);
    //! Submit dialog content
    void submit();

};

} // namespace
