
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cctype>
#include "inference/Configuration.hpp"
#include <cassert>

#define TOKEN_COMMENT1 "//"
#define TOKEN_COMMENT2_OPEN "/*"
#define TOKEN_COMMENT2_CLOSE "*/"
#define TOKEN_ASSIGNMENT "="
#define TOKEN_ARRAY_OPEN '['
#define TOKEN_ARRAY_CLOSE ']'
#define TOKEN_COL_SEPARATOR ","
#define TOKEN_ROW_SEPARATOR ";"

using namespace std;
using namespace ccruncher;
using namespace ccruncher_inf;

//=============================================================
// constructor
//=============================================================
ccruncher_inf::Configuration::Configuration(const string &filename) throw(Exception)
{
  load(filename);
}

//=============================================================
// removeComments
//=============================================================
void ccruncher_inf::Configuration::removeComments(string &line)
{
  if (mcomment)
  {
    size_t pos = line.find(TOKEN_COMMENT2_CLOSE);
    if (pos != string::npos) {
      line.erase(0, pos+2);
      mcomment = false;
      removeComments(line);
    }
    else {
      line.erase();
    }
  }
  else
  {
    size_t pos1 = line.find(TOKEN_COMMENT1);
    size_t pos2 = line.find(TOKEN_COMMENT2_OPEN);
    if (pos1 != string::npos && (pos2 == string::npos || pos1 < pos2)) {
      line.erase(pos1);
      removeComments(line);
    }
    else if (pos2 != string::npos) {
      string part1 = line.substr(0,pos2);
      string part2 = line.substr(pos2+1);
      mcomment = true;
      removeComments(part2);
      line = part1 + part2;
    }
  }
}

//=============================================================
// load configuration files
//=============================================================
void ccruncher_inf::Configuration::load(const string &filename) throw(Exception)
{
  ifstream file;
  file.open(filename.c_str());
  if (!file) throw Exception("can't open file '" + filename + "'");

  params.clear();
  openbrace = false;
  mcomment = false;
  haskey = false;
  int numline = 0;
  string line = "";

  try
  {
      while(getline(file, line))
      {
        numline++;
        removeComments(line);
        if (line.length() == 0) continue;
        size_t pos = line.find(TOKEN_ASSIGNMENT);
        if (pos != string::npos)
        {
          string key = trim(line.substr(0, pos));
          if (key.length() == 0) {
            throw Exception("invalid key (void)");
          }
          for(size_t i=0; i<key.length(); i++)
            if (!isalpha(key[i]) && key[i] != '.') throw Exception("invalid key '" + key + "'");
          string value = line.substr(min(pos+1,line.length()-1));
          if (find(key) >= 0) {
            throw Exception("repeated key '" + key + "'");
          }
          else {
            haskey = true;
            params.push_back(pair<string,vector<vector<string> > >(key,vector<vector<string> >()));
            params.back().second.push_back(vector<string>());
            parse1(value);
          }
        }
        else
        {
          parse1(line);
        }
      }
      if (mcomment) throw Exception("multi-line comment not closed");

      // remove ending void rows
      for(size_t k=0; k<params.size(); k++)
      {
        if (!params[k].second.empty() && params[k].second.back().empty()) {
          params[k].second.pop_back();
        }
      }

  }
  catch(Exception &e)
  {
    throw Exception(e, "syntax error at line " + T_as_string<int>(numline));
  }
  catch(...)
  {
    throw Exception("syntax error at line " + T_as_string<int>(numline));
  }
}

//===========================================================================
// parse1
//===========================================================================
void ccruncher_inf::Configuration::parse1(const string &s) throw(Exception)
{
  string str = trim(s);
  if (str.length() == 0) return;
  if (!haskey) throw Exception("syntax error (key not found)");

  if (str[0] == TOKEN_ARRAY_OPEN) {
    if (openbrace) throw Exception("syntax error (nested braces)");
    else {
      openbrace = true;
      str.erase(0,1);
    }
  }

  if (str[str.length()-1] == TOKEN_ARRAY_CLOSE) {
    if (!openbrace) throw Exception("syntax error (close brace)");
    else {
      str.erase(str.length()-1,1);
      parse2(str);
      openbrace = false;
      haskey = false;
    }
  }
  else {
    parse2(str);
  }
}

//===========================================================================
// parse2
//===========================================================================
void ccruncher_inf::Configuration::parse2(const string &str) throw(Exception)
{
  size_t pos1 = 0;
  size_t pos2 = str.find_first_of(TOKEN_ROW_SEPARATOR, pos1);
  while (pos2 != string::npos) {
      string token = str.substr(pos1, pos2-pos1);
      parse3(token);
      if (params.back().second.size() > 0) {
          params.back().second.push_back(vector<string>());
      }
      pos1 = pos2+1;
      pos2 = str.find_first_of(TOKEN_ROW_SEPARATOR, pos1);
  }
  if (pos1 < str.length()) {
      parse3(str.substr(pos1));
  }
}

//===========================================================================
// parse3
//===========================================================================
void ccruncher_inf::Configuration::parse3(const string &s) throw(Exception)
{
  string str = trim(s);
  if (str.length() == 0) return;

  size_t pos1 = 0;
  size_t pos2 = str.find_first_of(TOKEN_COL_SEPARATOR, pos1);
  while (pos2 != string::npos) {
      string token = str.substr(pos1, pos2-pos1);
      if (!haskey) throw Exception("key not found");
      params.back().second.back().push_back(trim(token));
      if (!openbrace) haskey = false;
      pos1 = pos2+1;
      pos2 = str.find_first_of(TOKEN_COL_SEPARATOR, pos1);
  }
  if (pos1 < str.length()) {
      if (!haskey) throw Exception("key not found");
      params.back().second.back().push_back(trim(str.substr(pos1)));
      if (!openbrace) haskey = false;
  }
}

//===========================================================================
// trim
//===========================================================================
string ccruncher_inf::Configuration::trim(const string& str)
{
    string s = str;
    // Remove leading and trailing whitespace
    static const char whitespace[] = " \n\t\v\r\f";
    s.erase(0, s.find_first_not_of(whitespace));
    s.erase(s.find_last_not_of(whitespace) + 1U);
    return s;
}

//===========================================================================
// find an entry by key
//===========================================================================
int ccruncher_inf::Configuration::find(const string &key) const
{
    for(size_t i=0; i<params.size(); i++) {
        if (params[i].first == key) return i;
    }
    return -1;
}

//=============================================================
// check if a key exists
//=============================================================
bool ccruncher_inf::Configuration::exists(const string &key) const
{
    return (find(key) >= 0);
}

//=============================================================
// check if entry is a value (eg. x1, or [x1])
//=============================================================
bool ccruncher_inf::Configuration::isValue(const std::string &key) const
{
    int pos = find(key);
    if (pos < 0) throw Exception("'" + key + "' key not found");
    const auto &value = params[pos].second;
    if (value.size() == 1 && value[0].size() == 1) return true;
    else return false;
}

//=============================================================
// check if entry is an array (eg. [x1, x2, x3], or [])
//=============================================================
bool ccruncher_inf::Configuration::isArray(const std::string &key) const
{
    int pos = find(key);
    if (pos < 0) throw Exception("'" + key + "' key not found");
    const auto &value = params[pos].second;
    if (value.size() == 1) return true;
    else return false;
}

//=============================================================
// check if entry is a matrix (eg. [x11, x12, x13; x21, x22, x23], or x11, or [x11], or [])
//=============================================================
bool ccruncher_inf::Configuration::isMatrix(const std::string &key) const
{
    int pos = find(key);
    if (pos < 0) throw Exception("'" + key + "' key not found");
    return true;
}

//=============================================================
// check if a key exists
//=============================================================
void ccruncher_inf::Configuration::save(const string &filename)
{
    ofstream output;
    output.open(filename.c_str());

    for(size_t i=0; i<params.size(); i++)
    {
        output << params[i].first << " = [";
        if (params[i].second.size() > 1) output << endl;
        for(size_t i1=0; i1 < params[i].second.size(); i1++)
        {
            if (i1 > 0) output << ";" << endl;
            for(size_t i2=0; i2 < params[i].second[i1].size(); i2++)
            {
                output << (i2>0?", ":"") << params[i].second[i1][i2];
            }
        }
        output << "]" << endl;
    }
}

