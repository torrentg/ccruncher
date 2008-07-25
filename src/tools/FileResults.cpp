
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
// FileResults.cpp - FileResults code
// --------------------------------------------------------------------------
//
// 2005/01/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cstdlib>
#include <cerrno>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "tools/FileResults.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::FileResults::FileResults(string &filename) throw(Exception)
{
  data = NULL;
  parseFile(filename);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::FileResults::~FileResults()
{
  if (data != NULL)
  {
    delete [] data;
  }
}

//===========================================================================
// parseFile
//===========================================================================
void ccruncher::FileResults::parseFile(string &filename) throw(Exception)
{
  int currline = 0;
  ifstream ifile;

  try
  {
    // opening input file stream
    ifile.open((const char *) filename.c_str());

    // checking file readeability
    if (!ifile.good())
    {
      throw Exception("FileResults::parseFile(): error opening file");
    }

    // reading each line  
    while(!ifile.eof())
    {
      ifile.getline(buffer, MAXLINESIZE);
      currline++;
      if (ifile.gcount() >= MAXLINESIZE)
      {
        throw Exception("FileResults::parseFile(): line is too long");
      }
      parseLine(buffer);
    }
  }
  catch(Exception &e)
  {
    ifile.close();
    throw Exception(e, "FileResults::parseFile(): error at line " + Parser::int2string(currline));
  }
  catch(...)
  {
    ifile.close();
    throw Exception("FileResults::parseFile(): error at line " + Parser::int2string(currline));
  }
}

//===========================================================================
// parseLine
//===========================================================================
void ccruncher::FileResults::parseLine(char *line) throw(Exception)
{
  char *tmp = ltrim(line);

  if (strlen(line) == 0)
  {
    // nothing to do
  }
  else if (isComment(tmp))
  {
    tmp = tmp+1;
    
    if (isMapValue(tmp))
    {
      string key = extractKey(tmp);
      string value = extractValue(tmp);
      info[key] = value;
    }
  }
  else
  {
    if (data == NULL)
    {
      ncolumns = getNumColumns();
      data = new vector<double>[ncolumns];
      cout << "numColumns=" << ncolumns << endl;
    }

    extractData(tmp);
  }
}

//===========================================================================
// ltrim
//===========================================================================
char * ccruncher::FileResults::ltrim(char *tira)
{
  assert(tira != NULL);

  int n = strlen(tira);

  for(int i=0;i<n;i++)
  {
    switch(tira[i])
    {
      case ' ': break;
      case '\t': break;
      default: return &tira[i];
    }
  }
  
  return &tira[n];
}

//===========================================================================
// isComment
// check that first character distinct that ' ' or '\t' is '#'
//===========================================================================
bool ccruncher::FileResults::isComment(char *tira)
{
  if(tira[0] == '#') return true;
  else return false;
}

//===========================================================================
// isMapValue
//===========================================================================
bool ccruncher::FileResults::isMapValue(char *tira)
{
  char *epos = strchr(tira, '=');
  char *ppos = strchr(tira, '.');
  
  if (ppos != NULL && epos != NULL && ppos < epos) return true;
  else return false;
}

//===========================================================================
// extractKey
//===========================================================================
string ccruncher::FileResults::extractKey(char *tira) throw(Exception)
{
  char *epos = strchr(tira, '=');
  
  if(epos == NULL)
  {
    throw Exception("FileResults::extractKey(): non valid key-value pair");
  }
  else
  {
    string ret(tira, epos-tira-1);
    return Utils::trim(ret);
  }
}

//===========================================================================
// extractValue
//===========================================================================
string ccruncher::FileResults::extractValue(char *tira) throw(Exception)
{
  char *epos = strchr(tira, '=');
  
  if(epos == NULL)
  {
    throw Exception("FileResults::extractValue(): non valid key-value pair");
  }
  else
  {
    string ret(epos+1);
    return Utils::trim(ret);
  }
}

//===========================================================================
// extractData
//===========================================================================
void ccruncher::FileResults::extractData(char *tira) throw(Exception)
{
  char *tmp = tira;
  double val;
  
  for (int i=0;i<ncolumns;i++)
  {
    tmp = ltrim(tmp);
    tmp = parseDouble(tmp, &val);
    data[i].push_back(val);
//cout << i << " -> " << val << endl;
  }

  tmp = ltrim(tmp);
  if (strlen(tmp) != 0)
  {
    cout << "remanent=" << tmp << endl;
    throw Exception("FileResults::extractData(): unexpected number of columns");
  }
}

//===========================================================================
// parseDouble
//===========================================================================
char* ccruncher::FileResults::parseDouble(char *pnum, double *val) throw(Exception)
{
  char *pstr = NULL;

  // inicialitzem estat errors numerics
  errno = 0;

  // parsejem el numero
  *val = strtod(pnum, &pstr); //atof(str.c_str());

  // comprovem si es tracta de un double
  if (errno != 0 || pstr == pnum || strlen(pnum) == 0)
  {
    throw Exception("FileResults::parseDouble(): invalid double value: " + string(pnum));
  }
  else
  {
    return pstr;
  }
}

//===========================================================================
// getNumColumns
//===========================================================================
int ccruncher::FileResults::getNumColumns() throw(Exception)
{
  string xtype("param.type");
  string xntranches("param.ntranches");
  
  string stype = getInfo(xtype);
  string sntranches = getInfo(xntranches);
  int vntranches = Parser::intValue(sntranches);

  if (stype == "values")
  {
    return vntranches+1;
  }
  else if (stype == "ratings")
  {
    string xnumratings("param.numratings");
    string snumratings = getInfo(xnumratings);
    int vnumratings = Parser::intValue(snumratings);
    return vnumratings*vntranches+1;
  }
  else
  {
    throw Exception("FileResults::getNumColumns(): unknow type value");
  }
}

//===========================================================================
// getInfo
//===========================================================================
string ccruncher::FileResults::getInfo(string &key) throw(Exception)
{
  map<string,string>::iterator pos = info.find(key);

  if (pos != info.end())
  {
    return pos->second;
  }
  else
  {
    throw Exception("FileResults::getInfo(): key " + key + " not found");
  }
}

//===========================================================================
// getColumn
//===========================================================================
vector<double>* ccruncher::FileResults::getColumn(int icol) throw(Exception)
{
  assert(data != NULL);
  
  if (icol < 0 || icol >= ncolumns)
  {
    throw Exception("FileResults::getColumn(): column index out of range");
  }
  else
  {
    return &data[icol];
  }
}

//===========================================================================
// getColumn
//===========================================================================
vector<double>* ccruncher::FileResults::getColumns()
{
  return data;
}

//===========================================================================
// getDate
//===========================================================================
Date ccruncher::FileResults::getDate(int icol) throw(Exception)
{
  string xtoken;
  
  xtoken = "column.";
  xtoken += Parser::int2string(icol+1);
  xtoken +=".date";
  string tira = getInfo(xtoken);
cout << "parsing " << tira << " ..." << endl;
  return Parser::dateValue(tira);
}

//===========================================================================
// getXMin
//===========================================================================
double ccruncher::FileResults::getXMin()
{
  assert(data != NULL);
  
  double ret = data[0][0];
  
  for(int i=1;i<ncolumns;i++)
  {
    for(unsigned int j=0;j<data[i].size();j++)
    {
      if (data[i][j] < ret) ret = data[i][j];
    }
  }
  
  return ret;
}

//===========================================================================
// getXMax
//===========================================================================
double ccruncher::FileResults::getXMax()
{
  assert(data != NULL);
  
  double ret = data[0][0];
  
  for(int i=1;i<ncolumns;i++)
  {
    for(unsigned int j=0;j<data[i].size();j++)
    {
      if (data[i][j] > ret) ret = data[i][j];
    }
  }
  
  return ret;
}
