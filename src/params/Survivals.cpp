
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
#include "params/Survivals.hpp"
#include "utils/Format.hpp"
#include "utils/Strings.hpp"
#include <climits>
#include <cassert>

// --------------------------------------------------------------------------

/* 
  increase this value if you use large time ranges (eg. 100 years) or you 
  require more precision (eg. ISURVFNUMBINS=1000).
*/
#define ISURVFNUMBINS 100 
#define EPSILON 1e-12

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Survivals::Survivals()
{
  // nothing to do
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Survivals::Survivals(const Ratings &ratings_) throw(Exception)
{
  setRatings(ratings_);
}

//===========================================================================
// constructor
// used by Transitions class
//===========================================================================
ccruncher::Survivals::Survivals(const Ratings &ratings_, const vector<int> &imonths,
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
  fillHoles();
  computeInvTable();
}

//===========================================================================
// set ratings
//===========================================================================
void ccruncher::Survivals::setRatings(const Ratings &ratings_) throw(Exception)
{
  ratings = ratings_;

  if (ratings.size() <= 0)
  {
      throw Exception("invalid number of ratings (" + Format::toString(ratings.size()) + " <= 0)");
  }
  else 
  {
    ddata = vector<vector<double> >(ratings.size());
    //TODO: review idata allocation
    //idata = vector<vector<double> >(nratings);
    //idata.insert(idata.begin(), nratings, vector<double>(ISURVFNUMBINS+1, NAN));
    idata.assign(ratings.size(), vector<double>(ISURVFNUMBINS+1, NAN));
  }
}

//===========================================================================
// destructor
//===========================================================================
int ccruncher::Survivals::size() const
{
  return ratings.size();
}

//===========================================================================
// insert an element
//===========================================================================
void ccruncher::Survivals::insertValue(const string &srating, int t, double value) throw(Exception)
{
  int irating = ratings.getIndex(srating);

  // checking rating index
  if (irating < 0 || irating >= ratings.size())
  {
    throw Exception("unknow rating at <survivals>: " + srating);
  }

  // validating time
  if (t < 0)
  {
    string msg = "survival value[" + srating + "][" + Format::toString(t) + "] has time < 0";
    throw Exception(msg);
  }

  // validating value
  if (value < -EPSILON || value-1.0 > EPSILON)
  {
    string msg = "survival value[" + srating + "][" + Format::toString(t) + 
                 "] out of range: " + Format::toString(value);
    throw Exception(msg);
  }

  // checking that is not previously defined
  if ((int) ddata[irating].size() >= t+1 && !isnan(ddata[irating][t]))
  {
    string msg = "survival value[" + srating + "][" + Format::toString(t) + "] redefined";
    throw Exception(msg);
  }

  // allocating space in vector (if needed)
  if ((int) ddata[irating].size() < t+1)
  {
    for(int i=ddata[irating].size(); i<t+1; i++)
    {
      ddata[irating].push_back(NAN);
    }
  }

  // inserting value
  ddata[irating][t] = value;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Survivals::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"survivals")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes not allowed in tag survival");
    }
  }
  else if (isEqual(name,"svalue")) {
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
void ccruncher::Survivals::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"survivals")) {
    validate();
    fillHoles();
    computeInvTable();
  }
  else if (isEqual(name,"svalue")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// validate given values
//===========================================================================
void ccruncher::Survivals::validate() throw(Exception)
{
  // checking ranges, that all values are between 0 and 1 is checked
  // at insertion function. don't rechecked here

  // finding default rating
  indexdefault = -1;
  for (int i=0; i<ratings.size(); i++)
  {
    if (ddata[i].size() == 0 || ddata[i][0] < EPSILON) {
      indexdefault = i;
      break;
    }
  }
  if (indexdefault < 0) {
    throw Exception("default rating not found");
  }

  // checking that all ratings (except default) have survival function defined
  for (int i=0; i<ratings.size(); i++)
  {
    if (ddata[i].size() == 0 && i != indexdefault) {
        string msg = "rating " + ratings.getName(i) + " without survival function defined";
      throw Exception(msg);
    }
  }

  // checking survival function value at t=0 is 1 if rating != default
  for (int i=0; i<ratings.size(); i++)
  {
    if (i != indexdefault)
    {
      if (isnan(ddata[i][0]))
      {
        ddata[i][0] = 1.0;
      }
      else if (fabs(ddata[i][0]-1.0) > EPSILON)
      {
        string msg = "rating " + ratings.getName(i) + " have a survival value distinct that 1 at t=0";
        throw Exception(msg);
      }
    }
  }

  // checking default rating survival function values (always 0)
  for (unsigned int j=0; j<ddata[indexdefault].size(); j++)
  {
    if (isnan(ddata[indexdefault][j]))
    {
      ddata[indexdefault][0] = 0.0;
    }
    else if (fabs(ddata[indexdefault][j]) > EPSILON)
    {
      throw Exception("default rating have a survival value distinct that 0");
    }
  }

  // checking monotonic decreasing
  for (int i=0; i<ratings.size(); i++)
  {
    if (i == indexdefault) continue;

    double pvalue = 2.0;

    for (unsigned int j=0; j<ddata[i].size(); j++)
    {
      if (!isnan(ddata[i][j]))
      {
        if (ddata[i][j] <= pvalue)
        {
          pvalue = ddata[i][j];
        }
        else
        {
          string msg = "survival function of rating " + ratings.getName(i) + " is not monotone at t=" + Format::toString((int)j);
          throw Exception(msg);
        }
      }
    }
  }
}

//===========================================================================
// interpole
//===========================================================================
double ccruncher::Survivals::interpole(double x, double x0, double y0, double x1, double y1) const
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

//===========================================================================
// interpole non given values
//===========================================================================
void ccruncher::Survivals::fillHoles()
{
  double x0, x1, y0, y1;

  for(int i=0; i<ratings.size(); i++)
  {
    if (i == indexdefault) continue;

    x0 = 0.0;
    y0 = 1.0;

    for(int j=1; j<(int)ddata[i].size(); j++)
    {
      if (!isnan(ddata[i][j]))
      {
        x1 = double(j);
        y1 = ddata[i][j];

        for (int k=(int)(x0+1); k<j; k++)
        {
          ddata[i][k] = interpole(double(k), x0, y0, x1, y1);
        }

        x0 = double(j);
        y0 = ddata[i][j];
      }
    }
  }

  // default rating values (always 0)
  for(unsigned int j=0; j<ddata[indexdefault].size(); j++)
  {
    ddata[indexdefault][j] = 0.0;
  }
}

//===========================================================================
// compute the inverse table
// for each rating, find inverse values for 0.01, 0.02, ..., 0.99 and set
// into idata. idata is used to speed up inverse() computations
//===========================================================================
void ccruncher::Survivals::computeInvTable()
{
  for (int irating=0; irating<ratings.size(); irating++)
  {
    if (irating == indexdefault)
    {
      for(int j=0; j<ISURVFNUMBINS+1; j++)
      {
        idata[indexdefault][j] = 0.0;
      }
    }
    else
    {
      for(int j=0; j<ISURVFNUMBINS+1; j++)
      {
        double x = double(j)/double(ISURVFNUMBINS);
        idata[irating][j] = inverse1(irating, x);
      }
    }
  }
}

//===========================================================================
// evalue irating-survival function at time (in months) t
//===========================================================================
double ccruncher::Survivals::evalue(const int irating, int t) const
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
    return ddata[irating][t];
  }
  else
  {
    return 1.0;
  }
}

//===========================================================================
// internal inverse function [see inverse() method]
// non optimal method based on secuential scan
//===========================================================================
double ccruncher::Survivals::inverse1(const int irating, double val) const
{
  assert(irating < ratings.size());
  assert(irating >= 0);
  assert(val-1.0 < EPSILON);
  assert(val > -EPSILON);

  // if default rating
  if (irating == indexdefault) {
    assert(fabs(val) < EPSILON);
    return 0.0;
  }

  // if sure -> t=0
  if (val > 1.0 - EPSILON) {
    return 0.0;
  }

  if(val < ddata[irating].back())
  {
    return (double)(INT_MAX);
  }

  double x0=-1.0, y0=-1.0, x1=-1.0, y1=-1.0;

  for (int j=1; j<(int)ddata[irating].size(); j++)
  {
    if (ddata[irating][j] <= val)
    {
      x0 = ddata[irating][j-1];
      y0 = (double)(j-1);
      x1 = ddata[irating][j];
      y1 = (double)(j);
      break;
    }
  }
  assert(x0 >= 0.0);
  return interpole(val, x0, y0, x1, y1);
}

//===========================================================================
// evalue inverse irating-survival function. 
// returns the default time in months
// if result time is bigger than maximum date, returns last_date+1_year
// obs: val is a value in [0,1]
//===========================================================================
double ccruncher::Survivals::inverse(const int irating, double val) const
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

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Survivals::getXML(int ilevel) throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<survivals>\n";

  for(int i=0; i<ratings.size(); i++)
  {
    for(unsigned int j=0; j<ddata[i].size(); j++)
    {
      ret += spc2 + "<svalue ";
      ret += "rating='" + ratings.getName(i) + "' ";
      ret += "t='" + Format::toString((int)j) + "' ";
      ret += "value='" + Format::toString(100.0*ddata[i][j]) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</survivals>\n";

  return ret;
}

//===========================================================================
// getMinCommonTime (in months)
//===========================================================================
int ccruncher::Survivals::getMinCommonTime() const
{
  int ret=INT_MAX;

  // searching min time
  for(int i=0; i<ratings.size(); i++)
  {
    if (int(ddata[i].size()) < ret)
    {
      ret = ddata[i].size();
    }
  }

  // exit function
  return ret;
}
