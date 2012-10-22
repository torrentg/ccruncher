#include <gzstream.h>
#include "gui/FindDefines.hpp"
#include "utils/ExpatParser.hpp"


//===========================================================================
// constructor
//===========================================================================
FindDefines::FindDefines(const string &filename)
{
  igzstream xmlstream((const char *) filename.c_str());
  if (xmlstream.peek() == EOF) return;
  parse(xmlstream);
}

//===========================================================================
// parse
//===========================================================================
void FindDefines::parse(istream &is)
{
  try
  {
    ExpatParser parser;
    parser.parse(is, this);
  }
  catch(std::exception &e)
  {
    // nothing to do
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
void FindDefines::epend(ExpatUserData &, const char *name_)
{
  // nothing to do
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void FindDefines::epdata(ExpatUserData &eu, const char *name_, const char *data, int len)
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

