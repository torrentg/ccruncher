#include "gui/TaskThread.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"

//===========================================================================
// constructor
//===========================================================================
TaskThread::TaskThread() : QThread(), montecarlo(NULL)
{
  ifile = "";
  odir = "";
  setStatus(inactive);
  connect(this, SIGNAL(terminated()), this, SLOT(setTerminated()));
}

//===========================================================================
// destructor
//===========================================================================
TaskThread::~TaskThread()
{
  deletemc();
}

//===========================================================================
// deletemc
//===========================================================================
void TaskThread::deletemc()
{
  if (montecarlo != NULL) {
    delete montecarlo;
    montecarlo = NULL;
  }
}

//===========================================================================
// run
//===========================================================================
void TaskThread::run()
{
  IData *idata = NULL;
  if (montecarlo != NULL) return;

  Timer timer(true);

  try
  {
    stop_ = false;

    // copyright info
    cout << Utils::copyright() << endl;
    Logger::header();

    // parsing input file
    setStatus(parsing);
    idata = new IData(ifile, defines, &stop_);

    // creating simulation object
    setStatus(simulating);
    montecarlo = new MonteCarlo();
    montecarlo->setFilePath(odir, true);
    montecarlo->setData(*idata);
    delete idata;

    // simulating
    montecarlo->run(Utils::getNumCores(), 1000, &stop_);

    // tracing some execution info
    Logger::footer(timer);

    setStatus(finished);
  }
  catch(std::exception &e)
  {
    cout << endl;
    cout << e.what() << endl;
    setStatus(failed);
    if (idata != NULL) delete idata;
  }

  deletemc();
}

//===========================================================================
// run
//===========================================================================
void TaskThread::stop()
{
  stop_ = true;
}

//===========================================================================
// set data info
//===========================================================================
void TaskThread::setData(const string &f_, const map<string,string> &m_, const string &d_)
{
  ifile = f_;
  defines = m_;
  odir = d_;
}

//===========================================================================
// setStatus
//===========================================================================
void TaskThread::setStatus(status s)
{
  status_ = s;
  emit statusChanged((int)s);
}

//===========================================================================
// setTerminated
//===========================================================================
void TaskThread::setTerminated()
{
  setStatus(aborted);
}

//===========================================================================
// return status
//===========================================================================
TaskThread::status TaskThread::getStatus() const
{
  return status_;
}

//===========================================================================
// return progress
//===========================================================================
int TaskThread::getProgress()
{
  switch(status_)
  {
    case inactive:
      return 0;
    case parsing:
      return -1;
    case simulating:
      return (int)(100.0*((float)montecarlo->getNumIterations()/(float)montecarlo->getMaxIterations()));
    case failed:
    case aborted:
    case finished:
      return 100;
    default:
      assert(false);
      return 0;
  }
}

