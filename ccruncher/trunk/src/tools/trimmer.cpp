
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
// trimmer.cpp - trimmer tool code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <cctype>

//---------------------------------------------------------------------------

#define MAX_LINE_SIZE 2048

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

void usage();
void version();
void copyright();
void trimFile(string s);
string rtrim(string s);

//---------------------------------------------------------------------------

bool bverbose = false;
bool bwarnings = false;
bool errors = false;
vector<string> filenames;

//===========================================================================
// main
//===========================================================================
int main(int argc, char *argv[])
{
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
    if (arg.substr(0,1) == "-")
    {
      if (arg == "-v")
      {
        bverbose = true;
      }
      else
      {
        cerr << "unknow option " << arg << endl;
        cerr << "use --help option for more information" << endl;
        return 1;
      }
    }
    else
    {
      filenames.push_back(arg);
    }
  }

  // license info
  copyright();

  // worker loop
  for (unsigned int i=0;i<filenames.size();i++)
  {
    try
    {
      trimFile(filenames[i]);
    }
    catch(std::exception &e)
    {
      cerr << "error trimming " << filenames[i] << " : " << e.what() << endl;
      errors = true;
    }
  }

  // exit function
  if (errors == true) return 1;
  else return 0;
}

//===========================================================================
// trimFile
//===========================================================================
void trimFile(string filename)
{
  char buf[MAX_LINE_SIZE];
  string curline = "";
  vector<string> lines;
  ifstream fin;
  ofstream fout;
  unsigned int nlines = 0;
  unsigned int nlines_ko = 0;
  bool hastabs = false;

  // reading file
  fin.open(filename.c_str());
  while (!fin.eof())
  {
    fin.getline(buf, MAX_LINE_SIZE);
    curline += string(buf);
    if (strlen(buf) < MAX_LINE_SIZE)
    {
      lines.push_back(curline);
      curline = "";
      nlines++;
    }
    else
    {
      // nothing to do
    }
  }
  fin.close();

  // writing file
  fout.open(filename.c_str(), ios::out|ios::trunc);
  for (unsigned int i=0;i<lines.size();i++)
  {
    string::size_type pos = lines[i].find_last_not_of(" \t\n");
    string tmp = lines[i].substr( 0, pos+1 );
    fout << tmp;

    if (tmp.size() != lines[i].size())
    {
      nlines_ko ++;
    }

    if (tmp.find_first_of("\t") != string::npos)
    {
      hastabs = true;
    }

    if (i != lines.size()-1)
    {
      fout << endl;
    }
  }
  fout.close();

  // exit function
  if (bverbose == true)
  {
    cout << filename << " (lines=" << nlines << ", trimmed=" << nlines_ko << ", tabs=" << (hastabs?"true":"false") << ")" << endl;
  }
}

//===========================================================================
// version
//===========================================================================
void version()
{
  cout << "trimmer-0.1" << endl;
}

//===========================================================================
// usage
//===========================================================================
void usage()
{
  cout << "\n"
  "  usage: trimmer [options] <file_1 ... file_n>\n"
  "\n"
  "  description:\n"
  "    trimmer purge spaces and tabs found at end of lines\n"
  "  arguments:\n"
  "    files      text files to be trimmed\n"
  "  options:\n"
  "    -v         be more verbose\n"
  "    --help     show this message and exit\n"
  "    --version  show version and exit\n"
  "  return codes:\n"
  "    0          OK. finished without errors\n"
  "    1          KO. finished with errors\n"
  "  examples:\n"
  "    trimmer file1.cpp\n"
  "    trimmer -v -w *.cpp\n"
  << endl;
}

//===========================================================================
// copyright
//===========================================================================
void copyright()
{
  cout << "\n"
  "  trimmer is Copyright (C) 2003-2005 Gerard Torrent\n"
  "  and licensed under the GNU General Public License, version 2.\n"
  "  more info at http://www.generacio.com/ccruncher\n"
  << endl;
}

