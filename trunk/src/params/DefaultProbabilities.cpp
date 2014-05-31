
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

#include <cassert>
#include "params/DefaultProbabilities.hpp"
#include "utils/Format.hpp"

#define EPSILON 1e-12

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::DefaultProbabilities::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (eu.ratings == nullptr || eu.ratings->size() == 0 || eu.date1 == NAD) {
    throw Exception("ratings or date1 is not defined");
  }

  if (isEqual(tag,"dprobs")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes not allowed in tag 'dprobs'");
    }
    this->assign(eu.ratings->size(), CDF(0, INFINITY));
  }
  else if (isEqual(tag,"dprob")) {
    string srating = getStringAttribute(attributes, "rating");
    Date t(eu.date1);
    const char *str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      t.add(str);
    }
    else {
      t = Date(str);
    }
    double value = getDoubleAttribute(attributes, "value");
    size_t irating = eu.ratings->indexOf(srating);
    (*this)[irating].add(t-eu.date1, value);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @throw Exception Validation error.
 */
void ccruncher::DefaultProbabilities::epend(ExpatUserData &eu, const char *tag)
{
  if (isEqual(tag,"dprobs"))
  {
    int idefault = -1;

    for(size_t i=0; i<size(); i++)
    {
      const vector<pair<double,double>> &points = (*this)[i].getPoints();

      if (points.size() == 0) {
        if (idefault < 0) {
          // no filled -> default rating
          (*this)[i].add(0.0, 1.0);
          idefault  = i;
        }
        else {
          string str = (*(eu.ratings))[i].getName();
          throw Exception("dprob for rating '" + str + "' not set");
        }
      }
      else if (points.front().first > EPSILON) {
        (*this)[i].add(0.0, 0.0);
      }
    }

    // checking content
    isValid(true);
  }
}

/**************************************************************************//**
 * @details Internal method. Sorts data, identifies default rating, fills
 *          info not entered by user, and checks data.
 * @throw Exception Validation error.
 */
bool ccruncher::DefaultProbabilities::isValid(bool throwException) const
{
  try
  {
    size_t idefault = getIndexDefault();

    // finding empty cdf's or with range bad-defined
    for(size_t i=0; i<size(); i++)
    {
      if (i != idefault && (*this)[i].evalue(0.0) > EPSILON) {
        throw Exception("cdf with dprob(0) > 0");
      }
      if ((*this)[i].getPoints().back().second < EPSILON) {
        throw Exception("cdf with dprob(x) = 0 for all x");
      }
    }

    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @return Index (0-based) of the 'default' rating. -1 if not found.
 * @throw Exception default rating not found
 */
size_t ccruncher::DefaultProbabilities::getIndexDefault() const
{
  for(size_t i=0; i<size(); i++)
  {
    if ((*this)[i].evalue(0.0) > 1.0-EPSILON) {
      return i;
    }
  }
  throw Exception("default rating not found");
}

