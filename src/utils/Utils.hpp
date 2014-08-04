
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _Utils_
#define _Utils_

#include <string>
#include <vector>

namespace ccruncher {

/**************************************************************************//**
 * @brief   Utilities functions.
 */
class Utils
{

  private:
  
    //! Constructor. Non-instantiable class
    Utils() {}

  public:

    //! Returns current timestamp as string
    static std::string timestamp();
    //! Returns a human-readable string with the compilation options
    static std::string getCompilationOptions();
    //! Returns a numeric value based on current time
    static unsigned long trand();
    //! Returns the number of available processors
    static int getNumCores();
    //! Tokenize a string
    static void tokenize(const std::string &str, std::vector<std::string> &tokens,
                         const std::string &delimiters=" ");
    //! Trims a string
    static std::string trim(const std::string &str);
    //! Converts a string to upper case
    static std::string uppercase(const std::string &str);
    //! Converts a string to lower case
    static std::string lowercase(const std::string &str);

};

} // namespace

#endif

