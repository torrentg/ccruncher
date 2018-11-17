
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <csignal>
#include <libconfig.h++>
#include <cassert>
#include <gsl/gsl_errno.h>
#include "utils/config.h"
#include "utils/Exception.hpp"
#include "inference/Metropolis.hpp"

using namespace std;
using namespace libconfig;
using namespace ccruncher;
using namespace ccruncher_inf;

#define UNEXPECTED_ERROR_MSG \
  "unexpected error. please report this bug sending input file, \n" \
  "ccruncher version and arguments to gtorrent@ccruncher.net\n"

Metropolis *mref = nullptr;

/**************************************************************************//**
 * @details Default action when a signal is trapped is to stop current
 *          simulation.
 */
void catchsignal(int)
{
  if (mref != nullptr) {
    mref->stop();
  }
  else {
    exit(EXIT_FAILURE);
  }
}

/**************************************************************************//**
 * @details Catch uncaught exceptions thrown by program.
 */
[[noreturn]]
void exception_handler()
{
  cerr << endl << UNEXPECTED_ERROR_MSG << endl;
  exit(EXIT_FAILURE);
}

/**************************************************************************//**
 * @brief Error handler for the GSL library.
 * @details Throws a Exception.
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
 * @brief Returns a filename with the given extension.
 * @param[in] filename File name.
 * @param[out] newext New file extension (including dot).
 * @return File name with replaced extension.
 */
string setext(const string &filename, const string &newext)
{
  size_t pos = filename.rfind('.');
  if (pos == string::npos) {
    return filename + newext;
  }
  else {
    return filename.substr(0, pos) + newext;
  }
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
  "Usage: ccruncher-inf <options> [FILE]\n"
  "\n"
  "Estimate the dependence parameters (factor loadings, factor correlations\n"
  "and degrees of freedom) of the CCruncher portfolio credit risk model using\n"
  "the Metropolis-Hastings method. More info at http://www.ccruncher.net.\n"
  "\n"
  "Mandatory arguments to long options are mandatory for short options too.\n"
  "\n"
  "  -s, --seed=NUM       seed used by RNG (default=0)\n"
  "  -n, --numiters=NUM   maximum number of iterations (default=0)\n"
  "  -b, --burnin=NUM     burn-in period. (default=0)\n"
  "  -r, --refresh=NUM    update trace info every num iterations\n"
  "  -h, --help           show this message and exit\n"
  "      --version        show version and exit\n"
  "\n"
  "Examples:\n"
  "  basic example     ccruncher-inf samples/inference01.cfg\n"
  "  using parameters  ccruncher-inf -s 99 -n 50000 -b 10000 -r 1000 samples/inference05.cfg\n"
  "\n"
  "Report bugs to gtorrent@ccruncher.net.\n"
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
  "ccruncher-inf " << PACKAGE_VERSION << " (" << SVN_VERSION << ")\n"
  "Copyright (c) 2018 Gerard Torrent.\n"
  "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl-2.0.html>.\n"
  "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
  "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
  "PARTICULAR PURPOSE. See the GNU General Public License for more details."
  << endl;
}

/**************************************************************************//**
 * @brief ccruncher-inf main procedure.
 */
int main(int argc, char *argv[])
{
  string filename = "";
  unsigned long seed = 0UL;
  int blocksize = 0;
  int numiters = 0;
  int burnin = 0;

  // short options
  const char* const options1 = "hr:n:b:s:";

  // long options (name + has_arg + flag + val)
  const struct option options2[] = {
    { "help",     0, nullptr, 'h' },
    { "refresh",  1, nullptr, 'r' },
    { "numiters", 1, nullptr, 'n' },
    { "burnin",   1, nullptr, 'b' },
    { "seed",     1, nullptr, 's' },
    { "version",  0, nullptr, 301 },
    { nullptr,    0, nullptr,   0 }
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

      case 'r': // refresh
          blocksize = atoi(optarg);
          if (blocksize <= 0) {
            cerr << "error: invalid refresh value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 'n': // numiters
          numiters = atoi(optarg);
          if (numiters <= 0) {
            cerr << "error: invalid numiters value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 'b': // burnin
          burnin = atoi(optarg);
          if (burnin <= 0) {
            cerr << "error: invalid burnin value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 's': // seed
          seed = (unsigned long) atol(optarg);
          if (seed == 0) {
            cerr << "error: invalid seed value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 301: // --version (show version and exit)
          version();
          return EXIT_SUCCESS;

      default: // unexpected error
          cerr << "panic: unexpected error parsing arguments." << endl;
          return EXIT_FAILURE;
    }
  }

  // retrieving input filename
  if (argc - optind != 1) {
    cerr << "error: config file not found" << endl;
    cerr << "use --help option for more information" << endl;
    return EXIT_FAILURE;
  }
  else {
    filename = string(argv[argc-1]);
  }

  // reading configuration file
  Config config;
  try {
    config.setOptions(Config::Option::OptionNone | Config::Option::OptionAutoConvert);
    config.readFile(filename.c_str());
  }
  catch(const FileIOException &fioex) {
    cerr << "error: reading file " << filename << " - " << fioex.what() << endl;
    return EXIT_FAILURE;
  }
  catch(const ParseException &pex) {
    cerr << "error: " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << endl;
    return EXIT_FAILURE;
  }

  // running MCMC
  Metropolis metropolis(seed);
  try {
    time_t time0 = time(nullptr);
    metropolis.init(config);
    mref = &metropolis;
    signal(SIGINT, catchsignal);
    signal(SIGABRT, catchsignal);
    signal(SIGTERM, catchsignal);
    gsl_set_error_handler(gsl_handler);
    ofstream output;
    output.open(setext(filename,".out").c_str());
    int num = metropolis.run(output, cout, blocksize, numiters, burnin);
    time_t time1 = time(nullptr);
    cout << endl;
    cout.unsetf(ios::showpos);
    cout.precision(0);
    cout << "elapsed time = " << difftime(time1,time0) << " seconds" << endl;
    cout << "simulations = " << num << endl;
    cout << "output file = " << setext(filename,".out") << endl;
  }
  catch(std::exception &e) {
    cerr << endl << "error: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  catch(...) {
    cerr << endl << UNEXPECTED_ERROR_MSG << endl;
    return EXIT_FAILURE;
  }

  // saving MCMC state
  try {
    Config &state = metropolis.getState();
    state.writeFile(setext(filename,".state").c_str());
  }
  catch(const FileIOException &) {
    cerr << "error writing file " << setext(filename,".state") << endl;
    return(EXIT_FAILURE);
  }
  catch(const exception &e) {
    cerr << "error: " << e.what() << endl;
    return(EXIT_FAILURE);
  }

  // exit function
  return EXIT_SUCCESS;
}

