#include <iostream>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "ui_SimulationWidget.h"
#include "gui/SimulationWidget.hpp"
#include "gui/DefinesDialog.hpp"
#include "gui/FindDefines.hpp"
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
  connect(&qstream, SIGNAL(print(QString)), this, SLOT(log(QString)), Qt::QueuedConnection);
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
  ui->log->textCursor().insertText(Utils::copyright().c_str());
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
    ui->progress->setValue(0);
    ui->odir->setEnabled(false);
    ui->odirButton->setEnabled(false);
    ui->defines->setEnabled(false);
    ui->definesButton->setEnabled(false);
    ui->runButton->setText(tr("Stop"));
    task.setData(ifile, defines, odir);
    task.start();
  }
}

//===========================================================================
// log a message
//===========================================================================
void SimulationWidget::log(const QString str)
{
  QTextCursor cursor = ui->log->textCursor();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(str);
  //ui->log->setTextCursor(cursor);
  //ui->log->textCursor().insertText(str);
}

//===========================================================================
// draw
//===========================================================================
void SimulationWidget::draw()
{
  if (task.getStatus() == SimulationTask::reading)
  {
    /*
    size_t totalbytes = task.getCsvFile().getFileSize();
    size_t readedbytes = task.getCsvFile().getReadedSize();
    float pct = 100.0 * (float)(readedbytes)/(float)(totalbytes);
    QString str = Format::bytes(readedbytes).c_str();
    progress->ui->progress->setValue(pct+0.5);
    progress->ui->progress->setFormat(str);
    */
    //TODO: complete code
    progress->show();
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
      val = (int)((float)(nsims)/(float)(msims) + 0.5);
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
// set status
// see SimulationTask::status
//===========================================================================
void SimulationWidget::setStatus(int val)
{
  switch(val)
  {
    case SimulationTask::reading:
      //TODO: set fadein instead of show
      progress->ui->progress->setValue(0);
      progress->ui->progress->setFormat("");
      progress->show();
      timer.start(REFRESH_MS);
      break;
    case SimulationTask::simulating: {
      /*
      size_t readedbytes = task.getCsvFile().getReadedSize();
      QString str = Format::bytes(readedbytes).c_str();
      progress->ui->progress->setFormat(str);
      */
      //TODO: complete this
      progress->ui->progress->setValue(100);
      progress->fade();
      task.free(1);
      break;
    }
    case SimulationTask::failed:
      // set progressbar to 100% and red color
    case SimulationTask::stopped:
      // let progress bar unchanged (if maxsims>0), 100% otherwise
    case SimulationTask::finished:
      timer.stop();
      task.free();
      ui->runButton->setText(tr("Run"));
      ui->progress->setFormat("");
      ui->progress->setValue(100);
      updateControls();
      break;
    default:
      assert(false);
  }
}

