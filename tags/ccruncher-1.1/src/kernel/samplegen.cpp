
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
// 2005/03/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added POSIX compliance in command line arguments
//
// 2005/03/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/05/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival function section
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/07/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . performing partial parsing (without portfolio)
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved performance problem when nclients is high
//
// 2005/08/06 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added getCompilationOptions() to version output
//
// 2007/08/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . file refactor
//
//===========================================================================

#include "utils/config.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <getopt.h>
#include "kernel/IData.hpp"
#include "utils/File.hpp"
#include "utils/Strings.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Exception.hpp"
#include "utils/Utils.hpp"
#include <MersenneTwister.h>

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

#define NOMINAL_MU     1000.00
#define NOMINAL_SIGMA2  400.00
#define NOMINAL_MIN     100.00
#define NOMINAL_MAX    1900.00

//---------------------------------------------------------------------------

MTRand mtw;

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
      { "nborrowers",   1,  NULL,  302 },
      { "nassets",      1,  NULL,  303 },
      { "copyright",    0,  NULL,  304 },
      { NULL,           0,  NULL,   0  }
  };

  string sfilename = "";
  long nborrowers = 0L;
  long nassets = 0L;

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

      case 302: // --nborrowers (set number borrowers)
          try
          {
            string sborrowers = string(optarg);
            nborrowers = Parser::longValue(sborrowers);
          }
          catch(Exception &e)
          {
            cerr << "invalid nborrowers value" << endl;
            return 1;
          }
          break;

      case 303: // --nassets (set number assets)
          try
          {
            string sassets = string(optarg);
            nassets = Parser::longValue(sassets);
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
  if (nborrowers == 0L || nassets == 0L)
  {
    cerr << "required arguments not especified" << endl;
    cerr << "use --help option for more information" << endl;
    return 1;
  }

  // license info
  // copyright();

  try
  {
    run(sfilename, nborrowers, nassets);
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
void run(string filename, int nborrowers, int nassets) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // parsing input file
  IData idata = IData(filename, false);

  cout << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";
  cout << "<ccruncher>\n";
  cout << idata.getParams().getXML(2);
  cout << idata.getInterests().getXML(2);
  cout << idata.getRatings().getXML(2);
  cout << idata.getTransitionMatrix().getXML(2);
  if (idata.hasSurvival()) {
    cout << idata.getSurvival().getXML(2);
  }
  cout << idata.getSectors().getXML(2);
  cout << idata.getCorrelationMatrix().getXML(2);
  cout << idata.getSegmentations().getXML(2);
  cout << "  <portfolio>\n";
  printXMLPortfolio(2, idata, nborrowers, nassets);
  cout << "  </portfolio>\n";
  cout << "</ccruncher>\n";
}

//===========================================================================
// getXMLPortfolio
//===========================================================================
void printXMLPortfolio(int ilevel, IData &idata, int nborrowers, int nassets) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string spc3 = Strings::blanks(ilevel+4);
  string spc4 = Strings::blanks(ilevel+6);
  int nratings = idata.getRatings().size();
  int nsectors = idata.getSectors().size();
  Date date1 = idata.getParams().begindate;

  for (int i=1;i<=nborrowers;i++)
  {
    cout << spc2 + "<borrower ";
    cout << "rating='" + idata.getRatings()[rand()%(nratings-1)].name + "' ";
    cout << "sector='" + idata.getSectors()[rand()%(nsectors)].name + "' ";
    cout << "name='borrower" + Format::int2string(i) + "' ";
    cout << "id='" + Format::int2string(i) + "'>\n";

    for (int j=1;j<=nassets;j++)
    {
      cout << spc3;
      cout << "<asset name='bond' ";
      cout << "id='" + Format::int2string(i) + "-" + Format::int2string(j) + "' ";
      cout << "date='" + Format::date2string(date1) + "'>\n";

      cout << getXMLData(ilevel+6, date1, 120, getNominal(), 0.70, 1.6);

      cout << spc3 + "</asset>\n";
    }

    cout << spc2 +  "</borrower>\n" << endl;
  }
}

//===========================================================================
// getNominal
//===========================================================================
double getNominal()
{
  double ret = mtw.randNorm(NOMINAL_MU, NOMINAL_SIGMA2);

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

  curr.date = issuedate;
  curr.cashflow = -nominal;
  curr.recovery = pctrecv*nominal;
  events.push_back(curr);

  for(int i=1; i<term; i++) {
    curr.date = addMonths(issuedate, i);
    curr.cashflow = nominal*rent/(double)(term);
    curr.recovery = pctrecv*nominal;
    events.push_back(curr);
  }

  ret += spc1 + "<data>\n";

  for(unsigned int i=0;i<events.size();i++)
  {
    ret += spc2;
    ret += "<values at='" + Format::date2string(events[i].date) + "' ";
    ret += "cashflow='" + Format::double2string(events[i].cashflow) + "' ";
    ret += "recovery='" + Format::double2string(events[i].recovery) + "' ";
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
  cout << "generator-" << VERSION << " (" << SVNVERSION << ")"  << endl;
  cout << "builded by " << BUILD_USER << "@" << BUILD_HOST << " at " << BUILD_DATE << endl;
  cout << "build options: " << Utils::getCompilationOptions() << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: generator [options] --nborrowers=num1 --nassets=num2 file.xml\n"
  "\n"
  "  description:\n"
  "    generator is a creditcruncher tool for generating input test files\n"
  "    ratings and sectors are extracted from template file\n"
  "  arguments:\n"
  "    file.xml         file used as template\n"
  "    --nborrowers=val number of borrowers in portfolio\n"
  "    --nassets=val    number of assets per borrower\n"
  "  options:\n"
  "    --help           show this message and exit\n"
  "    --version        show version and exit\n"
  "    --copyright      show copyright and exit\n"
  "  return codes:\n"
  "        0            OK. finished without errors\n"
  "        1            KO. finished with errors\n"
  "  examples:\n"
  "    generator --nborrowers=50000 --nassets=4 template.xml\n"
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