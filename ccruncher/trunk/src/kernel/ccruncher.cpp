
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// ccruncher.cpp - ccruncher main code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added POSIX compliance in command line arguments
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added gziped input files suport
//
// 2005/07/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed mpi argument
//
// 2005/07/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added mpi support
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/07/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added trace info (input file name + begin/end time)
//
// 2005/07/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . don't apply nice if user don't put nice number
//   . added trace info (simulations realized)
//
// 2005/07/28 - Gerard Torrent [gerard@fobos.generacio.com]
//   . defined signal handlers (to caught Ctrl-C, kill's, etc.)
//
// 2005/08/06 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added getCompilationOptions() to version output
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed signal handlers
//
// 2005/09/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added version info at stdout
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . MonteCarlo refactoring
//
// 2007/07/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added listloss method
//
//===========================================================================

#include "utils/config.h"

#ifndef _MSC_VER
  #include <sys/resource.h>
  #include <getopt.h>
#endif

#include <cerrno>
#include <iostream>
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include "utils/ccmpi.h"

//---------------------------------------------------------------------------

void startup(int argc, char *argv[]) throw(Exception);
int shutdown(int retcode);
void usage();
void version();
void copyright();
void setnice(int) throw(Exception);
void run(string, string) throw(Exception);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
bool bvalidate = false;
bool blistloss = false;
bool bverbose = false;
bool bforce = false;
int inice = -999;
int ihash = 0;

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
      { "listloss",     0,  NULL,  306 },
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

      case 306: // --listloss (list precomputed asset losses at time nodes)
          blistloss = true;
          break;

      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "ccruncher version and arguments at gerard@mail.generacio.com\n" << endl;
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
  if (spath == "" && !bvalidate && !blistloss)
  {
    cerr << "--path is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
    return shutdown(1);
  }

  // license info
  if (bverbose || !blistloss)
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
            "ccruncher version and arguments at gerard@mail.generacio.com\n" << endl;
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
  Timer timer;
  timer.start();

  // checking input file readeability
  File::checkFile(filename, "r");

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
  IData idata = IData(filename);

  // creating simulation object
  MonteCarlo simul;
  simul.setFilePath(path, bforce);
  simul.setHash(ihash);

  // initializing simulation
  simul.initialize(idata);

  // validate file and exit
  if (bvalidate == true)
  {
    Logger::addBlankLine();
    return;
  }

  // list precomputed losses and exit
  if (blistloss == true)
  {
    simul.printPrecomputedLosses();
    return;
  }

  // checking output dir
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

  // running simulation
  long nsims = simul.execute();
  double nseconds = timer.stop();

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("end time (dd/MM/yyyy hh:mm:ss)", Utils::timestamp());
  Logger::trace("total elapsed time", Timer::format(nseconds));
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
  cout << "\n"
  "  usage: ccruncher [options] <file>\n"
  "\n"
  "  description:\n"
  "    creditcruncher is a tool used to evalute VAR (value at risk)\n"
  "    of a pure credit portfolio using monte carlo techniques.\n"
  "    more info at http://www.generacio.com/ccruncher\n"
  "  arguments:\n"
  "    file        xml file containing the problem to be solved. This\n"
  "                file can be gziped (caution, zip format not suported)\n"
  "  options:\n"
  "    -f          force output files overwriting\n"
  "    -v          be more verbose\n"
  "    --path=dir  directory where output files will be placed (required)\n"
  "    --nice=num  set nice priority to num\n"
  "    --hash=num  print '.' for each num simulations (default=0)\n"
  "    --validate  perform input file validations and exit\n"
  "    --listloss  list precomputed assets losses at time nodes and exit\n"
  "    --help -h   show this message and exit\n"
  "    --version   show version and exit\n"
  "  return codes:\n"
  "    0           OK. finished without errors\n"
  "    1           KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher --validate input.xml\n"
  "    ccruncher --listloss input.xml\n"
  "    ccruncher --path=20070801 input.xml\n"
  "    ccruncher --hash=100 -fv --path=./E20070801 input.xml\n"
  "    ccruncher --hash=100 -fv --path=./E20070801 input.xml.gz\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "   ccruncher is Copyright (C) 2003-2007 Gerard Torrent and licensed\n"
  "     under the GNU General Public License, version 2. more info at\n"
  "               http://www.generacio.com/ccruncher\n"
  << endl;
}

