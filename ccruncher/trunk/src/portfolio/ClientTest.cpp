
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
// ClientTest.cpp - ClientTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
//===========================================================================

#include <iostream>
#include "Client.hpp"
#include "ClientTest.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// setUp
//===========================================================================
void ClientTest::setUp()
{
  XMLUtils::initialize();
}

//===========================================================================
// setUp
//===========================================================================
void ClientTest::tearDown()
{
  XMLUtils::terminate();
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ClientTest::getRatings()
{
  // note that ratings are unordered
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='muy bueno'/>\n\
      <rating name='B' order='2' desc='bueno'/>\n\
      <rating name='D' order='4' desc='malo'/>\n\
      <rating name='C' order='3' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // ratings list creation
  Ratings ratings;
  ASSERT_NO_THROW(ratings = Ratings(*(doc->getDocumentElement())));

  delete parser;
  return ratings;
}

//===========================================================================
// getSectors
//===========================================================================
Sectors ClientTest::getSectors()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='2' desc='otros sectores'/>\n\
    </sectors>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation
  Sectors sectors;
  ASSERT_NO_THROW(sectors = Sectors(*(doc->getDocumentElement())));

  delete parser;
  return sectors;
}

//===========================================================================
// getSegmentations1
//===========================================================================
Segmentations ClientTest::getSegmentations()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset'/>\n\
    <segmentation name='client' components='client'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='asset' components='asset'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='sector' components='client'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='client'>\n\
      <segment name='big'/>\n\
      <segment name='medium'/>\n\
    </segmentation>\n\
    <segmentation name='product' components='asset'>\n\
      <segment name='bond'/>\n\
    </segmentation>\n\
    <segmentation name='office' components='asset'>\n\
      <segment name='0001'/>\n\
      <segment name='0002'/>\n\
      <segment name='0003'/>\n\
      <segment name='0004'/>\n\
    </segmentation>\n\
  </segmentations>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // segmentation object creation
  Segmentations segmentations;
  ASSERT_NO_THROW(segmentations = Segmentations(*(doc->getDocumentElement())));

  delete parser;
  return segmentations;
}

//===========================================================================
// test1
//===========================================================================
void ClientTest::test1()
{
  // client definition
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='A' sector='S2' name='cliente1' id='cif1'>\n\
      <belongs-to concept='sector' segment='S2'/>\n\
      <belongs-to concept='size' segment='big'/>\n\
      <asset name='generic' id='op1'>\n\
        <belongs-to concept='product' segment='bond'/>\n\
        <belongs-to concept='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' exposure='500.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <belongs-to concept='product' segment='bond'/>\n\
        <belongs-to concept='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' exposure='500.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // client creation
  Client *client = NULL;
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  ASSERT_NO_THROW(client = new Client(&ratings, &sectors, &segmentations, NULL, *(doc->getDocumentElement())));

  if (client != NULL)
  {
    // assertions
    ASSERT(client->id == "cif1");
    ASSERT(client->name == "cliente1");
    ASSERT(client->irating == 0);
    ASSERT(client->isector == 1);

    ASSERT(client->belongsTo(1, 1));
    ASSERT(client->belongsTo(3, 2));
    ASSERT(client->belongsTo(4, 1));

    vector<Asset> *assets = NULL;
    ASSERT_NO_THROW(assets = client->getAssets());

    ASSERT(2 == assets->size());
    ASSERT((*assets)[0].getId() == "op1");
    ASSERT((*assets)[1].getId() == "op2");
  }

  // exit test
  if (client != NULL) delete client;
  delete parser;
  XMLUtils::terminate();
}

//===========================================================================
// test2
//===========================================================================
void ClientTest::test2()
{
  // client definition with invalid rating
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='K' sector='S2' name='cliente1' id='cif1'>\n\
      <asset name='generic' id='op1'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' exposure='500.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' exposure='500.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // client creation
  Client *client = NULL;
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  ASSERT_THROW(client = new Client(&ratings, &sectors, &segmentations, NULL, *(doc->getDocumentElement())));

  // exit test
  if (client != NULL) delete client;
  delete parser;
  XMLUtils::terminate();
}

//===========================================================================
// test3
//===========================================================================
void ClientTest::test3()
{
  // client definition with invalid asset (data repeated)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='A' sector='S2' name='cliente1' id='cif1'>\n\
      <asset name='generic' id='op1'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' exposure='500.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' exposure='500.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // client creation
  Client *client = NULL;
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  ASSERT_THROW(client = new Client(&ratings, &sectors, &segmentations, NULL, *(doc->getDocumentElement())));

  // exit test
  if (client != NULL) delete client;
  delete parser;
  XMLUtils::terminate();
}
