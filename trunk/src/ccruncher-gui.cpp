
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <clocale>
#include <exception>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_version.h>
#include <expat.h>
#include <zlib.h>
#include <qwt.h>
#include <QtGlobal>
#include <QApplication>
#include <QTextCodec>
#include <QVariant>
#include <QMap>
#include <QFileInfo>
#include <QLocale>
#include "gui/MainWindow.hpp"
#include "gui/Application.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/config.h"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_gui;

// functions declaration
void help();
void info();
void version();
void setnice(int niceval) throw(Exception);

/**************************************************************************//**
 * @details Catch uncaught exceptions thrown by program.
 */
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
 * @brief ccruncher-gui main procedure.
 */
int main(int argc, char *argv[])
{
  int inice = -999;
  QMap<QString,QVariant> properties;

  // short options
  const char* const options1 = "h" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  nullptr,  'h' },
      { "version",      0,  nullptr,  301 },
      { "nice",         1,  nullptr,  302 },
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
          return EXIT_FAILURE;

      case 'h': // -h or --help (show help and exit)
          help();
          return EXIT_SUCCESS;

      case 301: // --version (show version and exit)
          version();
          return EXIT_SUCCESS;

      case 302: // --nice=val (set nice value)
          try
          {
            string snice = string(optarg);
            inice = Parser::intValue(snice);
          }
          catch(Exception &)
          {
            cerr << "error: invalid nice value" << endl;
            return EXIT_FAILURE;
          }
          break;

      case 304: // --threads=val (set number of threads)
          try
          {
            string sthreads = string(optarg);
            int ithreads = Parser::intValue(sthreads);
            if (ithreads <= 0 || Utils::getNumCores() < ithreads) {
              throw Exception();
            }
            else {
              properties["ithreads"] = QVariant(ithreads);
            }
          }
          catch(Exception &)
          {
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

  //chdir("..");


  setlocale(LC_ALL, "C"); // sets decimal point to sprintf and strtod
  //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QLocale::setDefault(QLocale::c());
  gsl_set_error_handler(gsl_handler);

  // run application
  try
  {
    Application app(argc, argv);

    // setting new nice value (modify scheduling priority)
    if (inice != -999) {
      setnice(inice);
    }

    MainWindow w(properties);
    w.show();

    // opening files given as argument
    for(int i=optind; i<argc; i++)
    {
      QFileInfo filename(argv[i]);
      QUrl url = QUrl::fromLocalFile(filename.canonicalFilePath());
      /*
      if (!filename.toLower().endsWith("csv")) {
        url.setPath(url.toLocalFile());
        url.setScheme("exec");
      }
      */
      w.openFile(url);
    }

    return app.exec();
  }
  catch (std::exception &e)
  {
    cerr << "error: " << e.what() << endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    exception_handler();
    return EXIT_FAILURE;
  }
}

/**************************************************************************//**
 * @brief Modifies program scheduling priority.
 * @see nice unix command
 * @param[in] niceval New priority.
 * @throw Exception Invalid nice value.
 */
void setnice(int niceval) throw(Exception)
{
  (void) niceval;
#if !defined(_WIN32)
  if (niceval < PRIO_MIN || niceval > PRIO_MAX)
  {
    throw Exception("nice value out of range [" + to_string(PRIO_MIN) +
                    ".." + to_string(PRIO_MAX) + "]");
  }
  else
  {
    if(setpriority(PRIO_PROCESS, 0, niceval) != 0)
    {
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
  "Usage: ccruncher-gui [OPTION]... [FILE]...\n"
  "\n"
  "Simule the loss distribution of the credit portfolio described in the xml\n"
  "input FILEs using the Monte Carlo method. The input file format description and\n"
  "details of the simulation procedure can be found at http://www.ccruncher.net.\n"
  "\n"
  "Mandatory arguments to long options are mandatory for short options too.\n"
  "\n"
#if !defined(_WIN32)
  "      --nice=NICEVAL      set process priority to NICEVAL (see nice command)\n"
#endif
  "      --threads=NTHREADS  number of threads to use (default=number of cores)\n"
  "      --info              show build parameters and exit\n"
  "  -h, --help              show this message and exit\n"
  "      --version           show version and exit\n"
  "\n"
  "Exit status:\n"
  "  0   finished without errors\n"
  "  1   finished with errors\n"
  "\n"
  "Examples:\n"
  "  basic             ccruncher-gui\n"
  "  multiple files    ccruncher-gui samples/test04.xml data/*.csv\n"
  "\n"
  "Report bugs to gtorrent@ccruncher.net. Please include the output of\n"
  "'ccruncher-gui --info' in the body of your report and attach the input\n"
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
  "ccruncher-gui " << PACKAGE_VERSION << " (" << SVN_VERSION << ")\n"
  "Copyright (c) 2017 Gerard Torrent.\n"
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
  cout << "ccruncher-gui " << PACKAGE_VERSION << " (" << SVN_VERSION << ")" << endl;
  cout << "build host: " << BUILD_HOST << endl;
  cout << "build date: " << BUILD_DATE << endl;
  cout << "build author: " << BUILD_USER << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
  cout << "libraries: "
       << "gsl-" << gsl_version << ", "
       << "zlib-" << zlibVersion() << ", "
       << "expat-" << XML_ExpatVersionInfo().major << "." << XML_ExpatVersionInfo().minor << "." << XML_ExpatVersionInfo().micro << ", "
       << "qt-" << qVersion() << ", "
       << "qwt-" << QWT_VERSION_STR
       << endl;
#if !defined(_WIN32)
  cout << "nice value: default=" << getpriority(PRIO_PROCESS,0) << 
          ", min=" << PRIO_MIN << ", max=" << PRIO_MAX << endl;
#endif
  cout << "num cores: " << Utils::getNumCores() << endl;
}

