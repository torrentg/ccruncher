
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
//===========================================================================


#include <cfloat>
#include "CorrelationMatrix.hpp"
#include "utils/XMLUtils.hpp"
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
ccruncher::CorrelationMatrix::CorrelationMatrix(Sectors *sectors_, const DOMNode& node) throw(Exception)
{
  try
  {
    // posem valors per defecte
    init(sectors_);
    // recollim els parametres de la simulacio
    parseDOMNode(node);
    // validem les dades
    validate();
  }
  catch(Exception &e)
  {
    Utils::deallocMatrix(matrix, n);
    throw e;
  }
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
// interpreta un node XML params
//===========================================================================
void ccruncher::CorrelationMatrix::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "mcorrels"))
  {
    string msg = "CorrelationMatrix::parseDOMNode(): Invalid tag. Expected: mcorrels. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  epsilon = XMLUtils::getDoubleAttribute(attributes, "epsilon", DBL_MAX);
  if (epsilon == DBL_MAX || epsilon < 0.0)
  {
    throw Exception("CorrelationMatrix::parseDOMNode(): invalid attributes at <mcorrels>");
  }

  // recorrem tots els items
  DOMNodeList &children = *node.getChildNodes();
  if (&children != NULL)
  {
    for(unsigned int i=0;i<children.getLength();i++)
    {
      DOMNode &child = *children.item(i);

      if (XMLUtils::isVoidTextNode(child) || XMLUtils::isCommentNode(child))
      {
        continue;
      }
      else if (XMLUtils::isNodeName(child, "sigma"))
      {
        parseSigma(child);
      }
      else
      {
        string msg = "CorrelationMatrix::parseDOMNode(): invalid data structure at <mcorrels>: ";
        msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}


//===========================================================================
// interpreta un node XML time
//===========================================================================
void ccruncher::CorrelationMatrix::parseSigma(const DOMNode& node) throw(Exception)
{
  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();

  // agafem els atributs del node
  string sector1 = XMLUtils::getStringAttribute(attributes, "sector1", "");
  string sector2 = XMLUtils::getStringAttribute(attributes, "sector2", "");
  double value = XMLUtils::getDoubleAttribute(attributes, "value", DBL_MAX);

  if (sector1 == "" || sector2 == "" || value == DBL_MAX)
  {
    throw Exception("CorrelationMatrix::parseSigma(): invalid values at <sigma>");
  }

  // insertem el valor en la matriu de transicio
  insertSigma(sector1, sector2, value);

  // acabem de comprovar la estructura
  if (node.getChildNodes()->getLength() != 0)
  {
    throw Exception("CorrelationMatrix::parseSigma(): invalid data structure at <sigma>");
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
