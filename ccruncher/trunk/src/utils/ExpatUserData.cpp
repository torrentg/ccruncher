
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// ExpatUserData.cpp - ExpatUserData code
// --------------------------------------------------------------------------
//
// 2005/03/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cassert>
#include "ExpatUserData.hpp"

//===========================================================================
// void constructor (don't use)
//===========================================================================
ccruncher::ExpatUserData::ExpatUserData()
{
  // nothing to do
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::ExpatUserData::ExpatUserData(XML_Parser xmlparser_)
{
  xmlparser = xmlparser_;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::ExpatUserData::~ExpatUserData()
{
  // nothing to do
}

//===========================================================================
// getParser
//===========================================================================
XML_Parser ccruncher::ExpatUserData::getParser()
{
  return xmlparser;
}

//===========================================================================
// getCurrentHandlers
//===========================================================================
ExpatHandlers* ccruncher::ExpatUserData::getCurrentHandlers()
{
  ExpatUserData::ExpatUserDataToken &aux = pila.top();
  return aux.handlers;
}

//===========================================================================
// getCurrentName
//===========================================================================
string& ccruncher::ExpatUserData::getCurrentName()
{
  ExpatUserData::ExpatUserDataToken &aux = pila.top();
  return aux.name;
}

//===========================================================================
// removeCurrentHandlers
//===========================================================================
void ccruncher::ExpatUserData::removeCurrentHandlers()
{
  pila.pop();
}

//===========================================================================
// setCurrentHandlers
//===========================================================================
void ccruncher::ExpatUserData::setCurrentHandlers(string name, ExpatHandlers *eh)
{
  ExpatUserDataToken eudt(name, eh);
  pila.push(eudt);
}
