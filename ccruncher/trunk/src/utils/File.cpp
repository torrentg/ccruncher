
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// File.cpp - File code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <cerrno>
#include "Parser.hpp"
#include "File.hpp"

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
      throw Exception("File::getWorkDirectory(): unable to get work directory");
    }
    else
    {
      return normalizePath(string(ret));
    }
  }
  catch(...)
  {
    throw Exception("File::getWorkDirectory(): unable to get work directory");
  }
}

//===========================================================================
// normalizePath
// input=./dir1/dir2 -> output=/workdir/dir1/dir2/
//===========================================================================
string ccruncher::File::normalizePath(string path) throw(Exception)
{
  string ret = path;

  if (path.length() == 0)
  {
    throw Exception("File::normalizePath(): non valid path (void)");
  }

  if (ret.substr(0,1) != "." && ret.substr(0,1) != "/")
  {
    ret = "./" + ret;
  }

  if (ret == ".")
  {
    ret = getWorkDir();
  }

  if (ret.substr(0,2) == "./")
  {
    ret = getWorkDir() + ret.substr(2);
  }

  if (ret.substr(0,3) == "../")
  {
    ret = getWorkDir() + ret;
  }

  if (ret.substr(ret.length()-1, 1) != "/")
  {
    ret = ret + "/";
  }

  return ret;
}

//===========================================================================
// existDir
//===========================================================================
bool ccruncher::File::existDir(string dirname)
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
void ccruncher::File::makeDir(string dirname) throw(Exception)
{
  int aux;
  
  // setting errno default
  errno = 0;
  
  // creating directory
  aux = mkdir(dirname.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
  
  // checking creation
  if (aux != 0)
  {
    string code = "[" + Parser::int2string(errno) + "]";
    code = (errno==EACCES?"[EACCES]":code);
    code = (errno==EEXIST?"[EEXIST]":code);
    code = (errno==EMLINK?"[EMLINK]":code);
    code = (errno==ENOSPC?"[ENOSPC]":code);
    code = (errno==EROFS?"[EROFS]":code);
    throw Exception("File::makeDir(): unable to create directory " + dirname + " " + code);
  }
}

//===========================================================================
// checkFile
// allowed modes: r, w, rw
//===========================================================================
void ccruncher::File::checkFile(string pathname, string smode) throw(Exception)
{
  int aux;
  int mode = 0;

  // setting errno default
  errno = 0;

  // setting mode
  if (smode == "r") mode = R_OK;
  else if (smode == "w") mode = W_OK;
  else if (smode == "rw") mode = R_OK | W_OK;
  else throw Exception("File::checkFile(): panic. not allowed mode " + smode);

  // checking file
  aux = access(pathname.c_str(), mode);

  // checking return code
  if (aux != 0)
  {
    throw Exception("File::checkFile(): file " + pathname + " fails " + smode + " check");
  }
}
