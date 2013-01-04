
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

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class File
{

  private:

    // non-instantiable class
    File() {}

  private:

    // normalize a string
    static string normalize(const string &);

  public:

    // normalize path
    static string normalizePath(const string &) throw(Exception);
    // returns working directory
    static string getWorkDir() throw(Exception);
    // indicates if exist a directory
    static bool existDir(const string &);
    // create a directory
    static void makeDir(const string &) throw(Exception);
    // indicates if a path is absolute
    static bool isAbsolutePath(const string &);
    // check file status
    static void checkFile(const string &pathname, const string &smode) throw(Exception);
    // directory part of a filepath
    static string dirname(const string &pathname);
    // file part of a filepath
    static string filename(const string &pathname);
    // create a file path using path and a file name
    static string filepath(const string &path, const string &name);
    // return file size in bytes
    static size_t filesize(const string &filename);
    
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
