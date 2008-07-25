
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
// Interest.cpp - Interest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "interests/Interest.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"

// --------------------------------------------------------------------------

#define EPSILON 1e-7

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Interest::Interest()
{
  reset();
}

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Interest::Interest(const string &str)
{
  // nom del Interest
  name = str;
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Interest::reset()
{
  // flushing vector
  vrates.clear();

  // nom del Interest
  name = "UNDEFINED_INTEREST";

  // establim la data de la corba
  fecha = Date(1,1,1900);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interest::~Interest()
{
  // atention at vrates allocation
}

//===========================================================================
// metodes acces variable name
//===========================================================================
string ccruncher::Interest::getName() const
{
  return name;
}

//===========================================================================
// metodes acces variable name
//===========================================================================
Date ccruncher::Interest::getFecha() const
{
  return fecha;
}

//===========================================================================
// retorna el tipus de interes interpolat en el mes t
//===========================================================================
double ccruncher::Interest::getValue(const double t)
{
  Rate aux = vrates[0];

  if (t <= aux.t)
  {
    return aux.r;
  }
  else
  {
    for(unsigned int i=1;i<vrates.size();i++)
    {
      aux = vrates[i];

      if (t <= aux.t)
      {
        Rate prev = vrates[i-1];

        return prev.r + (t-prev.t)*(aux.r - prev.r)/(aux.t - prev.t);
      }
    }

    aux = vrates[vrates.size()-1];

    return aux.r;
  }
}

//===========================================================================
// retorna la data del mes t
//===========================================================================
Date ccruncher::Interest::idx2date(int t)
{
  return addMonths(fecha, t);
}

//===========================================================================
// retorna el index de la data date1
//===========================================================================
double ccruncher::Interest::date2idx(Date &date1)
{
  return (double)(date1-fecha)/30.3958;
}

//===========================================================================
// retorna el coeficient de revaloritzacio a temps t
// @param r tipus de interes anual
// @param t temps que es vol actualitzar (en mesos)
//===========================================================================
double ccruncher::Interest::updateCoef(const double r, const double t)
{
  return pow(1.0 + r, t/12.0);
}

//===========================================================================
// retorna el coeficient de transport entre date1 i date2
//===========================================================================
double ccruncher::Interest::getUpdateCoef(Date &date1, Date &date2) throw(Exception)
{
  double t1 = date2idx(date1);
  double t2 = date2idx(date2);

  double r1 = getValue(t1);
  double r2 = getValue(t2);

  double x1 = updateCoef(r1,t1);
  double x2 = updateCoef(r2,t2);

  return x2/x1;
}

//===========================================================================
// retorna el coeficient de actualitzacio a temps t
// @param r tipus de interes anual
// @param t temps que es vol actualitzar (en mesos)
//===========================================================================
double ccruncher::Interest::actualCoef(const double r, const double t)
{
  return 1.0/updateCoef(r,t);
}

//===========================================================================
// retorna el coeficient de transport entre date1 i date2
//===========================================================================
double ccruncher::Interest::getActualCoef(Date &date1, Date &date2) throw(Exception)
{
  double t1 = date2idx(date1);
  double t2 = date2idx(date2);

  double r1 = getValue(t1);
  double r2 = getValue(t2);

  double x1 = actualCoef(r1,t1);
  double x2 = actualCoef(r2,t2);

  return x2/x1;
}

//===========================================================================
// insertRate
//===========================================================================
void ccruncher::Interest::insertRate(Rate &val) throw(Exception)
{
  if (val.t < 0)
  {
    throw Exception("Interest::insertRate(): invalid time value");
  }

  // validem coherencia
  for (unsigned int i=0;i<vrates.size();i++)
  {
    Rate aux = vrates[i];

    if (fabs(aux.t-val.t) < EPSILON)
    {
      string msg = "Interest::insertRate(): time ";
      msg += Parser::double2string(val.t);
      msg += " repeated";
      throw Exception(msg);
    }
  }

  // inserim el valor
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
      throw eperror(eu, "incorrect number of attributes");
    }
    else
    {
      // getting attributes
      name = getStringAttribute(attributes, "name", "");
      fecha = getDateAttribute(attributes, "date", Date(1,1,1900));
      if (name == "" || fecha == Date(1,1,1900))
      {
        throw eperror(eu, "invalid attributes values at <interest>");
      }
    }
  }
  else if (isEqual(name_,"rate")) {
    // setting new handlers
    auxrate.reset();
    eppush(eu, &auxrate, name_, attributes);  
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
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
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Interest::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<interest name='" + name + "' date='" + Parser::date2string(fecha) + "'>\n";

  for (unsigned int i=0;i<vrates.size();i++)
  {
    ret += vrates[i].getXML(ilevel+2);
  }

  ret += spc + "</interest>\n";

  return ret;
}
