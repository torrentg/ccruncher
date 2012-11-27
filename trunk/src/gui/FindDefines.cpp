#include <zlib.h>
#include "gui/FindDefines.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// constructor
//===========================================================================
FindDefines::FindDefines(const string &filename)
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

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void FindDefines::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
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

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void FindDefines::epend(ExpatUserData &, const char *)
{
  // nothing to do
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void FindDefines::epdata(ExpatUserData &, const char *, const char *, int)
{
  // nothing to do
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
const map<string,string>& FindDefines::getDefines() const
{
  return defines;
}

