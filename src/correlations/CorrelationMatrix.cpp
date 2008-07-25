
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
// CorrelationMatrix.cpp - CorrelationMatrix code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <cfloat>
#include "correlations/CorrelationMatrix.hpp"
#include "utils/Parser.hpp"
#include "utils/Utils.hpp"
#include "math/CholeskyDecomposition.hpp"

//===========================================================================
// inicialitzador privat
//===========================================================================
void ccruncher::CorrelationMatrix::init(Sectors *sectors_) throw(Exception)
{
  epsilon = -1.0;
  sectors = sectors_;

  n = sectors->getSectors()->size();

  if (n <= 0)
  {
    throw Exception("CorrelationMatrix::init(): invalid matrix range");
  }

  // inicialitzem la matriu
  matrix = Utils::allocMatrix(n, n, NAN);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::CorrelationMatrix::CorrelationMatrix(Sectors *sectors_) throw(Exception)
{
  // posem valors per defecte
  init(sectors_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::CorrelationMatrix::~CorrelationMatrix()
{
  Utils::deallocMatrix(matrix, n);
}

//===========================================================================
// metodes acces variable begindate
//===========================================================================
int ccruncher::CorrelationMatrix::size()
{
  return n;
}

//===========================================================================
// metodes acces variable begindate
//===========================================================================
double ** ccruncher::CorrelationMatrix::getMatrix()
{
  return matrix;
}


//===========================================================================
// inserta un element en la matriu de transicio
//===========================================================================
void ccruncher::CorrelationMatrix::insertSigma(const string &sector1, const string &sector2, double value) throw(Exception)
{
  int row = sectors->getIndex(sector1);
  int col = sectors->getIndex(sector2);

  // validem sectors entrats
  if (row < 0 || col < 0)
  {
    string msg = "CorrelationMatrix::insertSigma(): undefined sector at <sigma> ";
    msg += sector1;
    msg += " -> ";
    msg += sector2;
    throw Exception(msg);
  }

  // validem valor entrat
  if (value < -(1.0 + epsilon) || value > (1.0 + epsilon))
  {
    string msg = "CorrelationMatrix::insertSigma(): value[";
    msg += sector1;
    msg += "][";
    msg += sector2;
    msg += "] out of range: ";
    msg += Parser::double2string(value);
    throw Exception(msg);
  }

  // comprovem que no es trobi definit
  if (!isnan(matrix[row][col]) || !isnan(matrix[col][row]))
  {
    string msg = "CorrelationMatrix::insertSigma(): redefined element [";
    msg += sector1;
    msg += "][";
    msg += sector2;
    msg += "] in <sigma>";
    throw Exception(msg);
  }

  // inserim en la matriu de correlacions
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
      throw eperror(eu, "invalid number of attributes in tag mcorrels");
    }
    else {
      epsilon = getDoubleAttribute(attributes, "epsilon", 1e-12);
      if (epsilon < 0.0 || epsilon > 1.0) {
        throw eperror(eu, "invalid attribute at <mcorrels>");
      }      
    }
  }
  else if (isEqual(name,"sigma")) {
    string sector1 = getStringAttribute(attributes, "sector1", "");
    string sector2 = getStringAttribute(attributes, "sector2", "");
    double value = getDoubleAttribute(attributes, "value", DBL_MAX);

    if (sector1 == "" || sector2 == "" || value == DBL_MAX)
    {
      throw eperror(eu, "invalid values at <sigma>");
    }
    else {
      insertSigma(sector1, sector2, value);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name));
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
    throw eperror(eu, "unexpected end tag " + string(name));
  }
}

//===========================================================================
// validacio del contingut de la classe
//===========================================================================
void ccruncher::CorrelationMatrix::validate() throw(Exception)
{
  // comprovem que tots els elements es troben definits
  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      if (isnan(matrix[i][j]))
      {
        string msg = "CorrelationMatrix::validate(): undefined element [";
        msg += (i+1);
        msg +=  "][";
        msg += (j+1);
        msg += "]";
        throw Exception(msg);
      }
    }
  }

  // comprovem que es tracta de una matriu definida positiva
  double **maux = Utils::allocMatrix(n, n, getMatrix());
  double *vaux = Utils::allocVector(n);
  bool ret = CholeskyDecomposition::choldc(maux, vaux, n);
  Utils::deallocVector(vaux);
  Utils::deallocMatrix(maux, n);
  if (ret == false)
  {
    throw Exception("CorrelationMatrix::validate(): matrix non definite prositive");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::CorrelationMatrix::getXML(int ilevel) throw(Exception)
{
  string spc1 = Utils::blanks(ilevel);
  string spc2 = Utils::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<mcorrels epsilon='" + Parser::double2string(epsilon) + "'>\n";

  for(int i=0;i<n;i++)
  {
    for(int j=i;j<n;j++)
    {
      ret += spc2 + "<sigma ";
      ret += "sector1 ='" + sectors->getName(i) + "' ";
      ret += "sector2 ='" + sectors->getName(j) + "' ";
      ret += "value ='" + Parser::double2string(matrix[i][j]) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</mcorrels>\n";

  return ret;
}
