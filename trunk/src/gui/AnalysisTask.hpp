
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

#ifndef _AnalysisTask_
#define _AnalysisTask_

#include <string>
#include <vector>
#include <QThread>
#include <gsl/gsl_histogram.h>
#include "utils/CsvFile.hpp"
#include "utils/Exception.hpp"

struct statval
{
  size_t iteration;
  double value;
  double std_err;
  statval(size_t n, double v, double e) : iteration(n), value(v), std_err(e) {}
};

// see http://en.wikipedia.org/wiki/Kahan_summation_algorithm
class kahan
{
  private:

    // current working sum
    double sum;
    // carry from the previous operation
    double carry;

  public:

    // constructor
    kahan(double val=0.0) : sum(val), carry(0.0) {}
    // add value
    void add(double val) {
      double y = val - carry;
      double t = sum + y;
      carry = (t - sum) - y;
      sum = t;
    }
    // return value
    double value() const { return sum; }

};

class AnalysisTask : public QThread
{
    Q_OBJECT

  public:

    // enum operation modes
    enum mode
    {
      none=0,
      histogram=1,
      expected_loss=2,
      value_at_risk=3,
      expected_shortfall=4
    };

    // status types
    enum status
    {
      reading=1,
      running=2,
      stopped=3,
      failed=4,
      finished=5
    };

  private:

    // csv file
    ccruncher::CsvFile csv;
    // segment index
    size_t isegment;
    // number of bins (hist)
    size_t numbins;
    // percentile (var, es)
    double percentile;
    // computed statistics (result)
    std::vector<statval> statvals;
    // histogram (result)
    gsl_histogram *hist;
    // number of procesed samples
    size_t nsamples;
    // progress (0..100)
    float progress;
    // mode
    mode mode_;
    // status
    status status_;
    // stop flag
    bool stop_;
    // message error (if any)
    std::string msgerr;

  private:

    // value at risk
    statval valueAtRisk(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);
    // expected shortfall
    static statval expectedShortfall(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);

    // set status
    void setStatus(status);
    // compute histogram
    void runHistogram(const std::vector<double> &);
    // compute EL
    void runExpectedLoss(const std::vector<double> &);
    // compute VaR
    void runValueAtRisk(std::vector<double> &);
    // compute ES
    void runExpectedShortfall(std::vector<double> &);

  public:

    // constructor
    AnalysisTask();
    // destructor
    ~AnalysisTask();
    // set csv filename
    void setFilename(const QString &) throw(ccruncher::Exception);
    // set data
    void setData(mode m, size_t isegment, double param=0.0);
    // return mode
    mode getMode() const;
    // return csvfile object
    ccruncher::CsvFile& getCsvFile();
    // return histogram
    const gsl_histogram *getHistogram() const;
    // return statvals
    const std::vector<statval>& getStatVals() const;
    // return the current number of samples
    size_t getNumSamples() const;
    // task
    void run();
    // stop current execution
    void stop();
    // return status
    status getStatus() const;
    // return progress
    float getProgress();
    // return error
    const std::string &getMsgErr() const;

  signals:

    // status updated
    void statusChanged(int);

};

#endif

