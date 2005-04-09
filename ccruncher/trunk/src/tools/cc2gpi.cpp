
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
// cc2gpi.cpp - cc2gpi code
// --------------------------------------------------------------------------
//
// 2005/03/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "utils/config.h"
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include "tools/FileResults.hpp"
#include "utils/File.hpp"

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void run(string) throw(Exception);

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
      { NULL,           0,  NULL,   0  }
  };
  
  // filename
  string sfilename="";
    
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
          
      case 300: // -h or --help (show help and exit)
          usage();
          return 0;
          
      case 301: // --version (show version and exit)
          version();
          return 0;
          
      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "cc2gpi version and arguments at gerard@fobos.generacio.com\n";
          return 1;
    }
  }

  // retrieving input filename  
  if (argc == optind)
  {
    cerr << "input file not specified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  else if (argc - optind > 1)
  {
    cerr << "last argument will be the input file" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  else
  {
    sfilename = string(argv[argc-1]);
  }

  // license info
  copyright();

  try
  {
    // running ...
    run(sfilename);
  }
  catch(Exception &e)
  {
    cerr << e;
    return 1;
  }
  catch(...)
  {
    cerr << "uncatched exception. please report this bug sending input file, \n"
            "cc2gpi version and arguments at gerard@fobos.generacio.com\n";
    return 1;
  }

  // exit function
  return 0;
}

//===========================================================================
// run
//===========================================================================
void run(string filename) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  try
  {
    FileResults fr(filename);

    //writing data file
    ofstream fout;
    fout.open("temp1.dat", ios::out|ios::trunc);
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);
    
    int ncolumns = fr.getNumColumns();
    vector<double> *data = fr.getColumns();
    int nrows = data[0].size();
        
    for(unsigned int i=0;i<data[0].size();i++)
    {
      for(int j=0;j<ncolumns;j++)
      {
        fout << data[j][i] << "\t";
      }
      fout << endl;
    }
    fout.close();
    
    //writing gpi file
    fout.open("temp1.gpi", ios::out|ios::trunc);
    fout << "reset\n";
    fout << "set xrange [" << fr.getXMin() <<  ":" << fr.getXMax() << "]\n";
    fout << "set xlabel 'portfolio value'\n";
    fout << "set ylabel 'number of ocurrences'\n";
    fout << "set grid\n";
    fout << "binsize=100\n";
    for(int i=1;i<ncolumns;i++)
    {
      fout << "set title 'Portfolio Value Histogram at " << fr.getDate(i) << "'\n";
      fout << "plot 'temp1.dat' using (floor($" << i+1 << "/binsize)*binsize):(1) smooth frequency with histeps notitle\n";
      fout << "pause -1 'press return to continue'\n";
    }
    fout.close();

    // -----------------------------------------------------------------------
    
    //writing data file
    fout.open("temp2.dat", ios::out|ios::trunc);
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);
    
    for(int i=0;i<ncolumns;i++)
    {
      sort(data[i].begin(), data[i].end());
    }    

    nrows = data[0].size();
    for(unsigned int i=0;i<data[0].size();i++)
    {
      fout << 100.0*(double)(i+1)/nrows << "\t";
      for(int j=1;j<ncolumns;j++)
      {
        fout << data[j][i] << "\t";
      }
      fout << endl;
    }
    fout.close();

    //writing gpi file
    fout.open("temp2.gpi", ios::out|ios::trunc);
    fout << "reset\n";
    fout << "set xrange [" << fr.getXMin() <<  ":" << fr.getXMax() << "]\n";
    fout << "set xlabel 'portfolio value'\n";
    fout << "set ylabel 'P(X < x)'\n";
    fout << "set grid\n";
    for(int i=1;i<ncolumns;i++)
    {
      fout << "set title 'Portfolio Value CDF at 18/02/2004'\n";
      fout << "plot 'temp2.dat' using ($" << i+1 << "):($1/100.0) with lines notitle\n";      
      fout << "pause -1 'press return to continue'\n";
    }
        
  }
  catch(Exception &e)
  {
    throw e;
  }
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "cc2gpi-" << VERSION << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: cc2gpi [options] <file>\n"
  "\n"
  "  description:\n"
  "    cc2gpi generates gnuplot instructions to plot creditcruncher results\n"
  "  arguments:\n"
  "    file        ccruncher output file\n"
  "  options:\n"
  "    --help      show this message and exit\n"
  "    --version   show version and exit\n"
  "  return codes:\n"
  "    0           OK. finished without errors\n"
  "    1           KO. finished with errors\n"
  "  examples:\n"
  "    cc2gpi aggregator1-portfolio-rest.out\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "  cc2gpi is Copyright (C) 2003-2005 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/ccruncher\n"
  << endl;
}
