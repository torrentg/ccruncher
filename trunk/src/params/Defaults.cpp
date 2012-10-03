
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
// inserts an element into data
//===========================================================================
void ccruncher::Defaults::insertValue(const string &t, const string &sector, int nobligors, int ndefaulted) throw(Exception)
{
  // parsing sector identifier
  int col = sectors.getIndex(sector);
  if (col < 0) throw Exception("unrecognized sector identifier (" + sector + ")");

  // retrieving data row
  int row = -1;
  map<string,int>::iterator it = indices.find(t);
  if (it == indices.end()) {
    data.push_back(vector<hdata>(sectors.size()));
    indices[t] = data.size()-1;
    row = data.size()-1;
  }
  else {
    row = it->second;
  }

  // inserting values
  if (data[row][col].nobligors >= 0) throw Exception("value defined twice");
  if (ndefaulted < 0 || nobligors < ndefaulted) throw Exception("invalid historical values");
  data[row][col] = hdata(nobligors, ndefaulted);
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
  else if (isEqual(name,"data")) {
    string t = getStringAttribute(attributes, "t");
    string sector = getStringAttribute(attributes, "sector");
    int nobligors = getIntAttribute(attributes, "obligors");
    int ndefaulted = getIntAttribute(attributes, "defaulted");
    insertValue(t, sector, nobligors, ndefaulted);
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
  else if (isEqual(name,"data")) {
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
  if (data.size() == 0) throw Exception("data not found");

  for(int i=0; i<(int)data.size(); i++)
  {
    for(int j=0; j<sectors.size(); j++)
    {
      if (data[i][j].nobligors < 0)
      {
        string t="unknow";
        map<string,int>::iterator it;
        for (it=indices.begin(); it!=indices.end(); it++) {
          if (it->second==i) {
            t = it->first;
            break;
          }
        }
        throw Exception("value not found (t=" + t + ", sector=" + sectors[i].name + ")");
      }
    }
  }
}

//===========================================================================
// returns data
//===========================================================================
const vector<vector<hdata> >& ccruncher::Defaults::getData() const
{
  return data;
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

  map<string,int>::iterator it;

  for(it=indices.begin(); it!=indices.end(); it++)
  {
    int row = it->second;
    for(int col=0; col<sectors.size(); col++)
    {
      ret += spc2 + "<data ";
      ret += "t='" + it->first  + "' ";
      ret += "sector='" + sectors[col].name + "' ";
      ret += "obligors='" + Format::toString(data[row][col].nobligors) + "' ";
      ret += "defaulted='" + Format::toString(data[row][col].ndefaulted) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</historical>\n";

  return ret;
}
