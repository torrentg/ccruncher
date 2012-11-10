#ifndef _AnalysisWidget_
#define _AnalysisWidget_

#include <QWidget>
#include <QString>
#include <qwt_text.h>
#include <qwt_scale_draw.h>
#include "utils/CsvFile.hpp"

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
    // csv file
    CsvFile csv;
    // plot magnifier
    QwtPlotMagnifier *magnifier;
    // plot panner
    QwtPlotPanner *panner;

  private:

    // draw histogram
    void drawHistogram(const vector<double> &);
    // draw expected loss
    void drawExpectedLoss(const vector<double> &);

  public:

    // constructor
    AnalysisWidget(const QString &filename, QWidget *parent=0);
    // destructor
    ~AnalysisWidget();

  public slots:

    // refresh current content
    void refresh(int i=-1);
    // enable zoom on axis x
    void setZoomX(bool checked);
    // enable zoom on axis y
    void setZoomY(bool checked);

};

class IntegerScaleDraw : public QwtScaleDraw
{
  QwtText label(double val) const {
    return QString::number((int)(val+0.5));
  }
};

#endif
