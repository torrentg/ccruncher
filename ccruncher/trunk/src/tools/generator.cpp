
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
// generator.cpp - generator tool code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include "kernel/IData.hpp"
#include "utils/File.hpp"
#include "utils/Parser.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void run(string, int, int);

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
  string sfilename = "";
  long nclients = 0L;
  long nassets = 0L;

  // parsing options
  for (int i=1;i<argc;i++)
  {
    string arg = string(argv[i]);

    if (arg == "--help")
    {
      usage();
      return 0;
    }
    else if (arg == "--version")
    {
      version();
      return 0;
    }
    else if (arg.length() >= 10 && arg.substr(0, 10) == "-nclients=")
    {
      try
      {
        nclients = Parser::longValue(arg.substr(10));
      }
      catch(Exception &e)
      {
        cerr << "invalid nclients value" << endl;
        return 1;
      }
    }
    else if (arg.length() >= 9 && arg.substr(0, 9) == "-nassets=")
    {
      try
      {
        nassets = Parser::longValue(arg.substr(9));
      }
      catch(Exception &e)
      {
        cerr << "invalid nassets value" << endl;
        return 1;
      }
    }
    else
    {
      if (sfilename != "" || arg.substr(0, 1) == "-")
      {
        cerr << "unexpected argument: " << arg << endl;
        return 1;
      }
      else
      {
        sfilename = arg;
      }
    }
  }

  // arguments validations
  if (sfilename == "")
  {
    cerr << "xml input file not specified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }
  if (nclients == 0L || nassets == 0L)
  {
    cerr << "required arguments not especified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }

  // license info
  // copyright();

  try
  {
    run(sfilename, nclients, nassets);
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
void run(string filename, int nclients, int nassets)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // parsing input file
  IData idata = IData(filename);

  cout << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";
  cout << "<!DOCTYPE creditcruncher SYSTEM 'creditcruncher-0.1.dtd'>\n";
  cout << "<creditcruncher>\n";
  cout << idata.params->getXML(2);
  cout << idata.interests->getXML(2);
  cout << idata.ratings->getXML(2);
  cout << idata.transitions->getXML(2);
  cout << idata.sectors->getXML(2);
  cout << idata.correlations->getXML(2);
  cout << idata.segmentations->getXML(2);
  cout << idata.aggregators->getXML(2);
  cout << "</creditcruncher>\n";
/*
  for (int i=0;i<NUM_CLIENTS;i++)
  {
    cout << "<client ";
    cout << "rating='" << rating[rand()%(NUM_RATINGS-1)] << "' ";
    cout << "sector='" << sectors[rand()%NUM_SECTORS] << "' ";
    cout << "name='client" << i << "' ";
    cout << "id='" << i << "'>\n";

    for (int j=0;j<NUM_ASSETS;j++)
    {
      cout << "  <asset class='bond' ";
      cout << "id='" << i << "-" << j << "'>\n";

      cout << "    ";
      cout << "<property name='issuedate' value='01/01/2000'/>\n";
      cout << "    ";
      cout << "<property name='term' value='120'/>\n";
      cout << "    ";
      cout << "<property name='nominal' value='1500'/>\n";
      cout << "    ";
      cout << "<property name='rate' value='0.07'/>\n";
      cout << "    ";
      cout << "<property name='ncoupons' value='120'/>\n";
      cout << "    ";
      cout << "<property name='adquisitiondate' value='1/1/2003'/>\n";
      cout << "    ";
      cout << "<property name='adquisitionprice' value='1500'/>\n";

      cout << "  </asset>\n";
    }

    cout << "</client>\n";
  }
*/
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "generator-0.1" << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: generator -nclients=num -nassets=num file.xml\n"
  "\n"
  "  description:\n"
  "    generator is a creditcruncher tool for generating input test files\n"
  "    ratings and sectors are extracted from template file\n"
  "  arguments:\n"
  "    file.xml   file used as template\n"
  "    -nclients  number of clients in portfolio\n"
  "    -nassets   number of assets per client\n"
  "  options:\n"
  "    --help     show this message and exit\n"
  "    --version  show version and exit\n"
  "  return codes:\n"
  "    0          OK. finished without errors\n"
  "    1          KO. finished with errors\n"
  "  examples:\n"
  "    generator -nclients=20 -nassets=3 template.xml\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "  generator is Copyright (C) 2003-2005 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/ccruncher\n"
  << endl;
}
