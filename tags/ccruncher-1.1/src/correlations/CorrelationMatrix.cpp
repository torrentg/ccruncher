
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
// CorrelationMatrix.cpp - CorrelationMatrix code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/04/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug when correlation = 0 and exist only 1 sector
//
// 2005/04/23 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug related to validation
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Arrays class
//   . implemented Strings class
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed pointers by references
//   . Sectors class refactoring
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/07/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed sector.order tag
//
//===========================================================================

#include <cmath>
#include <cfloat>
#include "correlations/CorrelationMatrix.hpp"
#include "utils/Format.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"

//===========================================================================
// private initializator
//===========================================================================
void ccruncher::CorrelationMatrix::init(Sectors &sectors_) throw(Exception)
{
  epsilon = -1.0;
  sectors = &sectors_;

  n = (*sectors).size();

  if (n <= 0)
  {
    throw Exception("invalid matrix dimension ("+Format::int2string(n)+" <= 0)");
  }

  // inicializing matrix
  matrix = Arrays<double>::allocMatrix(n, n, NAN);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::CorrelationMatrix::CorrelationMatrix(Sectors &sectors_) throw(Exception)
{
  // seting default values
  init(sectors_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::CorrelationMatrix::~CorrelationMatrix()
{
  Arrays<double>::deallocMatrix(matrix, n);
}

//===========================================================================
// returns size (number of sectors)
//===========================================================================
int ccruncher::CorrelationMatrix::size() const
{
  return n;
}

//===========================================================================
// returns matrix
//===========================================================================
double ** ccruncher::CorrelationMatrix::getMatrix() const
{
  return matrix;
}


//===========================================================================
// inserts an element into matrix
//===========================================================================
void ccruncher::CorrelationMatrix::insertSigma(const string &sector1, const string &sector2, double value) throw(Exception)
{
  int row = (*sectors).getIndex(sector1);
  int col = (*sectors).getIndex(sector2);

  // checking index sector
  if (row < 0 || col < 0)
  {
    string msg = "undefined sector at <sigma>, sector1=" + sector1 + ", sector2=" + sector2;
    throw Exception(msg);
  }

  // checking value
  if (value <= -(1.0 - epsilon) || value >= (1.0 - epsilon))
  {
    string msg = "correlation value[" + sector1 + "][" + sector2 + "] out of range: " + 
                 Format::double2string(value);
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
void ccruncher::CorrelationMatrix::epstart(ExpatUserData &eu, const char *name, const char **attributes)
{
  if (isEqual(name,"mcorrels")) {
    if (1 < getNumAttributes(attributes)) {
      throw Exception("invalid number of attributes in tag mcorrels");
    }
    else {
      epsilon = getDoubleAttribute(attributes, "epsilon", 1e-12);
      if (epsilon < 0.0 || epsilon > 1.0) {
        throw Exception("invalid attribute at <mcorrels>");
      }
    }
  }
  else if (isEqual(name,"sigma")) {
    string sector1 = getStringAttribute(attributes, "sector1", "");
    string sector2 = getStringAttribute(attributes, "sector2", "");
    double value = getDoubleAttribute(attributes, "value", DBL_MAX);

    if (sector1 == "" || sector2 == "" || value == DBL_MAX)
    {
      throw Exception("invalid values at <sigma>");
    }
    else {
      insertSigma(sector1, sector2, value);
    }
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::CorrelationMatrix::epend(ExpatUserData &eu, const char *name)
{
  if (isEqual(name,"mcorrels")) {
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
void ccruncher::CorrelationMatrix::validate() throw(Exception)
{
  // checking that all matrix elements exists
  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      if (isnan(matrix[i][j]))
      {
        string msg = "non defined correlation element [" + Format::int2string(i+1) + 
                     "][" + Format::int2string(j+1) + "]";
        throw Exception(msg);
      }
    }
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::CorrelationMatrix::getXML(int ilevel) const throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<mcorrels epsilon='" + Format::double2string(epsilon) + "'>\n";

  for(int i=0;i<n;i++)
  {
    for(int j=i;j<n;j++)
    {
      ret += spc2 + "<sigma ";
      ret += "sector1 ='" + (*sectors)[i].name + "' ";
      ret += "sector2 ='" + (*sectors)[j].name + "' ";
      ret += "value ='" + Format::double2string(matrix[i][j]) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</mcorrels>\n";

  return ret;
}
