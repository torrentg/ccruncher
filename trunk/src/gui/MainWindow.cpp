#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
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

  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
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
    "<h2>CCruncher</h2>"
    "<h3>Open-Source Tool for Credit Risk Modeling </h3>"
    "<p>"
      "version: " VERSION " (" SVN_VERSION ")<br/>"
      "build options: " + QString(Utils::getCompilationOptions().c_str()) + "<br/>"
      "url: <a href = 'http://www.ccruncher.net'>www.ccruncher.net</a>"
    "</p>");
}

//===========================================================================
// open file
//===========================================================================
void MainWindow::openFile()
{
  QString filename = QFileDialog::getOpenFileName(
              this,
              tr("Open File ..."),
              "", //ui->ifile->text(),
              tr("ccruncher files (*.xml *.gz *.csv);;input files (*.xml *.gz);;output files (*.csv);;All files (*.*)"));

  if (filename != "")
  {
    //TODO: check if file already opened
    //TODO: catch exceptions
    QWidget *child = NULL;
    if (!filename.toLower().endsWith("csv")) {
      child = new SimulationWidget(this);
    }
    else {
      child = new AnalysisWidget(filename, this);
    }
    mdiArea->addSubWindow(child);
    child->setToolTip(filename);
    QFileInfo pathInfo(filename);
    child->setWindowTitle(pathInfo.fileName());
    child->show();
  }
}

/*
//===========================================================================
// find mdi child
//===========================================================================
QMdiSubWindow* MainWindow::findMdiChild(const QString &fileName)
{
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
  foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
    MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
    if (mdiChild->currentFile() == canonicalFilePath)
      return window;
  }
  return 0;
}
*/

