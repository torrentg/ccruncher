
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QPixmap>
#include <QDesktopServices>
#include "ui_MainWindow.h"
#include "gui/MainWindow.hpp"
#include "gui/SimulationWidget.hpp"
#include "gui/AnalysisWidget.hpp"
#include "gui/XmlEditWidget.hpp"
#include "utils/config.h"

using namespace std;

//===========================================================================
// constructor
//===========================================================================
ccruncher_gui::MainWindow::MainWindow(const QMap<QString, QVariant> &map, QWidget *parent) : QMainWindow(parent),
  ui(new Ui::MainWindow), mainToolBar(NULL), childToolBar(NULL), properties(map),
  network(this)
{
  ui->setupUi(this);
  setAcceptDrops(true);

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

  connect(&network, SIGNAL(finished(QNetworkReply*)), this, SLOT(processHttpRequest(QNetworkReply*)));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(selectFile()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateToolBars(QMdiSubWindow*)));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(setStatusMsg()));

  // sending http request
  QUrl url("http://www.ccruncher.net/version?m=gui&v=" VERSION);
  network.get(QNetworkRequest(url));

  statusBar()->showMessage(tr("Ready"));
}

//===========================================================================
// destructor
//===========================================================================
ccruncher_gui::MainWindow::~MainWindow()
{
  delete ui;
  delete mdiArea;
}

//===========================================================================
// close event
//===========================================================================
void ccruncher_gui::MainWindow::closeEvent(QCloseEvent *event)
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
// drag-and-drop event
// see http://qt-project.org/wiki/Drag_and_Drop_of_files
//===========================================================================
void ccruncher_gui::MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

void ccruncher_gui::MainWindow::dragMoveEvent(QDragMoveEvent* event) {
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

void ccruncher_gui::MainWindow::dragLeaveEvent(QDragLeaveEvent* event) {
  event->accept();
}

void ccruncher_gui::MainWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* mimeData = event->mimeData();

  // check for our needed mime type, here a file or a list of files
  if (mimeData->hasUrls()) {
    QList<QUrl> urlList = mimeData->urls();
    // extract the local paths of the files
    for (int i=0; i<urlList.size() && i<32; ++i) {
      openFile(urlList.at(i));
    }
  }
}

//===========================================================================
// updateToolBars
//===========================================================================
void ccruncher_gui::MainWindow::updateToolBars(QMdiSubWindow *window)
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
void ccruncher_gui::MainWindow::about()
{
  QMessageBox about;

  about.setWindowTitle("About ...");
  about.setText("<h3>CCruncher</h3>");
  about.setInformativeText(
        "<p>"
        "version: " VERSION " [" SVN_VERSION "]<br/><br/>"
        "copyright: <a href='http://www.tatine.es'>Tatine</a><br/><br/>"
        "license: GPL<br/><br/>"
        "url: <a href='http://www.ccruncher.net'>www.ccruncher.net</a><br/><br/>"
        "</p>");
  about.setStandardButtons(QMessageBox::Ok);
  about.setDefaultButton(QMessageBox::Ok);
  about.setIconPixmap(QPixmap(":/images/logo.png"));
  about.exec();
}

//===========================================================================
// select file
//===========================================================================
void ccruncher_gui::MainWindow::selectFile()
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
      //url.setScheme("exec");
    }
    openFile(url);
  }
}

//===========================================================================
// open file
//===========================================================================
void ccruncher_gui::MainWindow::openFile(const QUrl &url)
{
  QString filename = url.path();
  if (url.scheme() == "file") filename = url.toLocalFile();
  QFileInfo fileinfo(filename);

  if (fileinfo.exists() && fileinfo.isReadable())
  {
    if (QDir(filename).exists()) {
      QString path = QDir::toNativeSeparators(filename);
      QDesktopServices::openUrl(QUrl("file:///" + path));
      return;
    }

    QMdiSubWindow *existing = findMdiChild(url.toString());
    if (existing) {
      mdiArea->setActiveSubWindow(existing);
      AnalysisWidget *analysis = dynamic_cast<AnalysisWidget*>(existing->widget());
      if (analysis != NULL) analysis->refresh();
      return;
    }

    QWidget *child = NULL;
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
        connect(child, SIGNAL(newStatusMsg(const QString &)), this, SLOT(setStatusMsg(const QString &)));
      }
      //TODO: unrecognized filename -> send to system
    }
    catch(std::exception &e)
    {
      cout << "error opening '" << fileinfo.fileName().toStdString() << "': " << e.what() << endl;
      return;
    }

    try {
      QMdiSubWindow *subwindow = mdiArea->addSubWindow(child);
      subwindow->setWindowIcon(child->windowIcon());
    } catch(std::exception &e) {
      //see http://qt-project.org/forums/viewthread/18819/
    }

    if (child->windowTitle().isEmpty()) {
      child->setWindowTitle(fileinfo.fileName());
    }
    child->setWindowFilePath(url.toString());
    child->show();
    child->nextInFocusChain()->setFocus();
  }
}

//===========================================================================
// find mdi child
//===========================================================================
QMdiSubWindow* ccruncher_gui::MainWindow::findMdiChild(const QString &filename)
{
  foreach (QMdiSubWindow *child, mdiArea->subWindowList()) {
    if (child->widget()->windowFilePath() == filename) {
      return child;
    }
  }
  return NULL;
}

//===========================================================================
// return properties
//===========================================================================
const QMap<QString,QVariant>& ccruncher_gui::MainWindow::getProperties() const
{
  return properties;
}

//===========================================================================
// process http request
//===========================================================================
void ccruncher_gui::MainWindow::processHttpRequest(QNetworkReply *reply)
{
  // catching redirection
  QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
  if (!redirection.isEmpty()) {
    reply->deleteLater();
    network.get(QNetworkRequest(redirection));
    return;
  }

  // reading status
  QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if (!status.isValid() || reply->error() != QNetworkReply::NoError) {
    reply->deleteLater();
    return;
  }

  // process response
  QString response(reply->readAll());
  reply->deleteLater();

  QString version = "";
  QRegExp regexp_version("version:([^\\n]*)");
  if (regexp_version.indexIn(response) >= 0) {
    version = regexp_version.cap(1).trimmed();
  }

  QString msg = "";
  QRegExp regexp_msg("msg:([^\\n]*)");
  if (regexp_msg.indexIn(response) >= 0) {
    msg = regexp_msg.cap(1).trimmed();
  }

  if (version != "" && VERSION < version)
  {
    QString str =
      "<p>There is a new version available (" + version + ")!</p>"
      "<p>Download it from <a href='http://www.ccruncher.net'>www.ccruncher.net</a>.</p>";
    if (msg != "") str += msg;
    QMessageBox::information(this, "CCruncher", str);
  }
}

//===========================================================================
// post a message in the status bar
//===========================================================================
void ccruncher_gui::MainWindow::setStatusMsg(const QString &msg)
{
  statusBar()->showMessage(msg);
}

