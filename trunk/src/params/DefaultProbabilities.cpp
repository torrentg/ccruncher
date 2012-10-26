
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "params/DefaultProbabilities.hpp"
#include "utils/Format.hpp"
#include "utils/Strings.hpp"
#include <climits>
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1e-12

//===========================================================================
// default constructor
//===========================================================================
ccruncher::DefaultProbabilities::DefaultProbabilities()
{
  // nothing to do
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::DefaultProbabilities::DefaultProbabilities(const Ratings &ratings_) throw(Exception)
{
  setRatings(ratings_);
}

//===========================================================================
// constructor
// used by Transitions class
//===========================================================================
ccruncher::DefaultProbabilities::DefaultProbabilities(const Ratings &ratings_, const vector<int> &imonths,
           const vector<vector<double> > &values) throw(Exception)
{
  setRatings(ratings_);

  // adding values
  for(int i=0; i<ratings.size(); i++)
  {
    for(unsigned int j=0; j<imonths.size(); j++)
    {
      insertValue(ratings.getName(i), imonths[j], values[i][j]);
    }
  }

  validate();
}

//===========================================================================
// set ratings
//===========================================================================
void ccruncher::DefaultProbabilities::setRatings(const Ratings &ratings_) throw(Exception)
{
  ratings = ratings_;

  if (ratings.size() <= 0)
  {
      throw Exception("invalid number of ratings (" + Format::toString(ratings.size()) + " <= 0)");
  }
  else
  {
    ddata = vector<vector<pd> >(ratings.size());
    //TODO: review idata allocation
    //idata = vector<vector<double> >(nratings);
    //idata.insert(idata.begin(), nratings, vector<double>(ISURVFNUMBINS+1, NAN));
    //idata.assign(ratings.size(), vector<double>(ISURVFNUMBINS+1, NAN));
    //TODO: eliminar aquests comentaris + resoldre tamany ddata
  }
}

//===========================================================================
// return ratings
//===========================================================================
const Ratings & ccruncher::DefaultProbabilities::getRatings() const
{
  return ratings;
}

//===========================================================================
// return index of default rating
//===========================================================================
int ccruncher::DefaultProbabilities::getIndexDefault() const
{
  return indexdefault;
}

//===========================================================================
// destructor
//===========================================================================
int ccruncher::DefaultProbabilities::size() const
{
  return ratings.size();
}

//===========================================================================
// insert an element
//===========================================================================
void ccruncher::DefaultProbabilities::insertValue(const string &srating, int t, double value) throw(Exception)
{
  int irating = ratings.getIndex(srating);

  // checking rating index
  if (irating < 0)
  {
    throw Exception("unknow rating at <dprobs>: " + srating);
  }

  // validating time
  if (t < 0)
  {
    string msg = "dprob[" + srating + "][" + Format::toString(t) + "] has time < 0";
    throw Exception(msg);
  }

  // validating value
  if (value < 0.0 || 1.0 < value)
  {
    string msg = "dprob[" + srating + "][" + Format::toString(t) +
                 "] out of range: " + Format::toString(value);
    throw Exception(msg);
  }

  // checking that is not previously defined
  for(size_t i=0; i<ddata[irating].size(); i++) {
    if (ddata[irating][i].month == t) {
      string msg = "dprob[" + srating + "][" + Format::toString(t) + "] redefined";
      throw Exception(msg);
    }
  }

  // inserting value
  ddata[irating].push_back(pd(t,value));
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::DefaultProbabilities::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"dprobs")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes not allowed in tag dprobs");
    }
  }
  else if (isEqual(name,"dprob")) {
    string srating = getStringAttribute(attributes, "rating");
    int t = getIntAttribute(attributes, "t");
    double value = getDoubleAttribute(attributes, "value");
    insertValue(srating, t, value);
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::DefaultProbabilities::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"dprobs")) {
    validate();
  }
  else if (isEqual(name,"dprob")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// validate given values
//===========================================================================
void ccruncher::DefaultProbabilities::validate() throw(Exception)
{
  // ranges are checked in insertValue() method. don't rechecked here

  // sorting values
    for(size_t i=0; i<ddata.size(); i++) {
    sort(ddata[i].begin(), ddata[i].end());
  }

  // finding default rating
  indexdefault = -1;
  for (int i=0; i<ratings.size(); i++)
  {
    if (ddata[i].size() == 0 || (ddata[i][0].month == 0 && ddata[i][0].prob > 1.0-EPSILON)) {
      indexdefault = i;
      break;
    }
  }
  if (indexdefault < 0) {
    throw Exception("default rating not found");
  }

  // checking that all ratings (except default) have pd function defined
  for (int i=0; i<ratings.size(); i++)
  {
    if (ddata[i].size() == 0 && i != indexdefault) {
      string msg = "rating " + ratings.getName(i) + " without dprob";
      throw Exception(msg);
    }
  }

  // checking dprob value at t=0 is 0 if rating != default
  for (int i=0; i<ratings.size(); i++)
  {
    if (i != indexdefault && ddata[i][0].month == 0 && ddata[i][0].prob > EPSILON)
    {
      string msg = "rating " + ratings.getName(i) + " have a dprob distinct than 0 at t=0";
      throw Exception(msg);
    }
  }

  // checking that dprob[default][t] = 1 for any t
  for (size_t j=0; j<ddata[indexdefault].size(); j++)
  {
    if (ddata[indexdefault][j].prob < 1.0-EPSILON)
    {
      throw Exception("default rating have a dprob less than 1");
    }
  }

  // checking monotonic increasing
  for (int i=0; i<ratings.size(); i++)
  {
    if (i == indexdefault) continue;

    for (unsigned int j=1; j<ddata[i].size(); j++)
    {
      if (ddata[i][j].prob < ddata[i][j-1].prob)
      {
        string msg = "dprob[" + ratings.getName(i) + "] is not monotonically increasing at t=" + Format::toString(ddata[i][j].month);
        throw Exception(msg);
      }
    }
  }
}

//===========================================================================
// interpole
//===========================================================================
double ccruncher::DefaultProbabilities::interpole(double x, double x0, double y0, double x1, double y1) const
{
  if (std::fabs(x1-x0) < 1e-14)
  {
    return y0;
  }
  else
  {
    return y0 + (x-x0)*(y1-y0)/(x1-x0);
  }
}
/*
//===========================================================================
// evalue irating-survival function at time (in months) t
//===========================================================================
double ccruncher::DefaultProbabilities::evalue(const int irating, int t) const
{
  assert(irating < ratings.size());
  assert(irating >= 0);
  assert(t >= 0);

  // if default rating
  if (irating == indexdefault) {
    return 0.0;
  }

  if (t < (int) ddata[irating].size())
  {
    return ddata[irating][t].prob;
  }
  else
  {
    return 1.0;
  }
}

//===========================================================================
// evalue inverse irating-survival function.
// returns the default time in months
// if result time is bigger than maximum date, returns last_date+1_year
// obs: val is a value in [0,1]
//===========================================================================
double ccruncher::DefaultProbabilities::inverse(const int irating, double val) const
{
  assert(irating >=0 && irating < ratings.size());
  assert(val >= 0.0 && val <= 1.0);

  // default rating allways is defaulted
  if (irating == indexdefault) {
    return 0.0;
  }

  // if val=0 => non-default
  if (val <= EPSILON) {
    return ddata[irating].size()+11.0;
  }

  // to avoid precision problems
  if (1.0-val < 1.0/ISURVFNUMBINS) {
    return inverse1(irating, val);
  }

  // interpolate (because we need day resolution)
  int k = (int)(floor(val*ISURVFNUMBINS));
  if (k >= ISURVFNUMBINS) {
    return 0.0;
  }

  double x0 = (double)(k+0)/double(ISURVFNUMBINS);
  double y0 = idata[irating][k];
  double x1 = (double)(k+1)/double(ISURVFNUMBINS);
  double y1 = idata[irating][k+1];

  if ((int)y0 == INT_MAX || (int)y1 == INT_MAX) {
    return ddata[irating].size()+11.0;
  }

  assert(x0 <= val && val <= x1);
  return interpole(val, x0, y0, x1, y1);
}
*/
//===========================================================================
// getXML
//===========================================================================
string ccruncher::DefaultProbabilities::getXML(int ilevel) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<dprobs>\n";

  for(int i=0; i<ratings.size(); i++)
  {
    for(unsigned int j=0; j<ddata[i].size(); j++)
    {
      ret += spc2 + "<dprob ";
      ret += "rating='" + ratings.getName(i) + "' ";
      ret += "t='" + Format::toString(ddata[i][j].month) + "' ";
      ret += "value='" + Format::toString(100.0*ddata[i][j].prob) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</dprobs>\n";

  return ret;
}

//===========================================================================
// getMinCommonTime (in months)
//===========================================================================
int ccruncher::DefaultProbabilities::getMinCommonTime() const
{
  int ret=INT_MAX;

  // searching min time
  for(int i=0; i<ratings.size(); i++)
  {
    if (i != indexdefault)
    {
      if (ddata[i].back().month < ret)
      {
        ret = ddata[i].back().month;
      }
    }
  }

  // exit function
  return ret;
}

