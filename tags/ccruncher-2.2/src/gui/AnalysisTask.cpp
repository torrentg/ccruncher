
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

#include <cmath>
#include <algorithm>
#include <gsl/gsl_sf_gamma.h>
#include "gui/AnalysisTask.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;

#define MJ_EPSILON 1e-12

//===========================================================================
// constructor
//===========================================================================
ccruncher_gui::AnalysisTask::AnalysisTask() : QThread(), hist(NULL)
{
  mode_ = none;
  nsamples = 0;
  progress = 0;
  msgerr = "";
  setStatus(finished);
  setTerminationEnabled(false);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher_gui::AnalysisTask::~AnalysisTask()
{
  if (hist != NULL) gsl_histogram_free(hist);
}

//===========================================================================
// set csv filename
//===========================================================================
void ccruncher_gui::AnalysisTask::setFilename(const QString &filename) throw(Exception)
{
  csv.open(filename.toStdString());
}

//===========================================================================
// set data
//===========================================================================
void ccruncher_gui::AnalysisTask::setData(mode m, size_t s, double p)
{
  //TODO: stop current execution + progress=0
  mode_ = m;
  isegment = s;
  switch(m)
  {
    case histogram:
      numbins = (size_t)(p+0.5);
      break;
    case expected_loss:
      break;
    case value_at_risk:
    case expected_shortfall:
      percentile = p;
      assert(0.0 < p && p < 1.0);
      break;
    default:
      assert(false);
  }
}

//===========================================================================
// return mode
//===========================================================================
AnalysisTask::mode ccruncher_gui::AnalysisTask::getMode() const
{
  return mode_;
}

//===========================================================================
// return csvfile object
//===========================================================================
CsvFile& ccruncher_gui::AnalysisTask::getCsvFile()
{
  return csv;
}

//===========================================================================
// return histogram
//===========================================================================
const gsl_histogram* ccruncher_gui::AnalysisTask::getHistogram() const
{
  return hist;
}

//===========================================================================
// return statvals
//===========================================================================
const vector<statval>& ccruncher_gui::AnalysisTask::getStatVals() const
{
  return statvals;
}

//===========================================================================
// return the current number of samples
//===========================================================================
size_t ccruncher_gui::AnalysisTask::getNumSamples() const
{
  if (mode_ == histogram) return nsamples;
  else if (!statvals.empty()) return statvals.back().iteration;
  else return 0;
}

//===========================================================================
// run
//===========================================================================
void ccruncher_gui::AnalysisTask::run()
{
  try
  {
    stop_ = false;
    progress = 0.0;
    statvals.clear();
    nsamples = 0;
    msgerr = "";

    setStatus(reading);
    vector<double> values;
    try {
      csv.getValues(isegment, values, &stop_);
      if (stop_) {
        setStatus(stopped);
        return;
      }
    }
    catch(Exception &e) {
      if (values.size() > 100) {
        msgerr = e.toString();
      }
      else {
        throw;
      }
    }
    catch(...) {
      throw;
    }

    setStatus(running);
    switch(mode_)
    {
      case histogram:
        runHistogram(values);
        break;
      case expected_loss:
        runExpectedLoss(values);
        break;
      case value_at_risk:
        runValueAtRisk(values);
        break;
      case expected_shortfall:
        runExpectedShortfall(values);
        break;
      default:
        assert(false);
    }
    if (stop_) setStatus(stopped);
    else if (msgerr.empty()) setStatus(finished);
    else setStatus(failed);
  }
  catch(std::exception &e)
  {
    msgerr += (msgerr.empty()?"":"\n") + string(e.what());
    setStatus(failed);
  }
}

//===========================================================================
// run histogram
//===========================================================================
void ccruncher_gui::AnalysisTask::runHistogram(const vector<double> &values)
{
  nsamples = 0;

  if (hist != NULL) {
    gsl_histogram_free(hist);
    hist = NULL;
  }

  if (values.empty()) return;

  double intpart;
  bool areints = true;
  double minval = values[0];
  double maxval = values[0];
  for (size_t i=1; i<values.size(); i++)
  {
    if (areints && fabs(modf(values[i],&intpart)) > 1e-14) areints = false;
    if (values[i] > maxval) maxval = values[i];
    else if (values[i] < minval) minval = values[i];
    if (stop_) return;
  }

  if (numbins == 0) {
    if (areints && maxval-minval+1 < 5000) {
      numbins = maxval - minval + 1;
      minval -= 0.5;
      maxval += 0.5;
    }
    else {
      numbins = std::max((int)sqrt(values.size()), 10);
    }
  }

  hist = gsl_histogram_alloc(numbins);
  gsl_histogram_set_ranges_uniform(hist, minval, maxval+0.01);

  for(size_t i=0; i<values.size(); i++)
  {
    gsl_histogram_increment(hist, values[i]);
    nsamples++;
    if (i%100 == 0 || stop_) {
      progress = 100.0*(float)(i+1)/(float)(values.size());
    }
    if (stop_) return;
  }
  progress = 100.0;
}

//===========================================================================
// runExpectedLoss
//===========================================================================
void ccruncher_gui::AnalysisTask::runExpectedLoss(const vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)2048);
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);
  kahan sum1, sum2;

  for (size_t i=0,n=0; i<numpoints; i++)
  {
    double x = (i+1)*step;

    while(n < (size_t)(x+0.5)) {
      sum1.add(values[n]);
      sum2.add(values[n]*values[n]);
      n++;
    }

    double s1 = sum1.value();
    double s2 = sum2.value();
    double mean = s1/n;
    double stdev = sqrt((n*s2-s1*s1)/(n*(n-1.0)));

    statvals.push_back(statval(n, mean, stdev/sqrt(n)));

    if (i%100 == 0 || stop_) {
      progress = 100.0*(float)(i+1)/(float)(numpoints);
    }
    if (stop_) return;
  }

  progress = 100.0;
}


//===========================================================================
// runValueAtRisk
//===========================================================================
void ccruncher_gui::AnalysisTask::runValueAtRisk(vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  // negate values (used in partial_sort)
  for(size_t i=0; i<values.size(); i++) {
    values[i] = -values[i];
  }

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    statval var = valueAtRisk(1.0-percentile, values.begin(), values.begin()+n);
    var.value = -var.value;

    statvals.push_back(var);
    progress = 100.0*(float)(i+1)/(float)(numpoints);
    if (stop_) return;
  }

  progress = 100.0;
}

//===========================================================================
// Value at Risk of range [first,last)
//===========================================================================
statval ccruncher_gui::AnalysisTask::valueAtRisk(double percentile, vector<double>::iterator first, vector<double>::iterator last)
{
  assert(0.0 < percentile && percentile < 1.0);
  assert(first < last);

  int n = last - first;
  int m = (int)(percentile*n + 0.5);

  if (m < 2) {
    return statval(n, *first, 0.0);
  }

  vector<double>::iterator middle = first + m;
  // sort in such a way that [first,middle) contains the smallest elements
  partial_sort(first, middle, last);

  // quantile stderr (Maritz-Jarret)
  double w, val;
  double d1=0.0, d2=0.0;
  kahan c1=0.0, c2=0.0;
  double a = m - 1.0;
  double b = n - m;

  // ibeta fails when a,b values are too big (requires too much continued fractions sums)
  // in order to grant ibeta convergence we scale a,b values
  if (a > b && a > 50000) {
    b *= 50000/a;
    a = 50000;
  }
  else if (b >= a && b > 50000) {
    a *= 50000/b;
    b = 50000;
  }

  for(int i=m-1; i>=0; i--) {
    try {
      // ibeta(x,a,b) = 1-ibeta(1-x,b,a)
      w =  gsl_sf_beta_inc(a, b, (double)(i+2)/(double)(n)) - gsl_sf_beta_inc(a, b, (double)(i+1)/(double)(n));
    }
    catch(Exception &e) {
      // ibeta convergence not achieved
      w = 0.0;
    }
    val = *(first+i);
    d1 = w*val;
    d2 = w*val*val;
    c2.add(d2);
    c1.add(d1);
    if (fabs(w) < MJ_EPSILON) {
      break;
    }
  }

  vector<double> ws;
  for(int i=m; i<n; i++) {
    try {
      // ibeta(x,a,b) = 1-ibeta(1-x,b,a)
      w =  gsl_sf_beta_inc(a, b, (double)(i+2)/(double)(n)) - gsl_sf_beta_inc(a, b, (double)(i+1)/(double)(n));
      ws.push_back(w);
    }
    catch(Exception &e) {
      // ibeta convergence not achieved
      break;
    }
    if (fabs(w) < MJ_EPSILON) {
      break;
    }
  }

  if (!ws.empty())
  {
    partial_sort(middle, middle+ws.size(), last);
    for(size_t i=0; i<ws.size(); i++) {
      w = ws[i];
      val = *(first+m+i);
      d1 = w*val;
      d2 = w*val*val;
      c2.add(d2);
      c1.add(d1);
    }
  }

  double var = *(middle-1);
  double std_err = sqrt(c2.value() - c1.value()*c1.value());

  return statval(n, var, std_err);
}

//===========================================================================
// runExpectedShortfall
//===========================================================================
void ccruncher_gui::AnalysisTask::runExpectedShortfall(vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  // negate values (used in partial_sort)
  for(size_t i=0; i<values.size(); i++) {
    values[i] = -values[i];
  }

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    statval es = expectedShortfall(1.0-percentile, values.begin(), values.begin()+n);
    es.value = -es.value;
    statvals.push_back(es);
    progress = 100.0*(float)(i+1)/(float)(numpoints);
    if (stop_) return;
  }
  progress = 100.0;
}

//===========================================================================
// Expected Shortfall of range [first,last)
//===========================================================================
statval ccruncher_gui::AnalysisTask::expectedShortfall(double percentile, vector<double>::iterator first, vector<double>::iterator last)
{
  assert(0.0 < percentile && percentile < 1.0);
  assert(first < last);

  int n = last - first;
  int m = (int)(percentile*n + 0.5);

  if (m < 2) {
    return statval(n, *first, 0.0);
  }

  vector<double>::iterator middle = first + m;
  // sort in such a way that [first,middle) contains the smallest elements
  partial_sort(first, middle, last);

  kahan sum1, sum2;
  for(vector<double>::iterator it=first; it<middle; ++it) {
    sum1.add(*it);
    sum2.add((*it)*(*it));
  }

  double s1 = sum1.value();
  double s2 = sum2.value();
  double mean = s1/m;
  // see http://en.wikipedia.org/wiki/Standard_deviation#Rapid_calculation_methods
  double stdev = sqrt((m*s2-s1*s1)/(m*(m-1.0)));
  double std_err = stdev/sqrt(m);

  return statval(n, mean, std_err);
}

//===========================================================================
// stop
//===========================================================================
void ccruncher_gui::AnalysisTask::stop()
{
  stop_ = true;
}

//===========================================================================
// setStatus
//===========================================================================
void ccruncher_gui::AnalysisTask::setStatus(status s)
{
  status_ = s;
  emit statusChanged((int)s);
}

//===========================================================================
// return status
//===========================================================================
AnalysisTask::status ccruncher_gui::AnalysisTask::getStatus() const
{
  return status_;
}

//===========================================================================
// return progress
//===========================================================================
float ccruncher_gui::AnalysisTask::getProgress()
{
  return progress;
}

//===========================================================================
// return error
//===========================================================================
const string & ccruncher_gui::AnalysisTask::getMsgErr() const
{
  return msgerr;
}
