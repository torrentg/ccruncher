
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
#include "utils/ccmpi.h"
#include <cassert>

//---------------------------------------------------------------------------

void startup(int argc, char *argv[]) throw(Exception);
int shutdown(int retcode);
void usage();
void version();
void copyright();
void setnice(int) throw(Exception);
void run(string, string) throw(Exception);
void catchsignal(int signal);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
bool bvalidate = false;
bool bverbose = false;
bool bforce = false;
int inice = -999;
int ihash = 0;
MonteCarlo *mcref = NULL;
#ifndef USE_MPI
bool blcopulas = false;
bool bldeftime = false;
#endif

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
      { "validate",     0,  NULL,  305 },
#ifndef USE_MPI
      { "lcopulas",     0,  NULL,  306 },
      { "ldeftime",     0,  NULL,  307 },
#endif
      { NULL,           0,  NULL,   0  }
  };

  // initialization routines
  try
  {
    startup(argc, argv);
  }
  catch(Exception &e)
  {
    cerr << e << endl;
    return shutdown(1);
  }
  catch(...)
  {
    cerr << "unknow error" << endl;
    return shutdown(1);
  }

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
          return shutdown(1);

      case 'h': // -h or --help (show help and exit)
          usage();
          return shutdown(0);

      case 'v': // -v (be verbose)
          bverbose = true;
          break;

      case 'f': // -f (force overwriting)
          bforce = true;
          break;

      case 301: // --version (show version and exit)
          version();
          return shutdown(0);

      case 302: // --path=dir (set output files path)
          spath = string(optarg);
          break;

      case 303: // --nice=val (set nice value)
          try
          {
            string snice = string(optarg);
            inice = Parser::intValue(snice);
          }
          catch(Exception &e)
          {
            cerr << "invalid nice value" << endl;
            return shutdown(1);
          }
          break;

      case 304: // --hash=val (set hash value)
          try
          {
            string shash = string(optarg);
            ihash = Parser::intValue(shash);
          }
          catch(Exception &e)
          {
            cerr << "invalid hash value" << endl;
            return shutdown(1);
          }
          break;

      case 305: // --validate (validate input file)
          bvalidate = true;
          break;

#ifndef USE_MPI
      case 306: // --lcopulas (list copula values)
          blcopulas = true;
          break;

      case 307: // --ldeftime (list default times)
          bldeftime = true;
          break;
#endif

      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "ccruncher version and arguments at gtorrent@ccruncher.net\n" << endl;
          return shutdown(1);
    }
  }

  // retrieving input filename
  if (argc == optind)
  {
    cerr << "xml input file not specified" << endl;
    cerr << "use --help option for more information" << endl;
    return shutdown(1);
  }
  else if (argc - optind > 1)
  {
    cerr << "last argument will be the xml input file" << endl;
    cerr << "use --help option for more information" << endl;
    return shutdown(1);
  }
  else
  {
    sfilename = string(argv[argc-1]);
  }

  // checking arguments consistency
  if (spath == "" && !bvalidate)
  {
    cerr << "--path is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
    return shutdown(1);
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
    run(sfilename, spath);
  }
  catch(Exception &e)
  {
    cerr << endl << e << endl;
    return shutdown(1);
  }
  catch(...)
  {
    cerr << "uncatched exception. please report this bug sending input file, \n"
            "ccruncher version and arguments at gtorrent@ccruncher.net\n" << endl;
    return shutdown(1);
  }

  // exit function
  return shutdown(0);
}

//===========================================================================
// startup
//===========================================================================
void startup(int argc, char *argv[]) throw(Exception)
{
#ifdef USE_MPI
  // start up MPI
  MPI::Init(argc, argv);

  // setting slaves output to /dev/null
  if (!Utils::isMaster())
  {
    Utils::setSilentMode();
  }

  // checking number of nodes (minimum required = 2 nodes)
  if (MPI::COMM_WORLD.Get_size() <= 1)
  {
    throw Exception("needed more than 1 cluster node to run ccruncher");
  }
#else
  // nothing to do
  assert(argc > 0);
  assert(argv != NULL);
#endif
}

//===========================================================================
// shutdown
//===========================================================================
int shutdown(int retcode)
{
#ifdef USE_MPI
  try {
    // shutdown MPI
    MPI::Finalize();
  }
  catch(...) {
    // nothing to do
  }
#else
  // nothing to do
#endif

  // allways returns the same code
  return retcode;
}

//===========================================================================
// run
//===========================================================================
void run(string filename, string path) throw(Exception)
{
  Timer timer(true);

  // checking input file readeability
  File::checkFile(filename, "r");

  // checking output directory
  if (bvalidate == false && !File::existDir(path))
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
  Logger::trace("ccruncher version", string(VERSION)+" ("+string(SVNVERSION)+")");
  Logger::trace("start time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("input file", filename);
#ifdef USE_MPI
  Logger::trace("number of nodes in cluster", Format::int2string(MPI::COMM_WORLD.Get_size()));
#endif
  Logger::previousIndentLevel();

  // parsing input file
  IData idata(filename);

  // creating simulation object
  MonteCarlo simul;
  simul.setFilePath(path, bforce);
  simul.setHash(ihash);
#ifndef USE_MPI
  simul.setAdditionalOutput(blcopulas, bldeftime);
#endif

  // initializing simulation
  simul.initialize(idata, bvalidate);

  // validate file and exit
  if (bvalidate == true)
  {
    Logger::addBlankLine();
    return;
  }

  // setting interruptions handlers
  mcref = &simul;
  signal(SIGINT, catchsignal);
  signal(SIGABRT, catchsignal);
  signal(SIGTERM, catchsignal);

  // running simulation
  long nsims = simul.execute();

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("total elapsed time", timer);
  Logger::trace("simulations realized", Format::long2string(nsims));
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
    throw Exception("nice value out of range [" + Format::int2string(PRIO_MIN) +
                    ".." + Format::int2string(PRIO_MAX) + "]");
  }
  else
  {
    if(setpriority(PRIO_PROCESS, 0, niceval) != 0)
    {
      string msg = Format::int2string(errno);
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
  cout << "ccruncher-" << VERSION << " (" << SVNVERSION << ")" << endl;
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
  "    ccruncher is a tool used to evalute VAR (value at risk)\n"
  "    of a pure credit portfolio using Monte Carlo simulation.\n"
  "    more info at http://www.ccruncher.net\n"
  "  arguments:\n"
  "    file        xml file containing the problem to be solved. This\n"
  "                file can be gziped (caution, zip format not suported)\n"
  "  options:\n"
  "    -f          force output files overwrite\n"
  "    -v          be verbose\n"
  "    --path=dir  directory where output files will be placed (required)\n"
  "    --nice=num  set nice priority to num\n"
  "    --hash=num  print '.' for each num simulations (default=0)\n"
  "    --validate  validates input file and exits\n"
#ifndef USE_MPI
  "    --lcopulas  list simulated copula values\n"
  "                for depuration and validation purposes only.\n"
  "                use with care. time and disk consuming option.\n"
  "                creates the file copula.csv\n"
  "    --ldeftime  list simulated borrower default times\n"
  "                for depuration and validation purposes only.\n"
  "                use with care. time and disk consuming option.\n"
  "                creates the file deftimes.csv\n"
#endif
  "    --help -h   show this message and exit\n"
  "    --version   show version and exit\n"
  "  return codes:\n"
  "    0           OK. finished without errors\n"
  "    1           KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher --validate samples/sample01.xml\n"
  "    ccruncher -f --path=data/sample01 samples/sample01.xml\n"
  "    ccruncher -fv --hash=100 --path=data/test01 samples/test01.xml\n"
  "    ccruncher -fv --hash=100 --path=data/test100 samples/test100.xml.gz\n"
#ifndef USE_MPI
  "    ccruncher -fv --hash=100 --ldeftime --path=data/test04 samples/test04.xml\n"
#endif
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "   ccruncher is Copyright (C) 2003-2010 Gerard Torrent and licensed\n"
  "     under the GNU General Public License, version 2. more info at\n"
  "                   http://www.ccruncher.net\n"
  << endl;
}

