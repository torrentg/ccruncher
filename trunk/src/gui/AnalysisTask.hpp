
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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

#ifndef _AnalysisTask_
#define _AnalysisTask_

#include <string>
#include <vector>
#include <QThread>
#include <gsl/gsl_histogram.h>
#include "utils/CsvFile.hpp"
#include "utils/Exception.hpp"

namespace ccruncher_gui {

struct statval
{
  size_t iteration;
  double value;
  double std_err;
  statval(size_t n, double v, double e) : iteration(n), value(v), std_err(e) {}
};

struct contrib
{
  std::string name;
  double value;
  contrib(std::string n="", double v=0.0) : name(n), value(v) {}
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
      evolution_el=2,
      evolution_var=3,
      evolution_es=4,
      contribution_el=5,
      contribution_es=6
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

    // stopped task exception
    class StopException : public ccruncher::Exception { };

  private:

    // csv file
    ccruncher::CsvFile csv;
    // segment index (-1 means rowSums)
    int isegment;
    // number of bins (hist)
    size_t numbins;
    // percentile (var, es)
    double percentile;
    // histogram (result)
    gsl_histogram *hist;
    // computed statistics (result)
    std::vector<statval> statvals;
    // computed contributions (result)
    std::vector<contrib> contribs;
    // progress (0..100)
    volatile float progress;
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
    static statval valueAtRisk(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);
    // expected shortfall
    static statval expectedShortfall(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);

    // read csv data
    void readData(int col, std::vector<double> &ret) throw(ccruncher::Exception);
    // read csv data
    void readData(std::vector<std::vector<double> > &ret) throw(ccruncher::Exception);
    // set status
    void setStatus(status);
    // compute histogram
    void runHistogram(const std::vector<double> &);
    // compute EL
    void runEvolutionEL(const std::vector<double> &);
    // compute VaR
    void runEvolutionVAR(std::vector<double> &);
    // compute ES
    void runEvolutionES(std::vector<double> &);
    // contribution EL
    void runContributionEL(const std::vector<std::vector<double> > &);
    // contribution ES
    void runContributionES(std::vector<std::vector<double> > &);

  public:

    // constructor
    AnalysisTask();
    // destructor
    ~AnalysisTask();
    // set csv filename
    void setFilename(const QString &) throw(ccruncher::Exception);
    // set data
    void setData(mode m, int isegment=-1, double param=0.0);
    // return mode
    mode getMode() const;
    // return csvfile object
    ccruncher::CsvFile& getCsvFile();
    // return histogram
    const gsl_histogram *getHistogram() const;
    // return statvals
    const std::vector<statval>& getStatVals() const;
    // return contributions
    const std::vector<contrib>& getContributions() const;
    // task
    void run();
    // stop current execution
    void stop();
    // return status
    status getStatus() const;
    // return progress
    float getProgress() const;
    // return error
    const std::string &getMsgErr() const;

  signals:

    // status updated
    void statusChanged(int);

};

}

#endif

