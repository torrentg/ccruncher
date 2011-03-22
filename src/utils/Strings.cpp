
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#include <cctype>
#include <algorithm>
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// tokenize
//===========================================================================
void ccruncher::Strings::tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
     // Found a token, add it to the vector.
     tokens.push_back(str.substr(lastPos, pos - lastPos));
     // Skip delimiters.  Note the "not_of"
     lastPos = str.find_first_not_of(delimiters, pos);
     // Find next "non-delimiter"
     pos = str.find_first_of(delimiters, lastPos);
  }
}

//===========================================================================
// rtrim
//===========================================================================
string ccruncher::Strings::rtrim(string s)
{
  string::size_type pos = s.find_last_not_of(" \t\n");

  if (pos == string::npos) {
    return s;
  }
  else {
    return s.substr( 0, pos+1 );
  }
}

//===========================================================================
// ltrim
//===========================================================================
string ccruncher::Strings::ltrim(string s)
{
  string::size_type pos = s.find_first_not_of(" \t\n");

  if (pos == string::npos) {
    return s;
  }
  else {
    return s.substr( pos, s.size()-pos );
  }
}

//===========================================================================
// trim
//===========================================================================
string ccruncher::Strings::trim(string s)
{
  return ltrim(rtrim(s));
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Strings::uppercase(string str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) toupper);

  return res;
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Strings::lowercase(string str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) tolower);

  return res;
}

//===========================================================================
// filler
//===========================================================================
string ccruncher::Strings::filler(int n, char c)
{
  return string(n, c);
}

//===========================================================================
// blanks
//===========================================================================
string ccruncher::Strings::blanks(int n)
{
  return filler(n, ' ');
}
