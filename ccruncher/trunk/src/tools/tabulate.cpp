
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
// tabulate.cpp - tabulate tool code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//
// 2005/01/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . refactoring
//
//===========================================================================

#include "SAX2Handlers.hpp"
#include "utils/File.hpp"
#include "utils/XMLUtils.hpp"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

void run(string) throw(Exception);
void usage();
void version();
void copyright();

//---------------------------------------------------------------------------

bool bverbose = false;

//===========================================================================
// main
//===========================================================================
int main(int argc, char* argv[])
{
  string sfilename = "";

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
    else if (arg == "-v")
    {
      bverbose = true;
    }
    else
    {
      if (sfilename != "" || arg.substr(0, 1) == "-")
      {
        cerr << "unexpected argument: " << arg << endl;
        cerr << "use --help option for more information" << endl;
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

  // license info
  copyright();

  try
  {
    run(sfilename);
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
void run(string filename) throw(Exception)
{
  // checking input file readeability
  File::checkFile(filename, "r");

  // initializing XML stuf
  XMLUtils::initialize();
  
  // creating XML objects
  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  SAX2Handlers handler;
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler);

  // parsing file content
  try
  {
    parser->parse(filename.c_str());
  }
  catch (const OutOfMemoryException& e)
  {
    throw Exception("OutOfMemoryException");
  }
  catch (const XMLException& e)
  {
    throw XMLUtils::XMLException2Exception(e);
  }
  catch (...)
  {
    throw Exception("Unexpected exception during parsing: " + filename);
  }

  // printing file content
  if (!handler.getSawErrors())
  {
    vector<double> *ret = handler.getTranches();
    int numtranches = handler.getNumTranches();
    int n = ret[0].size();

    for(int i=0;i<n;i++)
    {
      for (int j=0;j<numtranches;j++)
      {
        cout <<  ret[j][i] << "\t";
      }
      cout << "\n";
    }
  }

  // exit function
  delete parser;
  XMLUtils::terminate();
  
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "tabulate-0.1" << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: tabulate [options] <file.xml>\n"
  "\n"
  "  description:\n"
  "    trimmer purge spaces and tabs found at end of lines\n"
  "  arguments:\n"
  "    file       xml output creditcruncher file\n"
  "  options:\n"
  "    -v         be more verbose\n"
  "    --help     show this message and exit\n"
  "    --version  show version and exit\n"
  "  return codes:\n"
  "    0          OK. finished without errors\n"
  "    1          KO. finished with errors\n"
  "  examples:\n"
  "    tabulate data/aggregator1-portfolio-rest.xml\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cerr << "\n"
  "  tabulate is Copyright (C) 2003-2005 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/ccruncher\n"
  << endl;
}

