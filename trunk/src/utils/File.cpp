
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <locale>
#include <libgen.h>
#include <cstdio>
#include <fstream>
#include <cerrno>
#include "utils/Format.hpp"
#include "utils/File.hpp"

using namespace std;

// path separator (platform-dependent)
const string ccruncher::File::pathSeparator =
#ifdef _WIN32
  string("\\");
#else
  string("/");
#endif

/**************************************************************************//**
 * @details In windows replaces '/' by '\'.
 * @param[in] str File path to normalize.
 * @return String normalized.
 */
string ccruncher::File::normalize(const std::string &str)
{
#ifdef _WIN32
  string ret = str;
  for(size_t i=0; i<str.length(); i++) {
    if (ret[i] == '/') ret[i] = '\\';
  }
  return ret;
#else
  return str;
#endif
}

/**************************************************************************//**
 * @todo Use function PathIsRelative in windows.h
 * @param[in] path File path to check.
 * @return true = is absolute, false = otherwise.
 */
bool ccruncher::File::isAbsolutePath(const std::string &path)
{
  if (path.size() <= 0) {
    return false;
  }

#ifdef _WIN32
  string str = normalize(path);
  if (str.substr(0,1) == pathSeparator) { // \users, \\net-pc1\users
    return true;
  }
  else if (str.length() >= 3 && isalpha(str.c_str()[0]) && str.substr(2,1) == pathSeparator) { // C:\users
    return true;
  }
  else {
    return false;
  }
#else
  if (path.substr(0,1) == pathSeparator) { // /home
    return true;
  }
  else {
    return false;
  }
#endif
}

/**************************************************************************//**
 * @return Working directory ending with pathSeparator (eg. '/').
 * @throw Exception Error retrieving working directory.
 */
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
      if (aux.substr(aux.length()-1, 1) != pathSeparator) {
        aux = aux + pathSeparator;
      }

      return aux;
    }
  }
  catch(...)
  {
    throw Exception("unable to retrieve current working directory");
  }
}

/**************************************************************************//**
 * @details Normalize a directory path applying the following rules:
 *          - replaces incorrect path separator based on platform
 *          - relative path -> absolute path
 *          - appends ending path separator
 *          example: input=./dir1/dir2 -> output=/workdir/dir1/dir2/
 * @param[in] path Path to a directory.
 * @return Dir path normalized.
 * @throw Exception Non valid file path.
 */
string ccruncher::File::normalizePath(const string &path) throw(Exception)
{
  string ret = normalize(path);

  if (path.length() == 0)
  {
    throw Exception("error normalizing path: non valid path (length=0)");
  }

  if (ret.substr(0,1) != "." && !isAbsolutePath(ret))
  {
    ret = "." + pathSeparator + ret;
  }

  if (ret == ".")
  {
    ret = getWorkDir();
  }

  if (ret.substr(0,2) == ("." + pathSeparator))
  {
    ret = getWorkDir() + ret.substr(2);
  }

  if (ret.substr(0,3) == (".." + pathSeparator))
  {
    ret = getWorkDir() + ret;
  }

  if (ret.substr(ret.length()-1, 1) != pathSeparator)
  {
    ret = ret + pathSeparator;
  }

  return ret;
}

/**************************************************************************//**
 * @details Check existence trying to acces to directory.
 * @param[in] dirname Directory path.
 * @return Flag indicating if directory exist or can be accessed.
 */
bool ccruncher::File::existDir(const std::string &dirname)
{
  DIR *tmp;

  errno = 0;
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

/**************************************************************************//**
 * @details In Unix/Linux the directory permissions are '0777'.
 * @param[in] dirname Directory path.
 * @throw Exception Error creating directory.
 */
void ccruncher::File::makeDir(const std::string &dirname) throw(Exception)
{
  int aux;

  errno = 0;

  // creating directory
#ifdef _WIN32
  aux = mkdir(dirname.c_str());
#else
  aux = mkdir(dirname.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif

  // checking creation
  if (aux != 0)
  {
    string code = "[" + Format::toString(errno) + "]";
    code = (errno==EACCES?"[EACCES]":code);
    code = (errno==EEXIST?"[EEXIST]":code);
    code = (errno==EMLINK?"[EMLINK]":code);
    code = (errno==ENOSPC?"[ENOSPC]":code);
    code = (errno==EROFS?"[EROFS]":code);
    throw Exception("unable to create directory " + dirname + " [errno=" + code + "]");
  }
}

/**************************************************************************//**
 * @details Check is done accessing to file with the given read-write mode.
 * @param[in] pathname File path.
 * @param[in] smode Allowed modes: r=read, w=write, rw=read+write
 * @throw Exception invalid mode or file access error.
 */
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

/**************************************************************************//**
 * @details Strip last component from file name.
 * @param[in] pathname Path to file.
 * @result Directory path.
 */
string ccruncher::File::dirname(const string &pathname)
{
#ifdef _WIN32
  char buf[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  _splitpath(pathname.c_str(), drive, dir, fname, ext);
  snprintf(buf, _MAX_PATH, "%s%s", drive, dir);
  return string(buf);
#else
  return string(::dirname(const_cast<char*>(pathname.c_str())));
#endif
}

/**************************************************************************//**
 * @details Strip directory from filepath.
 * @param[in] pathname Path to file.
 * @result File name (with extension).
 */
string ccruncher::File::filename(const string &pathname)
{
#ifdef _WIN32
  char buf[_MAX_PATH];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  _splitpath(pathname.c_str(), drive, dir, fname, ext);
  snprintf(buf, _MAX_PATH, "%s%s", fname, ext);
  return string(buf);
#else
  return string(::basename(const_cast<char*>(pathname.c_str())));
#endif
}

/**************************************************************************//**
 * @param[in] path Path to file.
 * @param[in] name File name.
 * @result Filepath.
 */
string ccruncher::File::filepath(const string &path, const string &name)
{
  if (isAbsolutePath(name))
  {
    return normalize(name);
  }
  else
  {
    string str1 = normalize(path);
    string ret = ((str1=="." || str1=="."+pathSeparator)?"":str1);
    if (ret.length() > 0 && ret.substr(ret.length()-1,1) != pathSeparator) {
      ret += pathSeparator;
    }
    ret += normalize(name);
    return ret;
  }
}

/**************************************************************************//**
 * @param[in] filename Path to file.
 * @result File size in bytes (0 if not found).
 */
size_t ccruncher::File::filesize(const string &filename)
{
  std::ifstream f;
  f.open(filename.c_str(), std::ios_base::binary | std::ios_base::in);
  if (!f.good() || f.eof() || !f.is_open()) { return 0; }
  f.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f.tellg();
  f.seekg(0, std::ios_base::end);
  return static_cast<size_t>(f.tellg() - begin_pos);
}

