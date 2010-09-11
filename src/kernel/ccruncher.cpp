
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _MSC_VER
  #include <sys/resource.h>
  #include <getopt.h>
#endif

#include <cerrno>
#include <iostream>
#include <csignal>
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include <cassert>

//---------------------------------------------------------------------------

#define MAX_NUM_THREADS 16

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void setnice(int) throw(Exception);
void run(string, string, int) throw(Exception);
void catchsignal(int signal);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
bool bverbose = false;
bool bforce = false;
bool btrace = false;
int inice = -999;
int ihash = 0;
int ithreads = 1;
MonteCarlo *mcref = NULL;

//===========================================================================
// catchsignal
//===========================================================================
void catchsignal(int signal)
{
  // flush files before close
  if (mcref != NULL) {
    mcref->release();
  }
  exit(1);
} 

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
  // short options
  const char* const options1 = "hvf" ;

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

      case 'v': // -v (be verbose)
          bverbose = true;
          break;

      case 'f': // -f (force overwriting)
          bforce = true;
          break;

      case 301: // --version (show version and exit)
          version();
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

      case 306: // --trace (trace copula values and default times)
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
    cerr << "xml input file not specified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  else if (argc - optind > 1)
  {
    cerr << "last argument will be the xml input file" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  else
  {
    sfilename = string(argv[argc-1]);
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
    copyright();
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
void run(string filename, string path, int nthreads) throw(Exception)
{
  Timer timer(true);

  // checking input file readeability
  File::checkFile(filename, "r");

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
  Logger::trace("input file", filename);
  Logger::trace("number of threads", Format::toString(nthreads));
  Logger::previousIndentLevel();

  // parsing input file
  IData idata(filename);

  // creating simulation object
  MonteCarlo montecarlo;
  montecarlo.setFilePath(path, bforce);
  montecarlo.setHash(ihash);
  montecarlo.trace(btrace);

  // initializing simulation
  montecarlo.initialize(idata);

  // setting interruptions handlers
  mcref = &montecarlo;
  signal(SIGINT, catchsignal);
  signal(SIGABRT, catchsignal);
  signal(SIGTERM, catchsignal);

  // running simulation
  int nsims = montecarlo.execute(nthreads);

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("total elapsed time", timer);
  Logger::trace("simulations realized", Format::toString(nsims));
  Logger::previousIndentLevel();
  Logger::addBlankLine();
}

//===========================================================================
// setnice
//===========================================================================
void setnice(int niceval) throw(Exception)
{
#if !defined(_MSC_VER) && !defined(__CYGWIN__) 
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
// version
//===========================================================================
void version()
{
  cout << "ccruncher-" << PACKAGE_VERSION << " (" << SVN_VERSION << ")" << endl;
  cout << "builded by " << BUILD_USER << "@" << BUILD_HOST << " at " << BUILD_DATE << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
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
  "                   file can be gziped (caution, zip format not suported)\n"
  "  options:\n"
  "    -f             force output files overwrite\n"
  "    -v             be verbose\n"
  "    --path=dir     directory where output files will be placed (required)\n"
  "    --nice=num     set nice priority to num (optional)\n"
  "    --threads=num  number of threads (default=1)\n"
  "    --hash=num     print '.' for each num simulations (default=0)\n"
  "    --trace        for debuging and validation purposes only!\n"
  "                   bulk simulated copula values to file copula.csv\n"
  "    --help -h      show this message and exit\n"
  "    --version      show version and exit\n"
  "  return codes:\n"
  "    0              OK. finished without errors\n"
  "    1              KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher -f --path=data/sample01 samples/sample01.xml\n"
  "    ccruncher -fv --hash=100 --threads=4 --path=data/ samples/test01.xml\n"
  "    ccruncher -fv --hash=100 --threads=4 --path=data/ samples/test100.xml\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "   ccruncher is Copyright (C) 2003-2010 Gerard Torrent and licensed\n"
  "     under the GNU General Public License, version 2. More info at\n"
  "                   http://www.ccruncher.net\n"
  << endl;
}

