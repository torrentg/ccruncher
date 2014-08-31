
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCleanlooksStyle>
#include "ui_SimulationWidget.h"
#include "gui/SimulationWidget.hpp"
#include "gui/MainWindow.hpp"
#include "gui/DefinesDialog.hpp"
#include "gui/FindDefines.hpp"
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/Format.hpp"
#include "utils/Utils.hpp"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;

#define REFRESH_MS 100

/**************************************************************************//**
 * @param[in] filename CCruncher input file.
 * @param[in] parent Widget parent.
 */
ccruncher_gui::SimulationWidget::SimulationWidget(const QString &filename, QWidget *parent) :
    MdiChildWidget(parent), ui(new Ui::SimulationWidget), progress(nullptr),
    toolbar(nullptr)
{
  ui->setupUi(this);
  ui->progress->setStyle(new QCleanlooksStyle()); //QCleanlooksStyle, QMacStyle, QPlastiqueStyle, QWindowsXPStyle
  progress = new ProgressWidget(ui->frame);
  progress->setWindowFlags(Qt::WindowStaysOnTopHint);
  ui->frame->addLayer(progress);

  // edit action
  actionEdit = new QAction(QIcon(":/images/edit.png"), tr("&Edit"), this);
  actionEdit->setStatusTip(tr("Edit xml file"));
  connect(actionEdit, SIGNAL(triggered()), this, SLOT(editFile()));

  // defines action
  actionDefines = new QAction(QIcon(":/images/config.png"), tr("&Defines"), this);
  actionDefines->setStatusTip(tr("Set current defines"));
  connect(actionDefines, SIGNAL(triggered()), this, SLOT(showDefines()));

  // run action
  actionRun = new QAction(QIcon(":/images/run.png"), tr("&Run"), this);
  actionRun->setStatusTip(tr("Run simulation"));
  connect(actionRun, SIGNAL(triggered()), this, SLOT(submit()));

  // stop action
  actionStop = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
  actionStop->setStatusTip(tr("Stops current simulation"));
  connect(actionStop, SIGNAL(triggered()), this, SLOT(submit()));

  // analysis action
  actionAnal = new QAction(QIcon(":/images/chart.png"), tr("&Analysis"), this);
  actionAnal->setStatusTip(tr("Data analysis"));
  connect(actionAnal, SIGNAL(triggered()), this, SLOT(openData()));

  // creating toolbar
  toolbar = new QToolBar(tr("Simulation"), this);
  toolbar->addAction(actionEdit);
  toolbar->addAction(actionAnal);
  toolbar->addSeparator();
  toolbar->addAction(actionDefines);
  toolbar->addAction(actionRun);
  toolbar->addAction(actionStop);

  MainWindow *main = dynamic_cast<MainWindow*>(parent);
  if (main != nullptr) properties = main->getProperties();

  task.setStreamBuf(&qstream);
  connect(&timer, SIGNAL(timeout()), this, SLOT(draw()));
  connect(&task, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)), Qt::QueuedConnection);
  connect(&qstream, SIGNAL(print(const QString &)), this, SLOT(log(const QString &)), Qt::QueuedConnection);
  connect(ui->log, SIGNAL(anchorClicked(const QUrl &)), this, SIGNAL(anchorClicked(const QUrl &)));
  ui->ifile->setText(filename);
  //TODO: check exceptions
  setFile();
}

/**************************************************************************/
ccruncher_gui::SimulationWidget::~SimulationWidget()
{
  task.stop();
  task.wait();
  if (toolbar != nullptr) delete toolbar;
  delete ui;
}

/**************************************************************************//**
 * @details Opens XML editor to edit the input file.
 */
void ccruncher_gui::SimulationWidget::editFile()
{
  QUrl url = QUrl::fromLocalFile(ui->ifile->text());
  emit anchorClicked(url);
}

/**************************************************************************//**
 * @details Opens data analysis window to analyze a CSV output file.
 */
void ccruncher_gui::SimulationWidget::openData()
{
  QString html = ui->log->document()->toHtml();
  QRegExp regexp("href=[\"'](file://[^\"']*\\.csv)[\"']");
  int pos = 0;

  while ((pos = regexp.indexIn(html, pos)) != -1)
  {
    QUrl url(regexp.cap(1));
    emit anchorClicked(url);
    pos += regexp.matchedLength();
  }
}

/**************************************************************************//**
 * @details Update info related to file.
 */
void ccruncher_gui::SimulationWidget::setFile()
{
  string filename = ui->ifile->text().toStdString();
  FindDefines finder = FindDefines(filename);
  defines = finder.getDefines();
  setDefines();
  updateControls();
  clearLog();
}

/**************************************************************************/
void ccruncher_gui::SimulationWidget::selectDir()
{
  QString dirpath = QFileDialog::getExistingDirectory(
              this,
              tr("Select Ouput Directory ..."),
              ui->odir->text(),
              QFileDialog::ShowDirsOnly);

  if (dirpath != "") {
    ui->odir->setText(QDir::fromNativeSeparators(dirpath));
    setDir();
  }
}

/**************************************************************************//**
 * @details Update info related to output directory.
 */
void ccruncher_gui::SimulationWidget::setDir()
{
  if (ui->odir->text() == odirAux) {
    return;
  }
  else {
    odirAux = ui->odir->text();
    updateControls();
    clearLog();
  }
}

/**************************************************************************/
void ccruncher_gui::SimulationWidget::clearLog()
{
  actionAnal->setEnabled(false);
  ui->log->clear();
  task.getLogger() << copyright << endl;
  logcursor = ui->log->textCursor();
  logcursor.movePosition(QTextCursor::End);
  QTextBlockFormat blockFormat = logcursor.blockFormat();
  blockFormat.setAlignment(Qt::AlignCenter);
  logcursor.setBlockFormat(blockFormat);
  ui->progress->setPalette(QPalette());
  ui->progress->setValue(0);
}

/**************************************************************************//**
 * @details Review widgets enabled/disabled.
 */
void ccruncher_gui::SimulationWidget::updateControls()
{
  if (!QFile::exists(ui->ifile->text()))
  {
    ui->defines->setEnabled(false);
    ui->definesButton->setEnabled(false);
    actionDefines->setEnabled(false);
    ui->progress->setEnabled(false);
    ui->runButton->setEnabled(false);
    actionRun->setEnabled(false);
    ui->progress->setValue(0);
    ui->defines->clear();
  }
  else
  {
    ui->defines->setEnabled(true);
    ui->definesButton->setEnabled(true);
    actionDefines->setEnabled(true);
    QString odir = ui->odir->text();
    if (odir.trimmed().length() > 0 && QDir(odir).exists())
    {
      ui->progress->setEnabled(true);
      ui->runButton->setEnabled(true);
      actionRun->setEnabled(true);
    }
    else
    {
      ui->progress->setEnabled(false);
      ui->runButton->setEnabled(false);
      actionRun->setEnabled(false);
    }
  }
  ui->odir->setEnabled(true);
  ui->odirButton->setEnabled(true);
  actionStop->setEnabled(false);
}

/**************************************************************************/
void ccruncher_gui::SimulationWidget::submit()
{
  if (task.isRunning()) {
    task.stop();
  }
  else {
    if (SimulationTask::getNumRunningSims() > 0)
    {
      QMessageBox::StandardButton rc = QMessageBox::warning(this, tr("CCruncher"),
           tr("There is already another simulation in progress.\n Are you sure to continue?"),
           QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
      if (rc != QMessageBox::Yes) {
        return;
      }
    }

    task.wait();

    unsigned char ithreads = Utils::getNumCores();
    if (properties.find("ithreads") != properties.end()) {
      ithreads = (unsigned char)(properties["ithreads"].toInt());
    }
    string ifile = ui->ifile->text().toStdString();
    string odir = ui->odir->text().toStdString();
    task.setData(ifile, defines, odir, ithreads);
    bool rc = task.checkConflicts();
    if (rc) {
      if (fout.is_open()) fout.close();
      string filename = odir + "/ccruncher.out";
      fout.open(filename.c_str());
      clearLog();
      actionDefines->setEnabled(false);
      actionRun->setEnabled(false);
      actionStop->setEnabled(true);
      task.start();
    }
  }
}

/**************************************************************************//**
 * @details Modify log trace in order to set clickable links.
 * @param[in] line Line to linkify.
 */
void ccruncher_gui::SimulationWidget::linkify(QString &line)
{
  int pos;

  // linkifying files
  QRegExp regexp1("\\[(.*)\\]");
  pos = regexp1.indexIn(line, 0);
  if (pos != -1)
  {
    QString token = regexp1.cap(1).trimmed();
    if (QFile(token).exists()) // && !QDir(token).exists())
    {
      QString filename = QFileInfo(token).fileName();
      size_t ncols = task.getLogger().getNumCols();
      int len = regexp1.matchedLength();
      if (line.length() > (int)ncols) {
        len = std::max((size_t)0, len-(line.length()-ncols));
      }
      QString padding = "";
      for(int i=0; i<len-filename.length(); i++) padding += "&nbsp;";
      QUrl url = QUrl::fromLocalFile(token);
      token = padding + QString("<a href='%1'>%2</a>").arg(url.toString()).arg(filename);
      line.replace(regexp1.cap(), token);
    }
  }

  // linkifying http refs
  QRegExp regexp2("http://([^ \t\n]*)");
  pos = regexp2.indexIn(line, 0);
  if (pos != -1)
  {
    int len = regexp2.matchedLength();
    QString token = QString("<a href='%1'>%1</a>").arg(regexp2.cap());
    line.replace(pos, len, token);
  }

  // replace ending spaces by &nbsp;
  size_t num = 0;
  for(int i=line.length()-1; i>=0; i--) {
    if (line.at(i) == ' ') num++;
    else break;
  }
  if (num > 0) {
    line = line.mid(0, line.length()-num);
    for(size_t i=0; i<num; i++) {
      line += "&nbsp;";
    }
  }
}

/**************************************************************************//**
 * @param[in] str Message to log.
 */
void ccruncher_gui::SimulationWidget::log(const QString &str)
{
  if (str.length() == 0) return;

  if (fout.is_open()) {
    fout << str.toStdString();
  }

  int pos0 = 0;
  int pos1 = str.indexOf('\n');
  while(pos1 >= 0)
  {
    logline += str.mid(pos0, pos1-pos0);
    linkify(logline);

    QTextBlockFormat blockFormat = logcursor.blockFormat();
    blockFormat.setAlignment(Qt::AlignCenter);
    logcursor.setBlockFormat(blockFormat);
    logcursor.insertHtml(QString("<pre>%1<br/></pre>").arg(logline));

    logline = "";
    pos0 = pos1+1;
    pos1 = str.indexOf('\n', pos0);
  }
  logline += str.mid(pos0);
}

/**************************************************************************//**
 * @details Update widget status.
 */
void ccruncher_gui::SimulationWidget::draw()
{
  mutex.lock();

  if (task.getStatus() == SimulationTask::reading)
  {
    if (task.getIData() == nullptr) return;
    size_t mbytes = task.getIData()->getFileSize();
    size_t nbytes = task.getIData()->getReadedSize();

    QString str = Format::bytes(nbytes).c_str();
    progress->ui->progress->setFormat(str);

    int val = (int)(100.0*(float)(nbytes)/(float)(mbytes) + 0.5);
    progress->ui->progress->setValue(val);
  }
  else
  {
    if (task.getMonteCarlo() == nullptr) return;
    size_t msims = task.getMonteCarlo()->getMaxIterations();
    size_t nsims = task.getMonteCarlo()->getNumIterations();

    QString str = QString::number(nsims);
    ui->progress->setFormat(str);

    int val = 0;
    if (msims > 0) {
      val = (int)(100.0*(float)(nsims)/(float)(msims) + 0.5);
    }
    else {
      val = ui->progress->value();
      val += 1.0;
      if (val > 100) {
        ui->progress->setInvertedAppearance(!ui->progress->invertedAppearance());
        val = 0;
      }
    }
    ui->progress->setValue(val);
  }

  mutex.unlock();
}

/**************************************************************************/
void ccruncher_gui::SimulationWidget::showDefines()
{
  // checking for new defines in xml file ...
  FindDefines finder = FindDefines(ui->ifile->text().toStdString());
  map<string,string> aux = finder.getDefines();
  for(auto &it : aux) {
    if (defines.find(it.first) == defines.end()) {
      defines[it.first] = it.second;
    }
  }

  DefinesDialog dialog(this, defines);
  int rc = dialog.exec();

  if (rc == QDialog::Accepted)
  {
    defines = dialog.getDefines();
    setDefines();
    updateControls();
    clearLog();
  }
}

/**************************************************************************//**
 * @details Update info related to defines.
 */
void ccruncher_gui::SimulationWidget::setDefines()
{
  QString str;
  for(auto &it : defines)
  {
    str += QString(str.length()>0?", ":"") + it.first.c_str() +
           QString("=") + it.second.c_str();
  }
  ui->defines->setText(str);
}

/**************************************************************************//**
 * @param[in] event Event info.
 */
void ccruncher_gui::SimulationWidget::closeEvent(QCloseEvent *event)
{
  if (task.isRunning()) {
    QMessageBox::StandardButton rc = QMessageBox::question(this, tr("CCruncher"),
       tr("There is a Monte Carlo simulation in progress.\nDo you want to stop it?"),
       QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (rc != QMessageBox::Yes) {
      event->ignore();
      return;
    }
  }
  event->accept();
}

/**************************************************************************//**
 * @param[in] val New status.
 */
void ccruncher_gui::SimulationWidget::setStatus(int val)
{
  mutex.lock();
  switch(val)
  {
    case SimulationTask::reading:
      ui->progress->setValue(0);
      ui->odir->setEnabled(false);
      ui->odirButton->setEnabled(false);
      ui->defines->setEnabled(false);
      ui->definesButton->setEnabled(false);
      ui->runButton->setText(tr("Stop"));
      ui->progress->setPalette(QPalette());
      progress->ui->progress->setValue(0);
      progress->ui->progress->setFormat("");
      progress->fadein();
      timer.start(REFRESH_MS);
      break;
    case SimulationTask::simulating: {
      progress->ui->progress->setFormat("");
      progress->ui->progress->setValue(100);
      actionAnal->setEnabled(true);
      progress->fadeout();
      task.free(1);
      break;
    }
    case SimulationTask::failed: {
      QPalette pal = ui->progress->palette();
      pal.setColor(QPalette::Highlight, Qt::red);
      ui->progress->setPalette(pal);
    }
    case SimulationTask::stopped:
      // let progress bar unchanged (if maxsims>0), 100% otherwise
      progress->fadeout();
    case SimulationTask::finished:
      timer.stop();
      task.free();
      if (fout.is_open()) fout.close();
      // moves log to end
      ui->log->setTextCursor(logcursor);
      ui->runButton->setText(tr("Run"));
      ui->progress->setFormat("");
      ui->progress->setValue(100);
      updateControls();
      //cout << "HTML" << endl << ui->log->toHtml().toStdString() << endl;
      break;
    default:
      assert(false);
  }
  mutex.unlock();
}

