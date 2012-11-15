#ifndef _AnalysisWidget_
#define _AnalysisWidget_

#include <vector>
#include <QTimer>
#include <QWidget>
#include <QString>
#include "gui/AnalysisTask.hpp"

using namespace std;

// forward definitions
class QwtPlotMagnifier;
class QwtPlotPanner;

namespace Ui {
class AnalysisWidget;
}

class AnalysisWidget: public QWidget
{
    Q_OBJECT

  private:

    // interface widget
    Ui::AnalysisWidget *ui;
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

  public slots:

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

};

#endif
