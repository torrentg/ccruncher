
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// Strings.hpp - Strings header
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (segregated from Utils.hpp)
//
//===========================================================================

#ifndef _Strings_
#define _Strings_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cassert>
#include <vector>
#include <string>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Strings
{

  public:

    static void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
    static string rtrim(string s);
    static string ltrim(string s);
    static string trim(string s);
    static string uppercase(string str);
    static string lowercase(string str);
    static string filler(int, char);
    static string blanks(int);
};

//---------------------------------------------------------------------------

//===========================================================================
// tolower manage int. Wrapper for manage chars
//===========================================================================
inline char ltolower (char ch) { return tolower(ch); }

//===========================================================================
// toupper manage int. Wrapper for manage chars
//===========================================================================
inline char ltoupper (char ch) { return toupper(ch); }

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
