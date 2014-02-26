
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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
    QToolBar *mainToolBar;
    //! Child tool bar
    QToolBar *childToolBar;
    //! Default properties
    QMap<QString,QVariant> properties;
    //! Network manager
    QNetworkAccessManager network;

  private:

    //! Find mdi child
    QMdiSubWindow *findMdiChild(const QString &filename);

  protected:

    //! Close event
    void closeEvent(QCloseEvent *event);
     //! This event is called when the mouse enters the widgets area during a drag/drop operation
    void dragEnterEvent(QDragEnterEvent *event);
    //! This event is called when the mouse moves inside the widgets area during a drag/drop operation
    void dragMoveEvent(QDragMoveEvent *event);
    //! This event is called when the mouse leaves the widgets area during a drag/drop operation
    void dragLeaveEvent(QDragLeaveEvent *event);
    //! This event is called when the drop operation is initiated at the widget
    void dropEvent(QDropEvent *event);

  public:

    //! Constructor
    explicit MainWindow(const QMap<QString, QVariant> &properties = QMap<QString,QVariant>(), QWidget *parent = 0);
    //! Destructor
    ~MainWindow();
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
    void updateToolBars(QMdiSubWindow *window);
    //! Process http request
    void processHttpRequest(QNetworkReply *);
    //! Post a message in the status bar
    void setStatusMsg(const QString &msg="");

};

} // namespace

#endif

