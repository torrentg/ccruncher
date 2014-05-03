
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

#include <cstdio>
#include <cassert>
#include "kernel/IData.hpp"
#include "utils/Logger.hpp"
#include "utils/File.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define BUFFER_SIZE 128*1024

/**************************************************************************//**
 * @see http://www.cplusplus.com/reference/streambuf/streambuf/
 * @param[in] s Streambuf where the trace will be written.
 */
ccruncher::IData::IData(streambuf *s) : log(s), curfile(nullptr)
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
  size_t bytes = 0;
  filename = f;
  stop = s;
  parse_portfolio = p;

  try
  {
    if (filename == STDIN_FILENAME) {
      file = gzdopen(fileno(stdin), "rb");
      bytes = 0;
    }
    else {
      // gzFile reads both gziped and not-gziped files
      file = gzopen(filename.c_str(), "rb");
      bytes = File::filesize(filename);
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
    log << "reading input file" << flood('*') << endl;
    log << indent(+1);

    // trace file info
    log << "file name" << split << "["+filename+"]" << endl;
    if (filename != STDIN_FILENAME)
    {
      log << "file size" << split << Format::bytes(File::filesize(filename)) << endl;
    }

    // trace defines
    for(auto &it : m) {
      checkDefine(it.first, it.second);
      log << "define (user defined)" << split << it.first+"="+it.second << endl;
    }

    // parsing
    Timer timer(true);
    ExpatParser parser;
    parser.setDefines(m);
    parser.parse(file, this, stop);
    log << "file checksum (adler32)" << split << parser.getChecksum() << endl;
    log << "elapsed time parsing data" << split << timer << endl;
    log << indent(-1);
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
      log << "define (input file)" << split << key+"="+value << endl;
      eu.defines[key] = value;
    }
    else {
     // do nothing. User definitions (via command line or via gui)
     // prevail over input file definitions.
    }
  }
  else if (isEqual(tag,"parameters") && params.time0 == NAD) {
    eppush(eu, &params, tag, attributes);
  }
  else if (isEqual(tag,"interest") && interest.size() == 0) {
    interest.setDate(params.time0);
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
    dprobs.setRatings(ratings);
    dprobs.setDate(params.time0);
    eppush(eu, &dprobs, tag, attributes);
  }
  else if (isEqual(tag,"factors") && factors.size() == 0) {
    eppush(eu, &factors, tag, attributes);
  }
  else if (isEqual(tag,"correlations") && correlations.size() == 0) {
    correlations.setFactors(factors);
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
    eu.date1 = &(params.time0);
    eu.date2 = &(params.timeT);
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
      if (filename==STDIN_FILENAME) path = File::getWorkDir();
      else path = File::dirname(filename);
      string filepath = File::filepath(path, ref);
      file = gzopen(filepath.c_str(), "rb");
      if (file == nullptr) {
        throw Exception("can't open file '" + filepath + "'");
      }

      size_t bytes = File::filesize(filepath);
      mMutex.lock();
      curfile = file;
      cursize = bytes;
      mMutex.unlock();

      gzbuffer(file, BUFFER_SIZE);
      log << "included file name" << split << "["+filepath+"]" << endl;
      log << "included file size" << split << Format::bytes(bytes) << endl;

      ExpatParser parser;
      parser.setDefines(eu.defines);
      parser.UserData().date1 = eu.date1;
      parser.UserData().date2 = eu.date2;
      parser.UserData().interest = eu.interest;
      parser.UserData().ratings = eu.ratings;
      parser.UserData().factors = eu.factors;
      parser.UserData().segmentations = eu.segmentations;
      parser.parse(file, &portfolio, stop);

      log << "included file checksum (adler32)" << split << parser.getChecksum() << endl;

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
  if (params.maxiterations < 0) {
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
 * @return Simulation title.
 */
const string & ccruncher::IData::getTitle() const
{
  return title;
}

/**************************************************************************//**
 * @return Simulation description.
 */
const string & ccruncher::IData::getDescription() const
{
  return description;
}

/**************************************************************************//**
 * @return Simulation parameters.
 */
Params & ccruncher::IData::getParams()
{
  return params;
}

/**************************************************************************//**
 * @return Simulation yield curve.
 */
Interest & ccruncher::IData::getInterest()
{
  return interest;
}

/**************************************************************************//**
 * @return Simulation ratings.
 */
Ratings & ccruncher::IData::getRatings()
{
  return ratings;
}

/**************************************************************************//**
 * @return Simulation transition matrix.
 */
Transitions & ccruncher::IData::getTransitions()
{
  return transitions;
}

/**************************************************************************//**
 * @return Simulation default probabilities functions.
 */
DefaultProbabilities &ccruncher::IData::getDefaultProbabilities()
{
  return dprobs;
}

/**************************************************************************//**
 * @return Simulation factors.
 */
Factors & ccruncher::IData::getFactors()
{
  return factors;
}

/**************************************************************************//**
 * @return Simulation factor correlations matrix.
 */
Correlations & ccruncher::IData::getCorrelations()
{
  return correlations;
}

/**************************************************************************//**
 * @return Simulation segmentations.
 */
Segmentations & ccruncher::IData::getSegmentations()
{
  return segmentations;
}

/**************************************************************************//**
 * @return Simulation portfolio.
 */
Portfolio & ccruncher::IData::getPortfolio()
{
  return portfolio;
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

/**************************************************************************//**
 * @return Input file name.
 */
std::string ccruncher::IData::getFilename() const
{
  return filename;
}

