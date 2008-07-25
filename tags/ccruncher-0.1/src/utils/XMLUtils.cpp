
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
// XMLUtils.cpp - XMLUtils code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (see jama/tnt_stopwatch && boost/timer)
//
//===========================================================================

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include "XMLUtils.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include "Date.hpp"

//===========================================================================
// initialize XML stuff
//===========================================================================
void ccruncher::XMLUtils::initialize() throw(Exception)
{
  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch(const XMLException &e)
  {
    throw XMLUtils::XMLException2Exception(e);
  }
}

//===========================================================================
// terminate XML stuff
//===========================================================================
void ccruncher::XMLUtils::terminate() throw()
{
  try
  {
    XMLPlatformUtils::Terminate();
  }
  catch(...)
  {
    // nothing to do
  }
}

//===========================================================================
// determina si es tracta d'un node texte buit
//===========================================================================
bool ccruncher::XMLUtils::isVoidTextNode(const DOMNode &node)
{
  bool ret = false;

  // validem el node passat com argument
  if (&node == NULL || node.getNodeType() != DOMNode::TEXT_NODE)
  {
    return false;
  }

  // recollim el valor del texte
  XMLCh *val = XMLString::replicate(node.getNodeValue());

  // eliminem blancs i salts de linia
  XMLString::trim(val);

  // comprovem la llargada
  if (XMLString::stringLen(val) == 0)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  XMLString::release(&val);
  return ret;
}

//===========================================================================
// determina si es tracta d'un node de comentari
//===========================================================================
bool ccruncher::XMLUtils::isCommentNode(const DOMNode &node)
{
  // validem el node passat com argument
  if (&node == NULL || node.getNodeType() != DOMNode::COMMENT_NODE)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================
// determina si el nom del node es el indicat
//===========================================================================
bool ccruncher::XMLUtils::isNodeName(const DOMNode &node, const string &name)
{
  bool ret = false;

  // validem el node passat com argument
  if (&node == NULL)
  {
    return false;
  }

  // recollim el nom del node
  XMLCh *nom1 = XMLString::replicate(node.getNodeName());

  // eliminem blancs i salts de linia
  XMLString::trim(nom1);

  XMLCh *nom2 = XMLString::transcode(name.c_str());

  // comprovem si son iguals
  if (XMLString::equals(nom1, nom2))
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  XMLString::release(&nom1);
  XMLString::release(&nom2);
  return ret;
}

//===========================================================================
// donat un XMLCh* retorna un string
//===========================================================================
string ccruncher::XMLUtils::XMLCh2String(const XMLCh *str)
{
  char *aux = XMLString::transcode(str);

  string ret = string(aux);

  XMLString::release(&aux);

  return ret;
}

//===========================================================================
// donat un string retorna un XMLCh*
// la funcio que invoca String2XMLCh es responsable de realitzar el
// XMLString::release(&ret) per alliberar el valor retornat
//===========================================================================
XMLCh * ccruncher::XMLUtils::String2XMLCh(const string &val)
{
  return XMLString::transcode(val.c_str());
}

//===========================================================================
// donat retorna el valor del atribut com un string. default en cas d'error
//===========================================================================
string ccruncher::XMLUtils::getStringAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const string &defvalue)
{
  string ret;
  XMLCh *aux = XMLUtils::String2XMLCh(attrname);
  try
  {
    DOMNode *attr = attrs.getNamedItem(aux);

    if (attr == NULL)
    {
      throw Exception("XMLUtils::getStringAttribute(): attribute not exist");
    }
    else
    {
      const XMLCh *tmp = attr->getNodeValue();
      ret = XMLUtils::XMLCh2String(tmp);
    }
  }
  catch(...)
  {
    XMLString::release(&aux);
    ret = *(new string(defvalue));
  }

  XMLString::release(&aux);
  return ret;
}

//===========================================================================
// donat retorna el valor del atribut com un int. default en cas d'error
//===========================================================================
int ccruncher::XMLUtils::getIntAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const int defvalue)
{
  string val = XMLUtils::getStringAttribute(attrs, attrname, "");

  if (val == "")
  {
    return defvalue;
  }
  else
  {
    try
    {
      return Parser::intValue(val);
    }
    catch(...)
    {
      return defvalue;
    }
  }
}

//===========================================================================
// donat retorna el valor del atribut com un long. default en cas d'error
//===========================================================================
long ccruncher::XMLUtils::getLongAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const long defvalue)
{
  string val = XMLUtils::getStringAttribute(attrs, attrname, "");

  if (val == "")
  {
    return defvalue;
  }
  else
  {
    try
    {
      return Parser::longValue(val);
    }
    catch(...)
    {
      return defvalue;
    }
  }
}

//===========================================================================
// donat retorna el valor del atribut com un long. default en cas d'error
//===========================================================================
double ccruncher::XMLUtils::getDoubleAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const double defvalue)
{
  string val = XMLUtils::getStringAttribute(attrs, attrname, "");

  if (val == "")
  {
    return defvalue;
  }
  else
  {
    try
    {
      return Parser::doubleValue(val);
    }
    catch(...)
    {
      return defvalue;
    }
  }
}

//===========================================================================
// donat retorna el valor del atribut com un long. default en cas d'error
//===========================================================================
Date ccruncher::XMLUtils::getDateAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const Date &defvalue)
{
  string val = XMLUtils::getStringAttribute(attrs, attrname, "");

  if (val == "")
  {
    return defvalue;
  }
  else
  {
    try
    {
      return Parser::dateValue(val);
    }
    catch(...)
    {
      return *(new Date(defvalue));
    }
  }
}

//===========================================================================
// donat retorna el valor del atribut com un long. default en cas d'error
//===========================================================================
bool ccruncher::XMLUtils::getBooleanAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const bool &defvalue)
{
  string val = XMLUtils::getStringAttribute(attrs, attrname, "");

  if (val == "")
  {
    return defvalue;
  }
  else if (Utils::lowercase(val) == "true")
  {
    return true;
  }
  else if (Utils::lowercase(val) == "false")
  {
    return false;
  }
  else
  {
    return defvalue;
  }
}

//===========================================================================
// retorna un parser per poder parsejar un doc DOM
//===========================================================================
DOMBuilder * ccruncher::XMLUtils::getParser() throw(Exception)
{
  // inicialitzem el parser
  XMLCh tempStr[100];
  XMLString::transcode("LS", tempStr, 99);
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMBuilder *parser = impl->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

  // sortim
  return parser;
}

//===========================================================================
// donat un filename retorna un DOMDocument
//===========================================================================
DOMDocument * ccruncher::XMLUtils::getDocument(DOMBuilder *parser, const string &filename) throw(Exception)
{
  try
  {
    return parser->parseURI(filename.c_str());
  }
  catch(const XMLException &e)
  {
    throw XMLUtils::XMLException2Exception(e);
  }
  catch(const DOMException &e)
  {
    throw XMLUtils::DOMException2Exception(e);
  }
  catch(std::exception &e)
  {
    throw Exception(e, "XMLUtils::getDocument(): unexpected exception");
  }
}

//===========================================================================
// donat un string retorna un InputSource
//===========================================================================
Wrapper4InputSource * ccruncher::XMLUtils::getInputSource(const string &xmlcontent)
throw(Exception)
{
  MemBufInputSource* memBufIS=NULL;
  Wrapper4InputSource *wis = NULL;
  static const char* gMemBufId = "prodInfo";

  try
  {
    memBufIS = new MemBufInputSource((const XMLByte*)xmlcontent.c_str(),
         xmlcontent.length(), gMemBufId, false);
    wis = new Wrapper4InputSource(memBufIS);
//    delete memBufIS;
    return wis;
  }
  catch(const XMLException &e)
  {
    if (memBufIS != NULL) delete memBufIS;
    throw XMLUtils::XMLException2Exception(e);
  }
  catch(const DOMException &e)
  {
    if (memBufIS != NULL) delete memBufIS;
    throw XMLUtils::DOMException2Exception(e);
  }
  catch(std::exception &e)
  {
    if (memBufIS != NULL) delete memBufIS;
    throw Exception(e, "XMLUtils::String2Document(): unexpected exception");
  }
}

//===========================================================================
// donat un string retorna un DOMDocument
//===========================================================================
DOMDocument * ccruncher::XMLUtils::getDocument(DOMBuilder *parser, Wrapper4InputSource *wis) throw(Exception)
{
  try
  {
    return parser->parse(*wis);
  }
  catch(const XMLException &e)
  {
    throw XMLUtils::XMLException2Exception(e);
  }
  catch(const DOMException &e)
  {
    throw XMLUtils::DOMException2Exception(e);
  }
  catch(std::exception &e)
  {
    throw Exception(e, "XMLUtils::String2Document(): unexpected exception");
  }
}

//===========================================================================
// XMLException -> Exception
//===========================================================================
Exception ccruncher::XMLUtils::XMLException2Exception(const XMLException &e)
{
   string msg = XMLCh2String(e.getMessage());
   msg += " at file ";
   msg += e.getSrcFile();
   msg += " at line ";
   msg += e.getSrcLine();

   return Exception(msg);
}

//===========================================================================
// DOMException -> Exception
//===========================================================================
Exception ccruncher::XMLUtils::DOMException2Exception(const DOMException &e)
{
   return Exception(XMLCh2String(e.msg));
}
