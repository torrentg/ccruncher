
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

#include "segmentations/Segmentation.hpp"
#include "utils/Strings.hpp"
#include <cassert>

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
// returns i-th segment
//===========================================================================
const string& ccruncher::Segmentation::getSegment(int i)
{
  // assertions
  assert(i >= 0 && i < (int) vsegments.size());

  // return i-th segment
  return vsegments[i];
}

//===========================================================================
// return the index of the given segment
//===========================================================================
int ccruncher::Segmentation::indexOfSegment(const string &sname) throw(Exception)
{
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i] == sname)
    {
      return (int)i;
    }
  }

  throw Exception("segment " + sname + " not found");
}

//===========================================================================
// return the index of the given segment
//===========================================================================
int ccruncher::Segmentation::indexOfSegment(const char *sname) throw(Exception)
{
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i].compare(sname) == 0)
    {
      return (int)i;
    }
  }

  throw Exception("segment " + string(sname) + " not found");
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Segmentation::reset()
{
  vsegments.clear();
  modificable = false;
  insertSegment("unassigned"); // adding catcher segment
  name = "";
  components = obligor;
}

//===========================================================================
// insertSegment
//===========================================================================
int ccruncher::Segmentation::insertSegment(const string &sname) throw(Exception)
{
  if (sname == "")
  {
    throw Exception("trying to insert a segment with invalid name (void name)");
  }

  // checking coherence
  // performance trick (asset_id or obligor_id is always unique)
  if (modificable == false) 
  {
    for (unsigned int i=0;i<vsegments.size();i++)
    {
      if (vsegments[i] == sname)
      {
        throw Exception("segment " + vsegments[i] + " repeated");
      }
    }
  }

  // checking for patterns
  if (sname == "*")
  {
    if (name != "obligors" && name != "assets")
    {
      throw Exception("invalid segment name '*'");
    }
    else
    {
      modificable = true;
      return -1;
    }
  }

  // inserting value
  try
  {
    vsegments.push_back(sname);
    return vsegments.size()-1;
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"segmentation")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes in tag segmentation");
    }
    else {
      name = getStringAttribute(attributes, "name", "");
      string strcomp = getStringAttribute(attributes, "components", "");

      // checking name
      if (name == "") {
        throw Exception("tag <segmentation> with invalid name attribute");
      }

      // filling components variable
      if (strcomp == "asset") {
        components = asset;
      }
      else if (strcomp == "obligor") {
        components = obligor;
      }
      else {
        throw Exception("tag <segmentation> with invalid components attribute");
      }
    }
  }
  else if (isEqual(name_,"segment")) {
    string sname = getStringAttribute(attributes, "name", "");
    // checking segment name
    if (sname == "") {
      throw Exception("tag <segment> with invalid name attribute");
    }
    else {
      insertSegment(sname);
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"segmentation")) {
    // nothing to do
  }
  else if (isEqual(name_,"segment")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// addSegment
//===========================================================================
int ccruncher::Segmentation::addSegment(const string &sname) throw(Exception)
{
  if (modificable == false)
  {
    throw Exception("implicit segments defined. can't define other segments");
  }
  else
  {
    return insertSegment(sname);
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
  ret += (components==asset?"asset":"obligor");
  ret += "'>\n";

  if (name == "portfolio")
  {
    // nothing to do
  }
  else if (name == "obligors" || name == "assets")
  {
    ret += Strings::blanks(ilevel+2) + "<segment name='*'/>\n";
  }
  else
  {
    for (unsigned int i=0;i<vsegments.size();i++)
    {
      if (vsegments[i] != "unassigned")
      {
        ret += Strings::blanks(ilevel+2) + "<segment name='" + vsegments[i] + "'/>\n";
      }
    }
  }

  ret += spc + "</segmentation>\n";

  return ret;
}
