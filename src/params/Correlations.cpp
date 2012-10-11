
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
#include "params/Correlations.hpp"
#include "utils/Format.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Correlations::Correlations()
{
  // nothing to do
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Correlations::Correlations(const Sectors &sectors_) throw(Exception)
{
  setSectors(sectors_);
}

//===========================================================================
// set sectors
//===========================================================================
void ccruncher::Correlations::setSectors(const Sectors &sectors_) throw(Exception)
{
  if (sectors_.size() <= 0) {
    throw Exception("sectors not found");
  }
  else {
    sectors = sectors_;
    matrix.assign(size(), vector<double>(size(), NAN));
  }
}

//===========================================================================
// returns size (number of sectors)
//===========================================================================
int ccruncher::Correlations::size() const
{
  return sectors.size();
}

//===========================================================================
// returns matrix
//===========================================================================
const vector<vector<double> > &ccruncher::Correlations::getMatrix() const
{
  return matrix;
}

//===========================================================================
// inserts an element into matrix
//===========================================================================
void ccruncher::Correlations::insertCorrelation(const string &sector1, const string &sector2, double value) throw(Exception)
{
  int row = sectors.getIndex(sector1);
  int col = sectors.getIndex(sector2);

  // checking index sector
  if (row < 0 || col < 0)
  {
    string msg = "undefined sector at <correlation>, sector1=" + sector1 + ", sector2=" + sector2;
    throw Exception(msg);
  }

  // checking value
  if (row == col && (value < 0.0 || 1.0 < value) )
  {
    string msg = "factor loading [" + sector1 + "][" + sector2 + "] out of range [0,1]";
    throw Exception(msg);
  }
  if (row != col && (value < -1.0 || 1.0 < value) )
  {
    string msg = "correlation value[" + sector1 + "][" + sector2 + "] out of range [-1,+1]";
    throw Exception(msg);
  }

  // checking that value don't exist
  if (!isnan(matrix[row][col]) || !isnan(matrix[col][row]))
  {
    string msg = "redefined correlation [" + sector1 + "][" + sector2 + "] in <correlation>";
    throw Exception(msg);
  }

  // insering value into matrix
  matrix[row][col] = value;
  matrix[col][row] = value;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Correlations::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"correlations")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes not allowed in tag correlations");
    }
  }
  else if (isEqual(name,"correlation")) {
    string sector1 = getStringAttribute(attributes, "sector1");
    string sector2 = getStringAttribute(attributes, "sector2");
    double value = getDoubleAttribute(attributes, "value");
    insertCorrelation(sector1, sector2, value);
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Correlations::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"correlations")) {
    validate();
  }
  else if (isEqual(name,"correlation")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// validate class content
//===========================================================================
void ccruncher::Correlations::validate() throw(Exception)
{
  // checking that all matrix elements exists
  for (int i=0;i<size();i++)
  {
    for (int j=0;j<size();j++)
    {
      if (isnan(matrix[i][j]))
      {
        string msg = "non defined correlation element [" + Format::toString(i+1) + 
                     "][" + Format::toString(j+1) + "]";
        throw Exception(msg);
      }
    }
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Correlations::getXML(int ilevel) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<correlations>\n";

  for(int i=0;i<size();i++)
  {
    for(int j=i;j<size();j++)
    {
      ret += spc2 + "<correlation ";
      ret += "sector1='" + sectors.getName(i) + "' ";
      ret += "sector2='" + sectors.getName(j) + "' ";
      ret += "value='" + Format::toString(100.0*matrix[i][j]) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</correlations>\n";

  return ret;
}


//===========================================================================
// matrix element access
//===========================================================================
const vector<double>& ccruncher::Correlations::operator[] (int row) const
{
  assert(row >= 0 && row < (int)matrix.size());
  return matrix[row];
}
