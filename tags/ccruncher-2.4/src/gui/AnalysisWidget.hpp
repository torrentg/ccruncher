
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

#ifndef _AnalysisWidget_
#define _AnalysisWidget_

#include <vector>
#include <QTimer>
#include <QWidget>
#include <QString>
#include <QMutex>
#include <QToolBar>
#include <QAction>
#include "gui/MdiChildWidget.hpp"
#include "gui/AnalysisTask.hpp"
#include "gui/ProgressWidget.hpp"

// forward declarations
class QwtPlotMagnifier;
class QwtPlotPanner;

namespace ccruncher_gui {

namespace Ui {
class AnalysisWidget;
}

/**************************************************************************//**
 * @brief Data analysis widget.
 */
class AnalysisWidget: public MdiChildWidget
{
    Q_OBJECT

  private:

    //! Interface widget
    Ui::AnalysisWidget *ui;
    //! Progress widget
    ProgressWidget *progress;
    //! Internal timer
    QTimer timer;
    //! Task thread
    AnalysisTask task;
    //! Plot magnifier
    QwtPlotMagnifier *magnifier;
    //! Plot panner
    QwtPlotPanner *panner;
    //! Task progress (0..100)
    float task_progress;
    //! Mutex
    QMutex mutex;
    //! Numbins
    size_t numbins;
    //! Percentile
    double percentile;
    //! Confidence
    double confidence;
    //! Toolbar
    QToolBar *toolbar;
    //! Action zoom x-axis
    QAction *actionZoomX;
    //! Action zoom y-axis
    QAction *actionZoomY;
    //! Action refresh content
    QAction *actionRefresh;
    //! Action stops execution
    QAction *actionStop;
    //! Action copy to clipboard
    QAction *actionCopy;
    //! Plotted data as string
    QByteArray strdata;

  private:

    //! Reset results
    void reset();
    //! Draw histogram
    void drawHistogram();
    //! Draw curve
    void drawCurve();
    //! Draw piechart
    void drawPiechart();
    //! Submit task
    void submit();
    //! Returns current mode
    int getMode() const;
    //! Return current view index
    int getView() const;

  public:

    //! Constructor
    AnalysisWidget(const QString &filename, QWidget *parent=0);
    //! Destructor
    ~AnalysisWidget();
    //! Virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    //! Refresh content
    void refresh();
    //! Draw results
    void draw();
    //! Enable zoom on axis x
    void setZoomX(bool checked);
    //! Enable zoom on axis y
    void setZoomY(bool checked);
    //! Change mode
    void changeMode();
    //! Change segment
    void changeSegment();
    //! Change view
    void changeView();
    //! Change numbins
    void changeNumbins();
    //! Change percentile
    void changePercentile();
    //! Change confidence
    void changeConfidence();
    //! Set status
    void setStatus(int);
    //! Stop current action
    void stop();
    //! Copy data to clipboard
    void copyToClipboard();
    //! Show piechart info
    void showPiechartTooltip(QPoint point);

  signals:

    //! Post msg in app status bar
    void newStatusMsg(const QString &);

};

} // namespace

#endif

