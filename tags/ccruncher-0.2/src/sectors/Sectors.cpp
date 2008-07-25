
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
// Sectors.cpp - Sectors code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "sectors/Sectors.hpp"
#include "utils/Utils.hpp"

//===========================================================================
// constructor privat
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
  // cal assegurar que es destrueix vsectors;
}

//===========================================================================
// destructor
//===========================================================================
vector<Sector> * ccruncher::Sectors::getSectors()
{
  return &vsectors;
}

//===========================================================================
// insercio nou sector en la llista
//===========================================================================
void ccruncher::Sectors::insertSector(Sector &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vsectors.size();i++)
  {
    Sector aux = vsectors[i];

    if (aux.name == val.name)
    {
      string msg = "Sectors::insertSector(): sector name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.order == val.order)
    {
      string msg = "Sectors::insertSector(): sector order ";
      msg += val.order;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.desc == val.desc)
    {
      string msg = "Sectors::insertSector(): sector desc ";
      msg += val.desc;
      msg += " repeated";
      throw Exception(msg);
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
      throw eperror(eu, "attributes are not allowed in tag sectors");
    }
  }
  else if (isEqual(name_,"sector")) {
    auxsector.reset();
    eppush(eu, &auxsector, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Sectors::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"sectors")) {
    validations();
    auxsector.reset();
  }
  else if (isEqual(name_,"sector")) {
    insertSector(auxsector);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// validacions de la llista de sectors recollida
//===========================================================================
void ccruncher::Sectors::validations() throw(Exception)
{
  // validacio longitud
  if (vsectors.size() == 0)
  {
    throw Exception("Sectors::validations(): sectors have no elements");
  }

  // ordenem la llista de sectors per camp order
  sort(vsectors.begin(), vsectors.end());

  // comprovem que el camp order comença per 1 i no hi ha buits
  for(unsigned int i=0;i<vsectors.size();i++)
  {
    Sector aux = vsectors[i];

    if (aux.order != (int)(i+1))
    {
      string msg = "Sectors::validations(): incorrect order sector at or near order = ";
      msg += aux.order;
      throw Exception(msg);
    }
  }
}

//===========================================================================
// retorna el index del rating dins la llista (-1 si no es troba)
//===========================================================================
int ccruncher::Sectors::getIndex(const string &sector_name)
{

  for (unsigned int i=0;i<vsectors.size();i++)
  {
    if (vsectors[i].name == sector_name)
    {
      return (int) i;
    }
  }

  return -1;
}

//===========================================================================
// retorna el nom del rating dins la llista (-1 si no es troba)
//===========================================================================
string ccruncher::Sectors::getName(int index) throw(Exception)
{
  if (index < 0 || index >= (int) vsectors.size())
  {
    throw Exception("Sectors::getName(): index out of range");
  }
  else
  {
    return vsectors[index].name;
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Sectors::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<sectors>\n";

  for (unsigned int i=0;i<vsectors.size();i++)
  {
    ret += vsectors[i].getXML(ilevel+2);
  }

  ret += spc + "</sectors>\n";

  return ret;
}
