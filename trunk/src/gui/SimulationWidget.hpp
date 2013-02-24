
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

#ifndef _SimulationWidget_
#define _SimulationWidget_

#include <map>
#include <fstream>
#include <QUrl>
#include <QWidget>
#include <QString>
#include <QTimer>
#include <QMutex>
#include <QMap>
#include <QVariant>
#include <QTextCursor>
#include <QToolBar>
#include <QAction>
#include "gui/MdiChildWidget.hpp"
#include "gui/QStreamBuf.hpp"
#include "gui/SimulationTask.hpp"
#include "gui/ProgressWidget.hpp"

namespace Ui {
class SimulationWidget;
}

class SimulationWidget : public MdiChildWidget
{
    Q_OBJECT

  private:

    // interface widget
    Ui::SimulationWidget *ui;
    // internal timer
    QTimer timer;
    // task thread
    SimulationTask task;
    // defines
    std::map<std::string,std::string> defines;
    // stdout redirect
    QStreamBuf qstream;
    // progress widget
    ProgressWidget *progress;
    // mutex
    QMutex mutex;
    // log line
    QString logline;
    // log cursor
    QTextCursor logcursor;
    // toolbar
    QToolBar *toolbar;
    // actions
    QAction *actionEdit;
    QAction *actionDefines;
    QAction *actionRun;
    QAction *actionStop;
    QAction *actionAnal;
    // output directory
    QString odir;
    // log file output
    std::ofstream fout;
    // default properties
    QMap<QString,QVariant> properties;

  private:

    // set input file
    void setFile();
    // fill widget defines
    void setDefines();
    // update widgets status
    void updateControls();
    // clear log area
    void clearLog();
    // linkify a token
    void linkify(QString &);

  protected:

    // override close
    void closeEvent(QCloseEvent *event);

  public:

    // constructor
    explicit SimulationWidget(const QString &filename, QWidget *parent=0);
    // destructor
    ~SimulationWidget();
    // virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    // edit current file
    void editFile();
    // selects output directory
    void selectDir();
    // set output directory
    void setDir();
    // submit task
    void submit();
    // print message to log
    void log(const QString &);
    // draw widget
    void draw();
    // defines dialog
    void showDefines();
    // set status
    void setStatus(int);
    // open csv files
    void openData();

  signals:

    // anchor clicked
    void anchorClicked(const QUrl &);

};

#endif
