#ifndef _SimulationTask_
#define _SimulationTask_

#include <string>
#include <map>
#include <QThread>
#include "kernel/IData.hpp"
#include "kernel/MonteCarlo.hpp"
#include "utils/Logger.hpp"

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
    void setData(const std::string &, const std::map<std::string,std::string> &, const std::string &);
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

#endif

