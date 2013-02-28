
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

#ifndef _MainWindow_
#define _MainWindow_

#include <QMainWindow>
#include <QString>
#include <QMdiArea>
#include <QUrl>
#include <QMap>
#include <QCloseEvent>
#include <QToolBar>

namespace ccruncher_gui {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  private:

    // interface widget
    Ui::MainWindow *ui;
    // mdi area
    QMdiArea *mdiArea;
    // main tool bar
    QToolBar *mainToolBar;
    // child tool bar
    QToolBar *childToolBar;
    // dedfault properties
    QMap<QString,QVariant> properties;

  private:

    // find mdi child
    QMdiSubWindow *findMdiChild(const QString &filename);

  protected:

    // close event
    void closeEvent(QCloseEvent *event);

  public:

    // constructor
    explicit MainWindow(const QMap<QString, QVariant> &properties = QMap<QString,QVariant>(), QWidget *parent = 0);
    // destructor
    ~MainWindow();
    // return properties
    const QMap<QString, QVariant> &getProperties() const;

  public slots:

    // about dialog
    void about();
    // select file
    void selectFile();
    // open file
    void openFile(const QUrl &url);
    // update tool bars
    void updateToolBars(QMdiSubWindow *window);

};

}

#endif
