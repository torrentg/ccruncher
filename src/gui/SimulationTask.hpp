
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <map>
#include <QThread>
#include <QMetaType>
#include "utils/Logger.hpp"

// forward declaration
namespace ccruncher {
  class MonteCarlo;
  class XmlInputData;
}

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief Thread that manages the Monte Carlo simulation.
 */
class SimulationTask : public QThread
{
    Q_OBJECT
    Q_ENUMS(ccruncher_gui::SimulationTask::status)

  public:

    //! Status types
    enum class status
    {
      reading=1,    //!< Reading input file
      simulating=2, //!< Doing Monte Carlo
      stopped=3,    //!< Stopped by user
      failed=4,     //!< Execution failed
      finished=5    //!< Execution finished
    };

  private:

    //! Total number of running simulations
    static size_t num_running_sims;

    //! Logger
    ccruncher::Logger logger;
    //! Input filename
    std::string ifile;
    //! Output directory
    std::string odir;
    //! Defines
    std::map<std::string,std::string> defines;
    //! Output file creation mode
    char fmode;
    //! Number of threads
    unsigned char ithreads;
    //! Input data
    ccruncher::XmlInputData *idata;
    //! Simulator
    ccruncher::MonteCarlo *montecarlo;
    //! Status
    volatile status status_;
    //! Stop variable
    bool stop_;

  private:

    //! Set status
    void setStatus(status);
    //! Delete montecarlo object
    void deletemc();

  public:

    //! Constructor
    SimulationTask(std::streambuf *s=nullptr);
    //! Destructor
    virtual ~SimulationTask() override;
    //! Set streambuf
    void setStreamBuf(std::streambuf *);
    //! Set data info
    void setData(const std::string &, const std::map<std::string,std::string> &, const std::string &, unsigned char);
    //! Thread task
    virtual void run() override;
    //! Stop current execution
    void stop();
    //! Return status
    status getStatus() const;
    //! Return idata
    ccruncher::XmlInputData* getIData();
    //! Return MonteCarlo
    ccruncher::MonteCarlo* getMonteCarlo();
    //! Free memory (1=idata, 2=montecarlo, other=all)
    void free(int obj=0);
    //! Return logger
    ccruncher::Logger& getLogger();
    //! Check conflicts
    bool checkConflicts();

    //! Number of running simulations
    static size_t getNumRunningSims();

  signals:

    //! Status updated
    void statusChanged(ccruncher_gui::SimulationTask::status);

};

} // namespace

Q_DECLARE_METATYPE(ccruncher_gui::SimulationTask::status)
