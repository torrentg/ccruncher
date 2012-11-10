#include <cmath>
#include <vector>
#include <gsl/gsl_histogram.h>
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
  csv.open(filename.toStdString());
  vector<string> segments = csv.getHeaders();
  for(size_t i=0; i<segments.size(); i++) {
    ui->segments->addItem(segments[i].c_str());
  }

  int left, top, right, bottom;
  ui->plot->getContentsMargins(&left, &top, &right, &bottom);
  ui->plot->setContentsMargins(left, top+20, right, bottom);

  magnifier = new QwtPlotMagnifier(ui->plot->canvas());
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

  QKeySequence keys_refresh(QKeySequence::Refresh);
  QAction* actionRefresh = new QAction(this);
  actionRefresh->setShortcut(keys_refresh);
  QObject::connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
  this->addAction(actionRefresh);
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
  magnifier->setMouseFactor(1.0);
}
void AnalysisWidget::setZoomY(bool checked)
{
  assert(magnifier != NULL);
  magnifier->setAxisEnabled(QwtPlot::yLeft, checked);
  magnifier->setMouseFactor(1.0);
}

//===========================================================================
// refresh current content
//===========================================================================
void AnalysisWidget::refresh(int)
{
  int isegment = ui->segments->currentIndex();
  int iview = ui->view->currentIndex();
  if (isegment < 0 || iview < 0) return;

  ui->plot->detachItems();
  ui->plot->canvas()->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);

  ui->plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->plot->setAxisAutoScale(QwtPlot::yLeft);

  vector<double> values;
  csv.getValues(isegment, values);
  if (values.empty()) return;

  switch(iview)
  {
    case 0: //histogram
      drawHistogram(values);
      break;
    case 1: // expected loss
      drawExpectedLoss(values);
      break;
    case 2: // value at risk
    case 3: // expected shortfall
      QMessageBox::warning(this, "funtion not implemented", "try later");
      break;
    default:
      assert(false);
  }
}

//===========================================================================
// draw histogram
//===========================================================================
void AnalysisWidget::drawHistogram(const vector<double> &values)
{
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  ui->plot->setAxisTitle(QwtPlot::yLeft, "Frequency");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Portfolio Loss");
  //ui->plot->setAxisScaleDraw(QwtPlot::yLeft, new IntegerScaleDraw()); //TODO: remove?

  double minval = values[0];
  double maxval = values[0];
  for (size_t i=1; i<values.size(); i++)
  {
    if (values[i] > maxval) maxval = values[i];
    else if (values[i] < minval) minval = values[i];
  }

  size_t numbins = std::max((int)sqrt(values.size()), 10);
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
  ui->plot->replot();
}

//===========================================================================
// draw expected loss
//===========================================================================
void AnalysisWidget::drawExpectedLoss(const vector<double> &values)
{
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  ui->plot->setAxisTitle(QwtPlot::yLeft, "Expected Loss");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Iteration");

  double minval = values[0];
  double maxval = values[0];
  for (size_t i=1; i<values.size(); i++)
  {
    if (values[i] > maxval) maxval = values[i];
    else if (values[i] < minval) minval = values[i];
  }

  size_t numpoints = std::min(values.size(), (size_t)2048);
  double step = values.size()/(double)numpoints;
  QVector<QPointF> avgs(numpoints);
  QVector<QwtIntervalSample> ranges(numpoints);
  double sum1=0.0, sum2=0.0;

  for (size_t i=0,n=0; i<numpoints; i++)
  {
    double x = i*step;
    while(n <= (size_t)(x+0.5)) {
      sum1 += values[n];
      sum2 += values[n]*values[n];
      n++;
    }
    double y = sum1/n;
    double stdev = sqrt((n*sum2-sum1*sum1)/(n*(n-1.0)));
    double delta = gsl_cdf_ugaussian_Pinv(0.025)*stdev/sqrt(n);
    avgs[i] = QPointF(x+1.0, y);
    ranges[i] = QwtIntervalSample(x+1.0, QwtInterval(y-delta, y+delta));
  }

  QwtPlotCurve *d_curve = new QwtPlotCurve("title");
  d_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_curve->setStyle(QwtPlotCurve::Lines); //NoCurve
/*
  d_curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);
  QwtSymbol *symbol = new QwtSymbol(QwtSymbol::XCross);
  symbol->setSize(4);
  symbol->setPen(QPen(Qt::black));
  d_curve->setSymbol(symbol);
*/
  d_curve->setSamples(avgs);
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

  ui->plot->replot();
}

