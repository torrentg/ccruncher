
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

#include <QMessageBox>
#include "gui/DefinesDialog.hpp"
#include "ui_DefinesDialog.h"

using namespace std;

//===========================================================================
// constructor
//===========================================================================
DefinesDialog::DefinesDialog(QWidget *parent, const map<string,string> &defines_) :
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
  map<string,string>::iterator it;
  for (it=defines.begin(); it != defines.end(); it++)
  {
    int row = ui->table->rowCount();
    ui->table->insertRow(row);
    ui->table->setItem(row, 0, new QTableWidgetItem(it->first.c_str()));
    ui->table->setItem(row, 1, new QTableWidgetItem(it->second.c_str()));
  }
  int row = ui->table->rowCount();
  ui->table->insertRow(row);
  ui->table->setItem(row, 0, new QTableWidgetItem(""));
  ui->table->setItem(row, 1, new QTableWidgetItem(""));
  ui->table->blockSignals(false);
}

//===========================================================================
// destructor
//===========================================================================
DefinesDialog::~DefinesDialog()
{
  delete ui;
}

//===========================================================================
// return content
//===========================================================================
const map<string,string> & DefinesDialog::getDefines() const
{
  return defines;
}

//===========================================================================
// cell content changed
//===========================================================================
void DefinesDialog::cellChanged(int row, int col)
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


//===========================================================================
// submit dialog content
//===========================================================================
void DefinesDialog::submit()
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

