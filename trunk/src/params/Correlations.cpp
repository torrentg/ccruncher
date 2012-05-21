
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
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"

#define EPSILON 1e-12
#define TYPE_BASIC "basic"
#define TYPE_SPEARMAN "spearman"
#define TYPE_KENDALL "kendall"

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Correlations::Correlations()
{
  fcoerce = false;
  type = spearman;
  matrix = NULL;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Correlations::Correlations(const Sectors &sectors_) throw(Exception)
{
  fcoerce = false;
  type = spearman;
  matrix = NULL;
  setSectors(sectors_);
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::Correlations::Correlations(const Correlations &x) throw(Exception)
{
  matrix = NULL;
  *this = x;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Correlations::~Correlations()
{
  if (matrix != NULL && size() > 0) {
    Arrays<double>::deallocMatrix(matrix, size());
  }
}

//===========================================================================
// assignment operator
//===========================================================================
Correlations& ccruncher::Correlations::operator = (const Correlations &x)
{
  type = x.getType();
  fcoerce = x.fcoerce;
  setSectors(x.sectors);

  if (size() == 0)
  {
    matrix = NULL;
    return *this;
  }
  else
  {
    for(int i=0; i<size(); i++)
    {
      for(int j=0; j<size(); j++)
      {
        matrix[i][j] = x.matrix[i][j];
      }
    }
  }

  return *this;
}

//===========================================================================
// set sectors
//===========================================================================
void ccruncher::Correlations::setSectors(const Sectors &sectors_) throw(Exception)
{
  if (matrix != NULL && size() > 0)
  {
    Arrays<double>::deallocMatrix(matrix, size());
    matrix = NULL;
  }

  sectors = sectors_;
  if (sectors.size() <= 0)
  {
    throw Exception("invalid matrix dimension ("+Format::toString(size())+" <= 0)");
  }
  matrix = Arrays<double>::allocMatrix(size(), size(), NAN);
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
double ** ccruncher::Correlations::getMatrix() const
{
  return matrix;
}

//===========================================================================
// returns correlation type
//===========================================================================
CorrelationType ccruncher::Correlations::getType() const
{
  return type;
}

//===========================================================================
// returns coerce flag
//===========================================================================
bool ccruncher::Correlations::getCoerce() const
{
  return fcoerce;
}

//===========================================================================
// inserts an element into matrix
//===========================================================================
void ccruncher::Correlations::insertSigma(const string &sector1, const string &sector2, double value) throw(Exception)
{
  int row = sectors.getIndex(sector1);
  int col = sectors.getIndex(sector2);

  // checking index sector
  if (row < 0 || col < 0)
  {
    string msg = "undefined sector at <sigma>, sector1=" + sector1 + ", sector2=" + sector2;
    throw Exception(msg);
  }

  // checking value
  if (value <= -(1.0+EPSILON) || (1.0+EPSILON) <= value )
  {
    string msg = "correlation value[" + sector1 + "][" + sector2 + "] out of range: " + 
                 Format::toString(value);
    throw Exception(msg);
  }

  // checking that value don't exist
  if (!isnan(matrix[row][col]) || !isnan(matrix[col][row]))
  {
    string msg = "redefined correlation [" + sector1 + "][" + sector2 + "] in <sigma>";
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
    string strtype = getStringAttribute(attributes, "type", TYPE_SPEARMAN);
    if (strtype == TYPE_SPEARMAN) type = spearman;
    else if (strtype == TYPE_KENDALL) type = kendall;
    else if (strtype == TYPE_BASIC) type = basic;
    else throw Exception("unknow correlation type: " + strtype);
    fcoerce = getBooleanAttribute(attributes, "coerce", false);
  }
  else if (isEqual(name,"sigma")) {
    string sector1 = getStringAttribute(attributes, "sector1");
    string sector2 = getStringAttribute(attributes, "sector2");
    double value = getDoubleAttribute(attributes, "value");
    insertSigma(sector1, sector2, value);
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
  else if (isEqual(name,"sigma")) {
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

  string strtype = "";
  if (type == spearman) strtype= TYPE_SPEARMAN;
  else if (type == kendall) strtype = TYPE_KENDALL;
  else strtype = TYPE_BASIC;

  ret += spc1 + "<correlations type='" + strtype + "' coerce='" + Format::toString(fcoerce) + "'>\n";

  for(int i=0;i<size();i++)
  {
    for(int j=i;j<size();j++)
    {
      ret += spc2 + "<sigma ";
      ret += "sector1='" + sectors[i].name + "' ";
      ret += "sector2='" + sectors[j].name + "' ";
      ret += "value='" + Format::toString(100.0*matrix[i][j]) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</correlations>\n";

  return ret;
}
