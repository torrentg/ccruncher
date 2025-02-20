
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#ifndef _WIN32
  #include <sys/resource.h>
#endif

#include <cerrno>
#include <chrono>
#include <iostream>
#include <csignal>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <getopt.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_version.h>
#include <expat.h>
#include <zlib.h>
#include "kernel/MonteCarlo.hpp"
#include "kernel/XmlInputData.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/config.h"

using namespace std;
using namespace std::chrono;
using namespace ccruncher;

// functions declaration
void help();
void info();
void version();
void setnice(int);
void run();

// shared variables
string sfilename = "";
string spath = "";
char cmode = 'c';
int inice = -999;
size_t ihash = 1000;
unsigned char ithreads = 0;
map<string,string> defines;
bool stop = false;

/**************************************************************************//**
 * @details Default action when a signal is trapped is to stop current
 *          parsing/simulation.
 */
void catchsignal(int)
{
  stop = true;
  fclose(stdin);
}

/**************************************************************************//**
 * @details Catch uncaught exceptions thrown by program.
 */
[[noreturn]]
void exception_handler()
{
  cerr << endl <<
      "unexpected error. please report this bug sending input file, \n"
      "ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
  exit(EXIT_FAILURE);
}

/**************************************************************************//**
 * @brief Error handler for the GSL library.
 * @details Throws a ccruncher Exception.
 * @see http://www.gnu.org/software/gsl/
 */
[[noreturn]]
void gsl_handler(const char * reason, const char *file, int line, int gsl_errno)
{
  string msg = reason;
  msg += " (file=" + string(file);
  msg += ", line=" + to_string(line);
  msg += ", errno=" + to_string(gsl_errno) + ")";
  Exception gsl_exception(msg);
  throw Exception(gsl_exception, "gsl exception");
}

/**************************************************************************//**
 * @brief ccruncher-cmd main procedure.
 */
int main(int argc, char *argv[])
{
  // short options
  const char* const options1 = "hawD:o:" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] = {
      { "help",         0,  nullptr,  'h' },
      { "append",       0,  nullptr,  'a' },
      { "overwrite",    0,  nullptr,  'w' },
      { "define",       1,  nullptr,  'D' },
      { "output",       1,  nullptr,  'o' },
      { "version",      0,  nullptr,  301 },
      { "nice",         1,  nullptr,  302 },
      { "hash",         1,  nullptr,  303 },
      { "threads",      1,  nullptr,  304 },
      { "info",         0,  nullptr,  305 },
      { nullptr,        0,  nullptr,   0  }
  };

  // uncaught exceptions manager
  set_terminate(exception_handler);

  // parsing options
  while (1)
  {
    int curropt = getopt_long(argc, argv, options1, options2, nullptr);

    if (curropt == -1) {
      // no more options. exit while
      break;
    }

    switch(curropt)
    {
      case '?': // invalid option
          cerr << "error parsing arguments" << endl;
          cerr << "use --help option for more information" << endl;
          return EXIT_FAILURE;

      case 'h': // -h or --help (show help and exit)
          help();
          return EXIT_SUCCESS;

      case 'D': // -D key=val (define)
          {
            string str(optarg);
            size_t pos = str.find('=');
            if (pos == string::npos || pos == 0 || pos == str.length()-1 ) {
              cerr << "error parsing arguments (define with invalid format)" << endl;
              cerr << "use --help option for more information" << endl;
              return EXIT_FAILURE;
            }
            string key = Utils::trim(str.substr(0, pos));
            string value = Utils::trim(str.substr(pos+1));
            if (key.length() == 0 || value.length() == 0) {
              cerr << "error parsing arguments (define with invalid format)" << endl;
              cerr << "use --help option for more information" << endl;
              return EXIT_FAILURE;
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
            return EXIT_FAILURE;
          }
          cmode = curropt;
          break;

      case 'o': // -o dir, --output=dir (set output files path)
          spath = string(optarg);
          if (!Utils::existDir(spath)) {
            cerr << "error: directory '" << spath << "' does not exist" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 301: // --version (show version and exit)
          version();
          return EXIT_SUCCESS;

      case 302: // --nice=val (set nice value)
          try {
            string snice = string(optarg);
            inice = Parser::intValue(snice);
          }
          catch(Exception &) {
            cerr << "error: invalid nice value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 303: // --hash=val (set hash value)
          try {
            string shash = string(optarg);
            ihash = Parser::intValue(shash);
          }
          catch(Exception &) {
            cerr << "error: invalid hash value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 304: // --threads=val (set number of threads)
          try {
            string sthreads = string(optarg);
            int num = Parser::intValue(sthreads);
            if (num <= 0 || Utils::getNumCores() < num) {
              throw Exception();
            }
            else {
              ithreads = (unsigned char)(num);
            }
          }
          catch(Exception &) {
            cerr << "error: invalid threads value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 305: // --info (show info and exit)
          info();
          return EXIT_SUCCESS;

      default: // unexpected error
          cerr << 
            "unexpected error parsing arguments. Please report this bug sending input\n"
            "file, ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
          return EXIT_FAILURE;
    }
  }

  // retrieving input filename
  if (argc == optind) 
  {
    // reading data from stdin
    sfilename = "";
  }
  else if (argc - optind > 1)
  {
    cerr << "error: there is more than one input file" << endl;
    cerr << "use --help option for more information" << endl;
    return EXIT_FAILURE;
  }
  else
  {
    try {
      sfilename = string(argv[argc-1]);
      Utils::checkFile(sfilename, "r");
    }
    catch(Exception &) {
      cerr << "error: can't open file '" << sfilename << "'" << endl;
      return EXIT_FAILURE;
    }
  }

  try
  {
    // setting new nice value (modify scheduling priority)
    if (inice != -999) {
      setnice(inice);
    }

    // running simulation
    run();

    return EXIT_SUCCESS;
  }
  catch(std::exception &e) {
    cerr << endl << e.what() << endl;
    return EXIT_FAILURE;
  }
  catch(...) {
    exception_handler();
    return EXIT_FAILURE;
  }
}

/**************************************************************************//**
 * @brief Executes parsing and simulation.
 * @throw Exception Error parsing or simulating.
 */
void run()
{
  auto t1 = steady_clock::now();

  // setting interruptions handlers
  signal(SIGINT, catchsignal);
  signal(SIGABRT, catchsignal);
  signal(SIGTERM, catchsignal);

  // setting gsl error handler
  gsl_set_error_handler(gsl_handler);

  // header
  Logger log(cout.rdbuf());
  log << copyright << endl;
  log << header << endl;

  // parsing input file
  XmlInputData idata(cout.rdbuf());
  if (sfilename == "") {
    idata.readStdin(defines, &stop);
  }
  else {
    idata.readFile(sfilename, defines, &stop);
  }
  if (stop) throw Exception("parser stopped");

  // creating simulation object
  MonteCarlo montecarlo(cout.rdbuf());
  montecarlo.init(idata, spath, cmode);

  // running simulation
  montecarlo.run(ithreads, ihash, &stop);

  // footer
  auto t2 = steady_clock::now();
  long millis = duration_cast<milliseconds>(t2-t1).count();
  log << footer(millis) << endl;
}

/**************************************************************************//**
 * @brief Modifies program scheduling priority.
 * @see nice unix command
 * @param[in] niceval New priority.
 * @throw Exception Invalid nice value.
 */
void setnice(int niceval)
{
  (void) niceval;
#if !defined(_WIN32)
  if (niceval < PRIO_MIN || niceval > PRIO_MAX) {
    throw Exception("nice value out of range [" + to_string(PRIO_MIN) +
                    ".." + to_string(PRIO_MAX) + "]");
  }
  else {
    if(setpriority(PRIO_PROCESS, 0, niceval) != 0) {
      string msg = to_string(errno);
      msg = (errno==ESRCH?"ESRCH":msg);
      msg = (errno==EINVAL?"EINVAL":msg);
      msg = (errno==EPERM?"EPERM":msg);
      msg = (errno==EACCES?"EACCES":msg);
      throw Exception("error changing process priority [" + msg + "]");
    }
  }
#endif
}

/**************************************************************************//**
 * @brief Displays program help.
 * @details Follows POSIX guidelines. You can create man pages using help2man.
 * @see http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
 * @see http://www.gnu.org/software/help2man/
 */
void help()
{
  cout <<
  "Usage: ccruncher-cmd [OPTION]... [FILE]\n"
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
  "  -o, --output=DIRECTORY  place output files in DIRECTORY (default=current dir)\n"
#if !defined(_WIN32)
  "      --nice=NICEVAL      set process priority to NICEVAL (see nice command)\n"
#endif
  "      --threads=NTHREADS  number of threads to use (default=number of cores)\n"
  "      --hash=HASHNUM      print '.' for each HASHNUM simulations (default=" + to_string(ihash) + ")\n"
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

/**************************************************************************//**
 * @brief Displays program version.
 * @details Follows POSIX guidelines. You can create man pages using help2man.
 * @see http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
 * @see http://www.gnu.org/software/help2man/
 */
void version()
{
  cout <<
  "ccruncher-cmd " << PACKAGE_VERSION << " (" << GIT_VERSION << ")\n"
  "Copyright (c) 2025 Gerard Torrent.\n"
  "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl-2.0.html>.\n"
  "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
  "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
  "PARTICULAR PURPOSE. See the GNU General Public License for more details."
  << endl;
}

/**************************************************************************//**
 * @brief Displays program info.
 */
void info()
{
  cout << "ccruncher-cmd " << PACKAGE_VERSION << " (" << GIT_VERSION << ")" << endl;
  cout << "build host: " << BUILD_HOST << endl;
  cout << "build date: " << BUILD_DATE << endl;
  cout << "build author: " << BUILD_USER << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
  cout << "libraries: "
       << "gsl-" << gsl_version << ", "
       << "zlib-" << zlibVersion() << ", "
       << "expat-" << XML_ExpatVersionInfo().major << "." << XML_ExpatVersionInfo().minor << "." << XML_ExpatVersionInfo().micro
       << endl;
#if !defined(_WIN32)
  cout << "nice value: default=" << getpriority(PRIO_PROCESS,0) << 
          ", min=" << PRIO_MIN << ", max=" << PRIO_MAX << endl;
#endif
  cout << "num cores: " << Utils::getNumCores() << endl;
}
