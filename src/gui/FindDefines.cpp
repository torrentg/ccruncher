
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

/**************************************************************************//**
 * @param[in] filename Input file.
 */
ccruncher_gui::FindDefines::FindDefines(const std::string &filename)
{
  gzFile file = nullptr;

  try {
    file = gzopen(filename.c_str(), "rb");
    if (file == nullptr) return;
    ExpatParser parser;
    parser.parse(file, this);
    gzclose(file);
  }
  catch(...) {
    if (file != nullptr) gzclose(file);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher_gui::FindDefines::epstart(const char *tag, const char **attributes)
{
  if (isEqual(tag,"ccruncher")) {
    // nothing to do
  }
  else if (isEqual(tag,"title")) {
    // nothing to do
  }
  else if (isEqual(tag,"description")) {
    // nothing to do
  }
  else if (isEqual(tag,"defines")) {
    // nothing to do
  }
  else if (isEqual(tag,"define")) {
    string key = getStringAttribute(attributes, "name", "");
    string value = getStringAttribute(attributes, "value", "");
    defines[key] = value;
    if (macros.values.find(key) == macros.values.end()) {
      macros.values[key] = value;
    }
  }
  else {
    // stopping parser
    epstop();
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 */
void ccruncher_gui::FindDefines::epend(const char *)
{
  // nothing to do
}

/**************************************************************************//**
 * @see ExpatHandlers::epdata
 */
void ccruncher_gui::FindDefines::epdata(const char *, int)
{
  // nothing to do
}

/**************************************************************************//**
 * @return List of defines.
 */
const map<string,string>& ccruncher_gui::FindDefines::getDefines() const
{
  return defines;
}

