
//***************************************************************************
// validator code
//
// @author Gerard Torrent
//
//***************************************************************************

#include <iostream>

//---------------------------------------------------------------------------

using namespace std;

//***************************************************************************
// version
//***************************************************************************
void version()
{
  cout << "\n"
  "creditcruncher-0.1" 
  << endl;
}

//***************************************************************************
// usage
//***************************************************************************
void usage()
{
  cout << "\n"
  "  usage: validator [options] <XML file>\n"
  "\n"
  "  description:\n"
  "    creditcruncher is a tool that evalues the var (value at risk)\n"
  "    of a pure credit portfolio using monte carlo techniques.\n"
  "    keywords: copula, sectorial correlation, credit metrics, copula\n"
  "\n"
  "  options:\n"
  "    -h   show this message\n"
  "    -V   show version\n"
  "    -q   run silently; only print error msgs\n"
  "    -v   be more verbose, incl counts of errors\n"
  "\n"
  "  return codes:\n"
  "     0   OK. valid xml file\n"
  "     1   KO. non valid xml file\n"
  << endl;
}

//***************************************************************************
// copyright
//***************************************************************************
void copyright()
{
  cout << "\n"
  "  creditcruncher is Copyright (C) 2003-2004 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/creditcruncher\n"
  << endl;
}


//***************************************************************************
// main
//***************************************************************************
int main( int argc, char* argv[] )
{
  usage();
  copyright();
}
