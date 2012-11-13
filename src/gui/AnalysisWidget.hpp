#ifndef _AnalysisWidget_
#define _AnalysisWidget_

#include <vector>
#include <QWidget>
#include <QString>
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

    // internal struct
    struct statval
    {
      int iteration;
      double value;
      double std_err;
    };

  private:

    // interface widget
    Ui::AnalysisWidget *ui;
    // csv file
    CsvFile csv;
    // plot magnifier
    QwtPlotMagnifier *magnifier;
    // plot panner
    QwtPlotPanner *panner;
    // statistic values
    vector<statval> statvals;

  private:

    // draw histogram
    void drawHistogram(const vector<double> &, size_t numbins=0);
    // compute EL
    void computeEL(const vector<double> &);
    // compute VaR
    void computeVaR(vector<double> &, double);
    // compute ES
    void computeES(vector<double> &, double);

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
    // draw statistic
    void drawStatistic();
    // numbins changed
    void changeNumbins();
    // percentile changed
    void changePercentile();

};

#endif
