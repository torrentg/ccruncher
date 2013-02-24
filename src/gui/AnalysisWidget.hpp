
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

// forward definitions
class QwtPlotMagnifier;
class QwtPlotPanner;

namespace Ui {
class AnalysisWidget;
}

class AnalysisWidget: public MdiChildWidget
{
    Q_OBJECT

  private:

    // interface widget
    Ui::AnalysisWidget *ui;
    // progress widget
    ProgressWidget *progress;
    // internal timer
    QTimer timer;
    // task thread
    AnalysisTask task;
    // plot magnifier
    QwtPlotMagnifier *magnifier;
    // plot panner
    QwtPlotPanner *panner;
    // number of simulations
    size_t nsamples;
    // mutex
    QMutex mutex;
    // numbins
    size_t numbins;
    // percentile
    double percentile;
    // confidence
    double confidence;
    // toolbar
    QToolBar *toolbar;
    // actions
    QAction *actionZoomX;
    QAction *actionZoomY;
    QAction *actionRefresh;
    QAction *actionStop;

  private:

    // reset results
    void reset();
    // draw histogram
    void drawHistogram();
    // draw statistic
    void drawStatistic();
    // submit task
    void submit(size_t numbins=0);

  public:

    // constructor
    AnalysisWidget(const QString &filename, QWidget *parent=0);
    // destructor
    ~AnalysisWidget();
    // virtual method implementation
    QToolBar* getToolBar() { return toolbar; }

  public slots:

    // refresh
    void refresh();
    // draw results
    void draw();
    // enable zoom on axis x
    void setZoomX(bool checked);
    // enable zoom on axis y
    void setZoomY(bool checked);
    // segment changed
    void changeSegment();
    // view changed
    void changeView();
    // numbins changed
    void changeNumbins();
    // percentile changed
    void changePercentile();
    // confidence changed
    void changeConfidence();
    // set status
    void setStatus(int);
    // stop current action
    void stop();

};

#endif
