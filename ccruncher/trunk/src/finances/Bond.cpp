
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
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include "Bond.hpp"
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
  adquisitiondate = Date(1,1,1);
  adquisitionprice = NAN;

  filled = false;
  length = -1;
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
  if (filled == true)
  {
    throw Exception("Bond::setProperty(): all Bond properties filled");
  } 
  else if (name == "issuedate")
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
  else if (name == "adquisitiondate")
  {
    adquisitiondate = Parser::dateValue(value);
  }
  else if (name == "adquisitionprice")
  {
    adquisitionprice = Parser::doubleValue(value);
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
  if (filled == true)
  {
    return true;
  } 
  else if (issuedate == Date(1,1,1) || term == -1 || isnan(nominal) || isnan(rate) || 
           ncoupons == -1 || adquisitiondate == Date(1,1,1) || isnan(adquisitionprice))
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
  else if (adquisitionprice <= 0.0 || adquisitiondate >= addMonths(issuedate, term))
  {
    return false;
  }
  else
  {
    filled = true;
    return true;
  }
}

//===========================================================================
// getCashFlow
//===========================================================================
DateValues* ccruncher::Bond::simulate()
{
  int n = ncoupons+1;
  int m = term/ncoupons;
  double r = rate/(12.0/m);
  DateValues *ret = new DateValues[n];

  ret[0].date = issuedate;
  ret[0].cashflow = -nominal;
  ret[0].exposure = nominal;

  for (int i=1;i<n-1;i++)
  {
    ret[i].date = addMonths(issuedate, i*m);
    ret[i].cashflow = nominal*r;
    ret[i].exposure = nominal;
  }  

  ret[n-1].date = addMonths(issuedate, term);
  ret[n-1].cashflow = nominal*(1.0+r);
  ret[n-1].exposure = nominal;

  return ret;
}

//===========================================================================
// getSize
//===========================================================================
int ccruncher::Bond::getISize() throw(Exception)
{
  if (!validate())
  {
    throw Exception("Bond::getSize(): invalid properties");
  }
  
  if (length < 0)
  {
    throw Exception("Bond::getSize(): run getEvents() first");
  }  

  return length;
}

//===========================================================================
// getEvents
//===========================================================================
DateValues* ccruncher::Bond::getIEvents() throw(Exception)
{
  if (!validate()) {
    throw Exception("Bond::getEvents(): invalid properties");
  } 

  int n = ncoupons+1;
  DateValues *aux = simulate();

  for (int i=0;i<n;i++) {
    if (aux[i].date == adquisitiondate ) {
      length = n;
    }
  }
  
  if (length == -1) {
    length = n+1;
  }
  
  for (int i=0;i<n;i++) {
    if (aux[i].date < adquisitiondate ) {
      length--;
    } else {
      break;
    }
  }
  
  DateValues *events = new DateValues[length];
  int cont = 0;
  
  if (adquisitiondate < aux[0].date) {
    events[cont].date = adquisitiondate;
    events[cont].cashflow = -adquisitionprice;
    events[cont].exposure = +adquisitionprice;
    cont++;
  }
  
  for(int i=0;i<n-1;i++)
  {
    if (aux[i].date > adquisitiondate) {
      events[cont].date = aux[i].date;
      events[cont].cashflow = (i==0?0.0:aux[i].cashflow);
      events[cont].exposure = +adquisitionprice;
      cont++;      
    } else if (aux[i].date == adquisitiondate) {
      events[cont].date = aux[i].date;
      events[cont].cashflow = -adquisitionprice;
      events[cont].exposure = +adquisitionprice;
      cont++;          
    } else if (adquisitiondate < aux[i+1].date) {
      events[cont].date = adquisitiondate;
      events[cont].cashflow = -adquisitionprice;
      events[cont].exposure = +adquisitionprice;
      cont++;      
    }
  }  

  events[cont].date = aux[n-1].date;
  events[cont].cashflow = aux[n-1].cashflow;
  events[cont].exposure = +adquisitionprice;
  cont++;
  
  delete [] aux;
  return events;
}
