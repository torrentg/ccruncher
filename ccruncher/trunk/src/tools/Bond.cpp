
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
// Bond.cpp - Bond code
// --------------------------------------------------------------------------
//
// 2005/03/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (inherited from finances/Bond.cpp)
//
//===========================================================================

#include <cmath>
#include "tools/Bond.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Bond::Bond()
{
  issuedate = Date(1,1,1);
  term = -1;
  nominal = NAN;
  rate = NAN;
  ncoupons = -1;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Bond::~Bond()
{
  // nothing to do
}

//===========================================================================
// setProperty
//===========================================================================
void ccruncher::Bond::setProperty(string name, string value) throw(Exception)
{
  if (name == "issuedate")
  {
    issuedate = Parser::dateValue(value);
  }
  else if (name == "term")
  {
    term = Parser::intValue(value);
  }
  else if (name == "nominal")
  {
    nominal = Parser::doubleValue(value);
  }
  else if (name == "rate")
  {
    rate = Parser::doubleValue(value);
  }
  else if (name == "ncoupons")
  {
    ncoupons = Parser::intValue(value);
  }
  else
  {
    throw Exception("Bond::setProperty(): property not allowed (" + name + ")");
  }
}

//===========================================================================
// validate
//===========================================================================
bool ccruncher::Bond::validate()
{
  if (issuedate == Date(1,1,1) || term == -1 || isnan(nominal) || isnan(rate) || ncoupons == -1)
  {
    return false;
  }
  else if (term <= 0 || nominal <= 0.0 || rate < 0.0 || rate >= 1.0 || ncoupons <= 0)
  {
    return false;
  }
  else if (ncoupons > term || term%ncoupons != 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================
// simulate
//===========================================================================
vector <DateValues> ccruncher::Bond::simulate() throw(Exception)
{
  if(!validate())
  {
    throw Exception("Bond::simulate(): non valid properties");
  }

  int n = ncoupons+1;
  int m = term/ncoupons;
  double r = rate/(12.0/m);
  vector <DateValues> ret;
  DateValues curr;

  curr.date = issuedate;
  curr.cashflow = -nominal;
  curr.exposure = nominal;
  curr.recovery = 0.8*nominal;
  ret.push_back(curr);

  for (int i=1;i<n-1;i++)
  {
    curr.date = addMonths(issuedate, i*m);
    curr.cashflow = nominal*r;
    curr.exposure = 0.0;
    curr.recovery = 0.0*nominal;
    ret.push_back(curr);
  }

  curr.date = addMonths(issuedate, term);
  curr.cashflow = nominal*(1.0+r);
  curr.exposure = 0.0;
  curr.recovery = 0.0*nominal;
  ret.push_back(curr);

  return ret;
}
