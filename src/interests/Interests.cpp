
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
// Interests.cpp - Interests code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Interests.hpp"
#include "utils/Utils.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Interests::Interests(const DOMNode& node) throw(Exception)
{
  ispot = -1;

  // recollim els parametres de la simulacio
  parseDOMNode(node);

  // making validations
  validate();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interests::~Interests()
{
  // nothing to do
}

//===========================================================================
// return interests list
//===========================================================================
vector<Interest> * ccruncher::Interests::getInterests()
{
  return &vinterests;
}

//===========================================================================
// return interest by name
//===========================================================================
Interest * ccruncher::Interests::getInterest(string name) throw(Exception)
{
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == name)
    {
      return &(vinterests[i]);
    }
  }

  throw Exception("Interests::getInterest(): interest " + name + " not found");
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Interests::validate() throw(Exception)
{
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == "spot")
    {
      ispot = i;
      return;
    }
  }

  throw Exception("Interests::validate(): interest spot not found");
}

//===========================================================================
// insert a new interest in list
//===========================================================================
void ccruncher::Interests::insertInterest(Interest &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == val.getName())
    {
      string msg = "Interests::insertInterest(): interest name ";
      msg += val.getName();
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vinterests.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}

//===========================================================================
// interpreta un node XML
//===========================================================================
void ccruncher::Interests::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "interests"))
  {
    string msg = "Interests::parseDOMNode(): Invalid tag. Expected: interests. Found: ";
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
      else if (XMLUtils::isNodeName(child, "interest"))
      {
        Interest aux = Interest(child);
        insertInterest(aux);
      }
      else
      {
        throw Exception("Interests::parseDOMNode(): invalid data structure at <interests>");
      }
    }
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Interests::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<interests>\n";

  for (unsigned int i=0;i<vinterests.size();i++)
  {
    ret += vinterests[i].getXML(ilevel+2);
  }

  ret += spc + "</interests>\n";

  return ret;
}
