
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// tmaa.cpp - generator tool code
// --------------------------------------------------------------------------
//
// 2005/04/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival function (1-TMAA)
//   . changed name (tma -> mtrans)
//
//===========================================================================

#include "utils/config.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include "kernel/IData.hpp"
#include "utils/File.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

#define DEFAULTMAXYEARS 50

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void run(string, int, int) throw(Exception);

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
  // short options
  const char* const options1 = "" ;

  // long options (name + has_arg + flag + val)
  const struct option options2[] =
  {
      { "help",         0,  NULL,  300 },
      { "version",      0,  NULL,  301 },
      { "tma",          0,  NULL,  302 },      
      { "tmaa",         0,  NULL,  303 },
      { "survival",     0,  NULL,  304 },
      { "maxyears",     1,  NULL,  307 },
      { "copyright",    0,  NULL,  308 },
      { NULL,           0,  NULL,   0  }
  };

  string sfilename = "";
  int maxyears = DEFAULTMAXYEARS;
  int mode = 0;

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

      case 300: // --help (show help and exit)
          usage();
          return 0;

      case 301: // --version (show version and exit)
          version();
          return 0;

      case 302: // --tma (compute TMA)
          mode = 1;
          break;

      case 303: // --tmaa (compute TMAA)
          mode = 2;
          break;

      case 304: // --survival (compute Survival Function)
          mode = 3;
          break;
            
      case 307: // --maxyears=num (number of years)
          try
          {
            string smaxyears = string(optarg); 
            maxyears = Parser::intValue(smaxyears);
          }
          catch(Exception &e)
          {
            cerr << "invalid maxyears value" << endl;
            return 1;
          }
          break;

      case 308: // --copyright (show copyright and exit)
          copyright();
          return 0;

      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "generator version and arguments at gerard@fobos.generacio.com\n";
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
  
  // checking basic arguments existence
  if (mode == 0)
  {
    cerr << "indicate one of these options: --tma or tmaa or --survival" << endl;
    cerr << "please try again" << endl;
    return 1;
  }
  if (maxyears < 0 || maxyears > 999)
  {
    cerr << "maxyears out of range [0,999]" << endl;
    cerr << "please try again" << endl;
    return 1;
  }

  // license info
  copyright();

  try
  {
    run(sfilename, mode, maxyears);
  }
  catch(Exception &e)
  {
    cerr << e;
    return 1;
  }

  // exit
  return 0;
}

//===========================================================================
// run
//===========================================================================
void run(string filename, int mode, int maxyears) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // parsing input file
  IData idata = IData(filename);

  // retrieving transition matrix
  TransitionMatrix *tm = idata.transitions;

  // retrieving ratings
  Ratings *ratings = idata.ratings;
  
  // allocating space
  double **buf = Utils::allocMatrix(tm->n, maxyears+1);

  switch(mode)
  {
      case 1: // TMA
          ccruncher::tma(tm, maxyears, buf);
          break;
      case 2: 
          ccruncher::tmaa(tm, maxyears, buf);
          break;
      case 3: 
          ccruncher::survival(tm, maxyears, buf);
          break;
      default:
           cerr << "an unexpected error [15] occur" << endl;
           cerr << "please report it at gerard@fobos.generacio.com" << endl;
           return;
  }

  // printing header
  cout << "year\t";
  for(int i=0;i<tm->n;i++)
  {
    cout << ratings->getName(i) << "\t";
  }
  cout << "\n" << std::flush;

  // printing content
  for(int t=0;t<=maxyears;t++)
  {
    cout << t << "\t";
    for(int i=0;i<tm->n;i++)
    {
      cout << buf[i][t] << "\t";
    }
    cout << "\n" << std::flush;
  }

  // exit function
  Utils::deallocMatrix(buf, tm->n);
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "mtrans-" << VERSION << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: mtrans [options] file.xml\n"
  "\n"
  "  description:\n"
  "    mtrans is a ccruncher tool for compute the TMA (Forward Default Rate),\n"
  "    TMAA (Cumulative Forward Default Rate) and Survival Function from the \n"
  "    transition matrix included in a CreditCruncher input file\n"
  "  arguments:\n"
  "    file.xml        CreditCruncher input file\n"
  "    --tma           compute TMA (Forward Default Rate)\n"  
  "    --tmaa          compute TMAA (Cumulated Forward Default Rate)\n"
  "    --survival      compute Survival Function (1-TMAA)\n"
  "    --maxyears=num  maximum number of years (default 50)\n"
  "  options:\n"
  "    --help          show this message and exit\n"
  "    --version       show version and exit\n"
  "    --copyright     show copyright and exit\n"
  "  return codes:\n"
  "    0          OK. finished without errors\n"
  "    1          KO. finished with errors\n"
  "  examples:\n"
  "    mtrans --tma --maxyears=25 file.xml\n"
  "    mtrans --tmaa file.xml\n"
  "    mtrans --survival --maxyears=50 file.xml\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "  mtrans is Copyright (C) 2003-2005 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/ccruncher\n"
  << endl;
}
