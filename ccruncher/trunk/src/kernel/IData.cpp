
//***************************************************************************
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
// IData.cpp - IData code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <fstream>
#include <iostream>
#include "IData.hpp"
#include "utils/XMLUtils.hpp"

//***************************************************************************
// inicialitzador
//***************************************************************************
void ccruncher::IData::init()
{
   params = NULL;
   interests = NULL;
   ratings = NULL;
   transitions = NULL;
   sectors = NULL;
   correlations = NULL;
   segmentations = NULL;
   aggregators = NULL;
   portfolio = NULL;	
}

//***************************************************************************
// release
//***************************************************************************
void ccruncher::IData::release()
{
  // alliberem l'espai alocat
  if (params != NULL) delete params;
  if (interests != NULL) delete interests;
  if (ratings != NULL) delete ratings;
  if (transitions != NULL) delete transitions;
  if (sectors != NULL) delete sectors;
  if (correlations != NULL) delete correlations;
  if (segmentations != NULL) delete segmentations;
  if (aggregators != NULL) delete aggregators;
  if (portfolio != NULL) delete portfolio;

  // establim els punters a NULL
  init();
}

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::IData::IData(const string &xmlfilename) throw(Exception)
{
  DOMBuilder *parser = NULL;
  DOMDocument *doc = NULL;

  // inicialitzem el contingut
  init();

  // parsejem el document
  try
  {
    // checking file readability
    ifstream ifile((const char *) xmlfilename.c_str());

    if (!ifile.is_open()) 
    {
      throw Exception("IData::IData(): can't open file " + xmlfilename);
    }
    else 
    {
      ifile.close();
    }

    // parsing file
    XMLUtils::initialize();
    parser = XMLUtils::getParser();
    doc = XMLUtils::getDocument(parser, xmlfilename);
    parseDOMNode(*(doc->getDocumentElement()));
    delete parser;
    XMLUtils::terminate();
  }
  catch(Exception &e)
  {
    if (parser != NULL) delete parser;
    XMLUtils::terminate();
    release();
    throw e;
  }
  catch(std::exception &e)
  {
    string msg = "IData::IData(): ";
    msg += e.what();
    if (parser != NULL) delete parser;
    XMLUtils::terminate();    
    release();
    throw Exception(e, msg);
  } 
}

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::IData::IData(const DOMNode &node) throw(Exception)
{
  // inicialitzem el contingut
  init();

  try
  {
    // parsejem el document  
    parseDOMNode(node); 
  }
  catch(Exception &e)
  {
    release();
    throw e;
  }
  catch(std::exception &e)
  {
    release();
    string msg = "IData::IData(): generic error";
    msg += e.what();
    throw Exception(e, msg);
  } 
}

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::IData::~IData()
{
  release();
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "creditcruncher"))
  {
    string msg = "IData::parseDOMNode(): Invalid tag. Expected: creditcruncher. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // recorrem tots els items
  DOMNodeList &children = *node.getChildNodes();

  if (&children != NULL)
  {
    for(unsigned int i=0;i<children.getLength();i++)
    {
      DOMNode &child = *children.item(i);

      if (XMLUtils::isVoidTextNode(child) || XMLUtils::isCommentNode(child))
      {
        continue;
      }
      else if (XMLUtils::isNodeName(child, "params"))
      {
        parseParams(child);
      }
      else if (XMLUtils::isNodeName(child, "interests"))
      {
        parseInterests(child);
      }
      else if (XMLUtils::isNodeName(child, "ratings"))
      {
        parseRatings(child);
      }
      else if (XMLUtils::isNodeName(child, "mtransitions"))
      {
        parseTransitions(child);
      }
      else if (XMLUtils::isNodeName(child, "sectors"))
      {
        parseSectors(child);
      }
      else if (XMLUtils::isNodeName(child, "mcorrels"))
      {
        parseCorrelations(child);
      }
      else if (XMLUtils::isNodeName(child, "segmentations"))
      {
        parseSegmentations(child);
      }
      else if (XMLUtils::isNodeName(child, "aggregators"))
      {
        parseAggregators(child);
      }
      else if (XMLUtils::isNodeName(child, "portfolio"))
      {
        parsePortfolio(child);
      }
      else
      {
      	string msg = "IData::parseXML(): invalid data structure at <creditcruncher>: ";
      	msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseParams(const DOMNode &node) throw(Exception)
{
  if (params != NULL)
  {
    throw Exception("IData::parseParams(): tag params repeated");
  }
  else
  {
    params = new Params(node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseInterests(const DOMNode &node) throw(Exception)
{
  if (interests != NULL)
  {
    throw Exception("IData::parseInterests(): tag interests repeated");
  }
  else
  {
    interests = new Interests(node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseRatings(const DOMNode &node) throw(Exception)
{
  if (ratings != NULL)
  {
    throw Exception("IData::parseRatings(): tag ratings repeated");
  }
  else
  {
    ratings = new Ratings(node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseTransitions(const DOMNode &node) throw(Exception)
{
  if (ratings == NULL)
  {
    throw Exception("IDATA::parseTransitions(): tag <mtransition> defined before <ratings> tag");
  }
  else if (transitions != NULL)
  {
    throw Exception("IData::parseTransitions(): tag transitions repeated");
  }
  else
  {
    transitions = new TransitionMatrix(ratings, node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseSectors(const DOMNode &node) throw(Exception)
{
  if (sectors != NULL)
  {
    throw Exception("IData::parseSectors(): tag sectors repeated");
  }
  else
  {
    sectors = new Sectors(node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseCorrelations(const DOMNode &node) throw(Exception)
{
  if (sectors == NULL)
  {
    throw Exception("IData::parseCorrelations(): tag <mcorrels> defined before <sectors> tag");
  }
  else if (correlations != NULL)
  {
    throw Exception("IData::parseCorrelations(): tag correlations repeated");
  }
  else
  {
    correlations = new CorrelationMatrix(sectors, node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseSegmentations(const DOMNode &node) throw(Exception)
{
  if (segmentations != NULL)
  {
    throw Exception("IData::parseSegmentations(): tag segmentations repeated");
  }
  else
  {
    segmentations = new Segmentations(node);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parseAggregators(const DOMNode &node) throw(Exception)
{
  if (segmentations == NULL)
  {
    throw Exception("IData::parseAggregators(): tag <aggregators> defined before <segmentations> tag");
  }
  else if (aggregators != NULL)
  {
    throw Exception("IData::parseAggregators(): tag aggregators repeated");
  }
  else
  {
    aggregators = new Aggregators(node, segmentations);
  }
}

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::IData::parsePortfolio(const DOMNode &node) throw(Exception)
{
  if (interests == NULL)
  {
    throw Exception("IData::parsePortfolio(): tag <portfolio> defined before <interests> tag");
  }
  else if (ratings == NULL)
  {
    throw Exception("IData::parsePortfolio(): tag <portfolio> defined before <ratings> tag");
  }
  else if (sectors == NULL)
  {
    throw Exception("IData::parsePortfolio(): tag <portfolio> defined before <sectors> tag");
  }
  else if (segmentations == NULL)
  {
    throw Exception("IData::parsePortfolio(): tag <portfolio> defined before <segmentations> tag");
  }
  if (portfolio != NULL)
  {
    throw Exception("IData::parsePortfolio(): tag portfolio repeated");
  }
  else
  {
    portfolio = new Portfolio(ratings, sectors, segmentations, interests, node);
  }
}
