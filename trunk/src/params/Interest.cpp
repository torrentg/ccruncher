
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
#include "params/Interest.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include "utils/Parser.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Interest::Interest(const Date &date_, InterestType type_)
{
  type = type_;
  date = date_;
}

//===========================================================================
// set date
//===========================================================================
void ccruncher::Interest::setDate(const Date &d)
{
  date = d;
}

//===========================================================================
// return interest type
//===========================================================================
ccruncher::Interest::InterestType ccruncher::Interest::getType() const
{
  return type;
}

//===========================================================================
// returns the numbers of rates
//===========================================================================
int ccruncher::Interest::size() const
{
  return (int) rates.size();
}

//===========================================================================
// returns linear interpolated interest rate at day d from date
// we don't use cubic splines because curve can oscillate depending on
// entered points
//===========================================================================
void ccruncher::Interest::getValues(int d, double *t, double *r) const
{
  size_t n = rates.size();

  if (n == 0 || d <= 0)
  {
    *t = 0.0;
    *r = 0.0;
  }
  else if (d <= rates[0].d)
  {
    int diff = d - 0;
    int delta = rates[0].d - 0;
    *t = 0.0 + diff*(rates[0].y - 0)/delta;
    *r = 1.0 + diff*(rates[0].r - 1.0)/delta;
  }
  else if (rates[n-1].d <= d)
  {
    *t = rates[n-1].y;
    *r = rates[n-1].r;
  }
  else
  {
    vector<Rate>::const_iterator it2 = lower_bound(rates.begin(), rates.end(), Rate(d));
    assert(it2 != rates.begin());
    assert(it2 != rates.end());
    vector<Rate>::const_iterator it1 = it2-1;
    int diff = d - it1->d;
    int delta = it2->d - it1->d;
    *t = it1->y + diff*(it2->y - it1->y)/delta;
    *r = it1->r + diff*(it2->r - it1->r)/delta;
  }
}

//===========================================================================
// returns rate at date
//===========================================================================
double ccruncher::Interest::getValue(const Date &d) const
{
  double r, t;
  getValues(d-date, &t, &r);
  return r;
}

//===========================================================================
// returns factor to aply to transport a money value from date1 to date0
// where date0 is the interest curve date
//===========================================================================
double ccruncher::Interest::getFactor(const Date &date1) const
{
  if (date1 <= date) return 1.0;

  int d = date1 - date;
  double r, t; // t = time in years

  getValues(d, &t, &r);

  if (type == Simple)
  {
    return 1.0/(1.0+r*t);
  }
  else if (type == Compound)
  {
    return 1.0/pow(1.0+r, t);
  }
  else if (type == Continuous)
  {
    return 1.0/exp(r*t);
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
void ccruncher::Interest::insertRate(const Rate &val) throw(Exception)
{
  assert(date != NAD);

  if (date == NAD) throw Exception("interest curve without date");

  if (val.d < 0)
  {
    throw Exception("rate with invalid time");
  }

  if (val.d == 0 && val.r != 0.0)
  {
    throw Exception("rate at t=0 distinct than 0");
  }

  // checking if previously defined
  for (size_t i=0; i<rates.size(); i++)
  {
    Rate aux = rates[i];

    if (abs(aux.d-val.d) == 0)
    {
      throw Exception("rate time '" + val.t_str + "' repeated");
    }
  }

  // inserting value
  try
  {
    rates.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"interest"))
  {
    if (getNumAttributes(attributes) == 0) {
      type = Compound;
    }
    else if (getNumAttributes(attributes) == 1)
    {
      string str = Strings::trim(getStringAttribute(attributes, "type"));
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
  else if (isEqual(name_,"rate"))
  {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes");
    }

    Date t(date);
    const char *str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      t.add(str);
    }
    else {
      t = Date(str);
    }
    int d = t - date;

    double r = getDoubleAttribute(attributes, "r");
    if (r < -0.5 || 1.0 < r)
    {
      throw Exception("rate value " + Format::toString(r) + " out of range [-0.5, +1.0]");
    }

    insertRate(Rate(d, diff(date, t, 'Y'), r, str));
  }
  else
  {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"interest")) {
    if (rates.empty()) {
      throw Exception("interest has no rates");
    }
    else {
      sort(rates.begin(), rates.end());
    }
  }
  else if (isEqual(name_,"rate")) {
    // nothing to do
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

  for (unsigned int i=0; i<rates.size(); i++)
  {
    ret += Strings::blanks(ilevel+2);
    ret += "<rate ";
    ret += "t='" + rates[i].t_str + "' ";
    ret += "r='" + Format::toString(100.0*rates[i].r) + "%'";
    ret += "/>\n";
  }

  ret += spc + "</interest>\n";

  return ret;
}
