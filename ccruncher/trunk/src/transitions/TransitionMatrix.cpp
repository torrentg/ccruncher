
//***************************************************************************
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
// TransitionMatrix.cpp - TransitionMatrix code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <cfloat>
#include "TransitionMatrix.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/Parser.hpp"
#include "utils/Utils.hpp"
#include "math/PowMatrix.hpp"

//***************************************************************************
// inicialitzador privat
//***************************************************************************
void ccruncher::TransitionMatrix::init(Ratings *ratings_) throw(Exception)
{
  period = 0.0;
  epsilon = -1.0;
  ratings = ratings_;
  
  n = ratings->getRatings()->size();
  
  if (n <= 0)
  {
    throw Exception("TransitionMatrix::init(): invalid matrix range");
  }

  // inicialitzem la matriu  
  matrix = Utils::allocMatrix(n, n, NAN);  
}

//***************************************************************************
// constructor de copia
//***************************************************************************
ccruncher::TransitionMatrix::TransitionMatrix(TransitionMatrix &otm) throw(Exception)
{
  period = otm.period;
  ratings = otm.ratings;
  indexdefault = otm.indexdefault;
  epsilon = otm.epsilon;
  n = otm.n;
	
  // inicialitzem la matriu  
  matrix = Utils::allocMatrix(n, n);
  Utils::copyMatrix(otm.getMatrix(), n, n, matrix);	
}

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::TransitionMatrix::TransitionMatrix(Ratings *ratings_, const DOMNode& node) throw(Exception)
{
  try
  {
    // posem valors per defecte
    init(ratings_);
  
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

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::TransitionMatrix::~TransitionMatrix()
{
  Utils::deallocMatrix(matrix, n);
}

//***************************************************************************
// metodes acces variable begindate
//***************************************************************************
int ccruncher::TransitionMatrix::size()
{
  return n;
}

//***************************************************************************
// metodes acces variable begindate
//***************************************************************************
double ** ccruncher::TransitionMatrix::getMatrix()
{
  return matrix;
}

//***************************************************************************
// inserta un element en la matriu de transicio
//***************************************************************************
void ccruncher::TransitionMatrix::insertTransition(const string &rating1, const string &rating2, double value) throw(Exception)
{
  int row = ratings->getIndex(rating1); 
  int col = ratings->getIndex(rating2);

  // validem ratings entrats
  if (row < 0 || col < 0)
  {
    string msg = "TransitionMatrix::insertTransition(): undefined rating at <transition> ";
    msg += rating1;
    msg += " -> ";
    msg += rating2;
    throw Exception(msg);    
  }

  // validem valor entrat
  if (value < -epsilon || value > (1.0 + epsilon))
  {
    string msg = "TransitionMatrix::insertTransition(): value[";
    msg += rating1;
    msg += "][";
    msg += rating2;
    msg += "] out of range: ";
    msg += Parser::double2string(value);
    throw Exception(msg);
  }

  // comprovem que no es trobi definit
  if (!isnan(matrix[row][col]))
  {
    string msg = "TransitionMatrix::insertTransition(): redefined element [";
    msg += rating1;
    msg += "][";
    msg += rating2;
    msg += "] in <mtransitions>";
    throw Exception(msg);
  }

  // inserim en la matriu de transicio
  matrix[row][col] = value;
}

//***************************************************************************
// interpreta un node XML params
//***************************************************************************
void ccruncher::TransitionMatrix::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "mtransitions"))
  {
    string msg = "TransitionMatrix::parseDOMNode(): Invalid tag. Expected: mtransitions. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  period = XMLUtils::getDoubleAttribute(attributes, "period", DBL_MAX);
  epsilon = XMLUtils::getDoubleAttribute(attributes, "epsilon", DBL_MAX);
  if (period == DBL_MAX || epsilon == DBL_MAX)
  {
    throw Exception("TransitionMatrix::parseDOMNode(): invalid attributes at <mtransitions>");
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
      else if (XMLUtils::isNodeName(child, "transition"))
      {
        parseTransition(child);
      }
      else
      {
      	string msg = "TransitionMatrix::parseDOMNode(): invalid data structure at <mtransitions>: ";
      	msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}

//***************************************************************************
// interpreta un node XML time
//***************************************************************************
void ccruncher::TransitionMatrix::parseTransition(const DOMNode& node) throw(Exception)
{
  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();

  // agafem els atributs del rating
  string from = XMLUtils::getStringAttribute(attributes, "from", "");
  string to = XMLUtils::getStringAttribute(attributes, "to", "");
  double value = XMLUtils::getDoubleAttribute(attributes, "value", DBL_MAX);

  if (from == "" || to == "" || value == DBL_MAX)
  {
    throw Exception("TransitionMatrix::parseTransition(): invalid values at <transition>");
  }

  // insertem el valor en la matriu de transicio
  insertTransition(from, to, value);

  // acabem de comprovar la estructura
  if (node.getChildNodes()->getLength() != 0)
  {
    throw Exception("TransitionMatrix::parseTransition(): invalid data structure at <transition>");
  }
}

//***************************************************************************
// validacio del contingut de la classe
//***************************************************************************
void ccruncher::TransitionMatrix::validate() throw(Exception)
{
  // comprovem que tots els elements es troben definits
  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      if (matrix[i][j] == DBL_MAX)
      {
      	string msg = "TransitionMatrix::validate(): undefined element [";
      	msg += (i+1);
      	msg +=  "][";
      	msg += (j+1);
      	msg += "]";
        throw Exception(msg);
      }
    }
  }

  // comprovem que totes les files sumen 1
  for (int i=0;i<n;i++)
  {
    double sum = 0.0;

    for (int j=0;j<n;j++)
    {
      sum += matrix[i][j];
    }

    if (sum < (1.0-epsilon) || sum > (1.0+epsilon))
    {
      string msg = "TransitionMatrix::validate(): invalid row [";
      msg +=  (i+1);
      msg += "]. Sum = ";
      msg += Parser::double2string(sum);
      msg += " distinct 1.";
      throw Exception(msg);
    }
  }

  // determinem el rating de default
  indexdefault = -1;

  for (int i=0;i<n;i++)
  {
    if (matrix[i][i] > (1.0-epsilon) && matrix[i][i] < (1.0+epsilon))
    {
      if (indexdefault < 0)
      {
        indexdefault = i;
      }
      else
      {
        throw Exception("TransitionMatrix::validate(): found 2 or more default ratings");
      }
    }
  }

  // comprovem que rating default es troba definit
  if (indexdefault < 0)
  {
    throw Exception("TransitionMatrix::validate(): default rating not found");
  }
}

//***************************************************************************
// retorna el index del rating default
//***************************************************************************  
int ccruncher::TransitionMatrix::getIndexDefault()
{
  return indexdefault;
}

//***************************************************************************
// donat un rating inicial i un valor aleatori en [0,1] proporciona rating final
// @param x valor aleatori entre [0,1]
// @param rating rating inicial
// @return rating al final
//***************************************************************************
int ccruncher::TransitionMatrix::evalue(const int irating, const double val)
{
  double sum = 0.0;

  for (int i=0;i<n;i++)
  {
    sum += matrix[irating][i];
    
    if (val < sum)
    {
      return i;
    }
  }

  return n-1;
}

//***************************************************************************
// proporciona la matriu de transicio pel periode especificat, t
// @param t periode de la nova matriu de transicio
// @return la matriu de transicio pel periode t
//***************************************************************************
TransitionMatrix * ccruncher::translate(TransitionMatrix *otm, double t) throw(Exception)
{
  TransitionMatrix *ret = new TransitionMatrix(*otm);

  PowMatrix::pow(otm->matrix, t/otm->period, otm->n, ret->matrix);

  ret->period = t;

  return ret;
}
