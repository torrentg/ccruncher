
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

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

    //! Path separator symbol
    static const std::string pathSeparator;

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
                         const std::string &delimiters=" ", bool trim=false);
    //! Trims a string
    static std::string trim(const std::string &str);
    //! Converts a string to upper case
    static std::string uppercase(const std::string &str);
    //! Converts a string to lower case
    static std::string lowercase(const std::string &str);
    //! Convert '/' separators to underlying OS separators
    static std::string toNativeSeparators(const std::string &str);
    //! Canonicalize absolute pathname
    static std::string realpath(const std::string &path);
    //! Returns working directory
    static std::string getWorkDir();
    //! Indicates if exist a directory
    static bool existDir(const std::string &);
    //! Create a directory
    static void makeDir(const std::string &);
    //! Indicates if a path is absolute
    static bool isAbsolutePath(const std::string &);
    //! Check rw file status
    static void checkFile(const std::string &pathname, const std::string &smode);
    //! Directory part of a filepath
    static std::string dirname(const std::string &pathname);
    //! File part of a filepath
    static std::string filename(const std::string &pathname);
    //! Return file size in bytes
    static size_t filesize(const std::string &filename);
    //! Returns bytes as string (B, KB, MB)
    static std::string bytesToString(const size_t val);
    //! Format seconds in format hh:mm:ss.mmm
    static std::string millisToString(long millis);

};

} // namespace
