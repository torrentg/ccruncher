#ifndef _SimulationTask_
#define _SimulationTask_

#include <string>
#include <map>
#include <QThread>
#include "kernel/IData.hpp"
#include "kernel/MonteCarlo.hpp"
#include "utils/Logger.hpp"

using namespace std;
using namespace ccruncher;

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
    Logger log;
    // input filename
    string ifile;
    // output directory
    string odir;
    // defines
    map<string,string> defines;
    // data
    IData *idata;
    // simulator
    MonteCarlo *montecarlo;
    // status
    status status_;
    // stop flag
    bool stop_;

  private:

    // set status
    void setStatus(status);
    // delete montecarlo object
    void deletemc();

  public:

    // constructor
    SimulationTask(streambuf *s=NULL);
    // destructor
    ~SimulationTask();
    // set streambuf
    void setStreamBuf(streambuf *);
    // set data info
    void setData(const string &, const map<string,string> &, const string &);
    // task
    void run();
    // stop current execution
    void stop();
    // return status
    status getStatus() const;
    // return idata
    IData* getIData();
    // return MonteCarlo
    MonteCarlo* getMonteCarlo();
    // free memory (1=idata, 2=montecarlo, other=all)
    void free(int obj=0);
    // return logger
    Logger& getLogger();

  signals:

    // status updated
    void statusChanged(int);

};

#endif

