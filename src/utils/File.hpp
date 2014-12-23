
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

#ifndef _File_
#define _File_

#include <string>
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Functions related to file management.
 *
 * @details Some of the implemented methods are platform dependent. They
 *          are tested in the Windows and Linux platform, but no on IOS.
 */
class File
{

  public:

    //! Path separator symbol
    static const std::string pathSeparator;

  private:

    //! Normalize a string
    static std::string normalize(const std::string &);

  public:

    //! Normalize a directory path
    static std::string normalizePath(const std::string &path);
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
    //! Create a file path using path and a file name
    static std::string filepath(const std::string &path, const std::string &name);
    //! Return file size in bytes
    static size_t filesize(const std::string &filename);
    //! Returns bytes as string (B, KB, MB)
    static std::string bytesToString(const size_t val);

};

} // namespace

#endif

