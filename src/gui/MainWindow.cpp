
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <QMimeData>
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

/**************************************************************************//**
 * @param[in] map List of CCruncher options defined by user (eg. num threads).
 * @param[in] parent Widget parent.
 */
ccruncher_gui::MainWindow::MainWindow(const QMap<QString, QVariant> &map, QWidget *parent) : QMainWindow(parent),
  ui(new Ui::MainWindow), toolBar(nullptr), properties(map),
  network(this)
{
  ui->setupUi(this);
  setAcceptDrops(true);
  setWindowTitle(tr("CCruncher"));

  // setting mdi area
  mdiArea = new QMdiArea;
  setCentralWidget(mdiArea);

  // creating toolbar
  toolBar = addToolBar(tr("toolbar"));
  updateContextualActions(QList<QAction*>());

  // setting view mode
  ui->actionToolbar->setChecked(true);
  viewToolBar(true);
  ui->actionStatusbar->setChecked(true);
  viewStatusBar(true);
  ui->actionTabbedWindows->setChecked(true);
  viewTabbedWindows(true);

  connect(&network, SIGNAL(finished(QNetworkReply*)), this, SLOT(processHttpRequest(QNetworkReply*)));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(selectFile()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionToolbar, SIGNAL(triggered(bool)), this, SLOT(viewToolBar(bool)));
  connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(viewToolBar(bool)));
  connect(ui->actionStatusbar, SIGNAL(triggered(bool)), this, SLOT(viewStatusBar(bool)));
  connect(ui->actionTabbedWindows, SIGNAL(triggered(bool)), this, SLOT(viewTabbedWindows(bool)));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateSubWindow(QMdiSubWindow*)));
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(setStatusMsg()));

  // checking last version
  QUrl url("http://www.ccruncher.net/version?m=gui&v=" VERSION);
  network.get(QNetworkRequest(url));

  statusBar()->showMessage(tr("Ready"));
}

/**************************************************************************/
ccruncher_gui::MainWindow::~MainWindow()
{
  delete ui;
  delete mdiArea;
}

/**************************************************************************//**
 * @param[in] event Event info.
 */
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

/**************************************************************************//**
 * @see http://qt-project.org/wiki/Drag_and_Drop_of_files
 * @param[in] event Event info.
 */
void ccruncher_gui::MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

/**************************************************************************//**
 * @see http://qt-project.org/wiki/Drag_and_Drop_of_files
 * @param[in] event Event info.
 */
void ccruncher_gui::MainWindow::dragMoveEvent(QDragMoveEvent* event) {
  // if some actions should not be usable, like move, this code must be adopted
  event->acceptProposedAction();
}

/**************************************************************************//**
 * @see http://qt-project.org/wiki/Drag_and_Drop_of_files
 * @param[in] event Event info.
 */
void ccruncher_gui::MainWindow::dragLeaveEvent(QDragLeaveEvent* event) {
  event->accept();
}

/**************************************************************************//**
 * @see http://qt-project.org/wiki/Drag_and_Drop_of_files
 * @param[in] event Event info.
 */
void ccruncher_gui::MainWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* mimeData = event->mimeData();

  // check for our needed mime type, here a file or a list of files
  if (mimeData->hasUrls()) {
    QList<QUrl> urlList = mimeData->urls();
    // extract the local paths of the files
    for(int i=0; i<urlList.size() && i<32; ++i) {
      openFile(urlList.at(i));
    }
  }
}

/**************************************************************************//**
 * @details Main tool bar depend on current mdi sub-windows.
 * @param[in] window Current sub-window.
 */
void ccruncher_gui::MainWindow::updateSubWindow(QMdiSubWindow *window)
{
  if (window == nullptr)
  {
    if (!mdiArea->hasFocus() && mdiArea->subWindowList().size() > 0) {
      // mdiarea has lost focus, nothing to do
    }
    else {
      // last subwindow removed
      updateContextualActions(QList<QAction*>());
    }
  }
  else
  {
    MdiChildWidget* child = dynamic_cast<MdiChildWidget*>(window->widget());
    QToolBar *toolbar = (child?child->getToolBar():nullptr);
    if (toolbar == nullptr) {
      updateContextualActions(QList<QAction*>());
    }
    else {
      toolbar->setVisible(false);
      updateContextualActions(toolbar->actions());
    }
  }
}

/**************************************************************************//**
 * @details Action list are not deleted by toolbar or menu.
 * @param[in] actions Contextual action list to display.
 */
void ccruncher_gui::MainWindow::updateContextualActions(const QList<QAction *> &actions)
{
  toolBar->clear();
  toolBar->addAction(ui->actionOpen);

  ui->menuActions->clear();

  if (actions.size() == 0) {
    ui->menuActions->setEnabled(false);
    return;
  }

  toolBar->addSeparator();
  toolBar->addActions(actions);
  ui->menuActions->addActions(actions);
  ui->menuActions->setEnabled(true);
}

/**************************************************************************/
void ccruncher_gui::MainWindow::about()
{
  QMessageBox msg;

  msg.setWindowTitle("About ...");
  msg.setText("<h3>CCruncher</h3>");
  msg.setInformativeText(
        "<p>"
        "version: " VERSION " [" GIT_VERSION "]<br/><br/>"
        "copyright: Gerard Torrent<br/><br/>"
        "license: GPL<br/><br/>"
        "url: <a href='http://www.ccruncher.net'>www.ccruncher.net</a><br/><br/>"
        "</p>");
  msg.setStandardButtons(QMessageBox::Ok);
  msg.setDefaultButton(QMessageBox::Ok);
  msg.setIconPixmap(QPixmap(":/images/logo.png"));
  msg.exec();
}

/**************************************************************************/
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
    /*
    if (!filename.toLower().endsWith("csv")) {
      url.setPath(url.toLocalFile());
      url.setScheme("exec");
    }
    */
    openFile(url);
  }
}

/**************************************************************************//**
 * @details Open a file following these rules:
 *          - file://dirpath -> opens directory in a file browser.
 *          - file://filepath opened in a mdi-window -> active mdi-window
 *          - file://file.csv -> opens analysis window
 *          - file://file.xml -> opens xml editor
 *          - exec://filepath -> opens simulation window
 * @param[in] url Url to open.
 */
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
      if (analysis != nullptr) analysis->refresh();
      return;
    }

    QWidget *child = nullptr;
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
    catch(XmlEditWidget::NonEditableException &)
    {
      return;
    }
    catch(std::exception &e)
    {
      cerr << "error opening file '" << fileinfo.fileName().toStdString() << "'" << endl;
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

/**************************************************************************//**
 * @details Finds a MDI-child with the given filename.
 * @param[in] filename Filename to found.
 * @return MDI-child, NULL if not found.
 */
QMdiSubWindow* ccruncher_gui::MainWindow::findMdiChild(const QString &filename)
{
  foreach (QMdiSubWindow *child, mdiArea->subWindowList()) {
    if (child->widget()->windowFilePath() == filename) {
      return child;
    }
  }
  return nullptr;
}

/**************************************************************************//**
 * @return List of properties.
 */
const QMap<QString,QVariant>& ccruncher_gui::MainWindow::getProperties() const
{
  return properties;
}

/**************************************************************************//**
 * @details Process http request to determine if a new CCruncher version
 *          is available. In this case shows a dialog.
 * @param[in] reply Http stuff.
 */
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

/**************************************************************************//**
 * @param[in] msg Message to post.
 */
void ccruncher_gui::MainWindow::setStatusMsg(const QString &msg)
{
  statusBar()->showMessage(msg);
}

/**************************************************************************//**
 * @param[in] visible Toolbar visibility flag.
 */
void ccruncher_gui::MainWindow::viewToolBar(bool visible)
{
  toolBar->setVisible(visible);
  ui->actionToolbar->setChecked(visible);
}

/**************************************************************************//**
 * @param[in] visible Statusbar visibility flag.
 */
void ccruncher_gui::MainWindow::viewStatusBar(bool visible)
{
  ui->statusBar->setVisible(visible);
}

/**************************************************************************//**
 * @param[in] enable Tabbed window enabled mode.
 */
void ccruncher_gui::MainWindow::viewTabbedWindows(bool enable)
{
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);

  if (enable) {
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabShape(QTabWidget::Rounded); //QTabWidget::Triangular
    mdiArea->setTabsMovable(true);
  }
  else {
    mdiArea->setViewMode(QMdiArea::SubWindowView);
  }
}

