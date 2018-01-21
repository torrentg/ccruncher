
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

#pragma once

#include <string>
#include <vector>
#include <QThread>
#include <QMetaType>
#include <gsl/gsl_histogram.h>
#include "utils/CsvFile.hpp"
#include "utils/Exception.hpp"

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief Statistic value (eg. VaR).
 */
struct statval
{
  //! Number of samples
  size_t iteration;
  //! statistic value
  double value;
  //! Standard error
  double std_err;
  //! Constructor
  statval(size_t n, double v, double e) : iteration(n), value(v), std_err(e) {}
};

/**************************************************************************//**
 * @brief Contribution to risk (segmentation pie).
 */
struct contrib
{
  //! Segment name
  std::string name;
  //! Value
  double value;
  //! Constructor
  contrib(const std::string &n="", double v=0.0) : name(n), value(v) {}
};

/**************************************************************************//**
 * @brief Kahan summation algorithm.
 * @see http://en.wikipedia.org/wiki/Kahan_summation_algorithm
 */
class kahan
{
  private:

    //! Current working sum
    double sum;
    //! Carry from the previous operation
    double carry;

  public:

    //! Constructor
    kahan(double val=0.0) : sum(val), carry(0.0) {}
    //! Add value
    void add(double val) {
      double y = val - carry;
      double t = sum + y;
      carry = (t - sum) - y;
      sum = t;
    }
    //! Return value
    double value() const { return sum; }

};

/**************************************************************************//**
 * @brief Thread that performs the data analysis.
 */
class AnalysisTask : public QThread
{
    Q_OBJECT
    Q_ENUMS(ccruncher_gui::AnalysisTask::status)

  public:

    // operation modes
    enum class mode
    {
      none=0,             //!< None
      histogram=1,        //!< Histogram
      evolution_el=2,     //!< Statistic evolution (EL)
      evolution_var=3,    //!< Statistic evolution (VaR)
      evolution_es=4,     //!< Statistic evolution (ES)
      contribution_el=5,  //!< Segmentation pie chart (EL)
      contribution_es=6   //!< Segmentation pie chart (ES)
    };

    // status types
    enum class status
    {
      reading=1,   //!< Readind data
      running=2,   //!< Doing data analysis
      stopped=3,   //!< Task stopped by user
      failed=4,    //!< Task failes
      finished=5   //!< Task finished
    };

  private:

    //! Stopped task exception
    class StopException : public ccruncher::Exception { };

  private:

    //! Csv file
    ccruncher::CsvFile csv;
    //! Segment index (-1 means rowSums)
    int isegment;
    //! Number of bins (hist)
    size_t numbins;
    //! Percentile (var, es)
    double percentile;
    //! Histogram (result)
    gsl_histogram *hist;
    //! Computed statistics (result)
    std::vector<statval> statvals;
    //! Computed contributions (result)
    std::vector<contrib> contribs;
    //! Progress (0..100)
    volatile float progress;
    //! Execution mode
    mode mode_;
    //! Execution status
    status status_;
    //! Stop flag
    bool stop_;
    //! Error message error (if any)
    std::string msgerr;

  private:

    //! Value at Risk
    static statval valueAtRisk(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);
    //! Expected shortfall
    static statval expectedShortfall(double percentile, std::vector<double>::iterator first, std::vector<double>::iterator last);

    //! Read csv data
    void readData(int col, std::vector<double> &ret);
    //! Read csv data
    void readData(std::vector<std::vector<double>> &ret);
    //! Set status
    void setStatus(status);
    //! Compute histogram
    void runHistogram(const std::vector<double> &);
    //! Compute EL
    void runEvolutionEL(const std::vector<double> &);
    //! Compute VaR
    void runEvolutionVAR(std::vector<double> &);
    //! Compute ES
    void runEvolutionES(std::vector<double> &);
    //! Contribution EL
    void runContributionEL(const std::vector<std::vector<double>> &);
    //! Contribution ES
    void runContributionES(std::vector<std::vector<double>> &);

  public:

    //! Constructor
    AnalysisTask();
    //! Destructor
    virtual ~AnalysisTask() override;
    //! Set csv filename
    void setFilename(const QString &);
    //! Set data
    void setData(mode m, int isegment=-1, double param=0.0);
    //! Return mode
    mode getMode() const;
    //! Return csvfile object
    ccruncher::CsvFile& getCsvFile();
    //! Return histogram
    const gsl_histogram *getHistogram() const;
    //! Return statvals
    const std::vector<statval>& getStatVals() const;
    //! Return contributions
    const std::vector<contrib>& getContributions() const;
    //! Thread task
    virtual void run() override;
    //! Stop current execution
    void stop();
    //! Return status
    status getStatus() const;
    //! Return progress
    float getProgress() const;
    //! Return error
    const std::string &getMsgErr() const;

  signals:

    //! Status updated
    void statusChanged(ccruncher_gui::AnalysisTask::status);

};

} // namespace

Q_DECLARE_METATYPE(ccruncher_gui::AnalysisTask::status)
