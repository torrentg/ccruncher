
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

#ifndef _SimulationTask_
#define _SimulationTask_

#include <string>
#include <map>
#include <QThread>
#include "utils/Logger.hpp"

// forward declaration
namespace ccruncher {
  class MonteCarlo;
  class IData;
}

namespace ccruncher_gui {

class SimulationTask : public QThread
{
    Q_OBJECT

  public:

    // status types
    enum status
    {
      reading=1,
      simulating=2,
      stopped=3,
      failed=4,
      finished=5
    };

  private:

    // logger
    ccruncher::Logger log;
    // input filename
    std::string ifile;
    // output directory
    std::string odir;
    // defines
    std::map<std::string,std::string> defines;
    // output file creation mode
    char fmode;
    // indexes creation flag
    bool indexes;
    // number of threads
    unsigned char ithreads;
    // data
    ccruncher::IData *idata;
    // simulator
    ccruncher::MonteCarlo *montecarlo;
    // status
    volatile status status_;
    // stop flag
    bool stop_;
    // number of running simulations
    static size_t num_running_sims;

  private:

    // set status
    void setStatus(status);
    // delete montecarlo object
    void deletemc();

  public:

    // constructor
    SimulationTask(std::streambuf *s=NULL);
    // destructor
    ~SimulationTask();
    // set streambuf
    void setStreamBuf(std::streambuf *);
    // set data info
    void setData(const std::string &, const std::map<std::string,std::string> &, const std::string &, unsigned char, bool );
    // task
    void run();
    // stop current execution
    void stop();
    // return status
    status getStatus() const;
    // return idata
    ccruncher::IData* getIData();
    // return MonteCarlo
    ccruncher::MonteCarlo* getMonteCarlo();
    // free memory (1=idata, 2=montecarlo, other=all)
    void free(int obj=0);
    // return logger
    ccruncher::Logger& getLogger();
    // number of running simulations
    static size_t getNumRunningSims();
    // check conflicts
    bool checkConflicts();

  signals:

    // status updated
    void statusChanged(int);

};

}

#endif

