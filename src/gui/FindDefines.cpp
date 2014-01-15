
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <zlib.h>
#include "gui/FindDefines.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
ccruncher_gui::FindDefines::FindDefines(const string &filename)
{
  gzFile file = NULL;

  try {
    file = gzopen(filename.c_str(), "rb");
    if (file == NULL) return;
    ExpatParser parser;
    parser.parse(file, this);
    gzclose(file);
  }
  catch(...) {
    if (file != NULL) gzclose(file);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] name Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher_gui::FindDefines::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"ccruncher")) {
    // nothing to do
  }
  else if (isEqual(name_,"title")) {
    // nothing to do
  }
  else if (isEqual(name_,"description")) {
    // nothing to do
  }
  else if (isEqual(name_,"defines")) {
    // nothing to do
  }
  else if (isEqual(name_,"define")) {
    string key = getStringAttribute(attributes, "name", "");
    string value = getStringAttribute(attributes, "value", "");
    defines[key] = value;
    if (eu.defines.find(key) == eu.defines.end()) {
      eu.defines[key] = value;
    }
  }
  else {
    // stopping parser
    epstop(eu);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name Element name.
 */
void ccruncher_gui::FindDefines::epend(ExpatUserData &, const char *)
{
  // nothing to do
}

/**************************************************************************//**
 * @see ExpatHandlers::epdata
 */
void ccruncher_gui::FindDefines::epdata(ExpatUserData &, const char *, const char *, int)
{
  // nothing to do
}

//===========================================================================
// getDefines
//===========================================================================
const map<string,string>& ccruncher_gui::FindDefines::getDefines() const
{
  return defines;
}

