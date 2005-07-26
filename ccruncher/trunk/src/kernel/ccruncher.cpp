
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
// ccruncher.cpp - ccruncher main code
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

#ifdef USE_MPI
  #include <mpi.h>
#endif

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
bool bsimulate = true;
bool bverbose = false;
bool bforce = false;
int inice = 10;
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
      { NULL,           0,  NULL,   0  }
  };

  // initialization routines
  try
  {
    startup(argc, argv);
  }
  catch(Exception &e)
  {
    cerr << e;
    return shutdown(1);
  }
  catch(...)
  {
    cerr << "unknow error";
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
          bsimulate = false;
          break;

      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "ccruncher version and arguments at gerard@fobos.generacio.com\n";
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
  if (spath == "" && bsimulate == true)
  {
    cerr << "--path is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
    return shutdown(1);
  }

  // license info
  copyright();

  try
  {
    // setting nice value
    setnice(inice);

    // running simulation
    run(sfilename, spath);
  }
  catch(Exception &e)
  {
    cerr << e;
    return shutdown(1);
  }
  catch(...)
  {
    cerr << "uncatched exception. please report this bug sending input file, \n"
            "ccruncher version and arguments at gerard@fobos.generacio.com\n";
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
    throw Exception("needed more than 1 node to run ccruncher");
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
  // shutdown MPI
  MPI::Finalize();
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
  Logger::trace("start time", Utils::timestamp());
  Logger::trace("input file", filename);
  // TODO: add MPI info (num nodes, etc.)
  Logger::previousIndentLevel();

  // parsing input file
  IData idata = IData(filename);

  // creating simulation object
  MonteCarlo simul;
  simul.setFilePath(path, bforce);
  simul.setHash(ihash);

  // initializing simulation
  simul.initialize(&idata);

  // exiting if only validation
  if (bsimulate == false) return;

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
  simul.execute();

  // tracing some execution info
  Logger::trace("general information", '*');
  Logger::newIndentLevel();
  Logger::trace("end time", Utils::timestamp());
  Logger::trace("elapsed time", Timer::format(timer.read()));
  Logger::previousIndentLevel();
  Logger::addBlankLine();
}

//===========================================================================
// setnice
//===========================================================================
void setnice(int niceval) throw(Exception)
{
#ifndef _MSC_VER
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
  "    --nice=num  set nice priority to num (default 10)\n"
  "    --hash=num  print '#' for each num simulations (default=0)\n"
  "    --validate  perform input file validations and exit\n"
  "    --help -h   show this message and exit\n"
  "    --version   show version and exit\n"
  "  return codes:\n"
  "    0           OK. finished without errors\n"
  "    1           KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher --validate input.xml\n"
  "    ccruncher --path=20050601 input.xml\n"
  "    ccruncher --hash=100 -fv --path=./E20050601 input.xml\n"
  "    ccruncher --hash=100 -fv --path=./E20050601 input.xml.gz\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "   ccruncher is Copyright (C) 2003-2005 Gerard Torrent and licensed\n"
  "     under the GNU General Public License, version 2. more info at\n"
  "               http://www.generacio.com/ccruncher\n"
  << endl;
}
