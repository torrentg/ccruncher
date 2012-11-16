#include <cmath>
#include <algorithm>
#include <gsl/gsl_sf_gamma.h>
#include "gui/AnalysisTask.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
AnalysisTask::AnalysisTask() : QThread(), hist(NULL)
{
  mode_ = none;
  nsamples = 0;
  progress = 0;
  setStatus(inactive);
  setTerminationEnabled(false);
}

//===========================================================================
// destructor
//===========================================================================
AnalysisTask::~AnalysisTask()
{
  if (hist != NULL) gsl_histogram_free(hist);
}

//===========================================================================
// set csv filename
//===========================================================================
void AnalysisTask::setFilename(const QString &filename) throw(Exception)
{
  csv.open(filename.toStdString());
}

//===========================================================================
// set data
//===========================================================================
void AnalysisTask::setData(mode m, size_t s, double p)
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
      break;
    default:
      assert(false);
  }
}

//===========================================================================
// return mode
//===========================================================================
AnalysisTask::mode AnalysisTask::getMode() const
{
  return mode_;
}

//===========================================================================
// return csvfile object
//===========================================================================
CsvFile& AnalysisTask::getCsvFile()
{
  return csv;
}

//===========================================================================
// return histogram
//===========================================================================
const gsl_histogram* AnalysisTask::getHistogram() const
{
  return hist;
}

//===========================================================================
// return statvals
//===========================================================================
const vector<statval>& AnalysisTask::getStatVals() const
{
  return statvals;
}

//===========================================================================
// return the current number of samples
//===========================================================================
size_t AnalysisTask::getNumSamples() const
{
  if (mode_ == histogram) return nsamples;
  else if (!statvals.empty()) return statvals.back().iteration;
  else return 0;
}

//===========================================================================
// run
//===========================================================================
void AnalysisTask::run()
{
  try
  {
    stop_ = false;
    progress = 0.0;

    statvals.clear();
    nsamples = 0;

    //TODO: avoid re-read if isegment anf fsorted unchanged (implies values as member)

    setStatus(reading);
    vector<double> values;
    //TODO: set csv:getValues as cancelable task (&stop)
    csv.getValues(isegment, values);

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
    setStatus(finished);
  }
  catch(std::exception &e)
  {
    setStatus(failed);
  }
}

//===========================================================================
// run histogram
//===========================================================================
void AnalysisTask::runHistogram(const vector<double> &values)
{
  nsamples = 0;

  if (hist != NULL) {
    gsl_histogram_free(hist);
    hist = NULL;
  }

  double minval = values[0];
  double maxval = values[0];
  for (size_t i=1; i<values.size(); i++)
  {
    if (values[i] > maxval) maxval = values[i];
    else if (values[i] < minval) minval = values[i];
    if (stop_) return;
  }

  if (numbins == 0) {
    numbins = std::max((int)sqrt(values.size()), 10);
  }

  hist = gsl_histogram_alloc(numbins);
  gsl_histogram_set_ranges_uniform(hist, minval, maxval+1e-10);

  for(size_t i=0; i<values.size(); i++)
  {
    gsl_histogram_increment(hist, values[i]);
    nsamples++;
    if (i%100 == 0 || stop_) {
      progress = 100.0*(float)(i+1)/(float)(values.size());
    }
    if (stop_) return;
    //TODO: set timer to refresh
  }
  progress = 100.0;
}

//===========================================================================
// runExpectedLoss
//===========================================================================
void AnalysisTask::runExpectedLoss(const vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)2048);
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);
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

    statvals.push_back(statval(n, mean, stdev/sqrt(n)));

    if (i%100 == 0 || stop_) {
      progress = 100.0*(float)(i+1)/(float)(numpoints);
    }
    if (stop_) return;
    //TODO: set timer to refresh
  }

  progress = 100.0;
}


//===========================================================================
// runValueAtRisk
//===========================================================================
void AnalysisTask::runValueAtRisk(vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    //TODO: considere partial sort instead of full sort
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

    double var = values[m-1];
    double std_err = sqrt(c2 - c1*c1);
    statvals.push_back(statval(n, var, std_err));

    progress = 100.0*(float)(i+1)/(float)(numpoints);
    if (stop_) return;
    //TODO: set timer to refresh
  }

  progress = 100.0;
}

//===========================================================================
// runExpectedShortfall
//===========================================================================
void AnalysisTask::runExpectedShortfall(vector<double> &values)
{
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  for (size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    //TODO: considere partial sort instead of full sort
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
    double std_err = stdev/sqrt(k);
    statvals.push_back(statval(n, mean, std_err));

    progress = 100.0*(float)(i+1)/(float)(numpoints);
    if (stop_) return;
    //TODO: set timer to refresh
  }
  progress = 100.0;
}

//===========================================================================
// stop
//===========================================================================
void AnalysisTask::stop()
{
  stop_ = true;
}

//===========================================================================
// setStatus
//===========================================================================
void AnalysisTask::setStatus(status s)
{
  status_ = s;
  emit statusChanged((int)s);
}

//===========================================================================
// return status
//===========================================================================
AnalysisTask::status AnalysisTask::getStatus() const
{
  return status_;
}

//===========================================================================
// return progress
//===========================================================================
float AnalysisTask::getProgress()
{
  return progress;
}

