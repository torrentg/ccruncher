
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#include <dirent.h>
#include <locale>
#endif
#include <cstdio>
#include <cerrno>
#include "utils/Format.hpp"
#include "utils/File.hpp"

// --------------------------------------------------------------------------

#ifdef _MSC_VER
  #define PATHSEPARATOR string("\\")
#else
  #define PATHSEPARATOR string("/")
#endif

//===========================================================================
// getWorkDirectory
//===========================================================================
bool ccruncher::File::isAbsolutePath(const string &str)
{
  if (str.size() <= 0) {
    return false;
  }

#ifdef _MSC_VER
  // windows style path
  if (str.substr(0,1) == PATHSEPARATOR) {
    return true;
  }
  else if (str.substr(0,2) == PATHSEPARATOR+PATHSEPARATOR) {
    return true;
  }
  else if (isalpha(str.substr(0,1).c_str()[0]) && str.substr(1,1) == PATHSEPARATOR) {
    return true;
  }
  else {
    return false;
  }
#else
  // unix style path
  if (str.substr(0,1) == PATHSEPARATOR) {
    return true;
  }
  else {
    return false;
  }
#endif
}

//===========================================================================
// getWorkDirectory
//===========================================================================
string ccruncher::File::getWorkDir() throw(Exception)
{
  char tempname[1024];

  try
  {
    char *ret = getcwd(tempname, 1024);

    if (ret != tempname)
    {
      throw Exception("unable to retrieve current working directory");
    }
    else
    {
      string aux = string(ret);

      // appending '/' at last position
      if (aux.substr(aux.length()-1, 1) != PATHSEPARATOR) {
        aux = aux + PATHSEPARATOR;
      }

      return aux;
    }
  }
  catch(...)
  {
    throw Exception("unable to retrieve current working directory");
  }
}

//===========================================================================
// normalizePath
// input=./dir1/dir2 -> output=/workdir/dir1/dir2/
//===========================================================================
string ccruncher::File::normalizePath(const string &path) throw(Exception)
{
  string ret = path;

  if (path.length() == 0)
  {
    throw Exception("error normalizing path: non valid path (length=0)");
  }

  if (ret.substr(0,1) != "." && !isAbsolutePath(ret))
  {
    ret = "." + PATHSEPARATOR + ret;
  }

  if (ret == ".")
  {
    ret = getWorkDir();
  }

  if (ret.substr(0,2) == "." + PATHSEPARATOR)
  {
    ret = getWorkDir() + ret.substr(2);
  }

  if (ret.substr(0,3) == ".." + PATHSEPARATOR)
  {
    ret = getWorkDir() + ret;
  }

  if (ret.substr(ret.length()-1, 1) != PATHSEPARATOR)
  {
    ret = ret + PATHSEPARATOR;
  }

  return ret;
}

//===========================================================================
// existDir
//===========================================================================
bool ccruncher::File::existDir(const string &dirname)
{
  DIR *tmp;

  // inicialitzem estat errors
  errno = 0;

  // inicialitzem estat errors numerics
  tmp = opendir(dirname.c_str());

  if (tmp == NULL)
  {
    if (errno == EACCES)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    closedir(tmp);
    return true;
  }
}

//===========================================================================
// makeDir
//===========================================================================
void ccruncher::File::makeDir(const string &dirname) throw(Exception)
{
  int aux;

  // setting errno default
  errno = 0;

  // creating directory
#ifdef _MSC_VER
  aux = mkdir(dirname.c_str());
#else
  aux = mkdir(dirname.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif

  // checking creation
  if (aux != 0)
  {
    string code = "[" + Format::int2string(errno) + "]";
    code = (errno==EACCES?"[EACCES]":code);
    code = (errno==EEXIST?"[EEXIST]":code);
    code = (errno==EMLINK?"[EMLINK]":code);
    code = (errno==ENOSPC?"[ENOSPC]":code);
    code = (errno==EROFS?"[EROFS]":code);
    throw Exception("unable to create directory " + dirname + " [errno=" + code + "]");
  }
}

//===========================================================================
// checkFile
// allowed modes: r, w, rw
//===========================================================================
void ccruncher::File::checkFile(const string &pathname, const string &smode) throw(Exception)
{
  int aux;
  int mode = 0;

  // setting errno default
  errno = 0;

  // setting mode
  if (smode == "r") mode = R_OK;
  else if (smode == "w") mode = W_OK;
  else if (smode == "rw") mode = R_OK | W_OK;
  else throw Exception("error checking file " + pathname + ": " + smode + " is not an allowed mode");

  // checking file
  aux = access(pathname.c_str(), mode);

  // checking return code
  if (aux != 0)
  {
    throw Exception("file " + pathname + " fails " + smode + " check");
  }
}

