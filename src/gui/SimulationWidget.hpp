
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

namespace ccruncher_gui {

namespace Ui {
class SimulationWidget;
}

/**************************************************************************//**
 * @brief Monte Carlo simulation widget.
 */
class SimulationWidget : public MdiChildWidget
{
    Q_OBJECT

  private:

    //! Interface widget
    Ui::SimulationWidget *ui;
    //! Internal timer
    QTimer timer;
    //! Task thread
    SimulationTask task;
    //! Defines
    std::map<std::string,std::string> defines;
    //! Stdout redirect
    QStreamBuf qstream;
    //! Progress widget
    ProgressWidget *progress;
    //! Mutex
    QMutex mutex;
    //! Log line
    QString logline;
    //! Log cursor
    QTextCursor logcursor;
    //! Toolbar
    QToolBar *toolbar;
    //! Action edit file
    QAction *actionEdit;
    //! Action manage defines
    QAction *actionDefines;
    //! Action run simulation
    QAction *actionRun;
    //! Action stop analysis/simulation
    QAction *actionStop;
    //! Action analyze data
    QAction *actionAnal;
    //! Output directory (used to identify changes)
    QString odirAux;
    //! Log file output
    std::ofstream fout;
    //! User defined properties
    QMap<QString,QVariant> properties;

  private:

    //! Set input file
    void setFile();
    //! Fill widget defines
    void setDefines();
    //! Update widgets status
    void updateControls();
    //! Clear log area
    void clearLog();
    //! Linkify a token
    void linkify(QString &);

  protected:

    //! Override close
    virtual void closeEvent(QCloseEvent *event) override;

  public:

    //! Constructor
    SimulationWidget(const QString &filename, QWidget *parent=0);
    //! Destructor
    virtual ~SimulationWidget() override;
    //! Virtual method implementation
    virtual QToolBar* getToolBar() override { return toolbar; }

  public slots:

    //! Edit current file
    void editFile();
    //! Selects output directory
    void selectDir();
    //! Set output directory
    void setDir();
    //! Submit task
    void submit();
    //! Print message to log
    void log(const QString &);
    //! Draw widget
    void draw();
    //! Defines dialog
    void showDefines();
    //! Set status
    void setStatus(int);
    //! Open csv files
    void openData();

  signals:

    //! Anchor clicked
    void anchorClicked(const QUrl &);

};

} // namespace

#endif

