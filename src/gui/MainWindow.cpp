#include <iostream>
#include <map>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "ui_MainWindow.h"
#include "gui/MainWindow.hpp"
#include "gui/DefinesDialog.hpp"
#include "gui/FindDefines.hpp"
#include "utils/Utils.hpp"

#define REFRESH_MS 250

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(&timer, SIGNAL(timeout()), this, SLOT(refresh()));
  connect(&task, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)), Qt::QueuedConnection);
  check(true);
}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
  task.stop();
  task.wait();
  delete ui;
}

//===========================================================================
// select input file
//===========================================================================
void MainWindow::selectFile()
{
  QString filename = QFileDialog::getOpenFileName(
              this,
              tr("Select Input File ..."),
              QDir::currentPath(),
              tr("ccruncher files (*.xml *.gz);;All files (*.*)"));

  if (filename != "") {
    ui->ifile->setText(filename);
    setFile();
  }
}

//===========================================================================
// set input file
//===========================================================================
void MainWindow::setFile()
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
void MainWindow::selectDir()
{
  QString dirpath = QFileDialog::getExistingDirectory(
              this,
              tr("Select Ouput Directory ..."),
              QDir::currentPath(),
              QFileDialog::ShowDirsOnly);

  if (dirpath != "") {
    ui->odir->setText(dirpath);
    setDir();
  }
}

//===========================================================================
// select output directory
//===========================================================================
void MainWindow::setDir()
{
  check(true);
}

//===========================================================================
// review widget enabled/disabled
//===========================================================================
void MainWindow::check(bool clear)
{
  if (clear)
  {
    ui->log->clear();
    cout << Utils::copyright() << endl;
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
void MainWindow::run()
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
void MainWindow::print(const QString str)
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
void MainWindow::refresh()
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
void MainWindow::showDefines()
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
void MainWindow::setDefines()
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
// see TaskThread::status
//===========================================================================
void MainWindow::setStatus(int val)
{
  switch(val)
  {
    case 2: // parsing
    case 3: // simulating
      ui->progress->setValue(0);
      ui->ifile->setEnabled(false);
      ui->ifileButton->setEnabled(false);
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
      ui->ifileButton->setEnabled(true);
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

