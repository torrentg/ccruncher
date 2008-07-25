
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
// XMLUtilsTest.cpp - XMLUtilsTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (see jama/tnt_stopwatch && boost/timer)
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#include <iostream>
#include <string>
#include "XMLUtils.hpp"
#include "XMLUtilsTest.hpp"

//===========================================================================
// setUp
//===========================================================================
void XMLUtilsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void XMLUtilsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void XMLUtilsTest::test1()
{
  DOMBuilder *parser = NULL;
  Wrapper4InputSource *wis = NULL;
  DOMDocument *doc = NULL;
  DOMNode *node = NULL;
  DOMNode *child = NULL;
  DOMNodeList *children = NULL;
  const DOMNamedNodeMap *attributes = NULL;
  string nodename;
  string xmlcontent = "\
<?xml version='1.0' encoding='ISO-8859-1'?>\
<root>\
  <!-- i am a comment -->\
  <child1 attrib1='i am a string'/>\
  <child2 attrib2='3'/>\
  <child3 attrib3='5'/>\
  <child4 attrib4='6.5'/>\
  <child5 attrib5='15/9/2004'/>\
  <child6 attrib6='true'/>\
</root>\
  ";

  // creating parser and doc
  ASSERT_NO_THROW(XMLUtils::initialize());
  ASSERT_NO_THROW(parser = XMLUtils::getParser());
  ASSERT_NO_THROW(wis = XMLUtils::getInputSource(xmlcontent));
  ASSERT_NO_THROW(doc = XMLUtils::getDocument(parser, wis));

  // retrieving root node
  node = doc->getDocumentElement();
  nodename = XMLUtils::XMLCh2String(node->getNodeName());
  ASSERT("root" == nodename);

  // retrieving childs
  children = node->getChildNodes();
  ASSERT_EQUALS(14, (int) children->getLength());

  // testing comment node
  child = children->item(0);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(1);
  ASSERT(XMLUtils::isCommentNode(*child));

  // testing string attributes
  child = children->item(2);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(3);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child1" == nodename);
  attributes = child->getAttributes();
  ASSERT("i am a string" == XMLUtils::getStringAttribute(*attributes, "attrib1", "error"));

  // testing int attributes
  child = children->item(4);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(5);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child2" == nodename);
  attributes = child->getAttributes();
  ASSERT(3 == XMLUtils::getIntAttribute(*attributes, "attrib2", -1));

  // testing long attributes
  child = children->item(6);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(7);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child3" == nodename);
  attributes = child->getAttributes();
  ASSERT(5 == XMLUtils::getLongAttribute(*attributes, "attrib3", -1L));

  // testing double attributes
  child = children->item(8);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(9);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child4" == nodename);
  attributes = child->getAttributes();
  ASSERT(6.5 == XMLUtils::getDoubleAttribute(*attributes, "attrib4", -1.0));

  // testing date attributes
  child = children->item(10);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(11);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child5" == nodename);
  attributes = child->getAttributes();
  ASSERT(Date("15/09/2004") == XMLUtils::getDateAttribute(*attributes, "attrib5", Date()));

  // testing boolean attributes
  child = children->item(12);
  ASSERT(XMLUtils::isVoidTextNode(*child));
  child = children->item(13);
  nodename = XMLUtils::XMLCh2String(child->getNodeName());
  ASSERT("child6" == nodename);
  attributes = child->getAttributes();
  ASSERT(true == XMLUtils::getBooleanAttribute(*attributes, "attrib6", false));

  // closing document
  delete wis;
  delete parser;
  XMLUtils::terminate();
}
