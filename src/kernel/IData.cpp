
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include "kernel/IData.hpp"
#include "utils/Logger.hpp"
#include "utils/File.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include "utils/ExpatParser.hpp"
#include <cassert>

#define BUFFER_SIZE 128*1024

//===========================================================================
// default constructor
//===========================================================================
ccruncher::IData::IData(streambuf *s) : log(s), curfile(NULL)
{
  pthread_mutex_init(&mutex, NULL);
  hasmaintag = false;
  hasdefinestag = 0;
  title = "";
  description = "";
  stop = NULL;
  cursize = 0;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::IData::~IData()
{
  assert(curfile == NULL);
  if (curfile != NULL) gzclose(curfile);
  pthread_mutex_destroy(&mutex);
}

//===========================================================================
// init
//===========================================================================
void ccruncher::IData::init(const string &f, const map<string,string> &m, bool *stop_) throw(Exception)
{
  gzFile file = NULL;
  size_t bytes = 0;
  filename = f;
  stop = stop_;

  try
  {
    if (filename == STDIN_FILENAME) {
      file = gzdopen(fileno(stdin), "rb");
      bytes = 0;
    }
    else {
      // gzFile reads gziped files an not-gziped files
      file = gzopen(filename.c_str(), "rb");
      bytes = File::filesize(filename);
    }

    if (file == NULL) {
      throw Exception("can't open file " + filename);
    }

    pthread_mutex_lock(&mutex);
    curfile = file;
    cursize = bytes;
    pthread_mutex_unlock(&mutex);

    // big buffer to increase the speed of decompression
    gzbuffer(file, BUFFER_SIZE);
    parse(file, m);

    pthread_mutex_lock(&mutex);
    curfile = NULL;
    pthread_mutex_unlock(&mutex);
    gzclose(file);
  }
  catch(std::exception &e)
  {
    if (file != NULL) {
      pthread_mutex_lock(&mutex);
      curfile = NULL;
      pthread_mutex_unlock(&mutex);
      gzclose(file);
    }
    throw;
  }
}

//===========================================================================
// parse
//===========================================================================
void ccruncher::IData::parse(gzFile file, const map<string,string> &m) throw(Exception)
{
  try
  {
    // output header
    log << "reading input file" << flood('*') << endl;
    log << indent(+1);

    // trace file info
    log << "file name" << split << filename << endl;
    if (filename != STDIN_FILENAME)
    {
      log << "file size" << split << Format::bytes(File::filesize(filename)) << endl;
    }

    // trace defines
    map<string,string>::const_iterator it;
    for (it=m.begin() ; it != m.end(); it++) {
      checkDefine((*it).first, (*it).second);
      log << "define (command line)" << split << (*it).first+"="+(*it).second << endl;
    }

    // parsing
    Timer timer(true);
    ExpatParser parser;
    parser.setDefines(m);
    parser.parse(file, this, stop);
    log << "elapsed time parsing data" << split << timer << endl;
    log << indent(-1);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::IData::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"ccruncher")) {
    hasmaintag = true;
    return;
  }
  else if (hasmaintag == false) {
    throw Exception("ccruncher tag expected but not found");
  }

  // title tag
  if (isEqual(name_,"title")) {
    // nothing to do (see epdata method)
  }
  // description tag
  else if (isEqual(name_,"description")) {
    // nothing to do (see epdata method)
  }
  // section defines
  else if (isEqual(name_,"defines")) {
    if (hasdefinestag == 0) hasdefinestag = 1;
    else throw Exception("unexpected tag");
  }
  else if (isEqual(name_,"define")) {
    if (hasdefinestag != 1) throw Exception("unexpected tag");
    string key = getStringAttribute(attributes, "name");
    string value = getStringAttribute(attributes, "value");
    checkDefine(key, value);
    if (eu.defines.find(key) == eu.defines.end()) {
      log << "define (configuration file)" << split << key+"="+value << endl;
      eu.defines[key] = value;
    }
  }
  // section params
  else if (isEqual(name_,"parameters")) {
    if (params.maxiterations >= 0) {
      throw Exception("tag parameters repeated");
    }
    else {
      eppush(eu, &params, name_, attributes);
    }
  }
  // section interest
  else if (isEqual(name_,"interest")) {
    if (interest.size() != 0) {
      throw Exception("tag interest repeated");
    }
    else {
      interest.setDate(params.time0);
      eppush(eu, &interest, name_, attributes);
    }
  }
  // section ratings
  else if (isEqual(name_,"ratings")) {
    if (ratings.size() != 0) {
      throw Exception("tag ratings repeated");
    }
    else {
      eppush(eu, &ratings, name_, attributes);
    }
  }
  // section transition matrix
  else if (isEqual(name_,"transitions")) {
    if (ratings.size() == 0) {
      throw Exception("tag <mtransition> defined before <ratings> tag");
    }
    else if (transitions.size() > 0) {
      throw Exception("tag transitions repeated");
    }
    else {
      transitions.setRatings(ratings);
      eppush(eu, &transitions, name_, attributes);
    }
  }
  // section default probabilities
  else if (isEqual(name_,"dprobs")) {
    if (ratings.size() == 0) {
      throw Exception("tag <dprobs> defined before <ratings> tag");
    }
    else if (dprobs.size() != 0) {
      throw Exception("tag dprobs repeated");
    }
    else {
      dprobs.setRatings(ratings);
      dprobs.setDate(params.time0);
      eppush(eu, &dprobs, name_, attributes);
    }
  }
  // section sectors
  else if (isEqual(name_,"sectors")) {
    if (sectors.size() != 0) {
      throw Exception("tag sectors repeated");
    }
    else {
      eppush(eu, &sectors, name_, attributes);
    }
  }
  // section correlation matrix
  else if (isEqual(name_,"correlations")) {
    if (sectors.size() == 0) {
      throw Exception("tag <correlations> defined before <sectors> tag");
    }
    else if (correlations.size() > 0) {
      throw Exception("tag correlations repeated");
    }
    else {
      correlations.setSectors(sectors);
      eppush(eu, &correlations, name_, attributes);
    }
  }
  // section segmentations
  else if (isEqual(name_,"segmentations")) {
    if (segmentations.size() > 0) {
      throw Exception("tag segmentations repeated");
    }
    else {
      eppush(eu, &segmentations, name_, attributes);
    }
  }
  // section portfolio
  else if (isEqual(name_,"portfolio")) {
    if (interest.size() == 0) {
      throw Exception("tag <portfolio> defined before <interest> tag");
    }
    else if (ratings.size() == 0) {
      throw Exception("tag <portfolio> defined before <ratings> tag");
    }
    else if (sectors.size() == 0) {
      throw Exception("tag <portfolio> defined before <sectors> tag");
    }
    else if (segmentations.size() == 0) {
      throw Exception("tag <portfolio> defined before <segmentations> tag");
    }
    else if (portfolio.getObligors().size() != 0) {
      throw Exception("tag <portfolio> repeated");
    }
    else {
      parsePortfolio(eu, name_, attributes);
    }
  }
  // default catcher
  else {
    throw Exception("unexpected tag '" + string(name_) + "'");
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::IData::epdata(ExpatUserData &eu, const char *name_, const char *data, int len)
{
  if (isEqual(name_,"title")) {
    title += string(data, len);
  }
  else if (isEqual(name_,"description")) {
    description += string(data, len);
  }
  else {
    ExpatHandlers::epdata(eu, name_, data, len);
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::IData::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"ccruncher")) {
    validate();
  }
  else if (isEqual(name_,"title")) {
    // nothing to do
  }
  else if (isEqual(name_,"description")) {
    // nothing to do
  }
  else if (isEqual(name_,"defines")) {
    hasdefinestag = 2;
  }
  else if (isEqual(name_,"define")) {
    // nothing to do
  }
  else if (isEqual(name_,"parameters")) {
    // nothing to do
  }
  else if (isEqual(name_,"interest")) {
    // nothing to do
  }
  else if (isEqual(name_,"ratings")) {
    // nothing to do
  }
  else if (isEqual(name_,"transitions")) {
    // nothing to do
  }
  else if (isEqual(name_,"dprobs")) {
    // nothing to do
  }
  else if (isEqual(name_,"sectors")) {
    // nothing to do
  }
  else if (isEqual(name_,"correlations")) {
    // nothing to do
  }
  else if (isEqual(name_,"segmentations")) {
    // nothing to do
  }
  else if (isEqual(name_,"portfolio")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// parsePortfolio
//===========================================================================
void ccruncher::IData::parsePortfolio(ExpatUserData &eu, const char *name_, const char **attributes) throw(Exception)
{
  portfolio.init(ratings, sectors, segmentations, interest, params.time0, params.timeT);
  string ref = getStringAttribute(attributes, "include", "");

  if (ref == "")
  {
    eppush(eu, &portfolio, name_, attributes);
  }
  else
  {
    gzFile file = NULL;

    try
    {
      string path;
      if (filename==STDIN_FILENAME) path = File::getWorkDir();
      else path = File::dirname(filename);

      string filepath = File::filepath(path, ref);
      file = gzopen(filepath.c_str(), "rb");
      if (file == NULL) {
        throw Exception("can't open file " + filepath);
      }

      size_t bytes = File::filesize(filepath);
      pthread_mutex_lock(&mutex);
      curfile = file;
      cursize = bytes;
      pthread_mutex_unlock(&mutex);

      gzbuffer(file, BUFFER_SIZE);
      log << "included file name" << split << filepath << endl;
      log << "included file size" << split << Format::bytes(bytes) << endl;

      ExpatParser parser;
      parser.setDefines(eu.defines);
      parser.parse(file, &portfolio, stop);

      pthread_mutex_lock(&mutex);
      curfile = NULL;
      pthread_mutex_unlock(&mutex);
      gzclose(file);
    }
    catch(std::exception &e)
    {
      if (file != NULL) {
        pthread_mutex_lock(&mutex);
        curfile = NULL;
        pthread_mutex_unlock(&mutex);
        gzclose(file);
      }
      throw Exception(e, "error parsing file '" + ref + "'");
    }
  }
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::IData::validate() throw(Exception)
{
  if (params.maxiterations < 0) {
    throw Exception("parameters section not defined");
  }
  else if (interest.size() == 0) {
    throw Exception("interest section not defined");
  }
  else if (ratings.size() == 0) {
    throw Exception("ratings section not defined");
  }
  else if (transitions.size() == 0 && dprobs.size() == 0) {
    throw Exception("transitions or dprobs section not defined");
  }
  else if (sectors.size() == 0) {
    throw Exception("sectors section not defined");
  }
  else if (correlations.size() == 0) {
    throw Exception("correlation matrix section not defined");
  }
  else if (segmentations.size() == 0) {
    throw Exception("segmentations section not defined");
  }
  else if (portfolio.getObligors().size() == 0) {
    throw Exception("portfolio section not defined");
  }
}

//===========================================================================
// getTitle
//===========================================================================
const string & ccruncher::IData::getTitle() const
{
  return title;
}

//===========================================================================
// getDescription
//===========================================================================
const string & ccruncher::IData::getDescription() const
{
  return description;
}

//===========================================================================
// getParams
//===========================================================================
Params & ccruncher::IData::getParams()
{
  return params;
}

//===========================================================================
// getInterest
//===========================================================================
Interest & ccruncher::IData::getInterest()
{
  return interest;
}

//===========================================================================
// getRatings
//===========================================================================
Ratings & ccruncher::IData::getRatings()
{
  return ratings;
}

//===========================================================================
// getTransitions
//===========================================================================
Transitions & ccruncher::IData::getTransitions()
{
  return transitions;
}

//===========================================================================
// getDefaultProbabilities
//===========================================================================
DefaultProbabilities &ccruncher::IData::getDefaultProbabilities()
{
  return dprobs;
}

//===========================================================================
// getSectors
//===========================================================================
Sectors & ccruncher::IData::getSectors()
{
  return sectors;
}

//===========================================================================
// getCorrelations
//===========================================================================
Correlations & ccruncher::IData::getCorrelations()
{
  return correlations;
}

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations & ccruncher::IData::getSegmentations()
{
  return segmentations;
}

//===========================================================================
// getPortfolio
//===========================================================================
Portfolio & ccruncher::IData::getPortfolio()
{
  return portfolio;
}

//===========================================================================
// hasDefaultProbabilities
//===========================================================================
bool ccruncher::IData::hasDefaultProbabilities() const
{
  if (dprobs.size() > 0) return true;
  else return false;
}

//===========================================================================
// check define
//===========================================================================
void ccruncher::IData::checkDefine(const string &key, const string &value) const throw(Exception)
{
  if (key.length() == 0) throw Exception("invalid define name");

  for(size_t i=0; i<key.length(); i++)
  {
    if (!isalnum(key[i])) throw Exception("invalid define name '" + key + "'");
  }

  if (value.find_first_of("&<>\"'") != string::npos) {
    throw Exception("invalid define value '" + value + "'");
  }
}

//===========================================================================
// returns file size (in bytes)
//===========================================================================
size_t ccruncher::IData::getFileSize() const
{
  return cursize;
}

//===========================================================================
// returns readed bytes
//===========================================================================
size_t ccruncher::IData::getReadedSize() const
{
  size_t ret = 0;
  pthread_mutex_lock(&mutex);
  if (curfile == NULL) ret = cursize;
  else ret = gzoffset(curfile);
  pthread_mutex_unlock(&mutex);
  return ret;
}

