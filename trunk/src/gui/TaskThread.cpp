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

    // tracing some execution info
    Logger::trace("general information", '*');
    Logger::newIndentLevel();
    Logger::trace("ccruncher version", string(PACKAGE_VERSION)+" ("+string(SVN_VERSION)+")");
    Logger::trace("start time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
    Logger::trace("number of threads", Format::toString(Utils::getNumCores()));
    Logger::previousIndentLevel();

    // parsing input file
    setStatus(parsing);
    idata = new IData(ifile, defines, &stop_);

    // creating simulation object
    setStatus(simulating);
    montecarlo = new MonteCarlo();
    montecarlo->setFilePath(odir, true);
    montecarlo->setHash(1000);
    montecarlo->setNumThreads(Utils::getNumCores());
    montecarlo->setData(*idata);
    delete idata;

    // simulating
    montecarlo->run(&stop_);

    // tracing some execution info
    Logger::trace("general information", '*');
    Logger::newIndentLevel();
    Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
    Logger::trace("total elapsed time", timer);
    Logger::trace("simulations realized", Format::toString(montecarlo->getNumIterations()));
    Logger::previousIndentLevel();
    Logger::addBlankLine();

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

