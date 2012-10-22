#include "DefinesDialog.hpp"
#include "ui_DefinesDialog.h"

//===========================================================================
// constructor
//===========================================================================
DefinesDialog::DefinesDialog(QWidget *parent, const map<string,string> &defines_) :
    QDialog(parent), ui(new Ui::DefinesDialog)
{
  ui->setupUi(this);
  defines = defines_;
  ui->table->clearContents();

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

