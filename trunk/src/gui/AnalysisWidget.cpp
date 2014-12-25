
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

#include <cmath>
#include <vector>
#include <cassert>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_cdf.h>
#include <QMessageBox>
#include <QClipboard>
#include <QToolTip>
#include <QMimeData>
#include <QMetaType>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_symbol.h>
#include "ui_AnalysisWidget.h"
#include "gui/QwtPlotCanvasExt.hpp"
#include "gui/QwtPieChart.hpp"
#include "gui/AnalysisWidget.hpp"
#include "utils/Utils.hpp"

#if QWT_VERSION < 0x060100
  #define setMajorPen setMajPen
#endif

#define REFRESH_MS 100

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] filename Csv file name.
 * @param[in] parent Widget parent.
 */
ccruncher_gui::AnalysisWidget::AnalysisWidget(const QString &filename, QWidget *parent) :
  MdiChildWidget(parent), ui(new Ui::AnalysisWidget), progress(nullptr),
    magnifier(nullptr), panner(nullptr), task_progress(0.0f), toolbar(nullptr)
{
  blockSignals(true);

  ui->setupUi(this);

  numbins = (size_t) ui->numbins->value();
  percentile = ui->percentile->value();
  confidence = ui->confidence->value();

  progress = new ProgressWidget(ui->frame);
  progress->setWindowFlags(Qt::WindowStaysOnTopHint);
  ui->frame->addLayer(progress);

  ui->filename->setText(filename);
  QwtPlotCanvas *canvas = new QwtPlotCanvasExt(ui->plot);
  canvas->setFrameStyle(QFrame::StyledPanel|QFrame::Plain);
  canvas->setMouseTracking(false);
  connect(canvas, SIGNAL(mouseMoved(QPoint)), this, SLOT(showPiechartTooltip(QPoint)));

  ui->plot->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->plot->setCanvas(canvas);

  magnifier = new QwtPlotMagnifier(canvas);
  magnifier->setMouseFactor(1.0);
  panner = new QwtPlotPanner(canvas);

  // magnify x-axis
  actionZoomX = new QAction(tr("Zoom axis X"), this);
  actionZoomX->setCheckable(true);
  actionZoomX->setChecked(true);
  connect(actionZoomX, SIGNAL(triggered(bool)), this, SLOT(setZoomX(bool)));
  ui->plot->addAction(actionZoomX);

  // magnify y-axis
  actionZoomY = new QAction(tr("Zoom axis Y"), this);
  actionZoomY->setCheckable(true);
  actionZoomY->setChecked(true);
  connect(actionZoomY, SIGNAL(triggered(bool)), this, SLOT(setZoomY(bool)));
  ui->plot->addAction(actionZoomY);

  // copy data to clipboard
  actionCopy = new QAction(tr("Copy data to clipboard"), this);
  actionCopy->setShortcut(QKeySequence(QKeySequence::Copy));
  connect(actionCopy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
  ui->plot->addAction(actionCopy);

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
  qRegisterMetaType<ccruncher_gui::AnalysisTask::status>();
  connect(&task, SIGNAL(statusChanged(ccruncher_gui::AnalysisTask::status)), this, SLOT(setStatus(ccruncher_gui::AnalysisTask::status)), Qt::QueuedConnection);

  // open file & display
  ui->segment->blockSignals(true);
  task.setFilename(filename);
  vector<string> segments = task.getCsvFile().getHeaders();
  if (segments.size() > 1) {
    ui->segment->addItem("All");
    ui->segment->insertSeparator(1);
  }
  for(auto &segment : segments) {
    ui->segment->addItem(segment.c_str());
  }
  ui->segment->blockSignals(false);

  // set modes
  ui->mode->blockSignals(true);
  ui->mode->clear();
  if (segments.size() > 1) {
    ui->mode->addItem("Segmentation", 0);
  }
  ui->mode->addItem("Segments", 1);
  ui->mode->blockSignals(false);

  blockSignals(false);
  changeMode();
}

/**************************************************************************/
ccruncher_gui::AnalysisWidget::~AnalysisWidget()
{
  blockSignals(true);
  task.stop();
  task.wait(250);
  if (toolbar != nullptr) delete toolbar;
  delete ui;
}

/**************************************************************************//**
 * @param[in] checked Enable/disable zoom on x-axis.
 */
void ccruncher_gui::AnalysisWidget::setZoomX(bool checked)
{
  assert(magnifier != nullptr);
  magnifier->setAxisEnabled(QwtPlot::xBottom, checked);
}

/**************************************************************************//**
 * @param[in] checked Enable/disable zoom on y-axis.
 */
void ccruncher_gui::AnalysisWidget::setZoomY(bool checked)
{
  assert(magnifier != nullptr);
  magnifier->setAxisEnabled(QwtPlot::yLeft, checked);
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::reset()
{
  ui->plot->detachItems();
  ui->plot->setAxisAutoScale(QwtPlot::xBottom);
  ui->plot->setAxisAutoScale(QwtPlot::yLeft);
  ui->plot->setAxisTitle(QwtPlot::yLeft, "");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "");
  ui->plot->setTitle("");

  ui->numiterations->setText("");
  ui->statistic->setText("");
  ui->interval->setText("");
  ui->std_err->setText("");

  strdata.clear();
}

/**************************************************************************//**
 * @details Submit an analysis task in background using AnalysisTask.
 */
void ccruncher_gui::AnalysisWidget::submit()
{
  mutex.lock();
  if (task.isRunning()) task.stop();

  int imode = getMode();
  int iview = getView();
  int isegment = ui->segment->currentIndex();

  // combobox with no items
  if (imode < 0 || iview < 0 || isegment < 0) {
    mutex.unlock();
    return;
  }
  if (ui->segment->count() > 1) {
    // substract 'All' and separator items. 'All' index = -1
    isegment = isegment - (isegment==0?1:2);
  }

  task.wait();
  reset();

  if (imode == 0)
  {
    switch(iview)
    {
      case 1:
        task.setData(AnalysisTask::mode::contribution_el);
        break;
      case 3:
        task.setData(AnalysisTask::mode::contribution_es, -1, ui->percentile->value()/100.0);
        break;
      default:
        assert(false);
    }
  }
  else if (imode == 1)
  {
    switch(iview)
    {
      case 0:
        task.setData(AnalysisTask::mode::histogram, isegment, ui->numbins->value());
        break;
      case 1:
        task.setData(AnalysisTask::mode::evolution_el, isegment);
        break;
      case 2:
        task.setData(AnalysisTask::mode::evolution_var, isegment, ui->percentile->value()/100.0);
        break;
      case 3:
        task.setData(AnalysisTask::mode::evolution_es, isegment, ui->percentile->value()/100.0);
        break;
      default:
        assert(false);
    }
  }

  task_progress = task.getProgress();
  task.start();
  mutex.unlock();
}

/**************************************************************************//**
 * @details Output analysis results (can be parcials because task is still
 *          running).
 */
void ccruncher_gui::AnalysisWidget::draw()
{
  mutex.lock();

  if (task.getStatus() == AnalysisTask::status::reading)
  {
    size_t totalbytes = task.getCsvFile().getFileSize();
    size_t readedbytes = task.getCsvFile().getReadedSize();
    float pct = 100.0 * (float)(readedbytes)/(float)(totalbytes);
    QString str = Utils::bytesToString(readedbytes).c_str();
    progress->ui->progress->setFormat(str);
    progress->ui->progress->setValue(pct+0.5);
  }
  else
  {
    if (task_progress == task.getProgress())
    {
      // nothing to do
    }
    else
    {
      ui->plot->setEnabled(true);
      task_progress = task.getProgress();
      switch(task.getMode())
      {
        case AnalysisTask::mode::histogram:
          drawHistogram();
          break;
        case AnalysisTask::mode::evolution_el:
        case AnalysisTask::mode::evolution_var:
        case AnalysisTask::mode::evolution_es:
          drawCurve();
          break;
        case AnalysisTask::mode::contribution_el:
        case AnalysisTask::mode::contribution_es:
          drawPiechart();
          break;
        default:
          assert(false);
      }
    }
  }

  mutex.unlock();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::drawHistogram()
{
  ui->plot->detachItems();
  strdata.clear();
  strdata.append("LOW\tUP\tFREQ\n");

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->attach(ui->plot);

  ui->plot->setAxisTitle(QwtPlot::yLeft, "Frequency");
  ui->plot->setAxisTitle(QwtPlot::xBottom, "Portfolio Loss");

  const gsl_histogram *hist = task.getHistogram();
  if (hist == nullptr) return;
  numbins = gsl_histogram_bins(hist);
  ui->numbins->blockSignals(true);
  ui->numbins->setValue(numbins);
  ui->numbins->blockSignals(false);

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
    strdata.append(QString::number(lower) + "\t" +
                   QString::number(upper) + "\t" +
                   QString::number(val) + "\n");
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

  if (panner != nullptr) panner->setOrientations(Qt::Horizontal);
  ui->plot->replot();

  ui->numiterations->setText(QString::number(numiterations));
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::drawCurve()
{
  ui->plot->detachItems();
  strdata.clear();
  strdata.append("IT\tVAL\tSTDERR\n");

  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableX(true);
  grid->enableY(true);
  grid->enableXMin(false);
  grid->enableYMin(false);
  grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
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
    strdata.append(QString::number(statvals[i].iteration) + "\t" +
                   QString::number(statvals[i].value) + "\t" +
                   QString::number(statvals[i].std_err) + "\n");
  }

  // adjusting vertical scale (avoids initial 5%)
  double ymin=ranges.back().interval.minValue();
  double ymax=ranges.back().interval.maxValue();
  for(int i=(int)(numpoints*0.05); i<numpoints; i++) {
    if (ranges[i].interval.minValue() < ymin) ymin = ranges[i].interval.minValue();
    if (ranges[i].interval.maxValue() > ymax) ymax = ranges[i].interval.maxValue();
  }

  QwtPlotCurve *d_curve = new QwtPlotCurve("title");
  d_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
  d_curve->setStyle(QwtPlotCurve::Lines); //NoCurve
  d_curve->setSamples(values);
  d_curve->attach(ui->plot);

  if (ymin < ymax)
  {
    ui->plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);

    QwtPlotIntervalCurve *d_intervalCurve = new QwtPlotIntervalCurve("title");
    d_intervalCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    d_intervalCurve->setPen(QPen(Qt::white));
    QColor bg(Qt::blue);
    bg.setAlpha(150);
    d_intervalCurve->setBrush(QBrush(bg));
    d_intervalCurve->setStyle(QwtPlotIntervalCurve::Tube);
    d_intervalCurve->setSamples(ranges);
    d_intervalCurve->attach(ui->plot);
  }

  if (panner != nullptr) panner->setOrientations(Qt::Horizontal | Qt::Vertical);
  ui->plot->replot();

  ui->statistic_str->setText(name + ":");
  ui->numiterations->setText(QString::number(statvals.back().iteration));
  ui->statistic->setText(QString::number(statvals.back().value, 'f', 2));
  ui->interval->setText(QString(QChar(177)) + " " + QString::number(quantile*statvals.back().std_err, 'f', 2));
  ui->std_err->setText(QString::number(statvals.back().std_err, 'f', 2));
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::drawPiechart()
{
  ui->plot->detachItems();
  strdata.clear();
  strdata.append("SEGMENT\tVAL\n");

  const vector<statval> &statvals = task.getStatVals();
  if (statvals.empty()) return;

  QString name = ui->view->currentText();
  ui->statistic_str->setText(name + ":");
  ui->numiterations->setText(QString::number(statvals.back().iteration));
  ui->statistic->setText(QString::number(statvals.back().value, 'f', 2));
  ui->std_err->setText(QString::number(statvals.back().std_err, 'f', 2));

  const vector<contrib> &contribs = task.getContributions();
  if (contribs.empty()) return;

  vector<double> values(contribs.size());
  vector<string> names(contribs.size());
  for(size_t i=0; i<contribs.size(); i++) {
    names[i] = contribs[i].name;
    values[i] = contribs[i].value;
    strdata.append(QString(names[i].c_str()) + "\t" +
                   QString::number(values[i]) + "\n");
  }

  ui->plot->setTitle(name + " Disaggregation");
  QwtPieChart *piechart = new QwtPieChart();
  piechart->setSamples(values, names);
  piechart->attach(ui->plot);
  ui->plot->replot();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::refresh()
{
  numbins = 1;
  ui->numbins->blockSignals(true);
  // reset the number of bins forcing to recompute the number of
  // bins based on the amount of available new data.
  ui->numbins->setValue(1);
  ui->numbins->blockSignals(false);
  submit();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changeSegment()
{
  submit();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changeView()
{
  int imode = getMode();
  int iview = getView();

  ui->percentile->setVisible(iview<=1?false:true);
  ui->percentile_str->setVisible(iview<=1?false:true);

  ui->confidence->setVisible((imode==1 && iview>0)?true:false);
  ui->confidence_str->setVisible((imode==1 && iview>0)?true:false);

  ui->numbins->setVisible((imode==1 && iview==0)?true:false);
  ui->numbins_str->setVisible((imode==1 && iview==0)?true:false);

  ui->statistic->setVisible(iview==0?false:true);
  ui->statistic_str->setVisible(iview==0?false:true);
  ui->statistic_str->setText(ui->view->currentText() + ":");

  ui->interval->setVisible((imode==1 && iview>0)?true:false);
  ui->interval_str->setVisible((imode==1 && iview>0)?true:false);

  ui->std_err->setVisible(iview==0?false:true);
  ui->std_err_str->setVisible(iview==0?false:true);

  submit();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changeMode()
{
  int imode = getMode();

  // adapting plot zone
  ui->plot->canvas()->setMouseTracking(imode==0);
  ui->plot->enableAxis(QwtPlot::xBottom, imode==1);
  ui->plot->enableAxis(QwtPlot::yLeft, imode==1);
  magnifier->setEnabled(imode==1);
  panner->setEnabled(imode==1);
  actionZoomX->setEnabled(imode==1);
  actionZoomY->setEnabled(imode==1);

  // show/hide controls
  ui->segment->setVisible(imode==1);
  ui->segment_str->setVisible(imode==1);

  // filling view combobox
  int iview = getView();
  ui->view->blockSignals(true);
  ui->view->clear();
  if (imode == 0) {
    ui->view->addItem("Expected Loss", 1);
    ui->view->addItem("Expected Shortfall", 3);
  }
  else {
    ui->view->addItem("Histogram", 0);
    ui->view->addItem("Expected Loss", 1);
    ui->view->addItem("Value at Risk", 2);
    ui->view->addItem("Expected Shortfall", 3);
    // set segment All (because we come from mode=0)
    ui->segment->blockSignals(true);
    ui->segment->setCurrentIndex(0);
    ui->segment->blockSignals(false);
  }
  for(int i=0; i<ui->view->count(); i++) {
    if (ui->view->itemData(i).toInt() == iview) {
      ui->view->setCurrentIndex(i);
      break;
    }
  }
  ui->view->blockSignals(false);

  changeView();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changeConfidence()
{
  if (ui->confidence->value() != confidence) {
    mutex.lock();
    confidence = ui->confidence->value();
    drawCurve();
    mutex.unlock();
  }
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changeNumbins()
{
  if (ui->numbins->value() != (int)numbins) {
    numbins = (size_t) ui->numbins->value();
    submit();
  }
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::changePercentile()
{
  if (ui->percentile->value() != percentile) {
    percentile = ui->percentile->value();
    submit();
  }
}

/**************************************************************************//**
 * @param[in] val New Status.
 */
void ccruncher_gui::AnalysisWidget::setStatus(AnalysisTask::status val)
{
  switch(val)
  {
    case AnalysisTask::status::reading:
      emit newStatusMsg("reading ...");
      actionStop->setEnabled(true);
      ui->plot->setEnabled(false);
      progress->ui->progress->setFormat("");
      progress->ui->progress->setValue(0);
      progress->fadein();
      timer.start(REFRESH_MS);
      break;
    case AnalysisTask::status::running: {
      emit newStatusMsg("analyzing ...");
      size_t readedbytes = task.getCsvFile().getReadedSize();
      QString str = Utils::bytesToString(readedbytes).c_str();
      progress->ui->progress->setFormat(str);
      progress->ui->progress->setValue(100);
      progress->fadeout();
      ui->plot->setEnabled(true);
      break;
    }
    case AnalysisTask::status::failed:
      QMessageBox::warning(this, "CCruncher", QString("Error reading data.\n") + task.getMsgErr().c_str());
      actionStop->setEnabled(false);
      progress->fadeout();
      ui->plot->setEnabled(true);
      timer.stop();
      draw();
      emit newStatusMsg("error: " + QString(task.getMsgErr().c_str()));
      break;
    case AnalysisTask::status::stopped:
      actionStop->setEnabled(false);
      progress->fadeout();
      ui->plot->setEnabled(true);
      timer.stop();
      draw();
      emit newStatusMsg("stopped");
      break;
    case AnalysisTask::status::finished:
      actionStop->setEnabled(false);
      progress->fadeout();
      ui->plot->setEnabled(true);
      timer.stop();
      draw();
      if (task.getMsgErr().empty()) emit newStatusMsg("done");
      else emit newStatusMsg("warning: " + QString(task.getMsgErr().c_str()));
      break;
    default:
      assert(false);
  }
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::stop()
{
  task.stop();
}

/**************************************************************************/
void ccruncher_gui::AnalysisWidget::copyToClipboard()
{
  QMimeData *mimeData = new QMimeData();
  mimeData->setData("text/plain", strdata);
  QApplication::clipboard()->setMimeData(mimeData);
}

/**************************************************************************//**
 * @details Show info just under the given point.
 * @param[in] point Point in the pie chart.
 */
void ccruncher_gui::AnalysisWidget::showPiechartTooltip(QPoint point)
{
  mutex.lock();

  if (ui->plot->itemList().size() == 0 ||
      ui->plot->itemList().at(0)->rtti() != QwtPlotItem::Rtti_PlotUserItem) {
    mutex.unlock();
    return;
  }

  static bool status=false;
  QwtPieChart *piechart = (QwtPieChart *) ui->plot->itemList().at(0);

  string name;
  double val, pct;
  bool valid = piechart->getInfo(point, name, val, pct);
  if (valid) {
    QString msg = QString(name.c_str()) + "\n";
    msg += QString::number(val, 'f', 2) + "\n";
    msg += QString::number(pct*100, 'f', 2) + (status?"%":"% ");
    QToolTip::showText(QCursor::pos(), msg, ui->plot->canvas(), ui->plot->canvas()->rect());
    status = !status;
  }
  else {
    QToolTip::hideText();
  }
  mutex.unlock();
}

/**************************************************************************//**
 * @return Mode index. -1=undefined, 0=segmentation, 1=segments.
 */
int ccruncher_gui::AnalysisWidget::getMode() const
{
  int row = ui->mode->currentIndex();
  if (row < 0) return -1;
  bool ok = true;
  int ret = ui->mode->itemData(row).toInt(&ok);
  if (ok) return ret;
  else return -1;
}

/**************************************************************************//**
 * @return View index. -1=undefined, 0=histogram, 1=EL, 2=VaR, 3=ES.
 */
int ccruncher_gui::AnalysisWidget::getView() const
{
  int row = ui->view->currentIndex();
  if (row < 0) return -1;
  bool ok = true;
  int ret = ui->view->itemData(row).toInt(&ok);
  if (ok) return ret;
  else return -1;
}

