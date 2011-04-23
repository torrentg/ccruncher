
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#include "utils/ExpatUserData.hpp"
#include <cstring>
#include <cassert>

//===========================================================================
// void constructor (don't use)
//===========================================================================
ccruncher::ExpatUserData::ExpatUserData() : pila(10), pos(-1)
{
  // nothing to do
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::ExpatUserData::ExpatUserData(XML_Parser xmlparser_) : pila(10), pos(-1)
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
  return pila[pos].handlers;
}

//===========================================================================
// getCurrentName
//===========================================================================
const char* ccruncher::ExpatUserData::getCurrentName()
{
  return pila[pos].name;
}

//===========================================================================
// removeCurrentHandlers
//===========================================================================
void ccruncher::ExpatUserData::removeCurrentHandlers()
{
  pos--;
}

//===========================================================================
// setCurrentHandlers
//===========================================================================
void ccruncher::ExpatUserData::setCurrentHandlers(const char *name, ExpatHandlers *eh)
{
  pos++;
  assert(pos<10);
  pila[pos].handlers = eh;
  assert(strlen(name)<20);
  strcpy(pila[pos].name, name);
}

