#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCleanlooksStyle>
#include "ui_SimulationWidget.h"
#include "gui/SimulationWidget.hpp"
#include "gui/DefinesDialog.hpp"
#include "gui/FindDefines.hpp"
#include "utils/Format.hpp"
#include "utils/Utils.hpp"

#define REFRESH_MS 100

//===========================================================================
// constructor
//===========================================================================
SimulationWidget::SimulationWidget(const QString &filename, QWidget *parent) :
    MdiChildWidget(parent), ui(new Ui::SimulationWidget), progress(NULL),
    toolbar(NULL)
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

  task.setStreamBuf(&qstream);
  connect(&timer, SIGNAL(timeout()), this, SLOT(draw()));
  connect(&task, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)), Qt::QueuedConnection);
  connect(&qstream, SIGNAL(print(const QString &)), this, SLOT(log(const QString &)), Qt::QueuedConnection);
  connect(ui->log, SIGNAL(anchorClicked(const QUrl &)), this, SIGNAL(anchorClicked(const QUrl &)));
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
  if (toolbar != NULL) delete toolbar;
  delete ui;
}

//===========================================================================
// edit current file
//===========================================================================
void SimulationWidget::editFile()
{
  QUrl url = QUrl::fromLocalFile(ui->ifile->text());
  emit anchorClicked(url);
}

//===========================================================================
// open csv data files
//===========================================================================
void SimulationWidget::openData()
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

//===========================================================================
// set input file
//===========================================================================
void SimulationWidget::setFile()
{
  string filename = ui->ifile->text().toStdString();
  FindDefines finder = FindDefines(filename);
  defines = finder.getDefines();
  setDefines();
  updateControls();
  clearLog();
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
    ui->odir->setText(QDir::fromNativeSeparators(dirpath));
    setDir();
  }
}

//===========================================================================
// select output directory
//===========================================================================
void SimulationWidget::setDir()
{
  //TODO: check if odir changed
  updateControls();
  clearLog();
}

//===========================================================================
// clear log area
//===========================================================================
void SimulationWidget::clearLog()
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

//===========================================================================
// review widget enabled/disabled
//===========================================================================
void SimulationWidget::updateControls()
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

//===========================================================================
// submit task
//===========================================================================
void SimulationWidget::submit()
{
  if (task.isRunning()) {
    task.stop();
  }
  else {
    if (SimulationTask::getNumRunningSims() > 0)
    {
      QMessageBox::StandardButton rc = QMessageBox::warning(this, "CCruncher",
           "There is already another simulation in progress.\n Are you sure to continue?",
           QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
      if (rc != QMessageBox::Yes) {
        return;
      }
    }

    task.wait();
    clearLog();
    actionDefines->setEnabled(false);
    actionRun->setEnabled(false);
    actionStop->setEnabled(true);
    string ifile = ui->ifile->text().toStdString();
    string odir = ui->odir->text().toStdString();
    task.setData(ifile, defines, odir);
    task.start();
  }
}

//===========================================================================
// linkify
//===========================================================================
void SimulationWidget::linkify(QString &line)
{
  QStringList list = line.split(" ", QString::SkipEmptyParts);
  //TODO: apply only to last element
  for(int i=0; i<list.size(); i++)
  {
    QString token = list.at(i);

    if (token.startsWith("http://")) {
      token = QString("<a href='%1'>%1</a>").arg(token);
    }
    else if (QFile(token).exists() && !QDir(token).exists()) {
      QString filename = QFileInfo(token).fileName();
      size_t ncols = task.getLogger().getNumCols();
      int len = token.length();
      if (line.length() > (int)ncols) {
        len = std::max((size_t)0, token.length()-(line.length()-ncols));
      }
      QString padding = "";
      for(int i=0; i<len-filename.length(); i++) padding += "&nbsp;";
      QUrl url = QUrl::fromLocalFile(token);
      token = padding + QString("<a href='%1'>%2</a>").arg(url.toString()).arg(filename);
    }

    if (token != list.at(i)) {
      line.replace(list.at(i), token);
    }
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

//===========================================================================
// log a message
//===========================================================================
void SimulationWidget::log(const QString &str)
{
  if (str.length() == 0) return;

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

//===========================================================================
// draw
//===========================================================================
void SimulationWidget::draw()
{
  mutex.lock();

  if (task.getStatus() == SimulationTask::reading)
  {
    if (task.getIData() == NULL) return;
    size_t mbytes = task.getIData()->getFileSize();
    size_t nbytes = task.getIData()->getReadedSize();

    QString str = Format::bytes(nbytes).c_str();
    progress->ui->progress->setFormat(str);

    int val = (int)(100.0*(float)(nbytes)/(float)(mbytes) + 0.5);
    progress->ui->progress->setValue(val);
  }
  else
  {
    if (task.getMonteCarlo() == NULL) return;
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
        //TODO: review linux case (don't invert?)
        ui->progress->setInvertedAppearance(!ui->progress->invertedAppearance());
        val = 0;
      }
    }
    ui->progress->setValue(val);
  }

  mutex.unlock();
}

//===========================================================================
// show the defines dialog
//===========================================================================
void SimulationWidget::showDefines()
{
  // checking for new defines in xml file ...
  FindDefines finder = FindDefines(ui->ifile->text().toStdString());
  map<string,string> aux = finder.getDefines();
  map<string,string>::iterator it;
  for (it=aux.begin(); it != aux.end(); it++) {
    if (defines.find(it->first) == defines.end()) {
      defines[it->first] = it->second;
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
// close
//===========================================================================
void SimulationWidget::closeEvent(QCloseEvent *event)
{
  if (task.isRunning()) {
    QMessageBox::StandardButton rc = QMessageBox::question(this, "CCruncher",
       "There is a Monte Carlo simulation in progress.\nDo you want to stop it?",
       QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (rc != QMessageBox::Yes) {
      event->ignore();
      return;
    }
  }
  event->accept();
}

//===========================================================================
// set status
//===========================================================================
void SimulationWidget::setStatus(int val)
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

