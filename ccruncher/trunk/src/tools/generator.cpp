
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
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/Exception.hpp"
#include "MersenneTwister.h"

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

#define NOMINAL_MU     1000.00
#define NOMINAL_SIGMA2  100.00
#define NOMINAL_MIN     100.00

//---------------------------------------------------------------------------

MTRand mtw;

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void run(string, int, int);
string getXMLPortfolio(int, IData*, int, int);
double getNominal();

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
  IData idata = IData(filename, 0);

  cout << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";
  //cout << "<!DOCTYPE creditcruncher SYSTEM 'creditcruncher-0.1.dtd'>\n";
  cout << "<creditcruncher>\n";
  cout << idata.params->getXML(2);
  cout << idata.interests->getXML(2);
  cout << idata.ratings->getXML(2);
  cout << idata.transitions->getXML(2);
  cout << idata.sectors->getXML(2);
  cout << idata.correlations->getXML(2);
  cout << idata.segmentations->getXML(2);
  cout << idata.aggregators->getXML(2);
  cout << getXMLPortfolio(2, &idata, nclients, nassets);
  cout << "</creditcruncher>\n";
}

//===========================================================================
// version
//===========================================================================
string getXMLPortfolio(int ilevel, IData *idata, int nclients, int nassets)
{
  string ret = "";
  string spc1 = Utils::blanks(ilevel);
  string spc2 = Utils::blanks(ilevel+2);
  string spc3 = Utils::blanks(ilevel+4);
  string spc4 = Utils::blanks(ilevel+6);
  int nratings = idata->ratings->getRatings()->size();
  int nsectors = idata->sectors->getSectors()->size();
  Date date1 = idata->params->begindate;

  ret += spc1 + "<portfolio>\n";

  for (int i=1;i<=nclients;i++)
  {
    ret += spc2 + "<client ";
    ret += "rating='" + idata->ratings->getName(rand()%(nratings-1)) + "' ";
    ret += "sector='" + idata->sectors->getName(rand()%(nsectors)) + "' ";
    ret += "name='client" + Parser::int2string(i) + "' ";
    ret += "id='" + Parser::int2string(i) + "'>\n";

    for (int j=1;j<=nassets;j++)
    {
      ret += spc3;
      ret += "<asset class='bond' ";
      ret += "id='" + Parser::int2string(i) + "-" + Parser::int2string(j) + "'>\n";

      ret += spc4 + "<property name='issuedate' value='" + Parser::date2string(date1) + "'/>\n";
      ret += spc4 + "<property name='term' value='120'/>\n";
      ret += spc4 + "<property name='nominal' value='" + Parser::double2string(getNominal()) +"'/>\n";
      ret += spc4 + "<property name='rate' value='0.07'/>\n";
      ret += spc4 + "<property name='ncoupons' value='120'/>\n";
      ret += spc4 + "<property name='adquisitiondate' value='" + Parser::date2string(date1) + "'/>\n";
      ret += spc4 + "<property name='adquisitionprice' value='1500'/>\n";

      ret += spc3 + "</asset>\n";
    }

    ret += spc2 +  "</client>\n";

  }

  ret += spc1 + "</portfolio>\n";

  return ret;
}

//===========================================================================
// getNominal
//===========================================================================
double getNominal()
{
  double ret = mtw.randNorm(NOMINAL_MU, NOMINAL_SIGMA2);
  return (ret<=NOMINAL_MIN?NOMINAL_MIN:ret);
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
