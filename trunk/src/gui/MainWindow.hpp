
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _MainWindow_
#define _MainWindow_

#include <QMainWindow>
#include <QString>
#include <QMdiArea>
#include <QUrl>
#include <QMap>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QToolBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace ccruncher_gui {

namespace Ui {
class MainWindow;
}

/**************************************************************************//**
 * @brief Main window in the gui application.
 *
 * @details CCruncher options are indicated with a properties map.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

  private:

    //! Interface widget
    Ui::MainWindow *ui;
    //! Mdi area
    QMdiArea *mdiArea;
    //! Main tool bar
    QToolBar *toolBar;
    //! Default properties
    QMap<QString,QVariant> properties;
    //! Network manager
    QNetworkAccessManager network;

  private:

    //! Find mdi child
    QMdiSubWindow *findMdiChild(const QString &filename);
    //! Update toolbar and 'Actions' menu
    void updateContextualActions(const QList<QAction *> &actions);

  protected:

    //! Close event
    virtual void closeEvent(QCloseEvent *event) override;
     //! This event is called when the mouse enters the widgets area during a drag/drop operation
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    //! This event is called when the mouse moves inside the widgets area during a drag/drop operation
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    //! This event is called when the mouse leaves the widgets area during a drag/drop operation
    virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
    //! This event is called when the drop operation is initiated at the widget
    virtual void dropEvent(QDropEvent *event) override;

  public:

    //! Constructor
    MainWindow(const QMap<QString, QVariant> &properties = QMap<QString,QVariant>(), QWidget *parent = 0);
    //! Destructor
    virtual ~MainWindow() override;
    //! Return properties
    const QMap<QString, QVariant> &getProperties() const;

  public slots:

    //! Shows about dialog
    void about();
    //! Select a file
    void selectFile();
    //! Open a file
    void openFile(const QUrl &url);
    //! Update tool bars
    void updateSubWindow(QMdiSubWindow *window);
    //! Process http request
    void processHttpRequest(QNetworkReply *reply);
    //! Post a message in the status bar
    void setStatusMsg(const QString &msg="");
    //! Enable/disable tool bar
    void viewToolBar(bool visible);
    //! Enable/disable status bar
    void viewStatusBar(bool visible);
    //! Enable/disable tabbed windows
    void viewTabbedWindows(bool enable);

};

} // namespace

#endif

