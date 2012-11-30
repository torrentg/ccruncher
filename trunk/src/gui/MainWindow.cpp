#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include "ui_MainWindow.h"
#include "gui/MainWindow.hpp"
#include "gui/SimulationWidget.hpp"
#include "gui/AnalysisWidget.hpp"
#include "gui/XmlEditWidget.hpp"
#include "utils/Utils.hpp"
#include "utils/config.h"

//===========================================================================
// constructor
//===========================================================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), mainToolBar(NULL), childToolBar(NULL)
{
  ui->setupUi(this);

  // setting mdi area
  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setViewMode(QMdiArea::TabbedView);
  mdiArea->setTabsClosable(true);
  mdiArea->setTabShape(QTabWidget::Rounded); //QTabWidget::Triangular
  mdiArea->setTabsMovable(true);
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  setCentralWidget(mdiArea);

  // creating toolbar
  mainToolBar = addToolBar(tr("Main"));
  mainToolBar->addAction(ui->actionOpen);

  setWindowTitle(tr("CCruncher"));

  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(selectFile()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateToolBars(QMdiSubWindow*)));

  /*
    tema menu contextual dels tabs (restore/close), pe. afeguir entrada
    http://www.qtcentre.org/threads/25207-QMdiArea-context-menu-for-tabs

    QMdiAreaSubWindow *subWindow = mdiArea->addSubWindow(some_widget);
    QMenu *menu = child->systemMenu();
    QAction *closeAll = new QAction(tr("Close All"),menu);
    menu->addAction(closeAll);
    connect(closeAll, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
  */
  statusBar()->showMessage(tr("Ready"));
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
// close event
//===========================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
  mdiArea->closeAllSubWindows();
  if (mdiArea->currentSubWindow()) {
    event->ignore();
  } else {
    //TODO: save settings
    event->accept();
  }
}

//===========================================================================
// updateToolBars
//===========================================================================
void MainWindow::updateToolBars(QMdiSubWindow *window)
{
  if (window == NULL)
  {
    if (!mdiArea->hasFocus() && mdiArea->subWindowList().size() > 0) {
      // mdiarea has lost focus, nothing to do
    }
    else if (childToolBar != NULL) {
      // last subwindow removed
      //removeToolBar(childToolBar);
      childToolBar = NULL;
    }
    else {
      // nothing to do
    }
  }
  else
  {
    MdiChildWidget* child = dynamic_cast<MdiChildWidget*>(window->widget());
    QToolBar *toolbar = (child?child->getToolBar():NULL);
    if (childToolBar != toolbar) {
      removeToolBar(childToolBar);
      childToolBar = toolbar;
      if (childToolBar != NULL) {
        addToolBar(childToolBar);
        childToolBar->setVisible(true);
      }
    }
  }
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
              "",
              tr("ccruncher files (*.xml *.gz *.csv);;"
                 "input files (*.xml *.gz);;"
                 "output files (*.csv);;All files (*.*)")
            );

  if (!filename.isEmpty()) {
    QUrl url = QUrl::fromLocalFile(filename);
    if (!filename.toLower().endsWith("csv")) {
      url.setPath(url.toLocalFile());
      url.setScheme("exec");
    }
    openFile(url);
  }
}

//===========================================================================
// open file
//===========================================================================
void MainWindow::openFile(const QUrl &url)
{
  QWidget *child = NULL;
  QString filename = url.path();
  if (url.scheme() == "file") filename = url.toLocalFile();
  QFileInfo fileinfo(filename);

  if (fileinfo.exists() && fileinfo.isReadable())
  {
    QMdiSubWindow *existing = findMdiChild(url.toString());
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
      return;
    }

    try
    {
      if (url.scheme() == "exec") {
        child = new SimulationWidget(filename, this);
        connect(child, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(openFile(const QUrl &)));
      }
      else if (!filename.toLower().endsWith("csv")) {
        child = new XmlEditWidget(filename, this);
        connect(child, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(openFile(const QUrl &)));
      }
      else {
        child = new AnalysisWidget(filename, this);
      }
      //TODO: unrecognized filename -> send to system
    }
    catch(std::exception &e)
    {
      cout << "error opening '" << fileinfo.fileName().toStdString() << "': " << e.what() << endl;
      return;
    }

    try {
      mdiArea->addSubWindow(child);
    } catch(std::exception &e) {
      //see http://qt-project.org/forums/viewthread/18819/
    }

    if (child->windowTitle().isEmpty()) {
      child->setWindowTitle(fileinfo.fileName());
    }
    child->setWindowFilePath(url.toString());
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

