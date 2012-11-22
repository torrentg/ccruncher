#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include "ui_MainWindow.h"
#include "gui/MainWindow.hpp"
#include "gui/SimulationWidget.hpp"
#include "gui/AnalysisWidget.hpp"
#include "utils/Utils.hpp"
#include "utils/config.h"

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);

  mdiArea->setViewMode(QMdiArea::TabbedView);
  mdiArea->setTabsClosable(true);
  mdiArea->setTabShape(QTabWidget::Rounded); //QTabWidget::Triangular
  mdiArea->setTabsMovable(true);

  setWindowTitle(tr("CCruncher"));
  setUnifiedTitleAndToolBarOnMac(true);

  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(selectFile()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

  /*
    tema menu contextual dels tabs (restore/close), pe. afeguir entrada
    http://www.qtcentre.org/threads/25207-QMdiArea-context-menu-for-tabs

    QMdiAreaSubWindow *subWindow = mdiArea->addSubWindow(some_widget);
    QMenu *menu = child->systemMenu();
    QAction *closeAll = new QAction(tr("Close All"),menu);
    menu->addAction(closeAll);
    connect(closeAll, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
  */

}

//===========================================================================
// destructor
//===========================================================================
MainWindow::~MainWindow()
{
  delete ui;
  delete mdiArea;
}

//===========================================================================
// close app
//===========================================================================
void MainWindow::exit()
{
  //TODO: check that any simulation is running
  this->close();
}

//===========================================================================
// about dialog
//===========================================================================
void MainWindow::about()
{
  QMessageBox::about(NULL, tr("About ..."),
    "<h3>ccruncher-" VERSION " (" SVN_VERSION ")</h3>"
    //"<p>Open-Source Tool for<br/> Credit Risk Modeling </p>"
    "<p>"
      "copyright: Gerard Torrent<br/><br/>"
      "license: GPL<br/><br/>"
      "url: <a href = 'http://www.ccruncher.net'>www.ccruncher.net</a>"
    "</p>");
}

//===========================================================================
// select file
//===========================================================================
void MainWindow::selectFile()
{
  QString filename = QFileDialog::getOpenFileName(
              this,
              tr("Open File ..."),
              "", //ui->ifile->text(),
              tr("ccruncher files (*.xml *.gz *.csv);;input files (*.xml *.gz);;output files (*.csv);;All files (*.*)"));

  if (!filename.isEmpty()) {
    QUrl url = QUrl::fromLocalFile(filename);
    openFile(url);
  }
}

//===========================================================================
// open file
//===========================================================================
void MainWindow::openFile(const QUrl &url)
{
  QString filename = url.toLocalFile();

  //TODO: replace filename by QUrl
  if (!filename.isEmpty())
  {
    QMdiSubWindow *existing = findMdiChild(filename);
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
      return;
    }

    //TODO: catch exceptions
    QWidget *child = NULL;
    if (!filename.toLower().endsWith("csv")) {
      child = new SimulationWidget(filename, this);
      connect(child, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(openFile(const QUrl &)));
    }
    else {
      child = new AnalysisWidget(filename, this);
    }

    try {
      mdiArea->addSubWindow(child);
    } catch(std::exception &e) {
      //see http://qt-project.org/forums/viewthread/18819/
    }

    QFileInfo pathInfo(filename);
    child->setWindowTitle(pathInfo.fileName());
    child->setWindowFilePath(filename);
    child->show();
  }
}

//===========================================================================
// find mdi child
//===========================================================================
QMdiSubWindow* MainWindow::findMdiChild(const QString &filename)
{
  foreach (QMdiSubWindow *child, mdiArea->subWindowList()) {
    if (child->widget()->windowFilePath() == filename) {
      return child;
    }
  }
  return NULL;
}

