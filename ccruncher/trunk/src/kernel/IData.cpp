
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
// IData.cpp - IData code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug at XML read that hangs ccruncher when input file isn't
//     a true xml file
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival section
//
// 2005/05/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed aggregators class
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added gziped input files suport
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//   . check that sections are included into ccruncher main tag
//
// 2005/09/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . thread-safe modification (variable hasmaintag)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Portfolio refactoring
//   . IData refactoring
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simule and method params
//
//===========================================================================

#include <fstream>
#include <iostream>
#include "kernel/IData.hpp"
#include "utils/Logger.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Timer.hpp"
#include <gzstream.h>
#include <cassert>

//===========================================================================
// inicialitzador
//===========================================================================
void ccruncher::IData::init()
{
   parse_portfolio = true;
   hasmaintag = false;

   params = NULL;
   interests = NULL;
   ratings = NULL;
   transitions = NULL;
   survival = NULL;
   sectors = NULL;
   correlations = NULL;
   segmentations = NULL;
   portfolio = NULL;
}

//===========================================================================
// release
//===========================================================================
void ccruncher::IData::release()
{
  // alliberem l'espai alocat
  if (params != NULL) delete params;
  if (interests != NULL) delete interests;
  if (ratings != NULL) delete ratings;
  if (transitions != NULL) delete transitions;
  if (survival != NULL) delete survival;
  if (sectors != NULL) delete sectors;
  if (correlations != NULL) delete correlations;
  if (segmentations != NULL) delete segmentations;
  if (portfolio != NULL) delete portfolio;

  // setting pointers to NULL
  init();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::IData::IData()
{
  init();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::IData::IData(const string &xmlfilename, bool _parse_portfolio) throw(Exception)
{
  ExpatParser parser;

  // initializing content
  init();

  // setting parse_portfolio value
  parse_portfolio = _parse_portfolio;

  // parsing document
  try
  {
    // gziped file stream, if file isn't a gzip, is like a ifstream
    igzstream  xmlstream((const char *) xmlfilename.c_str());

    if (!xmlstream.good())
    {
      throw Exception("IData::IData(): can't open file " + xmlfilename);
    }
    else
    {
      // output trace
      Logger::addBlankLine();
      Logger::trace("reading input file", '*');
      Logger::newIndentLevel();

      // parsing
      parser.parse(xmlstream, this);

      // exit function
      Logger::previousIndentLevel();
    }
  }
  catch(Exception &e)
  {
    release();
    throw e;
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
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag ccruncher");
    }
    else {
      hasmaintag = true;
      return;
    }
  }
  else if (hasmaintag == false) {
    throw eperror(eu, "expected main ccruncher tag not found");
  }

  // section params
  if (isEqual(name_,"params")) {
    if (params != NULL) {
      throw eperror(eu, "tag params repeated");
    }
    else {
      Logger::trace("parsing parameters", true);
      params = new Params();
      eppush(eu, params, name_, attributes);
    }
  }
  // section interests
  else if (isEqual(name_,"interests")) {
    if (interests != NULL) {
      throw eperror(eu, "tag interests repeated");
    }
    else {
      Logger::trace("parsing interests", true);
      interests = new Interests();
      eppush(eu, interests, name_, attributes);
    }
  }
  // section ratings
  else if (isEqual(name_,"ratings")) {
    if (ratings != NULL) {
      throw eperror(eu, "tag ratings repeated");
    }
    else {
      Logger::trace("parsing ratings", true);
      ratings = new Ratings();
      eppush(eu, ratings, name_, attributes);
    }
  }
  // section transition matrix
  else if (isEqual(name_,"mtransitions")) {
    if (ratings == NULL) {
      throw eperror(eu, "tag <mtransition> defined before <ratings> tag");
    }
    else if (transitions != NULL) {
      throw eperror(eu, "tag transitions repeated");
    }
    else {
      Logger::trace("parsing transition matrix", true);
      transitions = new TransitionMatrix(*ratings);
      eppush(eu, transitions, name_, attributes);
    }
  }
  // section survival
  else if (isEqual(name_,"survival")) {
    if (ratings == NULL) {
      throw eperror(eu, "tag <survivaal> defined before <ratings> tag");
    }
    else if (survival != NULL) {
      throw eperror(eu, "tag survival repeated");
    }
    else {
      Logger::trace("parsing survival function", true);
      survival = new Survival(*ratings);
      eppush(eu, survival, name_, attributes);
    }
  }
  // section sectors
  else if (isEqual(name_,"sectors")) {
    if (sectors != NULL) {
      throw eperror(eu, "tag sectors repeated");
    }
     else {
      Logger::trace("parsing sectors", true);
      sectors = new Sectors();
      eppush(eu, sectors, name_, attributes);
    }
  }
  // section correlation matrix
  else if (isEqual(name_,"mcorrels")) {
    if (sectors == NULL) {
      throw eperror(eu, "tag <mcorrels> defined before <sectors> tag");
    }
    else if (correlations != NULL) {
      throw eperror(eu, "tag correlations repeated");
    }
    else {
      Logger::trace("parsing correlation matrix", true);
      correlations = new CorrelationMatrix(*sectors);
      eppush(eu, correlations, name_, attributes);
    }
  }
  // section segmentations
  else if (isEqual(name_,"segmentations")) {
    if (segmentations != NULL) {
      throw eperror(eu, "tag segmentations repeated");
    }
    else {
      Logger::trace("parsing segmentations", true);
      segmentations = new Segmentations();
      eppush(eu, segmentations, name_, attributes);
    }
  }
  // section portfolio
  else if (isEqual(name_,"portfolio")) {
    if (interests == NULL) {
      throw eperror(eu, "tag <portfolio> defined before <interests> tag");
    }
    else if (ratings == NULL) {
      throw eperror(eu, "tag <portfolio> defined before <ratings> tag");
    }
    else if (sectors == NULL) {
      throw eperror(eu, "tag <portfolio> defined before <sectors> tag");
    }
    else if (segmentations == NULL) {
      throw eperror(eu, "tag <portfolio> defined before <segmentations> tag");
    }
    if (portfolio != NULL) {
      throw eperror(eu, "tag portfolio repeated");
    }
    if (parse_portfolio == false) {
      // checking current content
      validate();
      // stops parsing
      epstop(eu);
    }
    else {
      Logger::trace("parsing portfolio", true);
      portfolio = new Portfolio(*ratings, *sectors, *segmentations, *interests);
      eppush(eu, portfolio, name_, attributes);
    }
  }
  // default catcher
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::IData::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"ccruncher")) {
    validate();
  }
  else if (isEqual(name_,"params")) {
    // nothing to do
  }
  else if (isEqual(name_,"interests")) {
    // nothing to do
  }
  else if (isEqual(name_,"ratings")) {
    // nothing to do
  }
  else if (isEqual(name_,"mtransitions")) {
    // nothing to do
  }
  else if (isEqual(name_,"survival")) {
    // nothing to do
  }
  else if (isEqual(name_,"sectors")) {
    // nothing to do
  }
  else if (isEqual(name_,"mcorrels")) {
    // nothing to do
  }
  else if (isEqual(name_,"segmentations")) {
    // nothing to do
  }
  else if (isEqual(name_,"portfolio")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::IData::validate() throw(Exception)
{
  if (params == NULL) {
    throw Exception("params section not defined");
  }
  else if (interests == NULL) {
    throw Exception("interests section not defined");
  }
  else if (ratings == NULL) {
    throw Exception("ratings section not defined");
  }
  else if (transitions == NULL && survival == NULL) {
    throw Exception("transition matrix or survival section not defined");
  }
  else if (sectors == NULL) {
    throw Exception("sectors section not defined");
  }
  else if (correlations == NULL) {
    throw Exception("correlation matrix section not defined");
  }
  else if (segmentations == NULL) {
    throw Exception("segmentations section not defined");
  }
  else if (portfolio == NULL && parse_portfolio == true) {
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
// getParams
//===========================================================================
Params & ccruncher::IData::getParams() const
{
  return *params;
}

//===========================================================================
// getInterests
//===========================================================================
Interests & ccruncher::IData::getInterests() const
{
  return *interests;
}

//===========================================================================
// getRatings
//===========================================================================
Ratings & ccruncher::IData::getRatings() const
{
  return *ratings;
}

//===========================================================================
// getTransitionMatrix
//===========================================================================
TransitionMatrix & ccruncher::IData::getTransitionMatrix() const
{
  return *transitions;
}

//===========================================================================
// getSurvival
//===========================================================================
Survival & ccruncher::IData::getSurvival() const
{
  return *survival;
}

//===========================================================================
// getSectors
//===========================================================================
Sectors & ccruncher::IData::getSectors() const
{
  return *sectors;
}

//===========================================================================
// getCorrelationMatrix
//===========================================================================
CorrelationMatrix & ccruncher::IData::getCorrelationMatrix() const
{
  return *correlations;
}

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations & ccruncher::IData::getSegmentations() const
{
  return *segmentations;
}

//===========================================================================
// getPortfolio
//===========================================================================
Portfolio & ccruncher::IData::getPortfolio() const
{
  return *portfolio;
}

//===========================================================================
// setSurvival
//===========================================================================
void ccruncher::IData::setSurvival(const Survival &survival_)
{
  survival = (Survival *) &(survival_);
}

//===========================================================================
// hasSurvival
//===========================================================================
bool ccruncher::IData::hasSurvival() const
{
  if (survival != NULL) return true;
  else return false;
}
