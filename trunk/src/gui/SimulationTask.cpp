
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

#include <QFileInfo>
#include <QMessageBox>
#include "gui/SimulationTask.hpp"
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/Utils.hpp"
#include "utils/Timer.hpp"
#include "utils/CsvFile.hpp"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;

size_t ccruncher_gui::SimulationTask::num_running_sims = 0;

/**************************************************************************//**
 * @param[in] s CCruncher execution trace destination.
 */
ccruncher_gui::SimulationTask::SimulationTask(streambuf *s) : QThread(), log(s),
    idata(nullptr), montecarlo(nullptr)
{
  ifile = "";
  odir = "";
  status_ = finished;
  fmode = 'w';
  stop_ = false;
  indexes = false;
  ithreads = 0;
  setTerminationEnabled(false);
}

/**************************************************************************/
ccruncher_gui::SimulationTask::~SimulationTask()
{
  if (isRunning()) {
    stop();
    wait(500);
  }
  free();
}

/**************************************************************************//**
 * @param[in] s CCruncher execution trace destination.
 */
void ccruncher_gui::SimulationTask::setStreamBuf(std::streambuf *s)
{
  log.rdbuf(s);
}

/**************************************************************************//**
 * @details Execute CCruncher.
 */
void ccruncher_gui::SimulationTask::run()
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
    montecarlo->setFilePath(odir, fmode, indexes);
    montecarlo->setData(*idata);

    // simulating
    setStatus(simulating);
    montecarlo->run(ithreads, 0, &stop_);

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

/**************************************************************************/
void ccruncher_gui::SimulationTask::stop()
{
  stop_ = true;
}

/**************************************************************************//**
 * @param[in] f_ CCruncher input file.
 * @param[in] m_ List of defines.
 * @param[in] d_ Output directory.
 * @param[in] n Number of threads.
 * @param[in] i Create file indexes.csv?
 */
void ccruncher_gui::SimulationTask::setData(const std::string &f_,
  const std::map<std::string,std::string> &m_, const std::string &d_,
  unsigned char n, bool i)
{
  ifile = f_;
  defines = m_;
  odir = d_;
  indexes = i;
  ithreads = n;
  assert(ithreads > 0);
}

/**************************************************************************//**
 * @param[in] s New status.
 */
void ccruncher_gui::SimulationTask::setStatus(status s)
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

/**************************************************************************//**
 * @return Execution status.
 */
SimulationTask::status ccruncher_gui::SimulationTask::getStatus() const
{
  return status_;
}

/**************************************************************************//**
 * @return CCruncher's input data object.
 */
IData* ccruncher_gui::SimulationTask::getIData()
{
  return idata;
}

/**************************************************************************//**
 * @return CCruncher's Monte carlo object.
 */
MonteCarlo* ccruncher_gui::SimulationTask::getMonteCarlo()
{
  return  montecarlo;
}

/**************************************************************************//**
 * @return CCruncher's logger.
 */
Logger& ccruncher_gui::SimulationTask::getLogger()
{
  return log;
}

/**************************************************************************//**
 * @details If the input file is huge then memory can be a problem.
 * @param[in] obj Objects to deallocate (1=idata, 2=montecarlo, other=all).
 */
void ccruncher_gui::SimulationTask::free(int obj)
{
  if (obj != 2 && idata != nullptr) {
    delete idata;
    idata = nullptr;
  }
  if (obj != 1 && montecarlo != nullptr) {
    delete montecarlo;
    montecarlo = nullptr;
  }
}

/**************************************************************************//**
 * @return Number of current simultaneous running simulations.
 */
size_t ccruncher_gui::SimulationTask::getNumRunningSims()
{
  return num_running_sims;
}

/**************************************************************************//**
 * @details Checks for conflicts related with previous executions before
 *          to proceed to simule.
 * @return true = you can do the simulation, false = don't execute.
 */
bool ccruncher_gui::SimulationTask::checkConflicts()
{
  fmode = 'w';
  IData data;

  try {
    data.init(ifile, defines, nullptr, false);
  }
  catch(...) {
    // error in input file
    // nothing to check
    // errors will be reported by ccruncher execution
    return true;
  }

  vector<string> nonfiles;
  vector<string> goodfiles;
  vector<string> badfiles;
  size_t numlines = 0;

  for(int i=0; i<data.getSegmentations().size(); i++)
  {
    const Segmentation &segmentation = data.getSegmentations().getSegmentation(i);
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
        if (headers[0] != segmentation.getName()) throw Exception("header differ");
      }
      else {
        if (segmentation.size() < (int)headers.size()) throw Exception("header differ");
        int pos = -1;
        for(size_t j=0; j<headers.size(); j++) {
          if (headers[j] == "unassigned") {
            if (j != headers.size()-1) throw Exception("header differ");
          }
          else {
            bool found = false;
            for(int k=1; k<segmentation.size(); k++) {
              if (headers[j] == segmentation.getSegment(k)) {
                if (pos >= 0 && k <= pos) throw Exception("header differ");
                pos = k;
                found = true;
                break;
              }
            }
            if (!found) throw Exception("header differ");
          }
        }
      }
      goodfiles.push_back(filename);
    }
    catch(std::exception &)
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
                   "What we do with existing output files?");
    dialog.addButton("Append", QMessageBox::YesRole);
    dialog.addButton("Overwrite", QMessageBox::NoRole);
    dialog.addButton("Abort", QMessageBox::RejectRole);
    int rc = dialog.exec();

    if (rc == 0) { // append
      fmode = 'a';
      if (data.getParams().rng_seed != 0) {
        QMessageBox::StandardButton val = QMessageBox::warning(nullptr, tr("CCruncher"),
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
    QMessageBox::StandardButton rc = QMessageBox::warning(nullptr, "CCruncher",
        "Existing files in the output directory will be overwritten.\n"
        "Are you sure to continue?",
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No);
    if (rc == QMessageBox::Yes) return true;
    else return false;
  }
}

