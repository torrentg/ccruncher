
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

#ifndef _Parser_
#define _Parser_

#include <string>
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Functions to parse usual types (int, double, date, etc.)
 */
class Parser
{

  private:

    //! Evalue a numeric expression without variables
    static double eval(const char *);

  public:

    //! Convert string to int
    static int intValue(const char *);
    //! Convert string to long
    static long longValue(const char *);
    //! Convert string to unsigned long
    static unsigned long ulongValue(const char *);
    //! Convert string to double
    static double doubleValue(const char *);
    //! Convert string to date
    static Date dateValue(const char *);
    //! Convert string to bool
    static bool boolValue(const char *);

    //! Convert string to int
    static int intValue(const std::string &str) { return intValue(str.c_str()); }
    //! Convert string to long
    static long longValue(const std::string &str) { return longValue(str.c_str()); }
    //! Convert string to unsigned long
    static unsigned long ulongValue(const std::string &str) { return ulongValue(str.c_str()); }
    //! Convert string to double
    static double doubleValue(const std::string &str) { return doubleValue(str.c_str()); }
    //! Convert string to date
    static Date dateValue(const std::string &str) { return dateValue(str.c_str()); }
    //! Convert string to bool
    static bool boolValue(const std::string &str) { return boolValue(str.c_str()); }

};

} // namespace

#endif

