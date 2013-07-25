
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class File
{

  public:

    // path separator symbol
    static const std::string pathSeparator;

  private:

    // normalize a string
    static std::string normalize(const std::string &);

  public:

    // normalize path
    static std::string normalizePath(const std::string &) throw(Exception);
    // returns working directory
    static std::string getWorkDir() throw(Exception);
    // indicates if exist a directory
    static bool existDir(const std::string &);
    // create a directory
    static void makeDir(const std::string &) throw(Exception);
    // indicates if a path is absolute
    static bool isAbsolutePath(const std::string &);
    // check file status
    static void checkFile(const std::string &pathname, const std::string &smode) throw(Exception);
    // directory part of a filepath
    static std::string dirname(const std::string &pathname);
    // file part of a filepath
    static std::string filename(const std::string &pathname);
    // create a file path using path and a file name
    static std::string filepath(const std::string &path, const std::string &name);
    // return file size in bytes
    static size_t filesize(const std::string &filename);
    
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
