#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "ui_SimulationWidget.h"
#include "gui/SimulationWidget.hpp"
#include "gui/DefinesDialog.hpp"
#include "gui/FindDefines.hpp"
#include "utils/Utils.hpp"

#define REFRESH_MS 250

//===========================================================================
// constructor
//===========================================================================
SimulationWidget::SimulationWidget(const QString &filename, QWidget *parent) :
    QWidget(parent), ui(new Ui::SimulationWidget)
{
  ui->setupUi(this);
  task.setStreamBuf(&qstream);
  connect(&timer, SIGNAL(timeout()), this, SLOT(refresh()));
  connect(&task, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)), Qt::QueuedConnection);
  connect(&qstream, SIGNAL(print(QString)), this, SLOT(print(QString)), Qt::QueuedConnection);
  ui->ifile->setText(filename);
  //TODO: check exceptions
  setFile();
}

//===========================================================================
// destructor
//===========================================================================
SimulationWidget::~SimulationWidget()
{
  task.stop();
  task.wait();
  delete ui;
}

//===========================================================================
// set input file
//===========================================================================
void SimulationWidget::setFile()
{
  string filename = ui->ifile->text().toStdString();
  FindDefines finder = FindDefines(filename);
  defines = finder.getDefines();
  setDefines();
  check(true);
}

//===========================================================================
// select output directory
//===========================================================================
void SimulationWidget::selectDir()
{
  QString dirpath = QFileDialog::getExistingDirectory(
              this,
              tr("Select Ouput Directory ..."),
              ui->odir->text(),
              QFileDialog::ShowDirsOnly);

  if (dirpath != "") {
    ui->odir->setText(dirpath);
    setDir();
  }
}

//===========================================================================
// select output directory
//===========================================================================
void SimulationWidget::setDir()
{
  check(true);
}

//===========================================================================
// review widget enabled/disabled
//===========================================================================
void SimulationWidget::check(bool clear)
{
  if (clear)
  {
    ui->log->clear();
    ui->log->textCursor().insertText(Utils::copyright().c_str());
    ui->progress->setValue(0);
  }

  if (!QFile::exists(ui->ifile->text()))
  {
    ui->defines->setEnabled(false);
    ui->definesButton->setEnabled(false);
    ui->progress->setEnabled(false);
    ui->runButton->setEnabled(false);
    ui->progress->setValue(0);
    ui->defines->clear();
  }
  else
  {
    ui->defines->setEnabled(true);
    ui->definesButton->setEnabled(true);
    QString odir = ui->odir->text();
    if (odir.trimmed().length() > 0 && QDir(odir).exists())
    {
      ui->progress->setEnabled(true);
      ui->runButton->setEnabled(true);
    }
    else
    {
      ui->progress->setEnabled(false);
      ui->runButton->setEnabled(false);
    }
  }
}

//===========================================================================
// run ccruncher
//===========================================================================
void SimulationWidget::run()
{
  if (task.isRunning()) {
    task.stop();
    return;
  }
  else
  {
    ui->log->clear();
    string ifile = ui->ifile->text().toStdString();
    string odir = ui->odir->text().toStdString();
    task.setData(ifile, defines, odir);
    task.start();
    timer.start(REFRESH_MS);
  }
}

//===========================================================================
// print message
//===========================================================================
void SimulationWidget::print(const QString str)
{
  QTextCursor cursor = ui->log->textCursor();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(str);
  //ui->log->setTextCursor(cursor);
  //ui->log->textCursor().insertText(str);
}

//===========================================================================
// refresh progress bar
//===========================================================================
void SimulationWidget::refresh()
{
  int val = task.getProgress();
  if (val < 0) {
    ui->progress->setTextVisible(false);
    val = ui->progress->value();
    val = (val+1)%100;
  }
  else {
    ui->progress->setTextVisible(true);
  }
  ui->progress->setValue(val);
}

//===========================================================================
// show the defines dialog
//===========================================================================
void SimulationWidget::showDefines()
{
  DefinesDialog dialog(this, defines);
  int rc = dialog.exec();

  if (rc == QDialog::Accepted)
  {
    defines = dialog.getDefines();
    setDefines();
    check(true);
  }
}

//===========================================================================
// show the defines dialog
//===========================================================================
void SimulationWidget::setDefines()
{
  QString str;
  map<string,string>::iterator it;
  for (it=defines.begin(); it != defines.end(); it++)
  {
      str += QString(str.length()>0?", ":"") + it->first.c_str() + QString("=") + it->second.c_str();
  }
  ui->defines->setText(str);
}

//===========================================================================
// set status
// see SimulationTask::status
//===========================================================================
void SimulationWidget::setStatus(int val)
{
  switch(val)
  {
    case 2: // parsing
    case 3: // simulating
      ui->progress->setValue(0);
      ui->ifile->setEnabled(true);
      ui->odir->setEnabled(false);
      ui->odirButton->setEnabled(false);
      ui->defines->setEnabled(false);
      ui->definesButton->setEnabled(false);
      ui->runButton->setText(tr("Stop"));
      break;
    case 1: // inactive
    case 4: // failed
    case 5: // aborted
    case 6: // finished
      timer.stop();
      refresh();
      ui->ifile->setEnabled(true);
      ui->odir->setEnabled(true);
      ui->odirButton->setEnabled(true);
      ui->defines->setEnabled(true);
      ui->definesButton->setEnabled(true);
      ui->progress->setEnabled(false);
      ui->runButton->setText(tr("Run"));
      check(false);
      break;
    default:
      assert(false);
  }
}

