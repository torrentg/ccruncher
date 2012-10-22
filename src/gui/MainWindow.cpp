#include <iostream>
#include <map>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "utils/Utils.hpp"

#define REFRESH_MS 250

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), montecarlo(NULL)
{
  ui->setupUi(this);

  ui->ifile->setText("/home/gerard/projects/ccruncher/samples/test02.xml");
  ui->odir->setText("/home/gerard/projects/ccruncher/data");

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
  }

  check();
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
  }

  check();
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
  }
  else
  {
    ui->defines->setEnabled(true);
    ui->definesButton->setEnabled(true);
    //TODO: comprovar perque s'habilita si no existeix
    if (QDir("/").exists(ui->odir->text()))
    {
      ui->progress->setEnabled(true);
      ui->runButton->setEnabled(true);
    }
  }
}

//===========================================================================
// run ccruncher
//===========================================================================
void MainWindow::run()
{
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
    this->setCursor(Qt::WaitCursor);

    // parsing input file
    cout << Utils::copyright() << endl;
    map<string,string> defines;
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
    //TODO: reset status
    cout << endl;
    cout << e.what() << endl;
  }
}

//===========================================================================
// print message
//===========================================================================
void MainWindow::print(const QString str)
{
  ui->log->appendPlainText(str);
  //ui->log->textCursor().insertText(str);
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
      this->setCursor(Qt::ArrowCursor);
      timer.stop();
      ui->ifile->setEnabled(true);
      ui->ifileButton->setEnabled(true);
      ui->odir->setEnabled(true);
      ui->odirButton->setEnabled(true);
      ui->defines->setEnabled(true);
      ui->definesButton->setEnabled(true);
    }
  }
}

