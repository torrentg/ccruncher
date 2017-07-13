
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include <limits>
#include <cassert>
#include "kernel/XmlInputData.hpp"
#include "utils/Utils.hpp"

using namespace std;
using namespace std::chrono;
using namespace ccruncher;

#define STDIN_FILENAME "<stdin>"
#define STRING_FILENAME "<user-defined>"
#define BUFFER_SIZE 128*1024

/**************************************************************************//**
 * @see http://www.cplusplus.com/reference/streambuf/streambuf/
 * @param[in] s Streambuf where the trace will be written.
 */
ccruncher::XmlInputData::XmlInputData(streambuf *s) : logger(s), curfile(nullptr)
{
  stop = nullptr;
  cursize = 0;
  parse_portfolio = true;
}

/**************************************************************************/
ccruncher::XmlInputData::~XmlInputData()
{
  assert(curfile == nullptr);
  if (curfile != nullptr) gzclose(curfile);
}

/**************************************************************************//**
 * @param[in] f File File object.
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::read(FILE *f)
{
  assert(f != nullptr);
  gzFile gzfile = nullptr;

  try
  {
    // gzFile reads both gziped and not-gziped files
    gzfile = gzdopen(fileno(f), "rb");
    if (gzfile == nullptr) {
      throw Exception("can't open file '" + filename + "': " + strerror(errno));
    }

    mMutex.lock();
    curfile = gzfile;
    mMutex.unlock();

    // big buffer to increase the speed of decompression
    gzbuffer(gzfile, BUFFER_SIZE);
    parse(gzfile);

    mMutex.lock();
    curfile = nullptr;
    mMutex.unlock();
    gzclose(gzfile);
  }
  catch(...)
  {
    if (gzfile != nullptr) {
      mMutex.lock();
      curfile = nullptr;
      mMutex.unlock();
      gzclose(gzfile);
    }
    throw;
  }
}

/**************************************************************************//**
 * @param[in] f File name (including path).
 * @param[in] m List of macros defined by user.
 * @param[in] s Variable to stop parser (can be null).
 * @param[in] p Parse whole input file (true), or exclude portfolio (false)
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::readFile(const std::string &f, const std::map<std::string,std::string> &m, bool *s, bool p)
{
  filename = f;
  stop = s;
  parse_portfolio = p;
  FILE *file = fopen(filename.c_str(), "rb");
  if (file == nullptr) {
    throw Exception(strerror(errno));
  }
  cursize = Utils::filesize(filename);
  macros.values = m;
  read(file);
}

/**************************************************************************//**
 * @param[in] m List of macros defined by user.
 * @param[in] s Variable to stop parser (can be null).
 * @param[in] p Parse whole input file (true), or exclude portfolio (false)
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::readStdin(const std::map<std::string,std::string> &m, bool *s, bool p)
{
  filename = STDIN_FILENAME;
  stop = s;
  parse_portfolio = p;
  cursize = 0;
  macros.values = m;
  read(stdin);
}

/**************************************************************************//**
 * @details Use only for debug or tests.
 * @param[in] str String containing the xml.
 * @param[in] m List of macros defined by user.
 * @param[in] s Variable to stop parser (can be null).
 * @param[in] p Parse whole input file (true), or exclude portfolio (false)
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::readString(const string &str, const std::map<std::string,std::string> &m, bool *s, bool p)
{
  filename = STRING_FILENAME;
  stop = s;
  parse_portfolio = p;
  cursize = str.size();
  macros.values = m;

  try {
    ExpatParser parser;
    parser.parse(str, this, s);
  }
  catch(std::exception &e) {
    throw Exception(e, "error parsing xml input");
  }
}

/**************************************************************************//**
 * @param[in] file File to parse.
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::parse(gzFile file)
{
  try
  {
    // output header
    logger << "reading input file" << flood('*') << endl;
    logger << indent(+1);

    // trace file info
    if (filename != STDIN_FILENAME && filename != STRING_FILENAME) {
      logger << "file name" << split << "[" + Utils::realpath(filename) + "]" << endl;
    }
    else {
      logger << "file name" << split << filename << endl;
    }
    if (cursize > 0) {
      logger << "file size" << split << Utils::bytesToString(cursize) << endl;
    }

    // trace user-defined macros
    for(auto &kv : macros.values) {
      checkMacro(kv.first, kv.second);
      logger << "macro (user defined)" << split << kv.first+"="+kv.second << endl;
    }

    // parsing
    auto t1 = steady_clock::now();
    ExpatParser parser;
    parser.parse(file, this, stop);

    if (stop == nullptr || !(*stop)) {
      // trace info
      auto t2 = steady_clock::now();
      long millis = duration_cast<milliseconds>(t2-t1).count();
      logger << "file checksum (adler32)" << split << parser.getChecksum() << endl;
      logger << "elapsed time" << split << Utils::millisToString(millis) << endl;
      logger << indent(-1);
      summary();
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error parsing file '" + filename + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart(const char *tag, const char **attributes)
{
  assert(tag != nullptr);
  assert(attributes != nullptr);

  switch(currentTags.size())
  {
    case 0:
      epstart0(tag, attributes);
      break;
    case 1:
      epstart1(tag, attributes);
      break;
    case 2:
      epstart2(tag, attributes);
      break;
    case 3:
      epstart3(tag, attributes);
      break;
    default:
      epstart4(tag, attributes);
  }
}

/**************************************************************************//**
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart0(const char *tag, const char **)
{
  if (isEqual(tag,"ccruncher")) {
    pushTag(XmlTag::CCRUNCHER);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart1(const char *tag, const char **attributes)
{
  if (isEqual(tag,"title") && !hasTag(XmlTag::TITLE)) {
    pushTag(XmlTag::TITLE);
  }
  else if (isEqual(tag,"description") && !hasTag(XmlTag::DESCRIPTION)) {
    pushTag(XmlTag::DESCRIPTION);
  }
  else if (isEqual(tag,"defines") && !hasTag(XmlTag::DEFINES)) {
    pushTag(XmlTag::DEFINES);
  }
  else if (isEqual(tag,"parameters") && !hasTag(XmlTag::PARAMETERS)) {
    pushTag(XmlTag::PARAMETERS);
  }
  else if (isEqual(tag,"interest") && !hasTag(XmlTag::INTEREST) && hasTag(XmlTag::PARAMETERS)) {
    pushTag(XmlTag::INTEREST);
    string type = getStringAttribute(attributes, "type", "compound");
    string spline = getStringAttribute(attributes, "spline", "linear");
    processTagInterest(type, spline);
  }
  else if (isEqual(tag,"ratings") && !hasTag(XmlTag::RATINGS)) {
    pushTag(XmlTag::RATINGS);
  }
  else if (isEqual(tag,"transitions") && !hasTag(XmlTag::TRANSITIONS) && hasTag(XmlTag::RATINGS)) {
    pushTag(XmlTag::TRANSITIONS);
    int period = getIntAttribute(attributes, "period");
    transitions = Transitions(ratings.size(), period);
  }
  else if (isEqual(tag,"dprobs") && !hasTag(XmlTag::DPROBS) && hasTag(XmlTag::RATINGS) && hasTag(XmlTag::PARAMETERS)) {
    pushTag(XmlTag::DPROBS);
    dprobs.assign(ratings.size(), CDF(0, INFINITY));
  }
  else if (isEqual(tag,"factors") && !hasTag(XmlTag::FACTORS)) {
    pushTag(XmlTag::FACTORS);
  }
  else if (isEqual(tag,"correlations") && !hasTag(XmlTag::CORRELATIONS) && hasTag(XmlTag::FACTORS)) {
    pushTag(XmlTag::CORRELATIONS);
    initCorrelations(factors.size());
  }
  else if (isEqual(tag,"segmentations") && !hasTag(XmlTag::SEGMENTATIONS)) {
    pushTag(XmlTag::SEGMENTATIONS);
  }
  else if (isEqual(tag,"portfolio") && !hasTag(XmlTag::PORTFOLIO) && hasTag(XmlTag::PARAMETERS) &&
           hasTag(XmlTag::RATINGS) && hasTag(XmlTag::FACTOR) && hasTag(XmlTag::SEGMENTATIONS)) {
    if (!parse_portfolio) epstop();
    string include = getStringAttribute(attributes, "include", "");
    if (include != "") {
      parseIncludedPortfolio(include);
    }
    pushTag(XmlTag::PORTFOLIO);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart2(const char *tag, const char **attributes)
{
  if (currentTags.back() == XmlTag::PORTFOLIO && isEqual(tag,"obligor")) {
    pushTag(XmlTag::OBLIGOR);
    string id = getStringAttribute(attributes, "id");
    const char *sfactor = getAttributeValue(attributes, "factor");
    const char *srating = getAttributeValue(attributes, "rating");
    const char *slgd = getAttributeValue(attributes, "lgd", nullptr);
    processTagObligor(id, sfactor, srating, slgd);
  }
  else if (currentTags.back() == XmlTag::DEFINES && isEqual(tag,"define")) {
    pushTag(XmlTag::DEFINE);
    string key = getStringAttribute(attributes, "name");
    string value = getStringAttribute(attributes, "value");
    processTagDefine(key, value);
  }
  else if (currentTags.back() == XmlTag::PARAMETERS && isEqual(tag,"parameter")) {
    pushTag(XmlTag::PARAMETER);
    string name = getStringAttribute(attributes, "name");
    string value = getStringAttribute(attributes, "value");
    params.setParamValue(name, value);
  }
  else if (currentTags.back() == XmlTag::INTEREST && isEqual(tag,"rate")) {
    pushTag(XmlTag::RATE);
    const char *t = getAttributeValue(attributes, "t");
    double r = getDoubleAttribute(attributes, "r");
    processTagRate(t, r);
  }
  else if (currentTags.back() == XmlTag::RATINGS && isEqual(tag,"rating")) {
    pushTag(XmlTag::RATING);
    string name = getStringAttribute(attributes, "name");
    string desc = getStringAttribute(attributes, "description", "");
    ratings.push_back(Rating(name, desc));
  }
  else if (currentTags.back() == XmlTag::TRANSITIONS && isEqual(tag,"transition")) {
    pushTag(XmlTag::TRANSITION);
    string from = getStringAttribute(attributes, "from");
    string to = getStringAttribute(attributes, "to");
    double value = getDoubleAttribute(attributes, "value");
    processTagTransition(from, to, value);
  }
  else if (currentTags.back() == XmlTag::DPROBS && isEqual(tag,"dprob")) {
    pushTag(XmlTag::DPROB);
    string srating = getStringAttribute(attributes, "rating");
    const char *t = getAttributeValue(attributes, "t");
    double value = getDoubleAttribute(attributes, "value");
    processTagDprob(srating, t, value);
  }
  else if (currentTags.back() == XmlTag::FACTORS && isEqual(tag,"factor")) {
    pushTag(XmlTag::FACTOR);
    string name = getStringAttribute(attributes, "name");
    string desc = getStringAttribute(attributes, "description", "");
    double loading = getDoubleAttribute(attributes, "loading");
    factors.push_back(Factor(name, loading, desc));
  }
  else if (currentTags.back() == XmlTag::CORRELATIONS && isEqual(tag,"correlation")) {
    pushTag(XmlTag::CORRELATION);
    string factor1 = getStringAttribute(attributes, "factor1");
    string factor2 = getStringAttribute(attributes, "factor2");
    double value = getDoubleAttribute(attributes, "value");
    processTagCorrelation(factor1, factor2, value);
  }
  else if (currentTags.back() == XmlTag::SEGMENTATIONS && isEqual(tag,"segmentation")) {
    pushTag(XmlTag::SEGMENTATION);
    string name = getStringAttribute(attributes, "name");
    bool enabled = getBooleanAttribute(attributes, "enabled", true);
    processTagSegmentation(name, enabled);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart3(const char *tag, const char **attributes)
{
  if (currentTags.back() == XmlTag::OBLIGOR && isEqual(tag,"asset")) {
    pushTag(XmlTag::ASSET);
    string id = getStringAttribute(attributes, "id");
    Date date = getDateAttribute(attributes, "date");
    const char *lgd = getAttributeValue(attributes, "lgd", nullptr);
    processTagAsset(id, date, lgd);
  }
  else if (currentTags.back() == XmlTag::OBLIGOR && isEqual(tag,"belongs-to")) {
    pushTag(XmlTag::BELONGSTO);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    const char *ssegment = getAttributeValue(attributes, "segment");
    processTagBelongsTo(XmlTag::OBLIGOR, ssegmentation, ssegment);
  }
  else if (currentTags.back() == XmlTag::SEGMENTATION && isEqual(tag,"segment")) {
    pushTag(XmlTag::SEGMENT);
    string name = getStringAttribute(attributes, "name");
    segmentations.back().addSegment(name);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::XmlInputData::epstart4(const char *tag, const char **attributes)
{
  if (currentTags.back() == XmlTag::DATA && isEqual(tag,"values")) {
    pushTag(XmlTag::VALUES);
    const char *t = getAttributeValue(attributes, "t");
    const char *ead = getAttributeValue(attributes, "ead");
    const char *lgd = getAttributeValue(attributes, "lgd", nullptr);
    processTagValues(t, ead, lgd);
  }
  else if (currentTags.back() == XmlTag::ASSET && isEqual(tag,"belongs-to")) {
    pushTag(XmlTag::BELONGSTO);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    const char *ssegment = getAttributeValue(attributes, "segment");
    processTagBelongsTo(XmlTag::ASSET, ssegmentation, ssegment);
  }
  else if (currentTags.back() == XmlTag::ASSET && isEqual(tag,"data")) {
    pushTag(XmlTag::DATA);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epdata
 * @todo Catch cdata sections using XML_SetCdataSectionHandler.
 * @param[in] tag Element name.
 * @param[in] data Chunck of data.
 * @param[in] len Data length.
 */
void ccruncher::XmlInputData::epdata(const char *data, int len)
{
  assert(data != nullptr);
  assert(len >= 0);

  if (currentTags.back() == XmlTag::TITLE) {
    title.append(data, len);
  }
  else if (currentTags.back() == XmlTag::DESCRIPTION) {
    description.append(data, len);
  }
  else {
    ExpatHandlers::epdata(data, len);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 */
void ccruncher::XmlInputData::epend(const char *)
{
  switch(currentTags.back())
  {
    case XmlTag::ASSET:
      processLastAsset();
      break;
    case XmlTag::OBLIGOR:
      processLastObligor();
      break;
    case XmlTag::CCRUNCHER:
      validate();
      fillCDFs();
      break;
    case XmlTag::TITLE:
      title = Utils::trim(title);
      break;
    case XmlTag::DESCRIPTION:
      description = Utils::trim(description);
      break;
    case XmlTag::PARAMETERS:
      params.isValid(true);
      break;
    case XmlTag::INTEREST:
      interest.getRate(params.getTime0()+1);
      break;
    case XmlTag::RATINGS:
      Input::validateRatings(ratings, true);
      break;
    case XmlTag::TRANSITIONS:
      transitions.getIndexDefault();
      break;
    case XmlTag::DPROBS:
      Input::validateCDFs(dprobs, true);
      break;
    case XmlTag::FACTORS:
      Input::validateFactors(factors, true);
      floadings = Input::getFactorLoadings(factors);
      break;
    case XmlTag::CORRELATIONS:
      Input::validateCorrelations(correlations, true);
      break;
    case XmlTag::SEGMENTATIONS:
      Input::validateSegmentations(segmentations, true);
      // sort segmentations: set 'enabled' before 'disabled'
      stable_sort(segmentations.begin(), segmentations.end(),
            [](const Segmentation &a, const Segmentation &b) -> bool {
              return a.isEnabled() && !b.isEnabled();
            });
      mNumEnabledSegmentations = getNumEnabledSegmentations();
      break;
    case XmlTag::PORTFOLIO:
      mIdAssets.clear();
      mIdObligors.clear();
      removeUnusedSegments();
      Input::validatePortfolio(obligors, factors.size(), ratings.size(),
          segmentations, params.getTime0(), params.getTimeT(), true);
      break;
    default:
      ;// nothing to do
  }

  popTag();
}

/**************************************************************************//**
 * @param[in] include File name containing xml portfolio.
 * @throw Exception Error parsing input file.
 */
void ccruncher::XmlInputData::parseIncludedPortfolio(const string &include)
{
  gzFile prevfile = curfile;
  gzFile file = nullptr;

  try
  {
    string filepath = getIncludedFileName(include);
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
    parser.parse(file, this, stop);

    if (stop == nullptr || !(*stop)) {
      logger << "included file checksum (adler32)" << split << parser.getChecksum() << endl;
    }

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
    throw Exception(e, "error parsing file '" + include + "'");
  }
}

/**************************************************************************//**
 * @throw Exception Input file section not found.
 */
void ccruncher::XmlInputData::validate()
{
  if (!hasTag(XmlTag::PARAMETERS)) {
    throw Exception("section 'parameters' not defined");
  }
  else if (!hasTag(XmlTag::RATINGS)) {
    throw Exception("section 'ratings' not defined");
  }
  else if (!hasTag(XmlTag::TRANSITIONS) && !hasTag(XmlTag::DPROBS)) {
    throw Exception("section 'transitions' or 'dprobs' not defined");
  }
  else if (!hasTag(XmlTag::FACTORS)) {
    throw Exception("section 'factors' not defined");
  }
  else if (!hasTag(XmlTag::CORRELATIONS)) {
    initCorrelations(factors.size());
  }
  else if (!hasTag(XmlTag::SEGMENTATIONS)) {
    throw Exception("section 'segmentations' not defined");
  }
  else if (!hasTag(XmlTag::PORTFOLIO)) {
    throw Exception("section 'portfolio' not defined");
  }
}

/**************************************************************************//**
 * @throw Exception Error printing input data.
 */
void ccruncher::XmlInputData::summary()
{
  logger << endl;
  logger << "input file summary" << flood('*') << endl;
  logger << indent(+1);

  logger << "initial date" << split << params.getTime0() << endl;
  logger << "end date" << split << params.getTimeT() << endl;
  logger << "number of ratings" << split << ratings.size() << endl;
  logger << "number of factors" << split << factors.size() << endl;
  logger << "copula type" << split << params.getCopula() << endl;

  if (!dprobs.empty()) {
    logger << "default probability functions" << split << "user defined" << endl;
  }
  else {
    logger << "default probability functions" << split << "computed" << endl;
    logger << "transition matrix period (months)" << split << transitions.getPeriod() << endl;
  }

  size_t numObligors = 0UL;
  size_t numAssets = 0UL;
  size_t numValues = 0UL;
  for(Obligor &obligor : obligors) {
    numObligors++;
    for(Asset &asset : obligor.assets) {
      numAssets++;
      numValues += asset.values.size();
    }
  }

  logger << "number of obligors" << split << numObligors << endl;
  logger << "number of assets" << split << numAssets << endl;
  logger << "number of values" << split << numValues << endl;
  logger << "number of segmentations" << split << mNumEnabledSegmentations << endl;

  logger << indent(-1);
}

/**************************************************************************//**
 * @return Current file size in bytes.
 */
size_t ccruncher::XmlInputData::getFileSize() const
{
  return cursize;
}

/**************************************************************************//**
 * @return Readed bytes in the current file.
 */
size_t ccruncher::XmlInputData::getReadedSize() const
{
  size_t ret = 0;
  mMutex.lock();
  if (curfile == nullptr) ret = cursize;
  else ret = gzoffset(curfile);
  mMutex.unlock();
  return ret;
}

/**************************************************************************//**
 * @param[in] name File name.
 * @result Included filepath.
 */
string ccruncher::XmlInputData::getIncludedFileName(const string &name)
{
  if (Utils::isAbsolutePath(name)) {
    return Utils::realpath(name);
  }

  string path;
  if (filename==STDIN_FILENAME) {
    path = Utils::getWorkDir();
  } else {
    path = Utils::dirname(filename);
  }

  string filepath = path + Utils::pathSeparator + Utils::toNativeSeparators(name);
  return Utils::realpath(filepath);
}

/**************************************************************************//**
 * @param[in] tag Tag to push to stack.
 */
void ccruncher::XmlInputData::pushTag(XmlTag tag)
{
  currentTags.push_back(tag);
  mHasTag[static_cast<unsigned char>(tag)] = true;
}

/**************************************************************************//**
 */
void ccruncher::XmlInputData::popTag()
{
  assert(!currentTags.empty());
  currentTags.pop_back();
}

/**************************************************************************//**
 * @param[in] tag Tag to check.
 * @result true = tag defined, false = otherwise.
 */
bool ccruncher::XmlInputData::hasTag(XmlTag tag)
{
  return mHasTag[static_cast<unsigned char>(tag)];
}

/**************************************************************************//**
 * @result Number of enabled segmentations.
 */
unsigned short ccruncher::XmlInputData::getNumEnabledSegmentations() const
{
  unsigned short ret = 0;
  for(const Segmentation &segmentation : segmentations) {
    if (segmentation.isEnabled()) {
      ret ++;
    }
  }
  return ret;
}

/**************************************************************************//**
 * @details Check the following rules:
 *          - define name has pattern [A-Za-z0-9_]+
 *          - define value don't contains [&<>\"'"]
 * @param[in] key Define key.
 * @param[in] value Define value.
 * @throw Exception Invalid define.
 */
void ccruncher::XmlInputData::checkMacro(const string &key, const string &value) const
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
 * @param[in] defines Already defined macros.
 * @param[in] key Macro name.
 * @param[in] value Macro value.
 * @throw Invalid values.
 */
void ccruncher::XmlInputData::processTagDefine(const string &key, const string &value)
{
  checkMacro(key, value);

  // Declare the macro found in the xml input file EXCEPT if this macro
  // has not defined previously (via command line or via gui or previous define)
  if (macros.values.find(key) == macros.values.end()) {
    logger << "macro (input file)" << split << key + "=" + value << endl;
    macros.values[key] = value;  }
}

/**************************************************************************//**
 * @param[in] type Interest type.
 * @param[in] spline Interest spline type.
 * @throw Invalid values.
 */
void ccruncher::XmlInputData::processTagInterest(const string &type, const string &spline)
{
  interest.setDate(params.getTime0());
  Interest::InterestType interestType = Interest::getInterestType(type);
  interest.setInterestType(interestType);
  Interest::SplineType splineType = Interest::getSplineType(spline);
  interest.setSplineType(splineType);
}

/**************************************************************************//**
 * @param[in] str Date (format DD/MM/YYYY) or interval (eg. +5M).
 * @param[in] r Interest rate.
 * @throw Invalid values.
 */
void ccruncher::XmlInputData::processTagRate(const char *str, double r)
{
  assert(str != nullptr);

  Date date = interest.getDate();
  if (isInterval(str)) date.add(str);
  else date = Date(str);
  interest.insertRate(date, r);
}

/**************************************************************************//**
 * @param[in] from Initial rating.
 * @param[in] to Ending rating.
 * @param[in] value Transition probability.
 * @throw Invalid rating or value.
 */
void ccruncher::XmlInputData::processTagTransition(const string &from, const string &to, double value)
{
  unsigned char row = Input::indexOfRating(ratings, from);
  unsigned char col = Input::indexOfRating(ratings, to);
  transitions.setValue(row, col, value);
}

/**************************************************************************//**
 * @param[in] srating Rating identifier.
 * @param[in] str Date (format DD/MM/YYYY) or time increment (eg. +5M).
 * @param[in] value Default probability of rating at given date.
 * @throw Invalid rating, time or value.
 */
void ccruncher::XmlInputData::processTagDprob(const string &srating, const char *str, double value)
{
  assert(str != nullptr);

  Date date(params.getTime0());
  if (isInterval(str)) {
    date.add(str);
  }
  else {
    date = Date(str);
  }
  size_t irating = Input::indexOfRating(ratings, srating);
  dprobs[irating].add(date-params.getTime0(), value);
}

/**************************************************************************//**
 * @param[in] factor1 Factor.
 * @param[in] factor2 Factor.
 * @param[in] value Correlation between factor1 and factor2.
 * @throw Invalid factor identifier or correlation out-of-range.
 */
void ccruncher::XmlInputData::processTagCorrelation(const string &factor1, const string &factor2, double value)
{
  size_t row = Input::indexOfFactor(factors, factor1);
  size_t col = Input::indexOfFactor(factors, factor2);

  if (std::isnan(value) || value < -1.0 || 1.0 < value) {
    throw Exception("value out of range [-1,+1]");
  }

  correlations[row][col] = value;
  correlations[col][row] = value;
}

/**************************************************************************//**
 * @param[in] name Segmentation name.
 * @param[in] enabled Enabled flag.
 * @throw Repeated segmentation name.
 */
void ccruncher::XmlInputData::processTagSegmentation(const string &name, bool enabled)
{
  for(Segmentation &segmentation : segmentations) {
    if (segmentation.getName() == name) {
      throw Exception("segmentation name '" + name + "' repeated");
    }
  }

  segmentations.push_back(Segmentation(name, enabled));
}

/**************************************************************************//**
 * @param[in] id Obligor identifier.
 * @param[in] sfactor Obligor's factor.
 * @param[in] srating Obligor's rating.
 * @param[in] slgd Obligor's lgd (can be null).
 * @throw Unrecognized segmentation or segment.
 */
void ccruncher::XmlInputData::processTagObligor(const string &id, const char *sfactor,
                                                const char *srating, const char *slgd)
{
  assert(sfactor != nullptr);
  assert(srating != nullptr);

  if(mIdObligors.find(id) != mIdObligors.end()) {
    throw Exception("obligor id '" + id + "' repeated");
  }
  else {
    size_t numObligors = mIdObligors.size() + 1;
    mIdObligors[id] = numObligors;
  }

  unsigned char ifactor = Input::indexOfFactor(factors, sfactor);
  unsigned char irating = Input::indexOfRating(ratings, srating);

  Obligor obligor(ifactor, irating);

  if (slgd != nullptr) {
    obligor.lgd = LGD(slgd);
  }

  obligors.push_back(move(obligor));
  mObligorSegments.assign(mNumEnabledSegmentations, 0);
}

/**************************************************************************//**
 * @param[in] parent Parent tag.
 * @param[in] ssegmentation Segmentation name.
 * @param[in] ssegment Segment name.
 * @throw Unrecognized segmentation or segment.
 */
void ccruncher::XmlInputData::processTagBelongsTo(XmlTag parent, const char *ssegmentation, const char *ssegment)
{
  assert(ssegmentation != nullptr);
  assert(ssegment != nullptr);

  unsigned short isegmentation = Input::indexOfSegmentation(segmentations, ssegmentation);
  unsigned short isegment = segmentations[isegmentation].indexOfSegment(ssegment);

  if (isegmentation >= mNumEnabledSegmentations) {
    // it is a disabled segmentation -> nothing to do
  }
  else if (parent == XmlTag::OBLIGOR) {
    if (mObligorSegments[isegmentation] != 0) {
      throw Exception("belongs-to '" + string(ssegmentation) + "' already defined");
    }
    mObligorSegments[isegmentation] = isegment;
  }
  else if (parent == XmlTag::ASSET) {
    vector<unsigned short> &segments = obligors.back().assets.back().segments;
    if (segments[isegmentation] != 0) {
      throw Exception("belongs-to '" + string(ssegmentation) + "' already defined");
    }
    segments[isegmentation] = isegment;
  }
  else {
    assert(false);
  }
}

/**************************************************************************//**
 * @param[in] id Asset identifier.
 * @param[in] date Asset creation date.
 * @param[in] lgd String representing a lgd function (can be null).
 * @throw Invalid values.
 */
void ccruncher::XmlInputData::processTagAsset(const string &id, const Date &date,  const char *lgd)
{
  if (obligors.back().assets.size() >= numeric_limits<unsigned short>::max()) {
      throw Exception("obligor has too much assets");
  }

  if (mIdAssets.find(id) != mIdAssets.end()) {
    throw Exception("asset id '" + id + "' repeated");
  }
  else {
    size_t numAssets = mIdAssets.size() + 1;
    mIdAssets[id] = numAssets;
  }

  if (lgd != nullptr) mAssetLGD = LGD(lgd);
  else mAssetLGD = LGD(NAN);

  Asset asset(mObligorSegments);
  asset.values.push_back(DateValues(date, 0.0, 0.0));
  obligors.back().assets.push_back(move(asset));
}

/**************************************************************************//**
 * @param[in] str String representing a Date (format dd/MM/YYY) or an interval (eg. +5M).
 * @param[in] ead String representing an ead function.
 * @param[in] lgd String representing a lgd function (can be null).
 * @throw Invalid values.
 */
void ccruncher::XmlInputData::processTagValues(const char *str, const char *ead, const char *lgd)
{
  assert(str != nullptr);
  assert(ead != nullptr);

  Asset &asset = obligors.back().assets.back();
  DateValues item;

  if (isInterval(str)) {
    item.date = asset.values[0].date;
    item.date.add(str);
  }
  else {
    item.date = Date(str);
  }

  if (item.date <= asset.values[0].date) {
    throw Exception("values with date previous or equal to asset creation date");
  }

  item.ead = EAD(ead);

  if (lgd != nullptr) item.lgd = LGD(lgd);
  else item.lgd = mAssetLGD;

  asset.values.push_back(move(item));
}

/**************************************************************************//**
 * @brief Check last obligor and removes from obligors list if not active
 *        in the simulation period.
 * @throw Exception Invalid obligor.
 */
void ccruncher::XmlInputData::processLastObligor()
{
  Obligor &obligor = obligors.back();

  // remove obligor if inactive
  if (!obligor.isActive(params.getTime0(), params.getTimeT())) {
    obligors.pop_back();
    return;
  }
}

/**************************************************************************//**
 * @brief Check last asset and remove datevalues out of simulation time range.
 * @throw Exception Invalid asset.
 */
void ccruncher::XmlInputData::processLastAsset()
{
  Asset &asset = obligors.back().assets.back();

  // sort datevalues + remove unused values + compute EAD current net values
  asset.prepare(params.getTime0(), params.getTimeT(), interest);

  // remove asset if inactive
  if (!asset.isActive(params.getTime0(), params.getTimeT())) {
    obligors.back().assets.pop_back();
    return;
  }

  // validate asset
  Input::validateAsset(asset, segmentations, params.getTime0(), params.getTimeT(), true);
}

