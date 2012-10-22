#include <iostream>
#include <map>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "ui_MainWindow.h"
#include "gui/MainWindow.hpp"
#include "gui/DefinesDialog.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"

#define REFRESH_MS 250

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), montecarlo(NULL)
{
  ui->setupUi(this);

  ui->ifile->setText("C:/Users/gtorrent/Projects/ccruncher/samples/test04.xml");
  ui->odir->setText("C:/Users/gtorrent/Projects/ccruncher/data");

  defines["numsims"] = "10000";
  setDefines();

  connect(&timer, SIGNAL(timeout()), this, SLOT(refresh()));
  check();
}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
  deletemc();
  delete ui;
}

//===========================================================================
// delete montecarlo
//===========================================================================
void MainWindow::deletemc()
{
  timer.stop();
  if (montecarlo != NULL) {
    if (montecarlo->isRunning()) {
      montecarlo->abort();
    }
    else {
      montecarlo->cancel();
    }
    montecarlo->wait();
    delete montecarlo;
    montecarlo = NULL;
  }
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
    check();
  }
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
    check();
  }
}

//===========================================================================
// review widget enabled/disabled
//===========================================================================
void MainWindow::check()
{
  if (!QFile::exists(ui->ifile->text()))
  {
    ui->defines->setEnabled(false);
    ui->definesButton->setEnabled(false);
    ui->progress->setEnabled(false);
    ui->runButton->setEnabled(false);
    ui->progress->setValue(0);
    ui->defines->clear();
    ui->log->clear();
    cout << Utils::copyright() << endl;
  }
  else
  {
    ui->defines->setEnabled(true);
    ui->definesButton->setEnabled(true);
    if (QDir(ui->odir->text()).exists())
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
  if (montecarlo != NULL && montecarlo->isRunning()) {
    stop();
    return;
  }

  try
  {
    // reseting environment
    deletemc();
    ui->log->clear();
    ui->progress->setValue(0);
    ui->ifile->setEnabled(false);
    ui->ifileButton->setEnabled(false);
    ui->odir->setEnabled(false);
    ui->odirButton->setEnabled(false);
    ui->defines->setEnabled(false);
    ui->definesButton->setEnabled(false);
    ui->runButton->setText(tr("Stop"));
    //this->setCursor(Qt::WaitCursor);

    // parsing input file
    cout << Utils::copyright() << endl;

    // tracing some execution info
    Logger::trace("general information", '*');
    Logger::newIndentLevel();
    Logger::trace("ccruncher version", string(PACKAGE_VERSION)+" ("+string(SVN_VERSION)+")");
    Logger::trace("start time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
    Logger::trace("number of threads", Format::toString(Utils::getNumCores()));
    Logger::previousIndentLevel();

    IData idata(ui->ifile->text().toStdString(), defines);

    // creating simulation object
    montecarlo = new MonteCarlo();
    montecarlo->setFilePath(ui->odir->text().toStdString(), true);
    montecarlo->setHash(1000);
    montecarlo->setNumThreads(Utils::getNumCores());
    montecarlo->setData(idata);

    // running simulation
    timer.start(REFRESH_MS);
    //montecarlo->run();
    montecarlo->start();
    //montecarlo->wait();
  }
  catch(std::exception &e)
  {
    cout << endl;
    cout << e.what() << endl;
    stop();
  }
}

//===========================================================================
// print message
//===========================================================================
void MainWindow::stop()
{
  timer.stop();
  deletemc();
  ui->ifile->setEnabled(true);
  ui->ifileButton->setEnabled(true);
  ui->odir->setEnabled(true);
  ui->odirButton->setEnabled(true);
  ui->defines->setEnabled(true);
  ui->definesButton->setEnabled(true);
  ui->progress->setEnabled(false);
  ui->runButton->setText(tr("Run"));
  check();
  //this->setCursor(Qt::ArrowCursor);
}

//===========================================================================
// print message
//===========================================================================
void MainWindow::print(const QString str)
{
  ui->log->textCursor().insertText(str);
}

//===========================================================================
// refresh progress bar
//===========================================================================
void MainWindow::refresh()
{
  if (montecarlo != NULL)
  {
    int n = montecarlo->getNumIterations();
    int N = montecarlo->getMaxIterations();
    float val = (float) n / (float) N;
    ui->progress->setValue((int)(100.0*val));
    if (!montecarlo->isRunning() && n > 0)
    {
      // tracing some execution info
      Logger::trace("general information", '*');
      Logger::newIndentLevel();
      Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
      Logger::trace("simulations realized", Format::toString(montecarlo->getNumIterations()));
      Logger::previousIndentLevel();
      Logger::addBlankLine();

      stop();
    }
  }
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
    str += QString("-D ") + it->first.c_str() + QString("=") + it->second.c_str() + QString(" ");
  }
  ui->defines->setText(str);
}

