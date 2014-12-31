
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#include <cstdio>
#include <chrono>
#include <cassert>
#include "kernel/IData.hpp"
#include "utils/Utils.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace std::chrono;
using namespace ccruncher;

#define BUFFER_SIZE 128*1024

/**************************************************************************//**
 * @see http://www.cplusplus.com/reference/streambuf/streambuf/
 * @param[in] s Streambuf where the trace will be written.
 */
ccruncher::IData::IData(streambuf *s) : logger(s), curfile(nullptr)
{
  hasmaintag = false;
  hasdefinestag = 0;
  title = "";
  description = "";
  stop = nullptr;
  cursize = 0;
  parse_portfolio = true;
}

/**************************************************************************/
ccruncher::IData::~IData()
{
  assert(curfile == nullptr);
  if (curfile != nullptr) gzclose(curfile);
}

/**************************************************************************//**
 * @param[in] f File name (including path).
 * @param[in] m List of defines defined by user (eg. using cmd or gui).
 * @param[in] s Variable to stop parser.
 * @param[in] p Parse whole input file (true), or exclude portfolio (false)
 * @throw Exception Error parsing input file.
 */
void ccruncher::IData::init(const std::string &f,
    const std::map<std::string,std::string> &m, bool *s, bool p)
{
  gzFile file = nullptr;
  filename = f;
  stop = s;
  parse_portfolio = p;

  try
  {
    size_t bytes = 0;

    if (filename == STDIN_FILENAME) {
      file = gzdopen(fileno(stdin), "rb");
      bytes = 0;
    }
    else {
      // gzFile reads both gziped and not-gziped files
      file = gzopen(filename.c_str(), "rb");
      bytes = Utils::filesize(filename);
    }

    if (file == nullptr) {
      throw Exception("can't open file '" + filename + "'");
    }

    mMutex.lock();
    curfile = file;
    cursize = bytes;
    mMutex.unlock();

    // big buffer to increase the speed of decompression
    gzbuffer(file, BUFFER_SIZE);
    parse(file, m);

    mMutex.lock();
    curfile = nullptr;
    mMutex.unlock();
    gzclose(file);
  }
  catch(...)
  {
    if (file != nullptr) {
      mMutex.lock();
      curfile = nullptr;
      mMutex.unlock();
      gzclose(file);
    }
    throw;
  }
}

/**************************************************************************//**
 * @param[in] file File to parse.
 * @param[in] m List of defines defined by user (eg. using cmd or gui).
 * @throw Exception Error parsing input file.
 */
void ccruncher::IData::parse(gzFile file, const map<string,string> &m)
{
  try
  {
    // output header
    logger << "reading input file" << flood('*') << endl;
    logger << indent(+1);

    // trace file info
    logger << "file name" << split << "["+filename+"]" << endl;
    if (filename != STDIN_FILENAME)
    {
      logger << "file size" << split << Utils::bytesToString(Utils::filesize(filename)) << endl;
    }

    // trace defines
    for(auto &it : m) {
      checkDefine(it.first, it.second);
      logger << "define (user defined)" << split << it.first+"="+it.second << endl;
    }

    // parsing
    auto t1 = steady_clock::now();
    ExpatParser parser;
    parser.setDefines(m);
    parser.parse(file, this, stop);
    auto t2 = steady_clock::now();
    long millis = duration_cast<milliseconds>(t2-t1).count();

    // trace info
    logger << "file checksum (adler32)" << split << parser.getChecksum() << endl;
    logger << "elapsed time parsing data" << split << Utils::millisToString(millis) << endl;
    logger << indent(-1);
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error parsing file '" + filename + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::IData::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"ccruncher") && !hasmaintag) {
    hasmaintag = true;
  }
  else if (!hasmaintag) {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
  else if (isEqual(tag,"title")) {
    // nothing to do (see epdata method)
  }
  else if (isEqual(tag,"description")) {
    // nothing to do (see epdata method)
  }
  else if (isEqual(tag,"defines") && hasdefinestag == 0) {
    hasdefinestag = 1;
  }
  else if (isEqual(tag,"define") && hasdefinestag == 1) {
    string key = getStringAttribute(attributes, "name");
    string value = getStringAttribute(attributes, "value");
    checkDefine(key, value);
    if (eu.defines.find(key) == eu.defines.end()) {
      logger << "define (input file)" << split << key+"="+value << endl;
      eu.defines[key] = value;
    }
    else {
     // do nothing. User definitions (via command line or via gui)
     // prevail over input file definitions.
    }
  }
  else if (isEqual(tag,"parameters") && params.size() == 0) {
    eppush(eu, &params, tag, attributes);
  }
  else if (isEqual(tag,"interest") && interest.size() == 0) {
    interest.setDate(params.getTime0());
    eppush(eu, &interest, tag, attributes);
  }
  else if (isEqual(tag,"ratings") && ratings.size() == 0) {
    eppush(eu, &ratings, tag, attributes);
  }
  else if (isEqual(tag,"transitions") && transitions.size() == 0) {
    transitions.setRatings(ratings);
    eppush(eu, &transitions, tag, attributes);
  }
  else if (isEqual(tag,"dprobs") && dprobs.size() == 0) {
    eppush(eu, &dprobs, tag, attributes);
  }
  else if (isEqual(tag,"factors") && factors.size() == 0) {
    eppush(eu, &factors, tag, attributes);
  }
  else if (isEqual(tag,"correlations") && correlations.size() == 0) {
    correlations = Correlations(factors.size());
    eppush(eu, &correlations, tag, attributes);
  }
  else if (isEqual(tag,"segmentations") && segmentations.size() == 0) {
    eppush(eu, &segmentations, tag, attributes);
  }
  else if (isEqual(tag,"portfolio") && portfolio.getObligors().size() == 0) {
    if (!parse_portfolio) epstop(eu);
    parsePortfolio(eu, tag, attributes);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epdata
 * @todo Catch cdata sections using XML_SetCdataSectionHandler.
 * @param[in,out] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] data Chunck of data.
 * @param[in] len Data length.
 */
void ccruncher::IData::epdata(ExpatUserData &eu, const char *tag, const char *data, int len)
{
  if (isEqual(tag,"title")) {
    title.append(data, len);
  }
  else if (isEqual(tag,"description")) {
    description.append(data, len);
  }
  else {
    ExpatHandlers::epdata(eu, tag, data, len);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in,out] eu Xml parsing data.
 * @param[in] tag Element name.
 */
void ccruncher::IData::epend(ExpatUserData &eu, const char *tag)
{
  if (isEqual(tag,"ccruncher")) {
    validate();
  }
  else if (isEqual(tag,"defines")) {
    hasdefinestag = 2;
  }
  else if (isEqual(tag,"parameters")) {
    eu.date1 = params.getTime0();
    eu.date2 = params.getTimeT();
  }
  else if (isEqual(tag,"interest")) {
    eu.interest = &interest;
  }
  else if (isEqual(tag,"ratings")) {
    eu.ratings = &ratings;
  }
  else if (isEqual(tag,"factors")) {
    eu.factors = &factors;
  }
  else if (isEqual(tag,"segmentations")) {
    eu.segmentations = &segmentations;
  }
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error parsing input file.
 */
void ccruncher::IData::parsePortfolio(ExpatUserData &eu, const char *tag, const char **attributes)
{
  // we push interest to stack (is optional)
  eu.interest = &interest;

  string ref = getStringAttribute(attributes, "include", "");

  if (ref == "")
  {
    eppush(eu, &portfolio, tag, attributes);
  }
  else
  {
    gzFile prevfile = curfile;
    gzFile file = nullptr;

    try
    {
      string path;
      if (filename==STDIN_FILENAME) path = Utils::getWorkDir();
      else path = Utils::dirname(filename);
      string filepath = Utils::filepath(path, ref);
      file = gzopen(filepath.c_str(), "rb");
      if (file == nullptr) {
        throw Exception("can't open file '" + filepath + "'");
      }

      size_t bytes = Utils::filesize(filepath);
      mMutex.lock();
      curfile = file;
      cursize = bytes;
      mMutex.unlock();

      gzbuffer(file, BUFFER_SIZE);
      logger << "included file name" << split << "["+filepath+"]" << endl;
      logger << "included file size" << split << Utils::bytesToString(bytes) << endl;

      ExpatParser parser;
      parser.setDefines(eu.defines);
      parser.getUserData().date1 = eu.date1;
      parser.getUserData().date2 = eu.date2;
      parser.getUserData().interest = eu.interest;
      parser.getUserData().ratings = eu.ratings;
      parser.getUserData().factors = eu.factors;
      parser.getUserData().segmentations = eu.segmentations;
      parser.parse(file, &portfolio, stop);

      logger << "included file checksum (adler32)" << split << parser.getChecksum() << endl;

      mMutex.lock();
      curfile = prevfile;
      mMutex.unlock();
      gzclose(file);
    }
    catch(std::exception &e)
    {
      if (file != nullptr) {
        mMutex.lock();
        curfile = prevfile;
        mMutex.unlock();
        gzclose(file);
      }
      throw Exception(e, "error parsing file '" + ref + "'");
    }
  }
}

/**************************************************************************//**
 * @throw Exception Input file section not found.
 */
void ccruncher::IData::validate()
{
  if (params.size() == 0) {
    throw Exception("section 'parameters' not defined");
  }
  else if (ratings.size() == 0) {
    throw Exception("section 'parameters' not defined");
  }
  else if (transitions.size() == 0 && dprobs.size() == 0) {
    throw Exception("section 'transitions' or 'dprobs' not defined");
  }
  else if (factors.size() == 0) {
    throw Exception("section 'factors' not defined");
  }
  else if (correlations.size() == 0) {
    throw Exception("section 'correlations' not defined");
  }
  else if (segmentations.size() == 0) {
    throw Exception("section 'segmentations' not defined");
  }
  else if (portfolio.getObligors().size() == 0) {
    throw Exception("section 'portfolio' not defined");
  }
}

/**************************************************************************//**
 * @details User can define Default probabilities functions explicitly OR
 *          transition matrix. This method indicates wich one has been
 *          entered by user.
 * @return true = exist dprobs defined, false = exist transition matrix.
 */
bool ccruncher::IData::hasDefaultProbabilities() const
{
  if (dprobs.size() > 0) return true;
  else return false;
}

/**************************************************************************//**
 * @details Check the following rules:
 *          - define name has pattern [A-Za-z0-9_]+
 *          - define value don't contains [&<>\"'"]
 * @param[in] key Define key.
 * @param[in] value Define value.
 * @throw Exception Invalid define.
 */
void ccruncher::IData::checkDefine(const string &key, const string &value) const
{
  if (key.length() == 0) throw Exception("invalid macro name");

  for(size_t i=0; i<key.length(); i++)
  {
    if (!isalnum(key[i]) && key[i] != '_') {
      throw Exception("invalid macro name '" + key + "'");
    }
  }

  if (value.find_first_of("&<>\"'") != string::npos) {
    throw Exception("invalid macro value '" + value + "'");
  }
}

/**************************************************************************//**
 * @return Current file size in bytes.
 */
size_t ccruncher::IData::getFileSize() const
{
  return cursize;
}

/**************************************************************************//**
 * @return Readed bytes in the current file.
 */
size_t ccruncher::IData::getReadedSize() const
{
  size_t ret = 0;
  mMutex.lock();
  if (curfile == nullptr) ret = cursize;
  else ret = gzoffset(curfile);
  mMutex.unlock();
  return ret;
}

