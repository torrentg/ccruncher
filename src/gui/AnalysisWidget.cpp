#include <cmath>
#include <vector>
#include <algorithm>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_cdf.h>
#include <QMessageBox>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_symbol.h>
#include "ui_AnalysisWidget.h"
#include "gui/AnalysisWidget.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
AnalysisWidget::AnalysisWidget(const QString &filename, QWidget *parent) :
  QWidget(parent), ui(new Ui::AnalysisWidget), magnifier(NULL), panner(NULL)
{
  ui->setupUi(this);

  ui->filename->setText(filename);
  ui->plot->canvas()->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);

  magnifier = new QwtPlotMagnifier(ui->plot->canvas());
  magnifier->setMouseFactor(1.0);
  panner = new QwtPlotPanner(ui->plot->canvas());

  // magnifie x-axis
  QAction *actionZoomX = new QAction("Zoom axis X", this);
  actionZoomX->setCheckable(true);
  actionZoomX->setChecked(true);
  connect(actionZoomX, SIGNAL(triggered(bool)), this, SLOT(setZoomX(bool)));
  ui->plot->addAction(actionZoomX);

  // magnifie y-axis
  QAction *actionZoomY = new QAction("Zoom axis Y", this);
  actionZoomY->setCheckable(true);
  actionZoomY->setChecked(true);
  connect(actionZoomY, SIGNAL(triggered(bool)), this, SLOT(setZoomY(bool)));
  ui->plot->addAction(actionZoomY);

  ui->plot->setContextMenuPolicy(Qt::ActionsContextMenu);

  // refresh action
  QKeySequence keys_refresh(QKeySequence::Refresh);
  QAction* actionRefresh = new QAction(this);
  actionRefresh->setShortcut(keys_refresh);
  QObject::connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
  this->addAction(actionRefresh);

  // open file & display
  csv.open(filename.toStdString());
  vector<string> segments = csv.getHeaders();
  for(size_t i=0; i<segments.size(); i++) {
    ui->segments->addItem(segments[i].c_str());
  }
}

//===========================================================================
// destructor
//===========================================================================
AnalysisWidget::~AnalysisWidget()
{
  delete ui;
}

//===========================================================================
// set zoom on axis
//===========================================================================
void AnalysisWidget::setZoomX(bool checked)
{
  assert(magnifier != NULL);
  magnifier->setAxisEnabled(QwtPlot::xBottom, checked);
}
void AnalysisWidget::setZoomY(bool checked)
{
  assert(magnifier != NULL);
  magnifier->setAxisEnabled(QwtPlot::yLeft, checked);
}

//===========================================================================
// refresh current content
//===========================================================================
void AnalysisWidget::refresh(int)
{
  int isegment = ui->segments->currentIndex();
  int iview = ui->view->currentIndex();
  if (isegment < 0 || iview < 0) return;

  ui->percentile->setVisible(iview==0?false:true);
  ui->percentile_str->setVisible(iview==0?false:true);
  ui->confidence->setVisible(iview==0?false:true);
  ui->confidence_str->setVisible(iview==0?false:true);
  ui->numbins->setVisible(iview==0?true:false);
  ui->numbins_str->setVisible(iview==0?true:false);

  ui->statistic->setVisible(iview==0?false:true);
  ui->statistic_str->setVisible(iview==0?false:true);
  ui->interval->setVisible(iview==0?false:true);
  ui->interval_str->setVisible(iview==0?false:true);
  ui->std_err->setVisible(iview==0?false:true);
  ui->std_err_str->setVisible(iview==0?false:true);

  ui->plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->plot->setAxisAutoScale(QwtPlot::yLeft);

  vector<double> values;
  csv.getValues(isegment, values);
  ui->numiterations->setText(QString::number(values.size()));
  if (values.empty()) {
    ui->statistic->setText("");
    ui->interval->setText("");
    ui->std_err->setText("");
    return;
  }

  switch(iview)
  {
    case 0:
      drawHistogram(values);
      break;
    case 1:
      computeEL(values);
      drawStatistic();
      break;
    case 2:
      computeVaR(values, ui->percentile->value()/100.0);
      drawStatistic();
      break;
    case 3:
      computeES(values, ui->percentile->value()/100.0);
      drawStatistic();
      break;
    default:
      assert(false);
  }
}

//===========================================================================
// computeEL
//===========================================================================
void AnalysisWidget::computeEL(const vector<double> &values)
{
  size_t numpoints = std::min(values.size(), (size_t)2048);
  double step = values.size()/(double)numpoints;
  statvals.resize(numpoints);
  double s1=0, s2=0;

  for (size_t i=0,n=0; i<numpoints; i++)
  {
    double x = (i+1)*step;

    while(n < (size_t)(x+0.5)) {
      s1 += values[n];
      s2 += values[n]*values[n];
      n++;
    }

    double mean = s1/n;
    double stdev = sqrt((n*s2-s1*s1)/(n*(n-1.0)));

    statvals[i].iteration = n;
    statvals[i].value = mean;
    statvals[i].std_err = stdev/sqrt(n);
  }
}

//===========================================================================
// computeVaR
//===========================================================================
void AnalysisWidget::computeVaR(vector<double> &values, double percentile)
{
  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.resize(numpoints);

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    sort(values.begin(), values.begin()+n);
    int m = (int)(n*percentile+0.5);
    double c1=0, c2=0;

    if (m < n)
    {
      // quantile stderr (Maritz-Jarret)
      double a = m - 1.0;
      double b = n - m;

      for(int j=m; j<n-1; j++) {
        double w =  gsl_sf_beta_inc(a, b, (double)(j+2)/(double)(n)) - gsl_sf_beta_inc(a, b, (double)(j+1)/(double)(n));
        c1 += w * values[j];
        c2 += w * values[j]*values[j];
        if (w < 1e-14) break;
        //TODO: considere threshold on c1, c2 variations (not w)
      }

      for(int j=m-1; j>=0; j--) {
        double w =  gsl_sf_beta_inc(a, b, (double)(j+2)/(double)(n)) - gsl_sf_beta_inc(a, b, (double)(j+1)/(double)(n));
        c1 += w * values[j];
        c2 += w * values[j]*values[j];
        if (w < 1e-14) break;
        //TODO: considere threshold on c1, c2 variations (not w)
      }
    }

    statvals[i].iteration = n;
    statvals[i].value = values[m-1];
    statvals[i].std_err = sqrt(c2 - c1*c1);
  }
}

//===========================================================================
// computeES
//===========================================================================
void AnalysisWidget::computeES(vector<double> &values, double percentile)
{
  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.resize(numpoints);

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    sort(values.begin(), values.begin()+n);
    int m = (int)(n*percentile+0.5);

    int k=0;
    double s1=0, s2=0;
    for(int j=m; j<n; j++) {
      s1 += values[j];
      s2 += values[j]*values[j];
      k++;
    }

    double mean = s1/k;
    double stdev = sqrt((k*s2-s1*s1)/(k*(k-1.0)));

    statvals[i].iteration = n;
    statvals[i].value = mean;
    statvals[i].std_err = stdev/sqrt(k);
  }
}

//===========================================================================
// draw histogram
//===========================================================================
void AnalysisWidget::drawHistogram(const vector<double> &values, size_t numbins)
{
  ui->plot->detachItems();
cout << "drawing histogram ..." << endl;
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  ui->plot->setAxisTitle(QwtPlot::yLeft, "Frequency");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Portfolio Loss");

  double minval = values[0];
  double maxval = values[0];
  for (size_t i=1; i<values.size(); i++)
  {
    if (values[i] > maxval) maxval = values[i];
    else if (values[i] < minval) minval = values[i];
  }

  if (numbins == 0) {
    numbins = std::max((int)sqrt(values.size()), 10);
  }
  ui->numbins->setValue(numbins);
  gsl_histogram *hist = gsl_histogram_alloc(numbins);
  gsl_histogram_set_ranges_uniform(hist, minval, maxval+1e-10);
  for(size_t i=0; i<values.size(); i++) gsl_histogram_increment(hist, values[i]);

  QVector<QwtIntervalSample> samples(numbins);
  for(size_t i=0; i<numbins; i++)
  {
    double lower, upper;
    gsl_histogram_get_range(hist, i, &lower, &upper);
    QwtInterval interval(lower, upper);
    interval.setBorderFlags(QwtInterval::ExcludeMaximum);
    samples[i] = QwtIntervalSample(gsl_histogram_get(hist, i), interval);
  }
  gsl_histogram_free(hist);

  QwtPlotHistogram *qhist = new QwtPlotHistogram("title");
  qhist->setStyle(QwtPlotHistogram::Columns); //Outline,Lines,Columns
  QColor color = Qt::red;
  color.setAlpha(180);
  qhist->setPen(QPen(Qt::black));
  qhist->setBrush(QBrush(color));

  QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
  symbol->setFrameStyle(QwtColumnSymbol::Raised);
  symbol->setLineWidth(2);
  symbol->setPalette(QPalette(color));
  qhist->setSymbol(symbol);

  qhist->setData(new QwtIntervalSeriesData(samples));
  qhist->attach(ui->plot);

  if (panner != NULL) panner->setOrientations(Qt::Horizontal);
  ui->plot->replot();
}

//===========================================================================
// draw statistic
//===========================================================================
void AnalysisWidget::drawStatistic()
{
  ui->plot->detachItems();

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  QString name = ui->view->currentText();
  ui->plot->setAxisTitle(QwtPlot::yLeft, name);
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Iteration");

  int numpoints = statvals.size();
  QVector<QPointF> values(numpoints);
  QVector<QwtIntervalSample> ranges(numpoints);

  double confidence = ui->confidence->value()/100.0;
  double quantile = fabs(gsl_cdf_ugaussian_Pinv((1.0-confidence)/2.0));

  for(int i=0; i<numpoints; i++)
  {
    double val = statvals[i].value;
    values[i] = QPointF(statvals[i].iteration, val);
    double delta = statvals[i].std_err * quantile;
    ranges[i] = QwtIntervalSample(statvals[i].iteration, QwtInterval(val-delta, val+delta));
  }

  // adjusting vertical scale (avoids initial 5%)
  double ymin=ranges.back().interval.minValue();
  double ymax=ranges.back().interval.maxValue();
  for(int i=(int)(numpoints*0.05); i<numpoints; i++) {
    if (ranges[i].interval.minValue() < ymin) ymin = ranges[i].interval.minValue();
    if (ranges[i].interval.maxValue() > ymax) ymax = ranges[i].interval.maxValue();
  }
  ui->plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);

  QwtPlotCurve *d_curve = new QwtPlotCurve("title");
  d_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_curve->setStyle(QwtPlotCurve::Lines); //NoCurve
  d_curve->setSamples(values);
  d_curve->attach(ui->plot);

  QwtPlotIntervalCurve *d_intervalCurve = new QwtPlotIntervalCurve("title");
  d_intervalCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_intervalCurve->setPen(QPen(Qt::white));
  QColor bg(Qt::blue);
  bg.setAlpha(150);
  d_intervalCurve->setBrush(QBrush(bg));
  d_intervalCurve->setStyle(QwtPlotIntervalCurve::Tube);
  d_intervalCurve->setSamples(ranges);
  d_intervalCurve->attach(ui->plot);

  if (panner != NULL) panner->setOrientations(Qt::Horizontal | Qt::Vertical);
  ui->plot->replot();

  ui->statistic_str->setText(name + ":");
  ui->statistic->setText(QString::number(statvals.back().value));
  ui->interval->setText(QString(QChar(177)) + " " + QString::number(quantile*statvals.back().std_err));
  ui->std_err->setText(QString::number(statvals.back().std_err));
}

//===========================================================================
// numbins changed
//===========================================================================
void AnalysisWidget::changeNumbins()
{
  int numbins = ui->numbins->value();
  int isegment = ui->segments->currentIndex();
  vector<double> values;
  csv.getValues(isegment, values);
  ui->plot->detachItems();
  drawHistogram(values, numbins);
}

//===========================================================================
// percentile changed
//===========================================================================
void AnalysisWidget::changePercentile()
{
  refresh();
}

