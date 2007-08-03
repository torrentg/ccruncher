
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
// Params.cpp - Params code - $Rev$
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
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . allowed maxseconds=0 or maxiterations=0 (0 remove stop criteria)
//
// 2005/08/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed copula identifier: normal -> gaussian
//
// 2005/09/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.simule
//
// 2005/09/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed default seed value from -1 to 0
//
// 2005/09/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed default maxiteration and maxseconds values 0 to -1
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simule and method params
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
//===========================================================================

#include "params/Params.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"

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
  maxiterations = -1L;
  maxseconds = -1L;
  copula_type = "";
  copula_seed = 0L;
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
      throw Exception("attributes are not allowed in tag params");
    }
  }
  else if (isEqual(name,"property")) {
    parseProperty(eu, atrs);
  }
  else {
    throw Exception("unexpected tag " + string(name));
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
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// return Date array = begindate, begindate+steplength, bengindate+2*steplength
//===========================================================================
Date * ccruncher::Params::getDates() const throw(Exception)
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
      throw Exception("invalid time.begintime");
    } else {
      begindate = aux;
    }
  }
  else if (name == "time.steps")
  {
    int aux = getIntAttribute(attributes, "value", 0);
    if (steps != 0 || aux <= 0) {
      throw Exception("invalid time.steps");
    } else {
      steps = aux;
    }
  }
  else if (name == "time.steplength")
  {
    int aux = getIntAttribute(attributes, "value", 0);
    if (steplength != 0 || aux <= 0) {
      throw Exception("invalid time.steplength");
    } else {
      steplength = aux;
    }
  }
  else if (name == "stopcriteria.maxiterations")
  {
    long aux = getLongAttribute(attributes, "value", -1L);
    if (maxiterations >= 0L || aux < 0L) {
      throw Exception("invalid stopcriteria.maxiterations");
    } else {
      maxiterations = aux;
    }
  }
  else if (name == "stopcriteria.maxseconds")
  {
    long aux = getLongAttribute(attributes, "value", -1L);
    if (maxseconds >= 0L || aux < 0L) {
      throw Exception("invalid stopcriteria.maxseconds");
    } else {
      maxseconds = aux;
    }
  }
  else if (name == "copula.type")
  {
    string aux = getStringAttribute(attributes, "value", "");
    if (copula_type != "" || aux != "gaussian") {
      throw Exception("invalid copula.type. supported values: gaussian");
    }
    else {
      copula_type = aux;
    }
  }
  else if (name == "copula.seed")
  {
    long aux = getLongAttribute(attributes, "value", -1L);
    if (aux == -1L) {
      throw Exception("invalid copula.seed");
    }
    else {
      copula_seed = aux;
    }
  }
  else if (name == "montecarlo.antithetic")
  {
    bool aux = getBooleanAttribute(attributes, "value", false);
    antithetic = aux;
  }
  else if (name == "portfolio.onlyActiveBorrowers")
  {
    bool aux = getBooleanAttribute(attributes, "value", false);
    onlyactive = aux;
  }
  else
  {
    throw Exception("found unexpected property: " + name);
  }
}

//===========================================================================
// check class content
//===========================================================================
void ccruncher::Params::validate(void) const throw(Exception)
{

  if (begindate == Date(1,1,1900))
  {
    throw Exception("property time.begindate not defined");
  }

  if (steps <= 0)
  {
    throw Exception("property time.steps not defined");
  }

  if (steplength <= 0)
  {
    throw Exception("property time.steplength not defined");
  }

  if (maxiterations < 0L)
  {
    throw Exception("property stopcriteria.maxiterations not defined");
  }

  if (maxseconds < 0L)
  {
    throw Exception("property stopcriteria.maxseconds not defined");
  }

  if (copula_type == "")
  {
    throw Exception("property copula.type not defined");
  }

  if (maxiterations == 0 && maxseconds == 0)
  {
    throw Exception("non finite stop criteria");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Params::getXML(int ilevel) const throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<params>\n";
  ret += spc2 + "<property name='time.begindate' value='" + Format::date2string(begindate) + "'/>\n";
  ret += spc2 + "<property name='time.steps' value='" + Format::int2string(steps) + "'/>\n";
  ret += spc2 + "<property name='time.steplength' value='" + Format::int2string(steplength) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxiterations' value='" + Format::long2string(maxiterations) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxseconds' value='" + Format::long2string(maxseconds) + "'/>\n";
  ret += spc2 + "<property name='copula.type' value='" + copula_type + "'/>\n";
  ret += spc2 + "<property name='copula.seed' value='" + Format::long2string(copula_seed) + "'/>\n";
  ret += spc2 + "<property name='montecarlo.antithetic' value='" + Format::bool2string(antithetic) + "'/>\n";
  ret += spc2 + "<property name='portfolio.onlyActiveBorrowers' value='" + Format::bool2string(onlyactive) + "'/>\n";
  ret += spc1 + "</params>\n";

  return ret;
}
