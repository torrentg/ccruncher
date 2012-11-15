#ifndef _AnalysisTask_
#define _AnalysisTask_

#include <vector>
#include <QThread>
#include <gsl/gsl_histogram.h>
#include "utils/CsvFile.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

struct statval
{
  size_t iteration;
  double value;
  double std_err;
  statval(size_t n, double v, double e) : iteration(n), value(v), std_err(e) {}
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
      inactive=1,
      reading=2,
      running=3,
      failed=4,
      finished=5
    };

  private:

    // csv file
    CsvFile csv;
    // segment index
    size_t isegment;
    // number of bins (hist)
    size_t numbins;
    // percentile (var, es)
    double percentile;
    // computed statistics (result)
    vector<statval> statvals;
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

  private:

    // set status
    void setStatus(status);
    // compute histogram
    void runHistogram(const vector<double> &);
    // compute EL
    void runExpectedLoss(const vector<double> &);
    // compute VaR
    void runValueAtRisk(vector<double> &);
    // compute ES
    void runExpectedShortfall(vector<double> &);

  public:

    // constructor
    AnalysisTask();
    // destructor
    ~AnalysisTask();
    // set csv filename
    void setFilename(const QString &) throw(Exception);
    // set data
    void setData(mode m, size_t isegment, double param=0.0);
    // return mode
    mode getMode() const;
    // return csvfile object
    CsvFile& getCsvFile();
    // return histogram
    const gsl_histogram *getHistogram() const;
    // return statvals
    const vector<statval>& getStatVals() const;
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

  signals:

    // status updated
    void statusChanged(int);

};

#endif

