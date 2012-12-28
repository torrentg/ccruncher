
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
#include <gsl/gsl_errno.h>
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
void run() throw(Exception);
void catchsignal(int signal);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
char cmode = 'c';
int inice = -999;
size_t ihash = 1000;
unsigned char ithreads = 0;
map<string,string> defines;
bool stop = false;

//===========================================================================
// catchsignal
//===========================================================================
void catchsignal(int)
{
  stop = true;
} 

//===========================================================================
// gsl error handler
//===========================================================================
void gsl_handler(const char * reason, const char *file, int line, int gsl_errno)
{
  string msg = reason;
  msg += " (file=" + string(file);
  msg += ", line=" + Format::toString(line);
  msg += ", errno=" + Format::toString(gsl_errno) + ")";
  Exception gsl_exception(msg);
  throw Exception(gsl_exception, "gsl exception");
}

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
  // short options
  const char* const options1 = "hawD:o:" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  NULL,  'h' },
      { "append",       0,  NULL,  'a' },
      { "overwrite",    0,  NULL,  'w' },
      { "define",       1,  NULL,  'D' },
      { "path",         1,  NULL,  'o' },
      { "version",      0,  NULL,  301 },
      { "nice",         1,  NULL,  302 },
      { "hash",         1,  NULL,  303 },
      { "threads",      1,  NULL,  304 },
      { NULL,           0,  NULL,   0  }
  };

  // parsing options
  while (1)
  {
    int curropt = getopt_long(argc, argv, options1, options2, NULL);

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

      case 'a': // -a --append
      case 'w': // -w --overwrite
          if (cmode != 'c') {
            cerr << "error: found more than one output files mode" << endl;
            return 1;
          }
          cmode = curropt;
          break;

      case 'o': // -o dir, --path=dir (set output files path)
          spath = string(optarg);
          break;

      case 301: // --version (show version and exit)
          cout << "ccruncher-" << PACKAGE_VERSION << " (" << SVN_VERSION << ")" << endl;
          cout << "build host: " << BUILD_HOST << endl;
          cout << "build date: " << BUILD_DATE << endl;
          cout << "build author: " << BUILD_USER << endl;
          cout << "build options: " << Utils::getCompilationOptions() << endl;
          return 0;

      case 302: // --nice=val (set nice value)
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

      case 303: // --hash=val (set hash value)
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

      case 304: // --threads=val (set number of threads)
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
  if (MAX_NUM_THREADS < ithreads)
  {
    cerr << "error: invalid number of threads" << endl;
    return 1;
  }
  if (ithreads == 0)
  {
    ithreads = Utils::getNumCores();
  }

  try
  {
    // setting new nice value (modify scheduling priority)
    if (inice != -999) {
      setnice(inice);
    }

    // running simulation
    run();
  }
  catch(std::exception &e)
  {
    cerr << endl << e.what() << endl;
    return 1;
  }
  catch(...)
  {
    cerr << endl <<
        "uncatched exception. please report this bug sending input file, \n"
        "ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
    return 1;
  }

  // exit function
  return 0;
}

//===========================================================================
// run
//===========================================================================
void run() throw(Exception)
{
  Timer timer(true);

  // setting interruptions handlers
  signal(SIGINT, catchsignal);
  signal(SIGABRT, catchsignal);
  signal(SIGTERM, catchsignal);

  // setting gsl error handler
  gsl_set_error_handler(gsl_handler);

  // checking output directory
  if (!File::existDir(spath))
  {
    throw Exception("path " + spath + " not exist");
  }

  // header
  Logger log(cout.rdbuf());
  log << copyright << endl;
  log << header << endl;

  // parsing input file
  IData *idata = new IData(cout.rdbuf());
  idata->init(sfilename, defines, &stop);

  // creating simulation object
  MonteCarlo montecarlo(cout.rdbuf());
  montecarlo.setFilePath(spath, cmode);
  montecarlo.setData(*idata);
  delete idata;

  // running simulation
  montecarlo.run(ithreads, ihash, &stop);

  // footer
  log << footer(timer) << endl;
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
  "    ccruncher-cmd --path=dir [options] [FILE]\n"
  "  description:\n"
  "    ccruncher is a tool used to simulate a portfolio of credits using\n"
  "    the Monte Carlo method. More info at http://www.ccruncher.net\n"
  "  arguments:\n"
  "    file           xml file containing the problem to be solved. This\n"
  "                   file can be gziped (caution, zip format not suported).\n"
  "                   With no FILE read standard input.\n"
  "  options:\n"
  "    --define key=val\n"
  "    -D key=val     declare a define named 'key' with value 'val'\n"
  "    -a\n"
  "    --append       output data is appended to existing files, if any\n"
  "    -w\n"
  "    --overwrite    existing output files are overwritten\n"
  "    -o dir\n"
  "    --path=dir     directory where output files will be placed (required)\n"
#if !defined(_WIN32)
  "    --nice=num     set priority to num (default=" + Format::toString(getpriority(PRIO_PROCESS,0)) + ", min=" + Format::toString(PRIO_MIN) + ", max=" + Format::toString(PRIO_MAX) + ")\n"
#endif
  "    --threads=num  number of threads (default=" + Format::toString(Utils::getNumCores()) + ")\n"
  "    --hash=num     print '.' for each num simulations (default=" + Format::toString(ihash) + ")\n"
  "    -h\n"
  "    --help         show this message and exit\n"
  "    --version      show version and exit\n"
  "  return codes:\n"
  "    0              OK. finished without errors\n"
  "    1              KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher-cmd --path=data/sample01 samples/sample01.xml\n"
  "    ccruncher-cmd -w --hash=5000 --path=data/ samples/test100.xml\n"
  "    ccruncher-cmd --append --path=data/ --threads=4 -D ndf=8 samples/sample.xml\n"
  << endl;
}

