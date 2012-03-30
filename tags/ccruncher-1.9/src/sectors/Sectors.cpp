
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

#include "sectors/Sectors.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// private constructor
//===========================================================================
ccruncher::Sectors::Sectors()
{
  // nothing to do
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Sectors::~Sectors()
{
  // nothing to do
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Sectors::size() const
{
  return vsectors.size();
}

//===========================================================================
// [] operator
//===========================================================================
Sector& ccruncher::Sectors::operator []  (int i)
{
  // assertions
  assert(i >= 0 && i < (int) vsectors.size());

  // return i-th sector
  return vsectors[i];
}

//===========================================================================
// [] operator. returns sector by name
//===========================================================================
Sector& ccruncher::Sectors::operator []  (const string &name) throw(Exception)
{
  for (unsigned int i=0;i<vsectors.size();i++)
  {
    if (vsectors[i].name == name)
    {
      return vsectors[i];
    }
  }

  throw Exception("sector " + name + " not found");
}

//===========================================================================
// return the index of the sector (-1 if rating not found)
//===========================================================================
int ccruncher::Sectors::getIndex(const char *name) const
{
  assert(name != NULL);
  for (unsigned int i=0;i<vsectors.size();i++)
  {
    if (vsectors[i].name.compare(name) == 0)
    {
      return i;
    }
  }
  return -1;
}

//===========================================================================
// return the index of the sector (-1 if rating not found)
//===========================================================================
int ccruncher::Sectors::getIndex(const string &name) const
{
  return getIndex(name.c_str());
}

//===========================================================================
// inserts a sector in list
//===========================================================================
void ccruncher::Sectors::insertSector(const Sector &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vsectors.size();i++)
  {
    Sector aux = vsectors[i];

    if (aux.name == val.name)
    {
      throw Exception("sector name " + val.name + " repeated");
    }
    else if (aux.desc == val.desc)
    {
      throw Exception("sector description " + val.desc + " repeated");
    }
  }

  try
  {
    vsectors.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Sectors::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"sectors")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag sectors");
    }
  }
  else if (isEqual(name_,"sector")) {
    auxsector = Sector();
    eppush(eu, &auxsector, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Sectors::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"sectors")) {
    validations();
    auxsector = Sector();
  }
  else if (isEqual(name_,"sector")) {
    insertSector(auxsector);
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// validacions de la llista de sectors recollida
//===========================================================================
void ccruncher::Sectors::validations() throw(Exception)
{
  // checking number of sectors
  if (vsectors.size() == 0)
  {
    throw Exception("sectors have no elements");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Sectors::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<sectors>\n";

  for (unsigned int i=0;i<vsectors.size();i++)
  {
    ret += vsectors[i].getXML(ilevel+2);
  }

  ret += spc + "</sectors>\n";

  return ret;
}
