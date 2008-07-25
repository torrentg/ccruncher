
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
//===========================================================================

#include "utils/config.h"
#include <sys/resource.h>
#include <cerrno>
#include <iostream>
#include <getopt.h>
#include "kernel/MonteCarlo.hpp"
#include "kernel/IData.hpp"
#include "utils/File.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"

//---------------------------------------------------------------------------

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
bool bmpi = false;
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
      { "mpi",          0,  NULL,  306 },
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
          catch(Exception &e)
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
          catch(Exception &e)
          {
            cerr << "invalid hash value" << endl;
            return 1;
          }
          break;
          
      case 305: // --validate (validate input file)
          bsimulate = false;
          break;
          
      case 306: // --mpi (use mpi environement)
          bmpi = true;          
          break;
          
      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "ccruncher version and arguments at gerard@fobos.generacio.com\n";
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
  if (spath == "" && bsimulate == true)
  {
    cerr << "--path is a required argument" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;  
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
    return 1;
  }
  catch(...)
  {
    cerr << "uncatched exception. please report this bug sending input file, \n"
            "ccruncher version and arguments at gerard@fobos.generacio.com\n";
    return 1;
  }

  // exit function
  return 0;
}

//===========================================================================
// run
//===========================================================================
void run(string filename, string path) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // initializing logger
  Logger::setVerbosity(bverbose?1:0);
  
  // parsing input file
  IData idata = IData(filename);

  // creating simulation object
  MonteCarlo simul;
  simul.setFilePath(path, bforce);
  simul.useMPI(bmpi);
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
}

//===========================================================================
// setnice
//===========================================================================
void setnice(int niceval) throw(Exception)
{
  if (niceval < PRIO_MIN || niceval > PRIO_MAX)
  {
    throw Exception("nice value out of range [" + Parser::int2string(PRIO_MIN) +
                    ".." + Parser::int2string(PRIO_MAX) + "]");
  }
  else
  {
    if(setpriority(PRIO_PROCESS, 0, niceval) != 0)
    {
      string msg = Parser::int2string(errno);
      msg = (errno==ESRCH?"ESRCH":msg);
      msg = (errno==EINVAL?"EINVAL":msg);
      msg = (errno==EPERM?"EPERM":msg);
      msg = (errno==EACCES?"EACCES":msg);
      throw Exception("error changing process priority [" + msg + "]");
    }
  }
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "ccruncher-" << VERSION << endl;
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
  "    file        xml file containing the problem to be solved\n"
  "  options:\n"
  "    -f          force output files overwriting\n"
  "    -v          be more verbose\n"
  "    --path=dir  directory where output files will be placed (required)\n"
  "    --nice=num  set nice priority to num (default 10)\n"
  "    --hash=num  print '#' for each num simulations (default=0)\n"
  "    --validate  perform input file validations and exit\n"
  "    --mpi       enable lam/mpi work mode\n"
  "    --help -h   show this message and exit\n"
  "    --version   show version and exit\n"
  "  return codes:\n"
  "    0           OK. finished without errors\n"
  "    1           KO. finished with errors\n"
  "  examples:\n"
  "    ccruncher --validate input.xml\n"
  "    ccruncher --path=20050601 input.xml\n"
  "    ccruncher --hash=100 -fv --path=./E20050601 input.xml\n"
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
