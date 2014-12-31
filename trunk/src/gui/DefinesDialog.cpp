
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#include <QMessageBox>
#include "gui/DefinesDialog.hpp"
#include "ui_DefinesDialog.h"

using namespace std;

/**************************************************************************//**
 * @param[in] parent Widget parent.
 * @param[in] defines_ List of defines.
 */
ccruncher_gui::DefinesDialog::DefinesDialog(QWidget *parent,
    const std::map<std::string,std::string> &defines_) :
    QDialog(parent), ui(new Ui::DefinesDialog)
{
  ui->setupUi(this);
  defines = defines_;
  ui->table->clearContents();

  ui->table->blockSignals(true);
  for(int i=ui->table->rowCount()-1; i>=0; i--)
  {
    ui->table->removeRow(i);
  }
  for(auto &it : defines)
  {
    int row = ui->table->rowCount();
    ui->table->insertRow(row);
    ui->table->setItem(row, 0, new QTableWidgetItem(it.first.c_str()));
    ui->table->setItem(row, 1, new QTableWidgetItem(it.second.c_str()));
  }
  int row = ui->table->rowCount();
  ui->table->insertRow(row);
  ui->table->setItem(row, 0, new QTableWidgetItem(""));
  ui->table->setItem(row, 1, new QTableWidgetItem(""));
  ui->table->blockSignals(false);
}

/**************************************************************************/
ccruncher_gui::DefinesDialog::~DefinesDialog()
{
  delete ui;
}

/**************************************************************************//**
 * @return Defines modified by user.
 */
const map<string,string> & ccruncher_gui::DefinesDialog::getDefines() const
{
  return defines;
}

/**************************************************************************//**
 * @details Modifies rows (removes unused rows, add row if don't exist
 *          a blank row to enter a new macro).
 * @param[in] row Row index.
 * @param[in] col Column index.
 */
void ccruncher_gui::DefinesDialog::cellChanged(int row, int col)
{
  Q_UNUSED(row)
  Q_UNUSED(col)

  ui->table->blockSignals(true);

  //remove unused rows
  for(int i=ui->table->rowCount()-1; i>=0; i--)
  {
    if (ui->table->item(i, 0)->text().length() == 0 &&
        ui->table->item(i, 1)->text().length() == 0 &&
        i < ui->table->rowCount()-1)
    {
      ui->table->removeRow(i);
    }
  }

  //add blank ending row
  int nr = ui->table->rowCount();
  if (ui->table->item(nr-1, 0)->text().length() > 0 ||
      ui->table->item(nr-1, 1)->text().length() > 0 )
  {
    ui->table->insertRow(nr);
    ui->table->setItem(nr, 0, new QTableWidgetItem(""));
    ui->table->setItem(nr, 1, new QTableWidgetItem(""));
  }

  ui->table->blockSignals(false);
}


/**************************************************************************//**
 * @details Validate dialog content and close dialog.
 */
void ccruncher_gui::DefinesDialog::submit()
{
  defines.clear();

  for(int i=ui->table->rowCount()-2; i>=0; i--)
  {
    string key = ui->table->item(i, 0)->text().trimmed().toStdString();
    string value = ui->table->item(i, 1)->text().trimmed().toStdString();

    if (key == "") {
      QMessageBox::critical(this, "CCruncher", "Found a void name");
      return;
    }
    if (defines.find(key) != defines.end()) {
      QMessageBox::critical(this, "CCruncher", "Found a repeated name");
      return;
    }

    defines[key] = value;
  }

  accept();
}

