
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
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/13 - Gerard Torrent [gerard@mail.generacio.com]
//   . added param montecarlo.method
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented Strings class
//   . implemented Arrays class
//
// 2005/06/26 - Gerard Torrent [gerard@mail.generacio.com]
//   . solved bug related to seed=0 (random seed)
//   . added montecarlo.method param to getXML() method
//
// 2005/07/21 - Gerard Torrent [gerard@mail.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . allowed maxseconds=0 or maxiterations=0 (0 remove stop criteria)
//
// 2005/08/12 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed copula identifier: normal -> gaussian
//
// 2005/09/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . added param montecarlo.simule
//
// 2005/09/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed default seed value from -1 to 0
//
// 2005/09/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed default maxiteration and maxseconds values 0 to -1
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed simule and method params
//
// 2006/02/11 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
// 2007/08/06 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed dates management
//
// 2009/02/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed from discrete time to continuous time
//
//===========================================================================

#include "params/Params.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Params::Params()
{
  init();
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Params::init()
{
  time0 = Date(1,1,1900);
  timeT = Date(1,1,1900);
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
// checks copula type string
//===========================================================================
string ccruncher::Params::getCopulaType() const throw(Exception)
{
  // gaussian case
  if (copula_type == "gaussian") {
    return "gaussian";
  }
  // t-student case t(x)
  else if (copula_type.length() >= 3 && copula_type.substr(0,2) == "t(" && copula_type.at(copula_type.length()-1) == ')') {
    return "t";
  }
  // non-valid copula type
  else {
    throw Exception("invalid copula type: " + copula_type + ". try 'gaussian' or 't(x)' with x>2");
  }
}

//===========================================================================
// checks copula type string
//===========================================================================
int ccruncher::Params::getCopulaParam() throw(Exception)
{
  if (getCopulaType() != "t") {
    throw Exception("copula without params");
  }
  // t-student case t(x), where x is a integer
  else {
    string val = copula_type.substr(2, copula_type.length()-3);
    int ndf = Parser::intValue(val);
    if (ndf <= 2) 
    {
      throw Exception("t-student copula requires ndf > 2");
    }
    return ndf;
  }
}

//===========================================================================
// parse a XML property
//===========================================================================
void ccruncher::Params::parseProperty(ExpatUserData &eu, const char **attributes) throw(Exception)
{
  // reading attribute name
  string name = getStringAttribute(attributes,"name", "");

  if (name == "time.0")
  {
    Date aux = getDateAttribute(attributes, "value", Date(1,1,1900));
    if (time0 != Date(1,1,1900) || aux == Date(1,1,1900)) {
      throw Exception("invalid time.begintime");
    } else {
      time0 = aux;
    }
  }
  else if (name == "time.T")
  {
    Date aux = getDateAttribute(attributes, "value", Date(1,1,1900));
    if (timeT != Date(1,1,1900) || aux == Date(1,1,1900)) {
      throw Exception("invalid time.begintime");
    } else {
      timeT = aux;
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
    copula_type = getStringAttribute(attributes, "value", "");
    if (getCopulaType() == "t") {
      getCopulaParam(); //parse and validate param
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

  if (time0 == Date(1,1,1900))
  {
    throw Exception("property time.0 not defined");
  }

  if (timeT == Date(1,1,1900))
  {
    throw Exception("property time.T not defined");
  }

  if (time0 >= timeT)
  {
    throw Exception("time.0 >= time.T");
  }

  if (timeT.getYear()-time0.getYear() > 101) {
    throw Exception("more than 100 years between time0 and timeT");
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
  ret += spc2 + "<property name='time.0' value='" + Format::date2string(time0) + "'/>\n";
  ret += spc2 + "<property name='time.T' value='" + Format::date2string(timeT) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxiterations' value='" + Format::long2string(maxiterations) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxseconds' value='" + Format::long2string(maxseconds) + "'/>\n";
  ret += spc2 + "<property name='copula.type' value='" + copula_type + "'/>\n";
  ret += spc2 + "<property name='copula.seed' value='" + Format::long2string(copula_seed) + "'/>\n";
  ret += spc2 + "<property name='montecarlo.antithetic' value='" + Format::bool2string(antithetic) + "'/>\n";
  ret += spc2 + "<property name='portfolio.onlyActiveBorrowers' value='" + Format::bool2string(onlyactive) + "'/>\n";
  ret += spc1 + "</params>\n";

  return ret;
}
