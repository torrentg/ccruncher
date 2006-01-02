
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
// Segmentation.cpp - Segmentation code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
// 2005/05/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added method getNumSegments
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class refactoring
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "segmentations/Segmentation.hpp"
#include "utils/Parser.hpp"
#include "utils/Strings.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentation::Segmentation()
{
  // default values
  reset();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Segmentation::~Segmentation()
{
  // cal assegurar que es destrueix vsegments
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Segmentation::size() const
{
  return vsegments.size();
}

//===========================================================================
// [] operator
//===========================================================================
Segment& ccruncher::Segmentation::operator []  (int i)
{
  // assertions
  assert(i >= 0 && i < (int) vsegments.size());

  // return i-th segment
  return vsegments[i];
}

//===========================================================================
// [] operator. returns segment by name
//===========================================================================
Segment& ccruncher::Segmentation::operator []  (const string &name) throw(Exception)
{
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i].name == name)
    {
      return vsegments[i];
    }
  }

  throw Exception("Segmentation::[]: segment " + name + " not found");
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Segmentation::reset()
{
  vsegments.clear();
  modificable = false;
  order = -1;
  name = "";
  components = client;

  // adding catcher segment
  Segment catcher = Segment(0, "rest");
  insertSegment(catcher);
}

//===========================================================================
// insertSegment
//===========================================================================
void ccruncher::Segmentation::insertSegment(const Segment &val) throw(Exception)
{
  if (val.name == "")
  {
    throw Exception("Segmentation::insertSegment(): invalid name value");
  }

  // checking coherence
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i].name == val.name)
    {
      string msg = "Segmentation::insertSegment(): segment ";
      msg += vsegments[i].name;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  // checking for patterns
  if (val.name == "*")
  {
    if (name != "client" && name != "asset")
    {
      throw Exception("Segmentation::insertSegment(): invalid segment name '*'");
    }
    else
    {
      modificable = true;
      return;
    }
  }

  // inserting value
  try
  {
    vsegments.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"segmentation")) {
    if (getNumAttributes(attributes) != 2) {
      throw eperror(eu, "incorrect number of attributes in tag segmentation");
    }
    else {
      name = getStringAttribute(attributes, "name", "");
      string strcomp = getStringAttribute(attributes, "components", "");

      // checking name
      if (name == "") {
        throw eperror(eu, "tag <segmentation> with invalid name attribute");
      }

      // filling components variable
      if (strcomp == "asset") {
        components = asset;
      }
      else if (strcomp == "client") {
        components = client;
      }
      else {
        throw eperror(eu, "tag <segmentation> with invalid components attribute");
      }
    }
  }
  else if (isEqual(name_,"segment")) {
    string sname = getStringAttribute(attributes, "name", "");
    // checking segment name
    if (sname == "") {
      throw eperror(eu, "tag <segment> with invalid name attribute");
    }
    else {
      Segment aux(vsegments.size(), sname);
      insertSegment(aux);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"segmentation")) {
    // nothing to do
  }
  else if (isEqual(name_,"segment")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// addSegment
//===========================================================================
void ccruncher::Segmentation::addSegment(const string segname) throw(Exception)
{
  if (modificable == false)
  {
    throw Exception("Segmentation::addSegment(): fixed segments");
  }
  else
  {
    Segment aux = Segment(vsegments.size(), segname);
    insertSegment(aux);
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Segmentation::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<segmentation name='" + name + "' components='";
  ret += (components==asset?"asset":"client");
  ret += "'>\n";

  if (name == "portfolio")
  {
    // nothing to do
  }
  else if (name == "client" || name == "asset")
  {
    ret += Strings::blanks(ilevel+2) + "<segment name='*'/>\n";
  }
  else
  {
    for (unsigned int i=0;i<vsegments.size();i++)
    {
      if (vsegments[i].name != "rest")
      {
        ret += Strings::blanks(ilevel+2) + "<segment name='" + vsegments[i].name + "'/>\n";
      }
    }
  }

  ret += spc + "</segmentation>\n";

  return ret;
}
