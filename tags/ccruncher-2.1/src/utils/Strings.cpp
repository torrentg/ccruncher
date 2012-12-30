
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
// trim
//===========================================================================
string ccruncher::Strings::trim(const string &s)
{
  if (s.length() == 0) return "";

  int pos1 = s.length()-1;
  for(int i=0; i<(int)s.length(); i++) {
    if (!isspace(s[i])) {
      pos1=i;
      break;
    }
  }

  int pos2 = 0;
  for(int i=(int)s.length()-1; i>=0; i--) {
    if (!isspace(s[i])) {
      pos2=i;
      break;
    }
  }

  if (pos2 < pos1) {
    return "";
  }
  else if (pos1 == pos2) {
    if (isspace(s[pos1])) return "";
    else return s.substr(pos1,1);
  }
  else {
    return s.substr(pos1,pos2-pos1+1);
  }
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Strings::uppercase(const string &str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) toupper);

  return res;
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Strings::lowercase(const string &str)
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
