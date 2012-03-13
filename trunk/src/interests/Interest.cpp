
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

#include <cmath>
#include <algorithm>
#include "interests/Interest.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1e-7

//===========================================================================
// constructor
//===========================================================================
ccruncher::Interest::Interest()
{
  type = Compound;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interest::~Interest()
{
  // nothing to do
}

//===========================================================================
// return interest type
//===========================================================================
InterestType ccruncher::Interest::getType() const
{
  return type;
}

//===========================================================================
// returns the numbers of rates
//===========================================================================
int ccruncher::Interest::size() const
{
  return (int) vrates.size();
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
    vector<Rate>::const_iterator it2 = lower_bound(vrates.begin(), vrates.end(), Rate(t));
    assert(it2 != vrates.begin());
    assert(it2 != vrates.end());
    vector<Rate>::const_iterator it1 = it2-1;
    return it1->r + (t-it1->t)*(it2->r - it1->r)/(it2->t - it1->t);
  }
}

//===========================================================================
// returns factor to aply to transport a money value from date1 to date2
// where is assumed that date2 is the interest curve date
//===========================================================================
double ccruncher::Interest::getFactor(const Date &date1, const Date &date2) const
{
  double t = date2.getMonthsTo(date1);
  double r = getValue(t);

  if (type == Simple)
  {
    return 1.0/(1.0+r*(t/12.0));
  }
  else if (type == Compound)
  {
    return 1.0/pow(1.0+r, t/12.0);
  }
  else if (type == Continuous)
  {
    return 1.0/exp(r*t/12.0);
  }
  else
  {
    assert(false);
    return NAN;
  }
}

//===========================================================================
// insertRate
//===========================================================================
void ccruncher::Interest::insertRate(Rate &val) throw(Exception)
{
  if (val.t < 0.0)
  {
    throw Exception("rate with invalid time: " + Format::toString(val.t) + " < 0");
  }

  // checking consistency
  for (unsigned int i=0;i<vrates.size();i++)
  {
    Rate aux = vrates[i];

    if (fabs(aux.t-val.t) < EPSILON)
    {
      string msg = "rate time ";
      msg += Format::toString(val.t);
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
    if (getNumAttributes(attributes) == 0) {
      type = Compound;
    }
    else if (getNumAttributes(attributes) == 1)
    {
      string str = Strings::trim(getStringAttribute(attributes, "type", ""));
      str = Strings::lowercase(str);
      if (str == "simple") {
        type = Simple;
      }
      else if (str == "compound") {
        type = Compound;
      }
      else if (str == "continuous") {
        type = Continuous;
      }
      else  {
        throw Exception("invalid type value at <interest>");
      }
    }
    else {
      throw Exception("incorrect number of attributes");
    }
  }
  else if (isEqual(name_,"rate")) {
    // setting new handlers
    auxrate = Rate();
    eppush(eu, &auxrate, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"interest")) {
    if (vrates.size() == 0) {
      throw Exception("interest has no rates");
    }
    else {
      sort(vrates.begin(), vrates.end());
    }
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
  string strtype = "";
  
  if (type == Simple) strtype = "simple";
  else if (type == Continuous) strtype = "continuous";
  else strtype = "compound";  
  
  ret += spc + "<interest type='" + strtype + "'>\n";;

  for (unsigned int i=0;i<vrates.size();i++)
  {
    ret += vrates[i].getXML(ilevel+2);
  }

  ret += spc + "</interest>\n";

  return ret;
}
