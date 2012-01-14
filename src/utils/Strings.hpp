
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

#ifndef _Strings_
#define _Strings_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <string>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Strings
{

  private:

    // non-instantiable class
    Strings() {};

  public:

    // tokenize a string
    static void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
    // right trim
    static string rtrim(string s);
    // left trim
    static string ltrim(string s);
    // trim
    static string trim(string s);
    // converts to upper case
    static string uppercase(string str);
    // converts to lower case
    static string lowercase(string str);
    // creates a filler with the given char
    static string filler(int, char);
    // returns a blank string with the given length
    static string blanks(int);
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
