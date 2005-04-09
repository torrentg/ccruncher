
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
// IData.cpp - IData code
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
//===========================================================================

#include <cassert>
#include <fstream>
#include <iostream>
#include "kernel/IData.hpp"
#include "utils/Logger.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Timer.hpp"

//===========================================================================
// inicialitzador
//===========================================================================
void ccruncher::IData::init()
{
   params = NULL;
   interests = NULL;
   ratings = NULL;
   transitions = NULL;
   sectors = NULL;
   correlations = NULL;
   segmentations = NULL;
   aggregators = NULL;
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
  if (sectors != NULL) delete sectors;
  if (correlations != NULL) delete correlations;
  if (segmentations != NULL) delete segmentations;
  if (aggregators != NULL) delete aggregators;
  if (portfolio != NULL) delete portfolio;

  // establim els punters a NULL
  init();
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
  else if (transitions == NULL) {
    throw Exception("transition matrix section not defined");
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
  else if (aggregators == NULL) {
    throw Exception("aggregators section not defined");
  }
  else if (portfolio == NULL) {
    throw Exception("portfolio section not defined");
  }
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
ccruncher::IData::IData(const string &xmlfilename) throw(Exception)
{
  ExpatParser parser;

  // inicialitzem el contingut
  init();

  // parsejem el document
  try
  {
    // checking file readability
    ifstream ifile((const char *) xmlfilename.c_str());

    if (!ifile.is_open())
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
      parser.parse(ifile, this);
      
      // exit function
      Logger::previousIndentLevel();
      ifile.close();
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
  if (isEqual(name_,"creditcruncher")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag creditcruncher");
    }
  }
  // section params
  else if (isEqual(name_,"params")) {
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
      transitions = new TransitionMatrix(ratings);
      eppush(eu, transitions, name_, attributes);
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
      correlations = new CorrelationMatrix(sectors);
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
  // section aggregators
  else if (isEqual(name_,"aggregators")) {
    if (segmentations == NULL) {
      throw eperror(eu, "tag <aggregators> defined before <segmentations> tag");
    }
    else if (aggregators != NULL) {
      throw eperror(eu, "tag aggregators repeated");
    }
    else {
      Logger::trace("parsing aggregators", true);
      aggregators = new Aggregators(segmentations);
      eppush(eu, aggregators, name_, attributes);
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
    else {
      Logger::trace("parsing portfolio", true);
      portfolio = new Portfolio(ratings, sectors, segmentations, interests);
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
  if (isEqual(name_,"creditcruncher")) {
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
  else if (isEqual(name_,"sectors")) {
    // nothing to do
  }
  else if (isEqual(name_,"mcorrels")) {
    // nothing to do
  }
  else if (isEqual(name_,"segmentations")) {
    // nothing to do
  }
  else if (isEqual(name_,"aggregators")) {
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
// destructor
//===========================================================================
ccruncher::IData::~IData()
{
  release();
}
