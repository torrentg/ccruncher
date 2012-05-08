
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
#include "params/Defaults.hpp"
#include "utils/Format.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Defaults::Defaults()
{
  period = 0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Defaults::Defaults(Sectors &sectors_, int period_) throw(Exception)
{
  setPeriod(period_);
  setSectors(sectors_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Defaults::~Defaults()
{
  // nothing to do
}

//===========================================================================
// set period
//===========================================================================
void ccruncher::Defaults::setPeriod(int val) throw(Exception)
{
  if (val <= 0) throw Exception("invalid period");
  else period = val;
}

//===========================================================================
// set sectors
//===========================================================================
void ccruncher::Defaults::setSectors(const Sectors &sectors_) throw(Exception)
{
  if (sectors_.size() <= 0) throw Exception("no sectors defined");
  sectors = sectors_;
}

//===========================================================================
// returns size (number of sectors)
//===========================================================================
int ccruncher::Defaults::size() const
{
  return values.size();
}

//===========================================================================
// returns matrix
//===========================================================================
double ** ccruncher::Defaults::getMatrix() const
{
  double **ret = Arrays<double>::allocMatrix(values.size(), sectors.size(), NAN);
  map<string,vector<double> >::const_iterator it;
  int row=0;

  for(it=values.begin(); it!=values.end(); it++)
  {
    for(int i=0; i<sectors.size(); i++)
    {
      ret[row][i] = it->second[i];
    }
    row++;
  }

  return ret;
}


//===========================================================================
// inserts an element into matrix
//===========================================================================
void ccruncher::Defaults::insertValue(const string &time, const string &sector, double val) throw(Exception)
{
  int col = sectors.getIndex(sector);
  if (col < 0) throw Exception("unrecognized sector identifier (" + sector + ")");

  map<string,vector<double> >::iterator it = values.find(time);
  if (it == values.end()) values[time] = vector<double>(sectors.size(), NAN);
  if (!isnan(values[time][col])) throw Exception("value defined twice");

  if (val < 0.0 || val > 1.0) throw Exception("value out of range [0,1]");

  values[time][col] = val;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Defaults::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"historical")) {
    int val = getIntAttribute(attributes, "period");
    setPeriod(val);
  }
  else if (isEqual(name,"defaults")) {
    string time = getStringAttribute(attributes, "t");
    string sector = getStringAttribute(attributes, "sector");
    double value = getDoubleAttribute(attributes, "value");
    insertValue(time, sector, value);
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Defaults::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"historical")) {
    validate();
  }
  else if (isEqual(name,"defaults")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// validate class content
//===========================================================================
void ccruncher::Defaults::validate() throw(Exception)
{
  if (values.size() == 0) throw Exception("data not found");

  map<string,vector<double> >::iterator it;
  for(it=values.begin(); it!=values.end(); it++)
  {
    for(int i=0; i<sectors.size(); i++)
    {
      if (isnan(it->second[i])) throw Exception("value not found (t=" + it->first + ", sector=" + sectors[i].name + ")");
    }
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Defaults::getXML(int ilevel) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<historical period='" + Format::toString(period) + "' >\n";

  map<string, vector<double> >::iterator it;

  for(it=values.begin(); it!=values.end(); it++)
  {
    for(int i=0; i<sectors.size(); i++)
    {
      ret += spc2 + "<defaults ";
      ret += "t='" + it->first  + "' ";
      ret += "sector='" + sectors[i].name + "' ";
      ret += "value='" + Format::toString(100.0*(it->second[i])) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</historical>\n";

  return ret;
}
