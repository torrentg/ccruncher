
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

#include "params/Factors.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// private constructor
//===========================================================================
ccruncher::Factors::Factors()
{
  // nothing to do
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Factors::size() const
{
  return vfactors.size();
}

//===========================================================================
// return rating name
//===========================================================================
const string& ccruncher::Factors::getName(int i) const
{
  assert(i >= 0 && i < (int) vfactors.size());
  return vfactors[i].name;
}

//===========================================================================
// return rating description
//===========================================================================
const string& ccruncher::Factors::getDescription(int i) const
{
  assert(i >= 0 && i < (int) vfactors.size());
  return vfactors[i].desc;
}

//===========================================================================
// return the index of the factor (-1 if not found)
//===========================================================================
int ccruncher::Factors::getIndex(const char *name) const
{
  assert(name != NULL);
  for (unsigned int i=0;i<vfactors.size();i++)
  {
    if (vfactors[i].name.compare(name) == 0)
    {
      return i;
    }
  }
  return -1;
}

//===========================================================================
// return the index of the factor (-1 if not found)
//===========================================================================
int ccruncher::Factors::getIndex(const string &name) const
{
  return getIndex(name.c_str());
}

//===========================================================================
// inserts a factor in list
//===========================================================================
void ccruncher::Factors::insertFactor(const Factor &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vfactors.size();i++)
  {
    Factor aux = vfactors[i];

    if (aux.name == val.name)
    {
      throw Exception("factor name " + val.name + " repeated");
    }
    else if (aux.desc == val.desc)
    {
      throw Exception("factor description " + val.desc + " repeated");
    }
  }

  try
  {
    vfactors.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Factors::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"factors")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag factors");
    }
  }
  else if (isEqual(name_,"factor")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("invalid number of attributes at factor");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description");
      insertFactor(Factor(name,desc));
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Factors::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"factors")) {
    validations();
  }
  else if (isEqual(name_,"factor")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// validations
//===========================================================================
void ccruncher::Factors::validations() throw(Exception)
{
  // checking number of factors
  if (vfactors.empty())
  {
    throw Exception("factors have no elements");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Factors::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<factors>\n";

  for (unsigned int i=0;i<vfactors.size();i++)
  {
    ret += Strings::blanks(ilevel+2);
    ret += "<factor ";
    ret += "name='" + vfactors[i].name + "' ";
    ret += "description='" + vfactors[i].desc + "'";
    ret += "/>\n";
  }

  ret += spc + "</factors>\n";

  return ret;
}