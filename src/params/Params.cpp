
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
// Params.cpp - Params code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.method
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//   . implemented Arrays class
//
// 2005/06/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug related to seed=0 (random seed)
//   . added montecarlo.method param to getXML() method
//
//===========================================================================

#include "params/Params.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Params::Params()
{
  // posem valors per defecte (incorrectes)
  init();
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Params::init()
{
  begindate = Date(1,1,1900);
  steps = 0;
  steplength = 0;
  maxiterations = 0L;
  maxseconds = 0L;
  smethod = "";
  copula_type = "";
  copula_seed = -1L;
  antithetic = false;
  onlyactive = false;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Params::~Params()
{
  // nothing to do
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Params::epstart(ExpatUserData &eu, const char *name, const char **atrs)
{
  if (isEqual(name,"params")) {
    // checking that don't have attributes
    if (getNumAttributes(atrs) > 0) {
      throw eperror(eu, "attributes are not allowed in tag params");
    }
  }
  else if (isEqual(name,"property")) {
    parseProperty(eu, atrs);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Params::epend(ExpatUserData &eu, const char *name)
{
  if (isEqual(name,"params")) {
    validate();
  }
  else if (isEqual(name,"property")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name));
  }
}

//===========================================================================
// return Date array = begindate, begindate+steplength, bengindate+2*steplength
//===========================================================================
Date * ccruncher::Params::getDates() throw(Exception)
{
  validate();

  Date *ret = Arrays<Date>::allocVector(steps+1);

  for (int i=0;i<=steps;i++)
  {
    ret[i] = addMonths(begindate, i*steplength);
  }

  return ret;
}

//===========================================================================
// parse a XML property
//===========================================================================
void ccruncher::Params::parseProperty(ExpatUserData &eu, const char **attributes) throw(Exception)
{
  // reading attribute name
  string name = getStringAttribute(attributes,"name", "");

  if (name == "time.begindate")
  {
    Date aux = getDateAttribute(attributes, "value", Date(1,1,1900));
    if (begindate != Date(1,1,1900) || aux == Date(1,1,1900)) {
      throw eperror(eu, "invalid time.begintime");
    } else {
      begindate = aux;
    }
  }
  else if (name == "time.steps")
  {
    int aux = getIntAttribute(attributes, "value", 0);
    if (steps != 0 || aux <= 0) {
      throw eperror(eu, "invalid time.steps");
    } else {
      steps = aux;
    }
  }
  else if (name == "time.steplength")
  {
    int aux = getIntAttribute(attributes, "value", 0);
    if (steplength != 0 || aux <= 0) {
      throw eperror(eu, "invalid time.steplength");
    } else {
      steplength = aux;
    }
  }
  else if (name == "stopcriteria.maxiterations")
  {
    long aux = getLongAttribute(attributes, "value", 0L);
    if (maxiterations != 0L || aux <= 0L) {
      throw eperror(eu, "invalid stopcriteria.maxiterations");
    } else {
      maxiterations = aux;
    }
  }
  else if (name == "stopcriteria.maxseconds")
  {
    long aux = getLongAttribute(attributes, "value", 0L);
    if (maxseconds != 0L || aux <= 0L) {
      throw eperror(eu, "invalid stopcriteria.maxseconds");
    } else {
      maxseconds = aux;
    }
  }
  else if (name == "copula.type")
  {
    string aux = getStringAttribute(attributes, "value", "");
    if (copula_type != "" || aux != "normal") {
      throw eperror(eu, "invalid copula.type. supported values: normal");
    }
    else {
      copula_type = aux;
    }
  }
  else if (name == "copula.seed")
  {
    long aux = getLongAttribute(attributes, "value", -1L);
    if (copula_seed != -1L || aux == -1L) {
      throw eperror(eu, "invalid copula.seed");
    }
    else {
      copula_seed = aux;
    }
  }
  else if (name == "montecarlo.method")
  {
    string aux = getStringAttribute(attributes, "value", "");
    if (smethod != "" || (aux != "rating-path" && aux != "time-to-default")) {
      throw eperror(eu, "invalid montecarlo.method. supported values: time-to-default, rating-path");
    }
    else {
      smethod = aux;
    }
  }
  else if (name == "montecarlo.antithetic")
  {
    bool aux = getBooleanAttribute(attributes, "value", false);
    antithetic = aux;
  }
  else if (name == "portfolio.onlyActiveClients")
  {
    bool aux = getBooleanAttribute(attributes, "value", false);
    onlyactive = aux;
  }
  else
  {
    throw eperror(eu, "found unexpected property: " + name);
  }
}

//===========================================================================
// check class content
//===========================================================================
void ccruncher::Params::validate(void) throw(Exception)
{

  if (begindate == Date(1,1,1900))
  {
    throw Exception("Params::validate(): property time.begindate not defined");
  }

  if (steps <= 0)
  {
    throw Exception("Params::validate(): property time.steps not defined");
  }

  if (steplength <= 0)
  {
    throw Exception("Params::validate(): property time.steplength not defined");
  }

  if (maxiterations <= 0L)
  {
    throw Exception("Params::validate(): property stopcriteria.maxiterations not defined");
  }

  if (maxseconds <= 0L)
  {
    throw Exception("Params::validate(): property stopcriteria.maxseconds not defined");
  }

  if (smethod == "")
  {
    throw Exception("Params::validate(): property montecarlo.method not defined");
  }

  if (copula_type == "")
  {
    throw Exception("Params::validate(): property copula.type not defined");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Params::getXML(int ilevel) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<params>\n";
  ret += spc2 + "<property name='time.begindate' value='" + Parser::date2string(begindate) + "'/>\n";
  ret += spc2 + "<property name='time.steps' value='" + Parser::int2string(steps) + "'/>\n";
  ret += spc2 + "<property name='time.steplength' value='" + Parser::int2string(steplength) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxiterations' value='" + Parser::long2string(maxiterations) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxseconds' value='" + Parser::long2string(maxseconds) + "'/>\n";
  ret += spc2 + "<property name='copula.type' value='" + copula_type + "'/>\n";
  ret += spc2 + "<property name='copula.seed' value='" + Parser::long2string(copula_seed) + "'/>\n";
  ret += spc2 + "<property name='montecarlo.method' value='" + smethod + "'/>\n";
  ret += spc2 + "<property name='montecarlo.antithetic' value='" + Parser::bool2string(antithetic) + "'/>\n";
  ret += spc2 + "<property name='portfolio.onlyActiveClients' value='" + Parser::bool2string(onlyactive) + "'/>\n";
  ret += spc1 + "</params>\n";

  return ret;
}
