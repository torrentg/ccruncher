#include <iostream>
#include <map>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "utils/Utils.hpp"

using namespace std;

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    qout(NULL), qerr(NULL), montecarlo(NULL)
{
  ui->setupUi(this);
  qout = new QDebugStream(std::cout, ui->log);
  qerr = new QDebugStream(std::cerr, ui->log);
  check();
}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
  if (qout != NULL) delete qout;
  if (qout != NULL) delete qerr;
  if (montecarlo != NULL) delete montecarlo;
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
  }
  else
  {
    ui->defines->setEnabled(true);
    ui->definesButton->setEnabled(true);
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

  try
  {
    // parsing input file
    map<string,string> defines;
    IData idata(ui->ifile->text().toStdString(), defines);

    // creating simulation object
    montecarlo = new MonteCarlo();
    montecarlo->setFilePath(ui->odir->text().toStdString(), true);
    montecarlo->setHash(1000);
    montecarlo->setNumThreads(Utils::getNumCores());
    montecarlo->setData(idata);
    montecarlo->start();
    //montecarlo.wait();
  }
  catch(std::exception &e)
  {
    cout << endl;
    cout << e.what() << endl;
  }

  cout << "done" << endl;
}

