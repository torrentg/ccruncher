#include "gui/SimulationTask.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"

//===========================================================================
// constructor
//===========================================================================
SimulationTask::SimulationTask(streambuf *s) : QThread(), log(s), montecarlo(NULL)
{
  ifile = "";
  odir = "";
  setStatus(finished);
  setTerminationEnabled(false);
}

//===========================================================================
// destructor
//===========================================================================
SimulationTask::~SimulationTask()
{
  deletemc();
}

//===========================================================================
// deletemc
//===========================================================================
void SimulationTask::deletemc()
{
  if (montecarlo != NULL) {
    delete montecarlo;
    montecarlo = NULL;
  }
}

//===========================================================================
// set streambuf
//===========================================================================
void SimulationTask::setStreamBuf(streambuf *s)
{
  log.rdbuf(s);
}

//===========================================================================
// run
//===========================================================================
void SimulationTask::run()
{
  IData *idata = NULL;
  if (montecarlo != NULL) return;

  Timer timer(true);

  try
  {
    stop_ = false;

    // header
    log << Utils::copyright() << endl;
    log << header << endl;

    // parsing input file
    setStatus(reading);
    idata = new IData(log.rdbuf());
    idata->init(ifile, defines, &stop_);

    // creating simulation object
    setStatus(simulating);
    montecarlo = new MonteCarlo(log.rdbuf());
    montecarlo->setFilePath(odir, true);
    montecarlo->setData(*idata);
    delete idata;

    // simulating
    montecarlo->run(Utils::getNumCores(), 0, &stop_);

    // footer
    log << footer(timer) << endl;
    setStatus(finished);
  }
  catch(std::exception &e)
  {
    log << indent(-100) << endl;
    log << e.what() << endl;
    if (stop_) setStatus(stopped);
    else setStatus(failed);
    if (idata != NULL) delete idata;
  }

  deletemc();
}

//===========================================================================
// stop
//===========================================================================
void SimulationTask::stop()
{
  stop_ = true;
}

//===========================================================================
// set data info
//===========================================================================
void SimulationTask::setData(const string &f_, const map<string,string> &m_, const string &d_)
{
  ifile = f_;
  defines = m_;
  odir = d_;
}

//===========================================================================
// setStatus
//===========================================================================
void SimulationTask::setStatus(status s)
{
  status_ = s;
  emit statusChanged((int)s);
}

//===========================================================================
// return status
//===========================================================================
SimulationTask::status SimulationTask::getStatus() const
{
  return status_;
}

//===========================================================================
// return progress
//===========================================================================
float SimulationTask::getProgress()
{
  switch(status_)
  {
    case reading:
      return 0.0;
    case simulating:
      return 100.0*((float)montecarlo->getNumIterations()/(float)montecarlo->getMaxIterations());
    case stopped:
    case failed:
    case finished:
      return 100.0;
    default:
      assert(false);
      return 0.0;
  }
}

