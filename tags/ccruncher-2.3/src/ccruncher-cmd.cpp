
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include <getopt.h>
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
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

void help();
void info();
void version();
void setnice(int) throw(Exception);
void run() throw(Exception);

//---------------------------------------------------------------------------

string sfilename = "";
string spath = "";
char cmode = 'c';
int inice = -999;
size_t ihash = 1000;
unsigned char ithreads = 0;
map<string,string> defines;
bool indexes = false;
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
  const char* const options1 = "hawiD:o:" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  NULL,  'h' },
      { "append",       0,  NULL,  'a' },
      { "overwrite",    0,  NULL,  'w' },
      { "indexes",      0,  NULL,  'i' },
      { "define",       1,  NULL,  'D' },
      { "output",       1,  NULL,  'o' },
      { "version",      0,  NULL,  301 },
      { "nice",         1,  NULL,  302 },
      { "hash",         1,  NULL,  303 },
      { "threads",      1,  NULL,  304 },
      { "info",         0,  NULL,  305 },
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
          help();
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
            string key = Utils::trim(str.substr(0, pos));
            string value = Utils::trim(str.substr(pos+1));
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

      case 'i': // -i --indexes
          indexes = true;
          break;

      case 'o': // -o dir, --output=dir (set output files path)
          spath = string(optarg);
          break;

      case 301: // --version (show version and exit)
          version();
          return 0;

      case 302: // --nice=val (set nice value)
          try
          {
            string snice = string(optarg);
            inice = Parser::intValue(snice);
          }
          catch(Exception &)
          {
            cerr << "error: invalid nice value" << endl;
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
            cerr << "error: invalid hash value" << endl;
            return 1;
          }
          break;

      case 304: // --threads=val (set number of threads)
          try
          {
            string sthreads = string(optarg);
            int num = Parser::intValue(sthreads);
            if (num <= 0 || Utils::getNumCores() < num) {
              throw Exception();
            }
            else {
              ithreads = (unsigned char)(num);
            }
          }
          catch(Exception &)
          {
            cerr << "error: invalid threads value" << endl;
            return 1;
          }
          break;

      case 305: // --info (show info and exit)
          info();
          return 0;

      default: // unexpected error
          cerr << 
            "unexpected error parsing arguments. Please report this bug sending input\n"
            "file, ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
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
    cerr << "error: last argument will be the xml input file" << endl;
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
      cerr << "error: can't open file '" << sfilename << "'" << endl;
      return 1;
    }
  }

  // checking arguments consistency
  if (spath == "")
  {
    cerr << "error: --output is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
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
    throw Exception("output '" + spath + "' not exist");
  }

  // header
  Logger log(cout.rdbuf());
  log << copyright << endl;
  log << header << endl;

  IData *idata = NULL;

  try
  {
    // parsing input file
    idata = new IData(cout.rdbuf());
    idata->init(sfilename, defines, &stop);

    // creating simulation object
    MonteCarlo montecarlo(cout.rdbuf());
    montecarlo.setFilePath(spath, cmode, indexes);
    montecarlo.setData(*idata);
    delete idata;

    // running simulation
    montecarlo.run(ithreads, ihash, &stop);

    // footer
    log << footer(timer) << endl;
  }
  catch(...)
  {
    if (idata != NULL) {
      delete idata;
    }
    throw;
  }
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
// help
// follows POSIX guidelines as described in:
// http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
// you can create man pages using help2man (http://www.gnu.org/software/help2man/)
//===========================================================================
void help()
{
  cout <<
  "Usage: ccruncher-cmd [OPTION]... -o DIRECTORY [FILE]\n"
  "\n"
  "Simule the loss distribution of the credit portfolio described in the xml\n"
  "input FILE using the Monte Carlo method. FILE can be gziped. If no one is\n"
  "given, then STDIN is considered. The input file format description and\n"
  "details of the simulation procedure can be found at http://www.ccruncher.net.\n"
  "\n"
  "Mandatory arguments to long options are mandatory for short options too.\n"
  "\n"
  "  -D, --define=KEY=VAL    replace '$KEY' strings by 'VAL' in input file\n"
  "  -a, --append            output data is appended to existing files\n"
  "  -w, --overwrite         existing output files are overwritten\n"
  "  -o, --output=DIRECTORY  directory where output files will be placed\n"
  "  -i, --indexes           create file indexes.csv with info about simulation\n"
#if !defined(_WIN32)
  "      --nice=NICEVAL      set process priority to NICEVAL (see nice command)\n"
#endif
  "      --threads=NTHREADS  number of threads to use (default=number of cores)\n"
  "      --hash=HASHNUM      print '.' for each HASHNUM simulations (default=" + Format::toString(ihash) + ")\n"
  "      --info              show build parameters and exit\n"
  "  -h, --help              show this message and exit\n"
  "      --version           show version and exit\n"
  "\n"
  "Exit status:\n"
  "  0   finished without errors\n"
  "  1   finished with errors\n"
  "\n"
  "Examples:\n"
  "  basic example      ccruncher-cmd -o data/ samples/test04.xml\n"
  "  forcing overwrite  ccruncher-cmd -w -o data/ samples/test100.xml\n"
  "  redefining values  ccruncher-cmd -w -o data/ -D ndf=8 samples/sample.xml\n"
  "\n"
  "Report bugs to gtorrent@ccruncher.net. Please include the output of\n"
  "'ccruncher-cmd --info' in the body of your report and attach the input\n"
  "file if at all possible.\n"
  << endl;
}

//===========================================================================
// version
// follows POSIX guidelines as described in:
// http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
// you can create man pages using help2man (http://www.gnu.org/software/help2man/)
//===========================================================================
void version()
{
  cout <<
  "ccruncher-cmd " << PACKAGE_VERSION << " (" << SVN_VERSION << ")\n"
  "Copyright (c) 2013 Gerard Torrent.\n"
  "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl-2.0.html>.\n"
  "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
  "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
  "PARTICULAR PURPOSE. See the GNU General Public License for more details."
  << endl;
}

//===========================================================================
// info
//===========================================================================
void info()
{
  cout << "ccruncher-cmd " << PACKAGE_VERSION << " (" << SVN_VERSION << ")" << endl;
  cout << "build host: " << BUILD_HOST << endl;
  cout << "build date: " << BUILD_DATE << endl;
  cout << "build author: " << BUILD_USER << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
#if !defined(_WIN32)
  cout << "nice value: default=" << getpriority(PRIO_PROCESS,0) << ", min=" << PRIO_MIN << ", max=" << PRIO_MAX << endl;
#endif
  cout << "num cores: " << Utils::getNumCores() << endl;
}

