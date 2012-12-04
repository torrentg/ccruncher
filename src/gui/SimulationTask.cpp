#include <QFileInfo>
#include <QMessageBox>
#include "gui/SimulationTask.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include "utils/CsvFile.hpp"

size_t SimulationTask::num_running_sims = 0;

//===========================================================================
// constructor
//===========================================================================
SimulationTask::SimulationTask(streambuf *s) : QThread(), log(s),
    idata(NULL), montecarlo(NULL)
{
  ifile = "";
  odir = "";
  status_ = finished;
  fmode = 'w';
  setTerminationEnabled(false);
}

//===========================================================================
// destructor
//===========================================================================
SimulationTask::~SimulationTask()
{
  if (isRunning()) {
    stop();
    wait(500);
  }
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
    montecarlo->setFilePath(odir, fmode);
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
  catch(...)
  {
    log << indent(-100) << endl;
    log << "panic: unknow error" << endl;
    setStatus(failed);
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

  switch(status_)
  {
    case reading:
      num_running_sims++;
      break;
    case simulating:
      break;
    case stopped:
    case failed:
    case finished:
      assert(num_running_sims > 0);
      num_running_sims--;
      break;
    default:
      assert(false);
  }

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

//===========================================================================
// number of running simulations
//===========================================================================
size_t SimulationTask::getNumRunningSims()
{
  return num_running_sims;
}

//===========================================================================
// check for conflicts
// return true = continue, false = stop
//===========================================================================
bool SimulationTask::checkConflicts()
{
  fmode = 'w';
  IData idata;

  try {
    idata.init(ifile, defines, NULL, false);
  }
  catch(...) {
    return false;
  }

  vector<string> nonfiles;
  vector<string> goodfiles;
  vector<string> badfiles;
  size_t numlines = 0;

  for(int i=0; i<idata.getSegmentations().size(); i++)
  {
    const Segmentation &segmentation = idata.getSegmentations().getSegmentation(i);
    string filename = segmentation.getFilename(odir);
    if (!QFileInfo(QString(filename.c_str())).exists()) {
      nonfiles.push_back(filename);
      continue;
    }
    try
    {
      CsvFile csv(filename);
      vector<string> headers = csv.getHeaders();
      size_t nlines = csv.getNumLines();
      if (numlines == 0) numlines = nlines;
      if (numlines != nlines) throw Exception("lines differ");
      if (segmentation.size() == 1) {
        if (headers.size() != 1) throw Exception("header differ");
        if (headers[0] != segmentation.name) throw Exception("header differ");
      }
      else {
        if (segmentation.size() < (int)headers.size()) throw Exception("header differ");
        if (segmentation.size()-headers.size() > 1) throw Exception("header differ");
        for(int j=1; j<segmentation.size(); j++) {
          if (segmentation.getSegment(j) != headers[j-1]) throw Exception("header differ");
        }
        if (segmentation.size() == (int)headers.size()) {
          if (headers.back() != "unassigned") throw Exception("header differ");
        }
      }
      goodfiles.push_back(filename);
    }
    catch(std::exception &e)
    {
      badfiles.push_back(filename);
    }
  }

  if (!nonfiles.empty() && badfiles.empty() && goodfiles.empty()) {
    // don't exist previous execution
    return true;
  }
  else if (nonfiles.empty() && badfiles.empty() && !goodfiles.empty()) {
    // previous execution found. append/overwrite/cancel dialog
    QMessageBox dialog(QMessageBox::Warning, "CCruncher",
                   "Found a previous execution in the output directory.\n"
                   "What we do with existent output files?");
    dialog.addButton("Append", QMessageBox::YesRole);
    dialog.addButton("Overwrite", QMessageBox::NoRole);
    dialog.addButton("Abort", QMessageBox::RejectRole);
    int rc = dialog.exec();

    if (rc == 0) { // append
      fmode = 'a';
      if (idata.getParams().rng_seed != 0) {
        QMessageBox::StandardButton val = QMessageBox::warning(NULL, tr("CCruncher"),
           tr("You indicated a RNG seed distinct than 0.\n"
           "This can cause repeated values.\n"
           "Are you sure to continue?"),
           QMessageBox::Yes | QMessageBox::No,
           QMessageBox::No);
        if (val == QMessageBox::Yes) return true;
        else return false;
      }
      else {
        return true;
      }
    }
    else if (rc == 1) { // overwrite
      fmode = 'w';
      return true;
    }
    else { // abort/esc
      return false;
    }
  }
  else
  {
    // previous execution found distinct than current
    fmode = 'w';
    QMessageBox::StandardButton rc = QMessageBox::warning(NULL, "CCruncher",
        "Existing files in the output directory will be overwrited.\n"
        "Are you sure to continue?",
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No);
    if (rc == QMessageBox::Yes) return true;
    else return false;
  }
}

