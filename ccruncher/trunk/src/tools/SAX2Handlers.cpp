
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
// SAX2Handlers.cpp - SAX2Handlers code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cfloat>
#include <limits>
#include "SAX2Count.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/XMLUtils.hpp"
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>

//---------------------------------------------------------------------------

using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
SAX2Handlers::SAX2Handlers()
{
  tokens[TOKEN_SEGMENTAGGREGATOR] = XMLUtils::String2XMLCh("segmentaggregator");
  tokens[TOKEN_VALUES]            = XMLUtils::String2XMLCh("values");
  tokens[TOKEN_NTRANCHES]         = XMLUtils::String2XMLCh("ntranches");
  tokens[TOKEN_ITEM]              = XMLUtils::String2XMLCh("item");
  tokens[TOKEN_TRANCH]            = XMLUtils::String2XMLCh("tranch");
  tokens[TOKEN_VALUE]             = XMLUtils::String2XMLCh("value");
  tokens[TOKEN_CASHFLOW]          = XMLUtils::String2XMLCh("cashflow");
  tokens[TOKEN_EXPOSURE]          = XMLUtils::String2XMLCh("exposure");
  tokens[TOKEN_TYPE]              = XMLUtils::String2XMLCh("type");
  tokens[TOKEN_NAME]              = XMLUtils::String2XMLCh("name");

  tranch = NULL;
  resetDocument();
}

//===========================================================================
// destructor
//===========================================================================
SAX2Handlers::~SAX2Handlers()
{
  if (tranch != NULL) delete [] tranch;
}

//===========================================================================
// Getter method
//===========================================================================
vector<double>* SAX2Handlers::getTranches()
{
  return tranch;
}

//===========================================================================
// Getter method
//===========================================================================
int SAX2Handlers::getNumTranches() const
{
  return m;
}

//===========================================================================
// Getter method
//===========================================================================
bool SAX2Handlers::getSawErrors() const
{
  return fSawErrors;
}

//===========================================================================
// getAttribute
//===========================================================================
string SAX2Handlers::getAttribute(string aname, const Attributes& attrs)
{
  XMLCh *aux = XMLUtils::String2XMLCh(aname);
  int index = attrs.getIndex(aux);

  if (index < 0)
  {
    delete aux;
    throw Exception("attribute not found");
  }
  else
  {
    delete aux;
    return XMLUtils::XMLCh2String(attrs.getValue(index));
  }
}

//===========================================================================
// startElement handler
//===========================================================================
void SAX2Handlers::startElement(const XMLCh* const uri,
                                const XMLCh* const localname,
                                const XMLCh* const qname,
                                const Attributes& attrs)
{
  if (*localname == *tokens[TOKEN_VALUE])
  {
    XMLCh *aux = (XMLCh*) attrs.getValue(tokens[TOKEN_NAME]);

    if (*aux == *tokens[TOKEN_CASHFLOW])
    {
      currenttag = TAG_CASHFLOW;
    }
    else if (*aux == *tokens[TOKEN_EXPOSURE])
    {
      currenttag = TAG_EXPOSURE;
    }
    else
    {
      throw Exception("unknow name value");
    }
  }
  else if (*localname == *tokens[TOKEN_TRANCH])
  {
    currenttag = TAG_TRANCH;

    jpos = ccruncher::Parser::intValue(getAttribute("num", attrs));
    if (jpos < 0 || jpos > m)
    {
      throw Exception("invalid tranch num");
    }
  }
  else if (*localname == *tokens[TOKEN_ITEM])
  {
    currenttag = TAG_ITEM;
    if (m <= 0)
    {
      throw new Exception("ntranches not set");
    }
    else
    {
//      double *tmp = new double[m];
//      for (int i=0;i<m;i++) tmp[i] = NAN;
//      rows.push_back(tmp);
      if (ipos == UINT_MAX) ipos = 0;
      else ipos++;
    }
  }
  else if (*localname == *tokens[TOKEN_SEGMENTAGGREGATOR])
  {
    currenttag = TAG_SEGMENTAGGREGATOR;

    XMLCh *aux = (XMLCh*) attrs.getValue(tokens[TOKEN_TYPE]);
    if (*aux != *tokens[TOKEN_VALUES])
    {
      throw Exception("type aggregator is not of type values");
    }

    m = ccruncher::Parser::intValue(getAttribute("ntranches", attrs));
    if (m <= 0)
    {
      throw Exception("ntranches can't be inf or equal to 0");
    }
    else
    {
      tranch = new vector<double>[m];
    }
  }
  else
  {
    throw Exception("invalid element " + XMLUtils::XMLCh2String(localname));
  }
}

//===========================================================================
// endElement handler
//===========================================================================
void SAX2Handlers::endElement(const XMLCh* const uri,
                                const XMLCh* const localname,
                                const XMLCh* const qname)
{
/*
  if (*localname == *tokens[TOKEN_ITEM])
  {
    cout << "\n";
  }
*/
  currenttag = TAG_NONE;
}

//===========================================================================
// characters handler
//===========================================================================
void SAX2Handlers::characters(const XMLCh* const chars,
                              const unsigned int length)
{
  if (currenttag == TAG_CASHFLOW)
  {
//rows[ipos][jpos] = ccruncher::Parser::doubleValue(XMLUtils::XMLCh2String(chars));
//cout << rows[ipos][jpos] << "\t";
    tranch[jpos].push_back(ccruncher::Parser::doubleValue(XMLUtils::XMLCh2String(chars)));
  }
}

//===========================================================================
// ignorableWhitespace handler
//===========================================================================
void SAX2Handlers::ignorableWhitespace(const XMLCh* const chars,
                                       const unsigned int length)
{
  // nothing to do
}

//===========================================================================
// resetDocument handler
//===========================================================================
void SAX2Handlers::resetDocument()
{
  m = 0;
  ipos = UINT_MAX;
  jpos = 0;
  currenttag = TAG_NONE;
  if (tranch != NULL) delete [] tranch;
  fSawErrors = false;
}

//===========================================================================
// error handler
//===========================================================================
void SAX2Handlers::error(const SAXParseException& e)
{
  fSawErrors = true;
  XERCES_STD_QUALIFIER cerr
      << "\nError at file " << StrX(e.getSystemId())
      << ", line " << e.getLineNumber()
      << ", char " << e.getColumnNumber()
      << "\n  Message: " << StrX(e.getMessage())
      << XERCES_STD_QUALIFIER endl;
}

//===========================================================================
// fatalError handler
//===========================================================================
void SAX2Handlers::fatalError(const SAXParseException& e)
{
  fSawErrors = true;
  XERCES_STD_QUALIFIER cerr
      << "\nFatal Error at file " << StrX(e.getSystemId())
      << ", line " << e.getLineNumber()
      << ", char " << e.getColumnNumber()
      << "\n  Message: " << StrX(e.getMessage())
      << XERCES_STD_QUALIFIER endl;
}

//===========================================================================
// warning handler
//===========================================================================
void SAX2Handlers::warning(const SAXParseException& e)
{
  XERCES_STD_QUALIFIER cerr
      << "\nWarning at file " << StrX(e.getSystemId())
      << ", line " << e.getLineNumber()
      << ", char " << e.getColumnNumber()
      << "\n  Message: " << StrX(e.getMessage())
      << XERCES_STD_QUALIFIER endl;
}

//===========================================================================
// resetErrors handler
//===========================================================================
void SAX2Handlers::resetErrors()
{
  fSawErrors = false;
}
