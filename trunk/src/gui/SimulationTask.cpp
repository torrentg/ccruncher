#include "gui/SimulationTask.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"

//===========================================================================
// constructor
//===========================================================================
SimulationTask::SimulationTask(streambuf *s) : QThread(), log(s),
    idata(NULL), montecarlo(NULL)
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
  free();
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
  Timer timer(true);

  free();

  try
  {
    stop_ = false;

    // header
    log << header << endl;

    // parsing input file
    setStatus(reading);
    idata = new IData(log.rdbuf());
    idata->init(ifile, defines, &stop_);

    // creating simulation object
    montecarlo = new MonteCarlo(log.rdbuf());
    montecarlo->setFilePath(odir, true);
    montecarlo->setData(*idata);

    // simulating
    setStatus(simulating);
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
  }
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
// return idata
//===========================================================================
IData* SimulationTask::getIData()
{
  return idata;
}

//===========================================================================
// return MonteCarlo
//===========================================================================
MonteCarlo* SimulationTask::getMonteCarlo()
{
  return  montecarlo;
}

//===========================================================================
// return logger
//===========================================================================
Logger& SimulationTask::getLogger()
{
  return log;
}

//===========================================================================
// free memory (1=idata, 2=montecarlo, other=all)
//===========================================================================
void SimulationTask::free(int obj)
{
  if (obj != 2 && idata != NULL) {
    delete idata;
    idata = NULL;
  }
  if (obj != 1 && montecarlo != NULL) {
    delete montecarlo;
    montecarlo = NULL;
  }
}

