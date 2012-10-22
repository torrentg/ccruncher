
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "utils/config.h"

#ifndef _WIN32
  #include <sys/resource.h>
#endif

#ifndef _MSC_VER
  #include <getopt.h>
#endif

#include <cerrno>
#include <iostream>
#include <csignal>
#include <map>
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define MAX_NUM_THREADS 16

//---------------------------------------------------------------------------

void usage();
void setnice(int) throw(Exception);
void run(const string &, const string &, int) throw(Exception);
void catchsignal(int signal);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
bool bverbose = true;
bool bforce = false;
bool btrace = false;
int inice = -999;
int ihash = 0;
int ithreads = 1;
map<string,string> defines;
MonteCarlo *mcref = NULL;

//===========================================================================
// catchsignal
//===========================================================================
void catchsignal(int signal)
{
  UNUSED(signal);
  
  if (mcref != NULL) 
  {
    if (mcref->isRunning()) 
    {
      mcref->abort();
    }
    else
    {
      exit(1);
    }
  }
} 

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
  // short options
  const char* const options1 = "hqfD:" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  NULL,  'h' },
      { "version",      0,  NULL,  301 },
      { "path",         1,  NULL,  302 },
      { "nice",         1,  NULL,  303 },
      { "hash",         1,  NULL,  304 },
      { "threads",      1,  NULL,  305 },
      { "trace",        0,  NULL,  306 },
      { NULL,           0,  NULL,   0  }
  };

  // parsing options
  while (1)
  {
    int curropt = getopt_long (argc, argv, options1, options2, NULL);

    if (curropt == -1)
    {
      // no more options. exit while
      break;
    }

    switch(curropt)
    {
      case '?': // invalid option
          cerr << "error parsing arguments" << endl;
          cerr << "use --help option for more information" << endl;
          return 1;

      case 'h': // -h or --help (show help and exit)
          usage();
          return 0;

      case 'q': // -q (be quiet)
          bverbose = false;
          break;

      case 'f': // -f (force overwriting)
          bforce = true;
          break;

      case 'D': // -D key=val (define)
          {
            string str(optarg);
            size_t pos = str.find('=');
            if (pos == string::npos || pos == 0 || pos == str.length()-1 ) {
              cerr << "error parsing arguments (define with invalid format)" << endl;
              cerr << "use --help option for more information" << endl;
              return 1;
            }
            string key = Strings::trim(str.substr(0, pos));
            string value = Strings::trim(str.substr(pos+1));
            if (key.length() == 0 || value.length() == 0) {
              cerr << "error parsing arguments (define with invalid format)" << endl;
              cerr << "use --help option for more information" << endl;
              return 1;
            }
            else {
              defines[key]= value;
            }
          }
          break;

      case 301: // --version (show version and exit)
          cout << Utils::version() << endl;
          return 0;

      case 302: // --path=dir (set output files path)
          spath = string(optarg);
          break;

      case 303: // --nice=val (set nice value)
          try
          {
            string snice = string(optarg);
            inice = Parser::intValue(snice);
          }
          catch(Exception &)
          {
            cerr << "invalid nice value" << endl;
            return 1;
          }
          break;

      case 304: // --hash=val (set hash value)
          try
          {
            string shash = string(optarg);
            ihash = Parser::intValue(shash);
          }
          catch(Exception &)
          {
            cerr << "invalid hash value" << endl;
            return 1;
          }
          break;

      case 305: // --threads=val (set number of threads)
          try
          {
            string sthreads = string(optarg);
            ithreads = Parser::intValue(sthreads);
          }
          catch(Exception &)
          {
            cerr << "invalid threads value" << endl;
            return 1;
          }
          break;

      case 306: // --trace (trace copula values to file copula.csv)
          btrace = true;
          break;

      default: // unexpected error
          cerr << 
                  "unexpected error parsing arguments. Please report this bug sending input\n"
                  "file, ccruncher version and arguments at gtorrent@ccruncher.net\n" << endl;
          return 1;
    }
  }

  // retrieving input filename
  if (argc == optind)
  {
    sfilename = STDIN_FILENAME;
  }
  else if (argc - optind > 1)
  {
    cerr << "last argument will be the xml input file" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  else
  {
    try
    {
      sfilename = string(argv[argc-1]);
      File::checkFile(sfilename, "r");
    }
    catch(Exception &) 
    {
      cerr << "can't open file " << sfilename << endl;
      return 1;
    }
  }

  // checking arguments consistency
  if (spath == "")
  {
    cerr << "--path is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  if (ithreads <= 0 || MAX_NUM_THREADS < ithreads)
  {
    cerr << "error: invalid number of threads" << endl;
    return 1;
  }

  // license info
  if (bverbose)
  {
    cout << endl << Utils::copyright() << endl;
  }

  try
  {
    // setting new nice value (modify scheduling priority)
    if (inice != -999) {
      setnice(inice);
    }

    // running simulation
    run(sfilename, spath, ithreads);
  }
  catch(Exception &e)
  {
    cerr << endl << e << endl;
    return 1;
  }
  catch(...)
  {
    cerr << "uncatched exception. please report this bug sending input file, \n"
            "ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
    return 1;
  }

  // exit function
  return 0;
}

//===========================================================================
// run
//===========================================================================
void run(const string &filename, const string &path, int nthreads) throw(Exception)
{
  Timer timer(true);
  
  // checking output directory
  if (!File::existDir(path))
  {
    if (bforce == false)
    {
      throw Exception("path " + path + " not exist");
    }
    else
    {
      File::makeDir(path);
    }
  }

  // initializing logger
  Logger::setVerbosity(bverbose?1:0);

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("ccruncher version", string(PACKAGE_VERSION)+" ("+string(SVN_VERSION)+")");
  Logger::trace("start time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("number of threads", Format::toString(nthreads));
  Logger::previousIndentLevel();

  // parsing input file
  IData *idata = new IData(filename, defines);

  // creating simulation object
  MonteCarlo montecarlo;
  montecarlo.setFilePath(path, bforce);
  montecarlo.setHash(ihash);
  montecarlo.setTrace(btrace);
  montecarlo.setNumThreads(nthreads);

  // initializing simulation
  montecarlo.setData(*idata);
  delete idata;

  // setting interruptions handlers
  mcref = &montecarlo;
  signal(SIGINT, catchsignal);
  signal(SIGABRT, catchsignal);
  signal(SIGTERM, catchsignal);

  // running simulation
  montecarlo.run();

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("total elapsed time", timer);
  Logger::trace("simulations realized", Format::toString(montecarlo.getNumIterations()));
  Logger::previousIndentLevel();
  Logger::addBlankLine();
}

//===========================================================================
// setnice
//===========================================================================
void setnice(int niceval) throw(Exception)
{
#if !defined(_WIN32)
  if (niceval < PRIO_MIN || niceval > PRIO_MAX)
  {
    throw Exception("nice value out of range [" + Format::toString(PRIO_MIN) +
                    ".." + Format::toString(PRIO_MAX) + "]");
  }
  else
  {
    if(setpriority(PRIO_PROCESS, 0, niceval) != 0)
    {
      string msg = Format::toString(errno);
      msg = (errno==ESRCH?"ESRCH":msg);
      msg = (errno==EINVAL?"EINVAL":msg);
      msg = (errno==EPERM?"EPERM":msg);
      msg = (errno==EACCES?"EACCES":msg);
      throw Exception("error changing process priority [" + msg + "]");
    }
  }
#endif
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout <<
  "  usage:\n"
  "    ccruncher [options] <file>\n"
  "  description:\n"
  "    ccruncher is a tool used to simulate a portfolio of credits using\n"
  "    the Monte Carlo method. More info at http://www.ccruncher.net\n"
  "  arguments:\n"
  "    file           xml file containing the problem to be solved. This\n"
  "                   file can be gziped (caution, zip format not suported).\n"
  "                   You can omit this parameter an enter data from stdin.\n"
  "  options:\n"
  "    -f             force output files overwrite\n"
  "    -q             quiet mode, non-verbose\n"
  "    -D key=val     declare a define named key with value val\n"
  "    --path=dir     directory where output files will be placed (required)\n"
  "    --nice=num     set nice priority to num (optional)\n"
  "    --threads=num  number of threads (default=1)\n"
  "    --hash=num     print '.' for each num simulations (default=0)\n"
  "    --help -h      show this message and exit\n"
  "    --version      show version and exit\n"
  "  return codes:\n"
  "    0              OK. finished without errors\n"
  "    1              KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher -qf --path=data/sample01 samples/sample01.xml\n"
  "    ccruncher -f --hash=1000 --path=data/ samples/test100.xml\n"
  "    ccruncher -f --hash=1000 --path=data/ --threads=4 -D ndf=8 samples/sample.xml\n"
  << endl;
}

