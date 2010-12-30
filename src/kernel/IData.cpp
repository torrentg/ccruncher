
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#include "kernel/IData.hpp"
#include "utils/Logger.hpp"
#include "utils/File.hpp"
#include "utils/ExpatParser.hpp"
#include <gzstream.h>
#include <cassert>

//===========================================================================
// inicialitzador
//===========================================================================
void ccruncher::IData::init()
{
   hasmaintag = false;
   title = "";
   description = "";
   portfolio = NULL;
}

//===========================================================================
// release
//===========================================================================
void ccruncher::IData::release()
{
  if (portfolio != NULL) delete portfolio;
  init();
}

//===========================================================================
// constructor
// used to do tests
//===========================================================================
ccruncher::IData::IData()
{
  init();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::IData::IData(const string &xmlfilename) throw(Exception)
{
  // initializing content
  filename = xmlfilename;
  init();

  // parsing document
  try
  {
    // gziped file stream, if file isn't a gzip, is like a ifstream
    igzstream xmlstream((const char *) xmlfilename.c_str());
    if (xmlstream.peek() == EOF)
    {
      throw Exception("can't open file " + xmlfilename);
    }
    else
    {
      // output trace
      Logger::addBlankLine();
      Logger::trace("reading input file", '*');
      Logger::newIndentLevel();

      // parsing
      ExpatParser parser;
      parser.parse(xmlstream, this);

      // exit function
      Logger::previousIndentLevel();
    }
  }
  catch(std::exception &e)
  {
    release();
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

  // descriptive tags
  if (isEqual(name_,"title")) {
    // nothing to do (see epdata method)
  }
  else if (isEqual(name_,"description")) {
    // nothing to do (see epdata method)
  }
  // section params
  else if (isEqual(name_,"parameters")) {
    if (params.maxiterations >= 0) {
      throw Exception("tag parameters repeated");
    }
    else {
      timer.start();
      eppush(eu, &params, name_, attributes);
    }
  }
  // section interest
  else if (isEqual(name_,"interest")) {
    if (interest.size() != 0) {
      throw Exception("tag interest repeated");
    }
    else {
      timer.start();
      eppush(eu, &interest, name_, attributes);
    }
  }
  // section ratings
  else if (isEqual(name_,"ratings")) {
    if (ratings.size() != 0) {
      throw Exception("tag ratings repeated");
    }
    else {
      timer.start();
      eppush(eu, &ratings, name_, attributes);
    }
  }
  // section transition matrix
  else if (isEqual(name_,"mtransitions")) {
    if (ratings.size() == 0) {
      throw Exception("tag <mtransition> defined before <ratings> tag");
    }
    else if (transitions.size() > 0) {
      throw Exception("tag transitions repeated");
    }
    else {
      timer.start();
      transitions.setRatings(ratings);
      eppush(eu, &transitions, name_, attributes);
    }
  }
  // section survival
  else if (isEqual(name_,"survival")) {
    if (ratings.size() == 0) {
      throw Exception("tag <survival> defined before <ratings> tag");
    }
    else if (survival.size() != 0) {
      throw Exception("tag survival repeated");
    }
    else {
      timer.start();
      survival.setRatings(ratings);
      eppush(eu, &survival, name_, attributes);
    }
  }
  // section sectors
  else if (isEqual(name_,"sectors")) {
    if (sectors.size() != 0) {
      throw Exception("tag sectors repeated");
    }
     else {
      timer.start();
      eppush(eu, &sectors, name_, attributes);
    }
  }
  // section correlation matrix
  else if (isEqual(name_,"mcorrels")) {
    if (sectors.size() == 0) {
      throw Exception("tag <mcorrels> defined before <sectors> tag");
    }
    else if (correlations.size() > 0) {
      throw Exception("tag correlations repeated");
    }
    else {
      timer.start();
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
      timer.start();
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
    else if (portfolio != NULL) {
      throw Exception("tag portfolio repeated");
    }
    else {
      parsePortfolio(eu, name_, attributes);
    }
  }
  // default catcher
  else {
    throw Exception("unexpected tag " + string(name_));
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
void ccruncher::IData::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"ccruncher")) {
    validate();
  }
  else if (isEqual(name_,"title")) {
    // nothing to do
  }
  else if (isEqual(name_,"description")) {
    // nothing to do
  }
  else if (isEqual(name_,"parameters")) {
    Logger::trace("parsing parameters", timer);
  }
  else if (isEqual(name_,"interest")) {
    Logger::trace("parsing interest", timer);
  }
  else if (isEqual(name_,"ratings")) {
    Logger::trace("parsing ratings", timer);
  }
  else if (isEqual(name_,"mtransitions")) {
    Logger::trace("parsing transition matrix", timer);
  }
  else if (isEqual(name_,"survival")) {
    Logger::trace("parsing survival function", timer);
  }
  else if (isEqual(name_,"sectors")) {
    Logger::trace("parsing sectors", timer);
  }
  else if (isEqual(name_,"mcorrels")) {
    Logger::trace("parsing correlation matrix", timer);
  }
  else if (isEqual(name_,"segmentations")) {
    Logger::trace("parsing segmentations", timer);
  }
  else if (isEqual(name_,"portfolio")) {
    Logger::trace("parsing portfolio", timer);
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
  timer.start();
  portfolio = new Portfolio(ratings, sectors, segmentations, interest, params.time0, params.timeT);
  string ref = getStringAttribute(attributes, "include", "");

  if (ref == "")
  {
    eppush(eu, portfolio, name_, attributes);
  }
  else
  {
    try
    {
      string path = File::dirname(filename);
      string filepath = File::filepath(path, ref);
      igzstream xmlstream((const char *) filepath.c_str());
      if (xmlstream.peek() == EOF)
      {
        throw Exception("can't open file " + filepath);
      }
      else
      {
        ExpatParser parser;
        parser.parse(xmlstream, portfolio);
      }
    }
    catch(std::exception &e)
    {
      release();
      throw Exception(e);
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
  else if (transitions.size() == 0 && survival.size() == 0) {
    throw Exception("transition matrix or survival section not defined");
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
  else if (portfolio == NULL) {
    throw Exception("portfolio section not defined");
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::IData::~IData()
{
  release();
}

//===========================================================================
// getTitle
//===========================================================================
string & ccruncher::IData::getTitle()
{
  return title;
}

//===========================================================================
// getDescription
//===========================================================================
string & ccruncher::IData::getDescription()
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
// getTransitionMatrix
//===========================================================================
TransitionMatrix & ccruncher::IData::getTransitionMatrix()
{
  return transitions;
}

//===========================================================================
// getSurvival
//===========================================================================
Survival & ccruncher::IData::getSurvival()
{
  return survival;
}

//===========================================================================
// getSectors
//===========================================================================
Sectors & ccruncher::IData::getSectors()
{
  return sectors;
}

//===========================================================================
// getCorrelationMatrix
//===========================================================================
CorrelationMatrix & ccruncher::IData::getCorrelationMatrix()
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
  return *portfolio;
}

//===========================================================================
// hasSurvival
//===========================================================================
bool ccruncher::IData::hasSurvival() const
{
  if (survival.size() != 0) return true;
  else return false;
}
