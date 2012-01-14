
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "kernel/IData.hpp"
#include "utils/File.hpp"
#include "utils/Strings.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Exception.hpp"
#include "utils/Utils.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define NOMINAL_MU     1000.00
#define NOMINAL_SIGMA2  400.00
#define NOMINAL_MIN     100.00
#define NOMINAL_MAX    1900.00

//---------------------------------------------------------------------------

gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void run(string, int, int) throw(Exception);
void printXMLPortfolio(int, IData&, int, int) throw(Exception);
string getXMLData(int, Date, int, double, double, double) throw(Exception);
double getNominal();

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
      { "nobligors",    1,  NULL,  302 },
      { "nassets",      1,  NULL,  303 },
      { "copyright",    0,  NULL,  304 },
      { NULL,           0,  NULL,   0  }
  };

  string sfilename = "";
  int nobligors = 0;
  int nassets = 0;

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

      case 302: // --nobligors (set number obligors)
          try
          {
            string sobligors = string(optarg);
            nobligors = Parser::intValue(sobligors);
          }
          catch(Exception &e)
          {
            cerr << "invalid nobligors value" << endl;
            return 1;
          }
          break;

      case 303: // --nassets (set number assets)
          try
          {
            string sassets = string(optarg);
            nassets = Parser::intValue(sassets);
          }
          catch(Exception &e)
          {
            cerr << "invalid nassets value" << endl;
            return 1;
          }
          break;

      case 304: // --copyright (show copyright and exit)
          copyright();
          return 0;

      default: // unexpected error
          cerr << "unexpected error parsing arguments. Please report this bug sending input file, \n"
                  "generator version and arguments at gtorrent@ccruncher.net\n";
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
  if (nobligors == 0L || nassets == 0L)
  {
    cerr << "required arguments not especified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }

  // license info
  // copyright();

  try
  {
    run(sfilename, nobligors, nassets);
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
void run(string filename, int nobligors, int nassets) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // parsing input file
  IData idata(filename);

  cout << "<?xml version='1.0' encoding='UTF-8'?>\n";
  cout << "<ccruncher>\n";
  cout << idata.getParams().getXML(2);
  cout << idata.getInterest().getXML(2);
  cout << idata.getRatings().getXML(2);
  if (idata.hasSurvival()) {
    cout << idata.getSurvival().getXML(2);
  }
  else {
    cout << idata.getTransitionMatrix().getXML(2);
  }
  cout << idata.getSectors().getXML(2);
  cout << idata.getCorrelationMatrix().getXML(2);
  cout << idata.getSegmentations().getXML(2);
  cout << "  <portfolio>\n";
  printXMLPortfolio(2, idata, nobligors, nassets);
  cout << "  </portfolio>\n";
  cout << "</ccruncher>\n";
}

//===========================================================================
// getXMLPortfolio
//===========================================================================
void printXMLPortfolio(int ilevel, IData &idata, int nobligors, int nassets) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string spc3 = Strings::blanks(ilevel+4);
  string spc4 = Strings::blanks(ilevel+6);
  int nratings = idata.getRatings().size();
  int nsectors = idata.getSectors().size();
  Date date1 = idata.getParams().time0;

  for (int i=1;i<=nobligors;i++)
  {
    cout << spc2 + "<obligor ";
    cout << "rating='" + idata.getRatings()[rand()%(nratings-1)].name + "' ";
    cout << "sector='" + idata.getSectors()[rand()%(nsectors)].name + "' ";
    cout << "id='" + Format::toString(i) + "'>\n";

    for (int j=1;j<=nassets;j++)
    {
      cout << spc3;
      cout << "<asset ";
      cout << "id='" + Format::toString(i) + "-" + Format::toString(j) + "' ";
      cout << "date='" + Format::toString(date1) + "'>\n";

      cout << getXMLData(ilevel+6, date1, 120, getNominal(), 0.70, 1.6);

      cout << spc3 + "</asset>\n";
    }

    cout << spc2 +  "</obligor>\n" << endl;
  }
}

//===========================================================================
// getNominal
//===========================================================================
double getNominal()
{
  double ret = NOMINAL_MU + gsl_ran_gaussian(rng, NOMINAL_SIGMA2);

  if (ret <= NOMINAL_MIN) {
    return NOMINAL_MIN;
  }
  else if (ret >= NOMINAL_MAX) {
    return NOMINAL_MAX;
  }
  else {
    return ret;
  }
}

//===========================================================================
// getXMLData
//===========================================================================
string getXMLData(int ilevel, Date issuedate, int term, double nominal, double pctrecv, double rent) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";
  vector<DateValues> events;
  DateValues curr;

  for(int i=0; i<term; i++) {
    curr.date = addMonths(issuedate, i+1);
    curr.exposure = Exposure(Exposure::Fixed,nominal*rent/(double)(term));
    curr.recovery = Recovery(Recovery::Fixed,pctrecv);
    events.push_back(curr);
  }
  
  for(int i=events.size()-2; i>=0; i--)
  {
    double val = events[i].exposure.getValue() + events[i+1].exposure.getValue();
    events[i].exposure = Exposure(Exposure::Fixed,val);
  }

  ret += spc1 + "<data>\n";

  for(unsigned int i=0;i<events.size();i++)
  {
    ret += spc2;
    ret += "<values at='" + Format::toString(events[i].date) + "' ";
    ret += "exposure='" + events[i].exposure.toString() + "' ";
    ret += "recovery='" + events[i].recovery.toString() + "' ";
    ret += "/>\n";
  }

  ret += spc1 + "</data>\n";

  return ret;
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "generator-" << PACKAGE_VERSION << " (" << SVN_VERSION << ")"  << endl;
  cout << "builded by " << BUILD_USER << "@" << BUILD_HOST << " at " << BUILD_DATE << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: generator [options] --nobligors=num1 --nassets=num2 file.xml\n"
  "\n"
  "  description:\n"
  "    generator is a ccruncher tool for generating input test files\n"
  "    ratings and sectors are extracted from template file\n"
  "  arguments:\n"
  "    file.xml         file used as template\n"
  "    --nobligors=val  number of obligors in portfolio\n"
  "    --nassets=val    number of assets per obligor\n"
  "  options:\n"
  "    --help           show this message and exit\n"
  "    --version        show version and exit\n"
  "    --copyright      show copyright and exit\n"
  "  return codes:\n"
  "        0            OK. finished without errors\n"
  "        1            KO. finished with errors\n"
  "  examples:\n"
  "    generator --nobligors=50000 --nassets=4 template.xml\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "  samplegen is Copyright (C) 2004-2012 Gerard Torrent and\n"
  "  licensed under the GNU General Public License, version 2.\n"
  "  More info at http://www.ccruncher.net\n"
  << endl;
}
