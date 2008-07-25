
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
// Params.cpp - Params code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "Params.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Params::Params(const DOMNode& node) throw(Exception)
{
  // posem valors per defecte (incorrectes)
  init();

  // recollim els parametres de la simulacio
  parseDOMNode(node);

  // validem les dades
  validate();
}

//===========================================================================
// inicialitzador privat
//===========================================================================
void ccruncher::Params::init()
{
  begindate = Date(1,1,1900);
  steps = 0;
  steplength = 0;
  maxiterations = 0L;
  maxseconds = 0L;
  copula_type = "";
  copula_seed = 0L;
  antithetic = false;
  onlyactive = false;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Params::~Params()
{
  // nothing to do
}

//===========================================================================
// return Date array = begindate, begindate+steplength, bengindate+2*steplength
//===========================================================================
Date * ccruncher::Params::getDates() throw(Exception)
{
  validate();

  Date *ret = new Date[steps+1];

  for (int i=0;i<=steps;i++)
  {
    ret[i] = addMonths(begindate, i*steplength);
  }

  return ret;
}

//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Params::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "params"))
  {
    string msg = "Params::parseDOMNode(): Invalid tag. Expected: params. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
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
      else if (XMLUtils::isNodeName(child, "property"))
      {
        parseProperty(child);
      }
      else
      {
        string msg = "Params::parseDOMNode(): invalid data structure at <params>: ";
        msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}

//===========================================================================
// interpreta un node XML time
//===========================================================================
void ccruncher::Params::parseProperty(const DOMNode& node) throw(Exception)
{
  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  string name = XMLUtils::getStringAttribute(attributes, "name", "");

  if (name == "time.begindate")
  {
    Date aux = XMLUtils::getDateAttribute(attributes, "value", Date(1,1,1900));
    if (begindate != Date(1,1,1900) || aux == Date(1,1,1900)) {
      throw Exception("Params::parseProperty(): found invalid time.begintime");
    } else {
      begindate = aux;
    }
  }
  else if (name == "time.steps")
  {
    int aux = XMLUtils::getIntAttribute(attributes, "value", 0);
    if (steps != 0 || aux <= 0) {
      throw Exception("Params::parseProperty(): found invalid time.steps");
    } else {
      steps = aux;
    }
  }
  else if (name == "time.steplength")
  {
    int aux = XMLUtils::getIntAttribute(attributes, "value", 0);
    if (steplength != 0 || aux <= 0) {
      throw Exception("Params::parseProperty(): found invalid time.steplength");
    } else {
      steplength = aux;
    }
  }
  else if (name == "stopcriteria.maxiterations")
  {
    long aux = XMLUtils::getLongAttribute(attributes, "value", 0L);
    if (maxiterations != 0L || aux <= 0L) {
      throw Exception("Params::parseProperty(): found invalid stopcriteria.maxiterations");
    } else {
      maxiterations = aux;
    }
  }
  else if (name == "stopcriteria.maxseconds")
  {
    long aux = XMLUtils::getLongAttribute(attributes, "value", 0L);
    if (maxseconds != 0L || aux <= 0L) {
      throw Exception("Params::parseProperty(): found invalid stopcriteria.maxseconds");
    } else {
      maxseconds = aux;
    }
  }
  else if (name == "copula.type")
  {
    string aux = XMLUtils::getStringAttribute(attributes, "value", "");
    if (copula_type != "" || aux == "") {
      throw Exception("Params::parseProperty(): found invalid copula.type");
    } else {
      copula_type = aux;
    }
  }
  else if (name == "copula.seed")
  {
    long aux = XMLUtils::getLongAttribute(attributes, "value", 0L);
    if (copula_seed != 0L || aux == 0L) {
      throw Exception("Params::parseProperty(): found invalid copula.seed");
    } else {
      copula_seed = aux;
    }
  }
  else if (name == "montecarlo.antithetic")
  {
    bool aux = XMLUtils::getBooleanAttribute(attributes, "value", false);
    antithetic = aux;
  }
  else if (name == "portfolio.onlyActiveClients")
  {
    bool aux = XMLUtils::getBooleanAttribute(attributes, "value", false);
    onlyactive = aux;
  }
  else
  {
    throw Exception("Params::parseProperty(): found unexpected property: " + name);
  }
}

//===========================================================================
// validacio del contingut de la classe
//===========================================================================
void ccruncher::Params::validate(void) throw(Exception)
{

  if (begindate == Date(1,1,1900))
  {
    throw Exception("Params::validate(): property time.begindate not defined");
  }

  if (steps <= 0)
  {
    throw Exception("Params::validate(): property time.steps not defined");
  }

  if (steplength <= 0)
  {
    throw Exception("Params::validate(): property time.steplength not defined");
  }

  if (maxiterations <= 0L)
  {
    throw Exception("Params::validate(): property stopcriteria.maxiterations not defined");
  }

  if (maxseconds <= 0L)
  {
    throw Exception("Params::validate(): property stopcriteria.maxseconds not defined");
  }

  if (copula_type == "")
  {
    throw Exception("Params::validate(): property copula.type not defined");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Params::getXML(int ilevel) throw(Exception)
{
  string spc1 = Utils::blanks(ilevel);
  string spc2 = Utils::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<params>\n";
  ret += spc2 + "<property name='time.begindate' value='" + Parser::date2string(begindate) + "'/>\n";
  ret += spc2 + "<property name='time.steps' value='" + Parser::int2string(steps) + "'/>\n";
  ret += spc2 + "<property name='time.steplength' value='" + Parser::int2string(steplength) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxiterations' value='" + Parser::long2string(maxiterations) + "'/>\n";
  ret += spc2 + "<property name='stopcriteria.maxseconds' value='" + Parser::long2string(maxseconds) + "'/>\n";
  ret += spc2 + "<property name='copula.type' value='" + copula_type + "'/>\n";
  ret += spc2 + "<property name='copula.seed' value='" + Parser::long2string(copula_seed) + "'/>\n";
  ret += spc2 + "<property name='montecarlo.antithetic' value='" + Parser::bool2string(copula_seed) + "'/>\n";
  ret += spc2 + "<property name='portfolio.onlyActiveClients' value='" + Parser::bool2string(onlyactive) + "'/>\n";
  ret += spc1 + "</params>\n";

  return ret;
}
