
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

#ifndef _WIN32
  #include <sys/resource.h>
#endif

#ifndef _MSC_VER
  #include <getopt.h>
#endif

#include <unistd.h>
#include <iostream>
#include <gsl/gsl_errno.h>
#include <QApplication>
#include <QTextCodec>
#include <QVariant>
#include <QMap>
#include "gui/MainWindow.hpp"
#include "gui/Application.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

void help();
void info();
void version();
void setnice(int niceval) throw(Exception);

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
// main function
//===========================================================================
int main(int argc, char *argv[])
{
  int inice = -999;
  QMap<QString,QVariant> properties;

  // short options
  const char* const options1 = "hi" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  NULL,  'h' },
      { "indexes",      0,  NULL,  'i' },
      { "version",      0,  NULL,  301 },
      { "nice",         1,  NULL,  302 },
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

      case 'i': // -i --indexes
          properties["indexes"] = QVariant(true);
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

  //chdir("..");

  Application app(argc, argv);
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  setlocale(LC_ALL, "C"); // sets decimal point to sprintf

  gsl_set_error_handler(gsl_handler);

  // run application
  try
  {
    // setting new nice value (modify scheduling priority)
    if (inice != -999) {
      setnice(inice);
    }

    MainWindow w(properties);
    w.show();

    // opening files given as argument
    for(int i=optind; i<argc; i++)
    {
      QString filename(argv[i]);
      QUrl url = QUrl::fromLocalFile(filename);
      if (!filename.toLower().endsWith("csv")) {
        url.setPath(url.toLocalFile());
        url.setScheme("exec");
      }
      w.openFile(url);
    }

    return app.exec();
  }
  catch (std::exception &e)
  {
    cerr << "error: " << e.what() << endl;
    return 1;
  }
  catch(...)
  {
    cerr <<
      "panic: unexpected error. Please report this bug sending input\n"
      "file, ccruncher version and arguments to gtorrent@ccruncher.net\n" << endl;
    return 1;
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
  //TODO: change description
  //TODO: use options + open files passed as argument

  cout <<
  "Usage: ccruncher-gui [OPTION]... [FILE]...\n"
  "\n"
  "Simule the loss distribution of the credit portfolio described in the xml\n"
  "input FILEs using the Monte Carlo method. The input file format description and\n"
  "details of the simulation procedure can be found at http://www.ccruncher.net.\n"
  "\n"
  "Mandatory arguments to long options are mandatory for short options too.\n"
  "\n"
  "  -i, --indexes           create file indexes.csv with info about simulation\n"
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
  "ccruncher-gui samples/test04.xml\n\n"
  "ccruncher-gui data/portfolio.csv\n\n"
  "ccruncher-gui samples/test04.xml data/*.csv\n\n"
  "\n"
  "Report bugs to gtorrent@ccruncher.net. Please include the output of\n"
  "'ccruncher-gui --info' in the body of your report and attach the input\n"
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
  "ccruncher-gui " << PACKAGE_VERSION << " (" << SVN_VERSION << ")\n"
  "Copyright (c) 2013 Gerard Torrent.\n"
  "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl-2.0.html>\n"
  "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\n"
  "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
  "See the GNU General Public License for more details."
  << endl;
}

//===========================================================================
// info
//===========================================================================
void info()
{
  cout << "ccruncher-gui " << PACKAGE_VERSION << " (" << SVN_VERSION << ")" << endl;
  cout << "build host: " << BUILD_HOST << endl;
  cout << "build date: " << BUILD_DATE << endl;
  cout << "build author: " << BUILD_USER << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
#if !defined(_WIN32)
  cout << "nice value: default=" << getpriority(PRIO_PROCESS,0) << ", min=" << PRIO_MIN << ", max=" << PRIO_MAX << endl;
#endif
  cout << "num cores: " << Utils::getNumCores() << endl;
}

