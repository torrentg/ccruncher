#ifndef TASKTHREAD_HPP
#define TASKTHREAD_HPP

#include <string>
#include <map>
#include <QThread>
#include "kernel/IData.hpp"
#include "kernel/MonteCarlo.hpp"

using namespace std;
using namespace ccruncher;

class TaskThread : public QThread
{
    Q_OBJECT

  public:

    // recovery types
    enum status
    {
      inactive=1,
      parsing=2,
      simulating=3,
      failed=4,
      aborted=5,
      finished=6
    };

  private:

    // input filename
    string ifile;
    // output directory
    string odir;
    // defines
    map<string,string> defines;
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
    TaskThread();
    // destructor
    ~TaskThread();
    // set data info
    void setData(const string &, const map<string,string> &, const string &);
    // task
    void run();
    // stop current execution
    void stop();
    // return status
    status getStatus() const;
    // return progress
    int getProgress();

  public slots:

    // set aborted status
    void setTerminated();

  signals:

    // status updated
    void statusChanged(int);

};

#endif

