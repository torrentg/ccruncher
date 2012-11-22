#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
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
    QWidget(parent), ui(new Ui::SimulationWidget), progress(NULL)
{
  ui->setupUi(this);
  progress = new ProgressWidget(ui->frame);
  progress->setWindowFlags(Qt::WindowStaysOnTopHint);
  ui->frame->addLayer(progress);

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
    ui->odir->setText(dirpath);
    setDir();
  }
}

//===========================================================================
// select output directory
//===========================================================================
void SimulationWidget::setDir()
{
  updateControls();
  clearLog();
}

//===========================================================================
// clear log area
//===========================================================================
void SimulationWidget::clearLog()
{
  ui->log->clear();
  ui->progress->setPalette(QPalette());
  ui->progress->setValue(0);
  log(Utils::copyright().c_str());
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
  ui->odir->setEnabled(true);
  ui->odirButton->setEnabled(true);
}

//===========================================================================
// submit task
//===========================================================================
void SimulationWidget::submit()
{
  if (task.isRunning()) {
    task.stop();
    return;
  }
  else {
    task.wait();
    ui->log->clear();
    string ifile = ui->ifile->text().toStdString();
    string odir = ui->odir->text().toStdString();
    task.setData(ifile, defines, odir);
    task.start();
  }
}

//===========================================================================
// linkify
//===========================================================================
/*
bool SimulationWidget::linkify(QString &token)
{
  if (token.startsWith("http://")) {
    token = QString("<a href='%1'>%1</a>").arg(token);
    return true;
  }
  else if (QFile(token).exists()) {
    token = QString("<a href='file:///%1'>%1</a>").arg(token);
    return true;
  }
  else {
    return false;
  }
}
*/
void SimulationWidget::linkify(QString &line)
{
  QStringList list = line.split(" ", QString::SkipEmptyParts);
  for(int i=0; i<list.size(); i++)
  {
    QString token = list.at(i);
    bool changed = true;
    if (token.startsWith("http://")) {
      token = QString("<a href='%1'>%1</a>").arg(token);
    }
    else if (QFile(token).exists() && !QDir(token).exists()) {
      QUrl url = QUrl::fromLocalFile(token);
      token = QString("<a href='%1'>%2</a>").arg(url.toString()).arg(token.replace('\\', '/'));
    }
    else {
      changed = false;
    }
    if (changed) {
      line.replace(list.at(i), token);
    }
  }
}

//===========================================================================
// log a message
//===========================================================================
void SimulationWidget::log(const QString &str)
{
  if (str.length() == 0) return;

  QTextCursor cursor = ui->log->textCursor();
  bool isatend = cursor.atBlockEnd();
  cursor.movePosition(QTextCursor::End);

  //TODO: replace static by member variable
  static QString line="";

  int pos0 = 0;
  int pos1 = str.indexOf('\n');
  while(pos1 >= 0)
  {
    line += str.mid(pos0, pos1-pos0);
    linkify(line);
    cursor.insertHtml(QString("<pre>%1<br/></pre>").arg(line));
    line = "";
    pos0 = pos1+1;
    pos1 = str.indexOf('\n', pos0);
  }
  line += str.mid(pos0);

  if (isatend) ui->log->setTextCursor(cursor);
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
    QMessageBox::StandardButton rc = QMessageBox::question(this, "title",
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

