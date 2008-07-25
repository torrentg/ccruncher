
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
// Interest.cpp - Interest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
// 2005/06/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . methods getActualCoef and getUpdateCoef replaced by getUpsilon
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . fecha renamed to date0
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
//===========================================================================

#include <cmath>
#include "interests/Interest.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1e-7

//===========================================================================
// private constructor
//===========================================================================
ccruncher::Interest::Interest()
{
  reset();
}

//===========================================================================
// private contructor
//===========================================================================
ccruncher::Interest::Interest(const string &str)
{
  // interest name
  name = str;
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Interest::reset()
{
  // flushing vector
  vrates.clear();

  // interest name
  name = "UNDEFINED_INTEREST";

  // setting an initial date
  date0 = Date(1,1,1900);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interest::~Interest()
{
  // atention at vrates allocation
}

//===========================================================================
// return interest name
//===========================================================================
string ccruncher::Interest::getName() const
{
  return name;
}

//===========================================================================
// returns initial date of this curve
//===========================================================================
Date ccruncher::Interest::getDate0() const
{
  return date0;
}

//===========================================================================
// returns interpolated interest rate at month t
//===========================================================================
double ccruncher::Interest::getValue(const double t) const
{
  unsigned int n = vrates.size();

  if (n == 0)
  {
    return 1.0;
  }
  else if (t <= vrates[0].t)
  {
    return vrates[0].r;
  }
  else if (vrates[n-1].t <= t)
  {
    return vrates[n-1].r;
  }
  else
  {
    for(register unsigned int i=1;i<n;i++)
    {
      if (t <= vrates[i].t)
      {
        return vrates[i-1].r + (t-vrates[i-1].t)*(vrates[i].r - vrates[i-1].r)/(vrates[i].t - vrates[i-1].t);
      }
    }

    assert(false);
    return vrates[n-1].r;
  }
}

//===========================================================================
// returns date at month t
//===========================================================================
Date ccruncher::Interest::idx2date(int t) const
{
  return addMonths(date0, t);
}

//===========================================================================
// returns index of date date1
//===========================================================================
inline double ccruncher::Interest::date2idx(const Date &date1) const
{
  return (double)(date1-date0)/30.3958;
}

//===========================================================================
// returns factor to aply to transport a money value from date0 to date1
// r: interest rate to apply
// t: time (in months)
//===========================================================================
inline double ccruncher::Interest::getUpsilon(const double r, const double t) const
{
  return pow(1.0 + r, t/12.0);
}

//===========================================================================
// returns factor to aply to transport a money value from date1 to date2
// satisfying interest curve rate values
//===========================================================================
double ccruncher::Interest::getUpsilon(const Date &date1, const Date &date2) const
{
  double t1 = date2idx(date1);
  double t2 = date2idx(date2);

  double r1 = getValue(t1);
  double r2 = getValue(t2);

  return getUpsilon(r1, -t1) * getUpsilon(r2, +t2);
}

//===========================================================================
// insertRate
//===========================================================================
void ccruncher::Interest::insertRate(Rate &val) throw(Exception)
{
  if (val.t < 0.0)
  {
    throw Exception("rate with invalid time: " + Format::double2string(val.t) + " < 0");
  }

  // checking consistency
  for (unsigned int i=0;i<vrates.size();i++)
  {
    Rate aux = vrates[i];

    if (fabs(aux.t-val.t) < EPSILON)
    {
      string msg = "rate time ";
      msg += Format::double2string(val.t);
      msg += " repeated";
      throw Exception(msg);
    }
  }

  // inserting value
  try
  {
    vrates.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"interest")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes");
    }
    else
    {
      // getting attributes
      name = getStringAttribute(attributes, "name", "");
      date0 = getDateAttribute(attributes, "date", Date(1,1,1900));
      if (name == "" || date0 == Date(1,1,1900))
      {
        throw Exception("invalid attributes values at <interest>");
      }
    }
  }
  else if (isEqual(name_,"rate")) {
    // setting new handlers
    auxrate.reset();
    eppush(eu, &auxrate, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"interest")) {
    // nothing to do
  }
  else if (isEqual(name_,"rate")) {
    insertRate(auxrate);
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Interest::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<interest name='" + name + "' date='" + Format::date2string(date0) + "'>\n";

  for (unsigned int i=0;i<vrates.size();i++)
  {
    ret += vrates[i].getXML(ilevel+2);
  }

  ret += spc + "</interest>\n";

  return ret;
}

