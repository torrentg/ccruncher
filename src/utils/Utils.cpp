
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <locale>
#include <libgen.h>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <cassert>
#include "utils/Exception.hpp"
#include "utils/Utils.hpp"

#ifdef _WIN32
  #include <windows.h>
#elif MACOS
  #include <sys/param.h>
  #include <sys/sysctl.h>
#else
  #include <unistd.h>
#endif

using namespace std;

// path separator (platform-dependent)
const string ccruncher::Utils::pathSeparator =
#ifdef _WIN32
  string("\\");
#else
  string("/");
#endif

/**************************************************************************//**
 * @details Format current timestamp as <code>'dd/MM/yyyy hh:mm:ss'</code>.
 * @return Current timestamp formated as string.
 */
string ccruncher::Utils::timestamp()
{
  time_t now = time(nullptr);
  tm lt = *(localtime(&now));

  char aux[80];
  aux[19] = 0;

  snprintf(aux, 80, "%02d/%02d/%04d %02d:%02d:%02d", lt.tm_mday, lt.tm_mon+1,
           lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return string(aux);
}

/**************************************************************************//**
 * @details This value can be used to seed a RNG (random number generator).
 *          It has microseconds resolution.
 * @return Value based on current time.
 */
unsigned long ccruncher::Utils::trand()
{
  timeval tv;
  gettimeofday(&tv,nullptr);
  return (unsigned long)(tv.tv_sec)*1000000UL + (unsigned long)(tv.tv_usec);
}

/**************************************************************************//**
 * @details Compilation options are:
 *          <ul>
 *            <li> debug. <code>\#define NDEBUG</code></li>
 *            <li> profiler. <code>\#define PROFILER</code></li>
 *          </ul>.
 *          String is formated using this pattern:
 *          <code>'debug[enabled] | profiler[disabled]'</code>.
 * @return A string with compilation options.
 */
string ccruncher::Utils::getCompilationOptions()
{
  string ret = "";

  // debug option
  ret += "debug";
#ifdef NDEBUG
  ret += "[disabled] | ";
#else
  ret += "[enabled] | ";
#endif

  // profiler option
  ret += "profiler";
#ifdef PROFILER
  ret += "[enabled] ";
#else
  ret += "[disabled] ";
#endif

  return ret;
}

/**************************************************************************//**
 * @details This method is platform dependent.
 *          Supported platforms are: Unix/Windows/Mac.
 * @return The number of cores.
 */
int ccruncher::Utils::getNumCores()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, nullptr, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, nullptr, 0);
        if(count < 1) { count = 1; }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

/**************************************************************************//**
 * @details Tokens are delimited by a character, and user can provide more
 *          than one character.
 * @param[in] str String to tokenize.
 * @param[out] tokens List of tokens found.
 * @param[in] delimiters Set of characters used as delimiters (eg. ',;').
 * @param[in] trim Trim and remove consecutive delimiters
 */
void ccruncher::Utils::tokenize(const string& str, vector<string>& tokens, const string& delimiters, const bool trim)
{
  string::size_type lastPos = (trim?str.find_first_not_of(delimiters, 0):0);
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (lastPos < str.length())
  {
    assert(lastPos <= pos);
    if (pos > str.length()) pos = str.length();
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = (trim?str.find_first_not_of(delimiters, pos):pos+1);
    pos = str.find_first_of(delimiters, lastPos);
  }
  if (!trim && !str.empty() && delimiters.find_first_of(str.back()) != string::npos) {
    tokens.push_back("");
  }
}

/**************************************************************************//**
 * @details Removes starting and ending space characters.
 * @param[in] str String to be trimmed.
 * @return String trimmed.
 */
string ccruncher::Utils::trim(const string &str)
{
  if (str.length() == 0) return "";

  int pos1 = str.length()-1;
  for(int i=0; i<(int)str.length(); i++) {
    if (!isspace(str[i])) {
      pos1=i;
      break;
    }
  }

  int pos2 = 0;
  for(int i=(int)str.length()-1; i>=0; i--) {
    if (!isspace(str[i])) {
      pos2=i;
      break;
    }
  }

  if (pos2 < pos1) {
    return "";
  }
  else if (pos1 == pos2) {
    if (isspace(str[pos1])) return "";
    else return str.substr(pos1,1);
  }
  else {
    return str.substr(pos1,pos2-pos1+1);
  }
}

/**************************************************************************//**
 * @param[in] str String to be uppercased.
 * @return String uppercased.
 */
string ccruncher::Utils::uppercase(const string &str)
{
  string res = str;
  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) toupper);
  return res;
}

/**************************************************************************//**
 * @param[in] str String to be lowercased.
 * @return String lowercased.
 */
string ccruncher::Utils::lowercase(const string &str)
{
  string res = str;
  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) tolower);
  return res;
}

/**************************************************************************//**
 * @details In windows replaces '/' by '\'.
 * @param[in] str File path to normalize.
 * @return String normalized.
 */
string ccruncher::Utils::toNativeSeparators(const std::string &str)
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
bool ccruncher::Utils::isAbsolutePath(const std::string &path)
{
  if (path.empty()) {
    return false;
  }

#ifdef _WIN32
  string str = toNativeSeparators(path);
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
 * @return Working directory without ending with pathSeparator.
 * @throw Exception Error retrieving working directory.
 */
string ccruncher::Utils::getWorkDir()
{
  char tempname[1024];
  char *ret = getcwd(tempname, 1024);

  if (ret != tempname) {
    throw Exception("unable to retrieve current working directory");
  }
  else {
    return string(ret);
  }
}

/**************************************************************************//**
 * @details Expands  all symbolic links and resolves references to /./
 *          and /../ and extra '/' characters to produce a canonicalized
 *          absolute pathname.
 * @param[in] path Path to a file or directory.
 * @return Canonicalized pathname (with non-ending slash).
 * @throw Exception Error reading path (eg. path non-existent)
 */
string ccruncher::Utils::realpath(const string &path)
{
#ifdef _WIN32
  char full_path[MAX_PATH];
  GetFullPathNameA(path.c_str(), MAX_PATH, full_path, NULL);
  return string(full_path);
#else
  char full_path[PATH_MAX];
  char *aux = ::realpath(path.c_str(), full_path);
  if (aux == nullptr) {
    throw Exception(strerror(errno));
  }
  else {
    return string(full_path);
  }
#endif
}

/**************************************************************************//**
 * @details Check existence trying to acces to directory.
 * @param[in] dirname Directory path.
 * @return Flag indicating if directory exist or can be accessed.
 */
bool ccruncher::Utils::existDir(const std::string &dirname)
{
  errno = 0;
  DIR *tmp = opendir(dirname.c_str());

  if (tmp == nullptr) {
    if (errno == EACCES) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    closedir(tmp);
    return true;
  }
}

/**************************************************************************//**
 * @details In Unix/Linux the directory permissions are '0777'.
 * @param[in] dirname Directory path.
 * @throw Exception Error creating directory.
 */
void ccruncher::Utils::makeDir(const std::string &dirname)
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
  if (aux != 0) {
    string code = "[" + to_string(errno) + "]";
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
void ccruncher::Utils::checkFile(const string &pathname, const string &smode)
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
  if (aux != 0) {
    throw Exception("file " + pathname + " fails " + smode + " check");
  }
}

/**************************************************************************//**
 * @details Strip last component from file name.
 * @param[in] pathname Path to file.
 * @result Directory path.
 */
string ccruncher::Utils::dirname(const string &pathname)
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
  size_t len = pathname.length()+1;
  char *aux = new char[len];
  std::strncpy (aux, pathname.c_str(), len);
  string ret(::dirname(aux));
  delete[] aux;
  return ret;
#endif
}

/**************************************************************************//**
 * @details Strip directory from filenames.
 * @param[in] pathname Path to file.
 * @result File name (with extension).
 */
string ccruncher::Utils::filename(const string &pathname)
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
  size_t len = pathname.length()+1;
  char *aux = new char[len];
  std::strncpy (aux, pathname.c_str(), len);
  string ret(::basename(aux));
  delete[] aux;
  return ret;
#endif
}

/**************************************************************************//**
 * @param[in] filename Path to file.
 * @result File size in bytes (0 if not found).
 */
size_t ccruncher::Utils::filesize(const string &filename)
{
  std::ifstream f;
  f.open(filename.c_str(), std::ios_base::binary | std::ios_base::in);
  if (!f.good() || f.eof() || !f.is_open()) { return 0; }
  f.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f.tellg();
  f.seekg(0, std::ios_base::end);
  return static_cast<size_t>(f.tellg() - begin_pos);
}

/**************************************************************************//**
 * @details Returns bytes as string (B, KB, MB, GB)
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Utils::bytesToString(const size_t val)
{
  std::ostringstream oss;

  if (val < 1024) {
    oss << val << " B";
  }
  else if (val < 1024*1024) {
    double kbs = double(val)/double(1024);
    oss << fixed << setprecision(2) << kbs << " KB";
  }
  else {
    double mbs = double(val)/double(1024*1024);
    oss << fixed << setprecision(2) << mbs << " MB";
  }

  return oss.str();
}

/**************************************************************************//**
 * @param[in] millis Numbers of elapsed milliseconds.
 * @return Elapsed time formated like hh:mm:ss.mmm
 */
string ccruncher::Utils::millisToString(long millis)
{
  long ms = abs(millis);

  // computing hours
  int hh = ms/(60*60*1000);
  ms -= hh*(60*60*1000);

  // computing minutes
  int min = ms/(60*1000);
  ms -= min*(60*1000);

  // computing seconds
  int sec = ms/1000;
  ms -= sec*1000;

  // formating elapsed time
  char buf[20];
  snprintf(buf, 20, "%02d:%02d:%02d.%03d", hh, min, sec, int(ms));

  // exit function
  return string(buf);
}

