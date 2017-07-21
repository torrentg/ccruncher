
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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
#include <clocale>
#include <cassert>
#include <gsl/gsl_sf_gamma.h>
#include "gui/AnalysisTask.hpp"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;

#define MJ_EPSILON 1e-12

/**************************************************************************/
ccruncher_gui::AnalysisTask::AnalysisTask() : QThread(), hist(nullptr)
{
  mode_ = mode::none;
  progress = 0.0f;
  msgerr = "";
  isegment = 0;
  numbins = 1;
  percentile = 0.1;
  stop_ = false;
  setStatus(status::finished);
  setTerminationEnabled(false);
}

/**************************************************************************/
ccruncher_gui::AnalysisTask::~AnalysisTask()
{
  gsl_histogram_free(hist);
}

/**************************************************************************//**
 * @param[in] filename CSV filename.
 * @throw Exception Error opening file.
 */
void ccruncher_gui::AnalysisTask::setFilename(const QString &filename)
{
  csv.open(filename.toStdString());
}

/**************************************************************************//**
 * @note Assumes that there isn't a current execution.
 * @param[in] m Execution mode.
 * @param[in] s Segment (if required).
 * @param[in] p Percentile or number of bins (if required).
 */
void ccruncher_gui::AnalysisTask::setData(mode m, int s, double p)
{
  // set task data
  progress = 0.0f;
  mode_ = m;
  switch(m)
  {
    case mode::histogram:
      isegment = s;
      numbins = (size_t)(p+0.5);
      break;
    case mode::evolution_el:
      isegment = s;
      break;
    case mode::evolution_var:
    case mode::evolution_es:
      isegment = s;
      percentile = p;
      assert(0.0 < p && p < 1.0);
      break;
    case mode::contribution_el:
      break;
    case mode::contribution_es:
      percentile = p;
      assert(0.0 < p && p < 1.0);
      break;
    default:
      assert(false);
  }
}

/**************************************************************************//**
 * @return Exceution mode.
 */
AnalysisTask::mode ccruncher_gui::AnalysisTask::getMode() const
{
  return mode_;
}

/**************************************************************************//**
 * @return Csv file.
 */
CsvFile& ccruncher_gui::AnalysisTask::getCsvFile()
{
  return csv;
}

/**************************************************************************//**
 * @return Computed histogram, NULL if histogram not computed.
 */
const gsl_histogram* ccruncher_gui::AnalysisTask::getHistogram() const
{
  return hist;
}

/**************************************************************************//**
 * @return Computed statistic.
 */
const vector<statval>& ccruncher_gui::AnalysisTask::getStatVals() const
{
  return statvals;
}

/**************************************************************************//**
 * @return Segmentation values.
 */
const std::vector<contrib>& ccruncher_gui::AnalysisTask::getContributions() const
{
  return contribs;
}

/**************************************************************************//**
 * @param[in] col Column index (if <0 return rowsum).
 * @param[out] ret Data readed (rowsum if col negative).
 * @throw Exception Error reading data.
 * @throw StopException Read has been interrupted by user (using stop).
 */
void ccruncher_gui::AnalysisTask::readData(int col, std::vector<double> &ret)
{
  try {
    setStatus(status::reading);
    ret.clear();
    // if col<0 return rowsums
    csv.getColumn(col, ret, &stop_);
    if (stop_) throw StopException();
  }
  catch(Exception &e) {
    if (ret.size() > 100) {
      // we assume that is caused because ccruncher is
      // running and file content is not flushed
      msgerr = e.toString();
    }
    else {
      throw;
    }
  }
  catch(...) {
    throw;
  }
}

/**************************************************************************//**
 * @param[out] content Csv data.
 * @throw Exception Error reading data.
 * @throw StopException Read has been interrupted by user (using stop).
 */
void ccruncher_gui::AnalysisTask::readData(std::vector<std::vector<double>> &content)
{
  try {
    setStatus(status::reading);
    content.clear();
    csv.getColumns(content, &stop_);
    if (stop_) throw StopException();
  }
  catch(Exception &e) {
    if (content.size() > 0 && content[0].size() > 100) {
      // we assume that is caused because ccruncher is
      // running and file content is not flushed
      msgerr = e.toString();
    }
    else {
      throw;
    }
  }
  catch(...) {
    throw;
  }
}

/**************************************************************************//**
 * @details Execute data analysis.
 */
void ccruncher_gui::AnalysisTask::run()
{
  try
  {
    stop_ = false;
    progress = 0.0f;
    statvals.clear();
    contribs.clear();
    msgerr = "";
    setlocale(LC_ALL, "C"); // sets decimal point to sprintf and strtod

    switch(mode_)
    {
      case mode::histogram: {
        vector<double> values;
        readData(isegment, values);
        runHistogram(values);
        break;
      }
      case mode::evolution_el: {
        vector<double> values;
        readData(isegment, values);
        runEvolutionEL(values);
        break;
      }
      case mode::evolution_var: {
        vector<double> values;
        readData(isegment, values);
        runEvolutionVAR(values);
        break;
      }
      case mode::evolution_es: {
        vector<double> values;
        readData(isegment, values);
        runEvolutionES(values);
        break;
      }
      case mode::contribution_el: {
        vector<vector<double>> content;
        readData(content);
        runContributionEL(content);
        break;
      }
      case mode::contribution_es: {
        vector<vector<double>> content;
        readData(content);
        runContributionES(content);
        break;
      }
      default:
        assert(false);
    }
    setStatus(status::finished);
  }
  catch(StopException &e1)
  {
    assert(stop_);
    setStatus(status::stopped);
  }
  catch(std::exception &e2)
  {
    msgerr += (msgerr.empty()?"":"\n") + string(e2.what());
    setStatus(status::failed);
  }
}

/**************************************************************************//**
 * @details Result is set in variable hist.
 * @param[in] values Data values to computed histogram.
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runHistogram(const std::vector<double> &values)
{
  setStatus(status::running);

  gsl_histogram_free(hist);
  hist = nullptr;

  if (values.empty()) return;

  double intpart;
  bool areints = true;
  double minval = values[0];
  double maxval = values[0];
  for(auto value : values)
  {
    if (areints && fabs(modf(value,&intpart)) > 1e-14) areints = false;
    if (value > maxval) maxval = value;
    else if (value < minval) minval = value;
    if (stop_) throw StopException();
  }

  if (numbins <= 1) {
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
    if (i%100 == 0 || stop_) {
      progress = 100.0f*(float)(i+1)/(float)(values.size());
    }
    if (stop_) throw StopException();
  }
  progress = 100.0f;
}

/**************************************************************************//**
 * @details Given a list of values compute EL at regular steps. Let result
 *          in variable statvals.
 * @param[in] values Data values.
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runEvolutionEL(const std::vector<double> &values)
{
  setStatus(status::running);
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)2048);
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);
  kahan sum1, sum2;

  for(size_t i=0, n=0; i<numpoints; i++)
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
      progress = 100.0f*(float)(i+1)/(float)(numpoints);
    }
    if (stop_) throw StopException();
  }

  progress = 100.0f;
}

/**************************************************************************//**
 * @details Given a list of values compute VaR at regular steps. Let result
 *          in variable statvals.
 * @param[in] values Data values.
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runEvolutionVAR(std::vector<double> &values)
{
  setStatus(status::running);
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  // negate values (used in partial_sort)
  for(auto &value : values) {
    value = -value;
  }

  for(size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    statval var = valueAtRisk(1.0-percentile, values.begin(), values.begin()+n);
    var.value = -var.value;

    statvals.push_back(var);
    progress = 100.0f*(float)(i+1)/(float)(numpoints);
    if (stop_) throw StopException();
  }

  progress = 100.0f;
}

/**************************************************************************//**
 * @details Compute the VaR in range [first, last).
 * @param[in] percentile Percentile in [0,1].
 * @param[in] first Iterator to the first element to consider in a list.
 * @param[in] last Iterator to the last element to consider in a list.
 * @return VaR value.
 */
statval ccruncher_gui::AnalysisTask::valueAtRisk(double percentile,
    std::vector<double>::iterator first, std::vector<double>::iterator last)
{
  assert(0.0 < percentile && percentile < 1.0);
  assert(first < last);

  int n = last - first;
  int m = (int)(percentile*n + 0.5);

  if (m < 2) {
    return statval(n, *first, 0.0);
  }

  auto middle = first + m;
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
      w =  gsl_sf_beta_inc(a, b, (double)(i+2)/(double)(n)) -
           gsl_sf_beta_inc(a, b, (double)(i+1)/(double)(n));
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
      w =  gsl_sf_beta_inc(a, b, (double)(i+2)/(double)(n)) -
           gsl_sf_beta_inc(a, b, (double)(i+1)/(double)(n));
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

/**************************************************************************//**
 * @details Given a list of values compute ES at regular steps. Let result
 *          in variable statvals.
 * @param[in] values Data values.
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runEvolutionES(std::vector<double> &values)
{
  setStatus(status::running);
  statvals.clear();

  size_t numpoints = std::min(values.size(), (size_t)100); //2048
  double step = values.size()/(double)numpoints;
  statvals.reserve(numpoints);

  // negate values (used in partial_sort)
  for(auto &value : values) {
    value = -value;
  }

  for(size_t i=0; i<numpoints; i++)
  {
    int n = (int)((i+1)*step+0.5);
    statval es = expectedShortfall(1.0-percentile, values.begin(), values.begin()+n);
    es.value = -es.value;
    statvals.push_back(es);
    progress = 100.0f*(float)(i+1)/(float)(numpoints);
    if (stop_) throw StopException();
  }
  progress = 100.0f;
}

/**************************************************************************//**
 * @details Compute the Expected Shortfall in range [first, last).
 * @param[in] percentile Percentile in [0,1].
 * @param[in] first Iterator to the first element to consider in a list.
 * @param[in] last Iterator to the last element to consider in a list.
 * @return ES value.
 */
statval ccruncher_gui::AnalysisTask::expectedShortfall(double percentile,
    std::vector<double>::iterator first, std::vector<double>::iterator last)
{
  assert(0.0 < percentile && percentile < 1.0);
  assert(first < last);

  int n = last - first;
  int m = (int)(percentile*n + 0.5);

  if (m < 2) {
    return statval(n, *first, 0.0);
  }

  auto middle = first + m;
  // sort in such a way that [first,middle) contains the smallest elements
  partial_sort(first, middle, last);

  kahan sum1, sum2;
  for(auto it=first; it<middle; ++it) {
    sum1.add(*it);
    sum2.add((*it)*(*it));
  }

  double s1 = sum1.value();
  double s2 = sum2.value();
  double mean = s1/m;
  // see http://en.wikipedia.org/wiki/Standard_deviation#Rapid_calculation_methods
  double stdev = sqrt((m*s2-s1*s1)/double(m*(m-1)));
  double std_err = stdev/sqrt(m);
  // if stdev near 0 (eg. 1e-307) can cause std_err be NaN
  if (std::isnan(std_err)) std_err = 0.0;

  return statval(n, mean, std_err);
}

/**************************************************************************//**
 * @details Compute risk contribution. Let result in variable contribs.
 * @param[in] content Csv data (columns = segments).
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runContributionEL(const std::vector<std::vector<double>> &content)
{
  setStatus(status::running);

  statvals.clear();
  statvals.reserve(1);
  contribs.clear();
  contribs.reserve(content.size());

  vector<string> names = csv.getHeaders();
  size_t ncols = content.size();
  size_t nrows = content[0].size();

  // obtain portfolio losses
  vector<double> portfolio(nrows, 0.0);
  for(size_t i=0; i<nrows; i++) {
    for(size_t j=0; j<ncols; j++) {
      portfolio[i] += content[j][i];
    }
  }

  // compute portfolio EL
  kahan sum1, sum2;
  for(size_t i=0; i<nrows; i++) {
    sum1.add(portfolio[i]);
    sum2.add(portfolio[i]*portfolio[i]);
  }
  int n = nrows;
  double s1 = sum1.value();
  double s2 = sum2.value();
  double mean = s1/n;
  double stdev = sqrt((n*s2-s1*s1)/(n*(n-1.0)));
  statvals.push_back(statval(n, mean, stdev/sqrt(n)));

  // compute contributions
  contribs.resize(ncols);
  for(size_t j=0; j<ncols; j++) {
    contribs[j].name = names[j];
    contribs[j].value = 0.0;
  }
  vector<kahan> sums(ncols);
  for(size_t i=0; i<nrows; i++)
  {
    for(size_t j=0; j<ncols; j++) {
      sums[j].add(content[j][i]);
    }

    if (i%100 == 0 || stop_ || i==(nrows-1)) {
      for(size_t j=0; j<ncols; j++) {
        contribs[j].value = sums[j].value()/(i+1.0);
      }
      progress = 100.0f*(float)(i+1)/(float)(nrows);
    }

    if (stop_) throw StopException();
  }

  progress = 100.0f;
}

/**************************************************************************//**
 * @details Compute risk contribution. Let result in variable contribs.
 * @param[in] content Csv data (columns = segments).
 * @throw StopException Action stoped by user.
 */
void ccruncher_gui::AnalysisTask::runContributionES(std::vector<std::vector<double>> &content)
{
  setStatus(status::running);

  statvals.clear();
  statvals.reserve(1);
  contribs.clear();
  contribs.reserve(content.size());

  vector<string> names = csv.getHeaders();
  size_t ncols = content.size();
  size_t nrows = content[0].size();

  // obtain portfolio losses
  vector<double> portfolio(nrows, 0.0);
  vector<double> aux(nrows, 0.0);
  for(size_t i=0; i<nrows; i++) {
    for(size_t j=0; j<ncols; j++) {
      portfolio[i] += content[j][i];
    }
    aux[i] = -portfolio[i];
  }

  // compute portfolio VaR
  statval var = valueAtRisk(1.0-percentile, aux.begin(), aux.end());
  var.value = -var.value;

  // compute portfolio ES
  for(size_t i=0; i<nrows; i++) {
    aux[i] = -portfolio[i];
  }
  statval es = expectedShortfall(1.0-percentile, aux.begin(), aux.end());
  es.value = -es.value;
  statvals.push_back(es);

  // select rows bigger than VaR
  size_t n = 0;
  for(size_t i=0; i<nrows; i++) {
    if (portfolio[i] >= var.value) {
      for(size_t j=0; j<ncols; j++) {
        content[j][n] = content[j][i];
      }
      n++;
    }
  }
  content.resize(n);

  // compute contributions
  contribs.resize(ncols);
  for(size_t j=0; j<ncols; j++) {
    contribs[j].name = names[j];
    contribs[j].value = 0.0;
  }
  vector<kahan> sums(ncols);
  for(size_t i=0; i<n; i++)
  {
    for(size_t j=0; j<ncols; j++) {
      sums[j].add(content[j][i]);
    }

    if (i%100 == 0 || stop_ || i==(n-1)) {
      for(size_t j=0; j<ncols; j++) {
        contribs[j].value = sums[j].value()/(i+1.0);
      }
      progress = 100.0f*(float)(i+1)/(float)(n);
    }

    if (stop_) throw StopException();
  }

  progress = 100.0f;
}

/**************************************************************************/
void ccruncher_gui::AnalysisTask::stop()
{
  stop_ = true;
}

/**************************************************************************//**
 * @param[in] s New status.
 */
void ccruncher_gui::AnalysisTask::setStatus(status s)
{
  status_ = s;
  emit statusChanged(s);
}

/**************************************************************************//**
 * @return Execution status.
 */
AnalysisTask::status ccruncher_gui::AnalysisTask::getStatus() const
{
  return status_;
}

/**************************************************************************//**
 * @return Current progress (0..100).
 */
float ccruncher_gui::AnalysisTask::getProgress() const
{
  return progress;
}

/**************************************************************************//**
 * @return Error message (if any).
 */
const string & ccruncher_gui::AnalysisTask::getMsgErr() const
{
  return msgerr;
}

