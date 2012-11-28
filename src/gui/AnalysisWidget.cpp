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
#include "utils/Format.hpp"
#include <cassert>

#define REFRESH_MS 100

//===========================================================================
// constructor
//===========================================================================
AnalysisWidget::AnalysisWidget(const QString &filename, QWidget *parent) :
  MdiChildWidget(parent), ui(new Ui::AnalysisWidget), progress(NULL),
    magnifier(NULL), panner(NULL), nsamples(0), toolbar(NULL)
{
  blockSignals(true);

  ui->setupUi(this);

  numbins = ui->numbins->value();
  percentile = ui->percentile->value();
  confidence = ui->confidence->value();

  progress = new ProgressWidget(ui->frame);
  progress->setWindowFlags(Qt::WindowStaysOnTopHint);
  ui->frame->addLayer(progress);

  ui->filename->setText(filename);
  ui->plot->canvas()->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);
  ui->plot->setContextMenuPolicy(Qt::ActionsContextMenu);

  magnifier = new QwtPlotMagnifier(ui->plot->canvas());
  magnifier->setMouseFactor(1.0);
  panner = new QwtPlotPanner(ui->plot->canvas());

  // magnifie x-axis
  actionZoomX = new QAction(tr("Zoom axis X"), this);
  actionZoomX->setCheckable(true);
  actionZoomX->setChecked(true);
  connect(actionZoomX, SIGNAL(triggered(bool)), this, SLOT(setZoomX(bool)));
  ui->plot->addAction(actionZoomX);

  // magnifie y-axis
  actionZoomY = new QAction(tr("Zoom axis Y"), this);
  actionZoomY->setCheckable(true);
  actionZoomY->setChecked(true);
  connect(actionZoomY, SIGNAL(triggered(bool)), this, SLOT(setZoomY(bool)));
  ui->plot->addAction(actionZoomY);

  // refresh action
  QKeySequence keys_refresh(QKeySequence::Refresh);
  actionRefresh = new QAction(QIcon(":/images/refresh.png"), tr("&Refresh"), this);
  actionRefresh->setStatusTip(tr("Refresh current analysis"));
  actionRefresh->setShortcut(keys_refresh);
  QObject::connect(actionRefresh, SIGNAL(triggered()), this, SLOT(refresh()));
  this->addAction(actionRefresh);

  // stop action
  QKeySequence keys_stop(Qt::Key_Escape);
  actionStop = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
  actionStop->setStatusTip(tr("Stops current analysis"));
  actionStop->setShortcut(keys_stop);
  QObject::connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
  this->addAction(actionStop);

  // creating toolbar
  toolbar = new QToolBar(tr("Analysis"), this);
  toolbar->addAction(actionRefresh);
  toolbar->addAction(actionStop);

  // signals & slots
  connect(&timer, SIGNAL(timeout()), this, SLOT(draw()));
  connect(&task, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)), Qt::QueuedConnection);

  // open file & display
  task.setFilename(filename);
  vector<string> segments = task.getCsvFile().getHeaders();
  for(size_t i=0; i<segments.size(); i++) {
    ui->segments->addItem(segments[i].c_str());
  }

  blockSignals(false);
}

//===========================================================================
// destructor
//===========================================================================
AnalysisWidget::~AnalysisWidget()
{
  blockSignals(true);
  task.stop();
  task.wait(250);
  if (toolbar != NULL) delete toolbar;
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
// reset results
//===========================================================================
void AnalysisWidget::reset()
{
  int iview = ui->view->currentIndex();
  if (iview < 0) return;

  ui->percentile->setVisible(iview<=1?false:true);
  ui->percentile_str->setVisible(iview<=1?false:true);
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

  ui->statistic_str->setText(ui->view->currentText() + ":");

  ui->plot->detachItems();
  ui->plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->plot->setAxisAutoScale(QwtPlot::yLeft);
  ui->plot->setAxisTitle(QwtPlot::yLeft, "");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "");

  ui->numiterations->setText("");
  ui->statistic->setText("");
  ui->interval->setText("");
  ui->std_err->setText("");
}

//===========================================================================
// submit task
//===========================================================================
void AnalysisWidget::submit(size_t numbins)
{
  mutex.lock();
  if (task.isRunning()) task.stop();

  int isegment = ui->segments->currentIndex();
  int iview = ui->view->currentIndex();
  if (isegment < 0 || iview < 0) return;

  switch(iview)
  {
    case 0:
      task.setData(AnalysisTask::histogram, isegment, numbins);
      break;
    case 1:
      task.setData(AnalysisTask::expected_loss, isegment);
      break;
    case 2:
      task.setData(AnalysisTask::value_at_risk, isegment, ui->percentile->value()/100.0);
      break;
    case 3:
      task.setData(AnalysisTask::expected_shortfall, isegment, ui->percentile->value()/100.0);
      break;
    default:
      assert(false);
  }

  task.wait();
  reset();
  nsamples = 0;
  task.start();
  mutex.unlock();
}

//===========================================================================
// draw
//===========================================================================
void AnalysisWidget::draw()
{
  mutex.lock();

  if (task.getStatus() == AnalysisTask::reading)
  {
    size_t totalbytes = task.getCsvFile().getFileSize();
    size_t readedbytes = task.getCsvFile().getReadedSize();
    float pct = 100.0 * (float)(readedbytes)/(float)(totalbytes);
    QString str = Format::bytes(readedbytes).c_str();
    progress->ui->progress->setFormat(str);
    progress->ui->progress->setValue(pct+0.5);
  }
  else
  {
    if (nsamples == task.getNumSamples())
    {
      // nothing to do
    }
    else
    {
      ui->plot->setEnabled(true);
      nsamples = task.getNumSamples();
      switch(task.getMode())
      {
        case AnalysisTask::histogram:
          drawHistogram();
          break;
        default:
          drawStatistic();
          break;
      }
    }
  }

  mutex.unlock();
}

//===========================================================================
// draw histogram
//===========================================================================
void AnalysisWidget::drawHistogram()
{
  ui->plot->detachItems();

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  ui->plot->setAxisTitle(QwtPlot::yLeft, "Frequency");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Portfolio Loss");

  const gsl_histogram *hist = task.getHistogram();
  if (hist == NULL) return;
  size_t numbins = gsl_histogram_bins(hist);
  this->numbins = numbins;
  ui->numbins->setValue(numbins);

  QVector<QwtIntervalSample> samples(numbins);
  size_t numiterations = 0;
  for(size_t i=0; i<numbins; i++)
  {
    double lower, upper;
    gsl_histogram_get_range(hist, i, &lower, &upper);
    QwtInterval interval(lower, upper);
    interval.setBorderFlags(QwtInterval::ExcludeMaximum);
    double val = gsl_histogram_get(hist, i);
    samples[i] = QwtIntervalSample(val, interval);
    numiterations += (int)(val+0.5);
  }

  QwtPlotHistogram *qhist = new QwtPlotHistogram("title");
  qhist->setStyle(QwtPlotHistogram::Columns); //Outline,Lines,Columns
  QColor color = Qt::red;
  color.setAlpha(180);
  qhist->setPen(QPen(Qt::black));
  qhist->setBrush(QBrush(color));

  QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
  symbol->setFrameStyle(QwtColumnSymbol::Plain); //Raised
  symbol->setLineWidth(2);
  symbol->setPalette(QPalette(color));
  qhist->setSymbol(symbol);

  qhist->setData(new QwtIntervalSeriesData(samples));
  qhist->attach(ui->plot);

  if (panner != NULL) panner->setOrientations(Qt::Horizontal);
  ui->plot->replot();

  ui->numiterations->setText(QString::number(numiterations));
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

  const vector<statval> &statvals = task.getStatVals();
  int numpoints = statvals.size();
  if (numpoints == 0) return;
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
  ui->numiterations->setText(QString::number(statvals.back().iteration));
  ui->statistic->setText(QString::number(statvals.back().value, 'f', 2));
  ui->interval->setText(QString(QChar(177)) + " " + QString::number(quantile*statvals.back().std_err, 'f', 2));
  ui->std_err->setText(QString::number(statvals.back().std_err, 'f', 2));
}

//===========================================================================
// refresh
//===========================================================================
void AnalysisWidget::refresh()
{
  submit();
}

//===========================================================================
// segment changed
//===========================================================================
void AnalysisWidget::changeSegment()
{
  submit();
}

//===========================================================================
// view changed
//===========================================================================
void AnalysisWidget::changeView()
{
  submit();
}

//===========================================================================
// confidence changed
//===========================================================================
void AnalysisWidget::changeConfidence()
{
  if (ui->confidence->value() != confidence) {
    mutex.lock();
    confidence = ui->confidence->value();
    drawStatistic();
    mutex.unlock();
  }
}

//===========================================================================
// numbins changed
//===========================================================================
void AnalysisWidget::changeNumbins()
{
  if (ui->numbins->value() != (int)numbins) {
    numbins = ui->numbins->value();
    submit(ui->numbins->value());
  }
}

//===========================================================================
// percentile changed
//===========================================================================
void AnalysisWidget::changePercentile()
{
  if (ui->percentile->value() != percentile) {
    percentile = ui->percentile->value();
    submit();
  }
}

//===========================================================================
// set status
//===========================================================================
void AnalysisWidget::setStatus(int val)
{
  switch(val)
  {
    case AnalysisTask::reading:
      actionStop->setEnabled(true);
      ui->plot->setEnabled(false);
      progress->ui->progress->setFormat("");
      progress->ui->progress->setValue(0);
      progress->fadein();
      timer.start(REFRESH_MS);
      break;
    case AnalysisTask::running: {
      size_t readedbytes = task.getCsvFile().getReadedSize();
      QString str = Format::bytes(readedbytes).c_str();
      progress->ui->progress->setFormat(str);
      progress->ui->progress->setValue(100);
      progress->fadeout();
      ui->plot->setEnabled(true);
      break;
    }
    case AnalysisTask::failed:
      QMessageBox::warning(this, "CCruncher", QString("Error reading data.\n") + task.getMsgErr().c_str());
    case AnalysisTask::stopped:
      //TODO: indicates that user stoped
    case AnalysisTask::finished:
      actionStop->setEnabled(false);
      progress->fadeout();
      ui->plot->setEnabled(true);
      timer.stop();
      draw();
      break;
    default:
      assert(false);
  }
}

//===========================================================================
// stop current action
//===========================================================================
void AnalysisWidget::stop()
{
  task.stop();
}

