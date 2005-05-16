
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// Survival.cpp - Survival code
// --------------------------------------------------------------------------
//
// 2005/05/14 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cfloat>
#include <cassert>
#include "survival/Survival.hpp"
#include "utils/Parser.hpp"
#include "utils/Utils.hpp"

//===========================================================================
// private initializator
//===========================================================================
void ccruncher::Survival::init(Ratings *ratings_) throw(Exception)
{
  maxmonths = 0;
  ratings = ratings_;
  epsilon = 1e-12;

  nratings = ratings->getRatings()->size();

  if (nratings <= 0) {
    throw Exception("Survival::init(): invalid number of ratings");
  }
  else {
    data = new vector<double>[nratings];
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Survival::Survival(Ratings *ratings_) throw(Exception)
{
  // posem valors per defecte
  init(ratings_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Survival::~Survival()
{
  // nothing to do
}

//===========================================================================
// insert an element
//===========================================================================
void ccruncher::Survival::insertValue(const string &srating, int t, double value) throw(Exception)
{
  int irating = ratings->getIndex(srating);

  // validem ratings entrats
  if (irating < 0 || irating > nratings)
  {
    string msg = "Survival::insertValue(): undefined rating at <survival>: ";
    msg += srating;
    throw Exception(msg);
  }
  
  // validating time 
  if (t < 0)
  {
    string msg = "Survival::insertValue(): value[";
    msg += srating;
    msg += "][";
    msg += Parser::int2string(t);
    msg += "] has time < 0";
    throw Exception(msg);
  }

  // validating value
  if (value < -epsilon || value-1.0 > epsilon)
  {
    string msg = "Survival::insertValue(): value[";
    msg += srating;
    msg += "][";
    msg += Parser::int2string(t);
    msg += "] out of range: ";
    msg += Parser::double2string(value);
    throw Exception(msg);
  }

  // checking that not exist
  if (data[irating].size() >= t+1 && !isnan(data[irating][t]))
  {
    string msg = "Survival::insertValue(): value[";
    msg += srating;
    msg += "][";
    msg += Parser::int2string(t);
    msg += "] redefined";
    throw Exception(msg);  
  }

  // allocating space in vector (if needed)
  if (data[irating].size() < t+1)
  {
    for(int i=data[irating].size();i<t+1;i++)
    {
      data[irating].push_back(NAN);
    }
  }

  // inserting value
  data[irating][t] = value;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Survival::epstart(ExpatUserData &eu, const char *name, const char **attributes)
{
  if (isEqual(name,"survival")) {
    if (getNumAttributes(attributes) < 1 || getNumAttributes(attributes) > 2) {
      throw eperror(eu, "invalid number of attributes in tag survival");
    }
    else {
      epsilon = getDoubleAttribute(attributes, "epsilon", 1e-12);
      maxmonths = getIntAttribute(attributes, "maxmonths", INT_MAX);
      if (maxmonths == INT_MAX || maxmonths < 0 || epsilon < 0.0 || epsilon > 0.5) {
        throw eperror(eu, "invalid attributes at <survival>");
      }      
    }
  }
  else if (isEqual(name,"svalue")) {
    string srating = getStringAttribute(attributes, "rating", "");
    int t = getIntAttribute(attributes, "t", INT_MAX);
    double value = getDoubleAttribute(attributes, "value", DBL_MAX);

    if (srating == "" || t == INT_MAX || value == DBL_MAX) {
      throw eperror(eu, "invalid values at <svalue>");
    }
    else {
      insertValue(srating, t, value);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Survival::epend(ExpatUserData &eu, const char *name)
{
  if (isEqual(name,"survival")) {
    validate();
    fillHoles();
  }
  else if (isEqual(name,"svalue")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name));
  }
}

//===========================================================================
// validate given values
//===========================================================================
void ccruncher::Survival::validate() throw(Exception)
{
  // checking ranges, that all values are between 0 and 1 is checked
  // at insertion function. don't rechecked here
  
  // checking that all ratings (except default) have survival function defined
  for (int i=0;i<nratings-1;i++)
  {
    if (data[i].size() <= 0) {
      string msg = "Survival::validate(): rating ";
      msg += ratings->getName(i) + " haven't survival function defined";
      throw Exception(msg);
    }
  }

  // checking survival function value at t=0 is 1 if rating != default
  for (int i=0;i<nratings-1;i++)
  {
    if (isnan(data[i][0]))
    {
      data[i][0] = 1.0;
    }
    if (fabs(data[i][0]-1.0) > epsilon)
    {
      string msg = "Survival::validate(): rating ";
      msg += ratings->getName(i) + " have a value distinct that 1 at t=0";
      throw Exception(msg);
    }
  }
   
  // checking default rating survival function values (always 0) 
  for (int j=0;j<data[nratings-1].size();j++)
  {
    if (isnan(data[nratings-1][j])) {
      data[nratings-1][0] = 0.0;
    }
    if (fabs(data[nratings-1][j]) > epsilon)
    {
      throw Exception("Survival::validate(): default rating have a survival value distinct that 0");
    }    
  }

  // checking monotonicity
  for (int i=0;i<nratings-1;i++)
  {
    double pvalue = 2.0;
    
    for (int j=0;j<data[i].size();j++)
    {
      if (!isnan(data[i][j]))
      {
        if (data[i][j] < pvalue)
        {
          pvalue = data[i][j];
        }
        else
        {
          string msg = "Survival::validate(): rating ";
          msg += ratings->getName(i) + " is not monotone at t=" + Parser::int2string(j);
          throw Exception(msg);
        }
      }
    }
  }
}

//===========================================================================
// interpole non given values
//===========================================================================
double ccruncher::Survival::interpole(double x, double x0, double y0, double x1, double y1)
{
  if (fabs(x1-x0) < 1e-14) {
    return y0;
  } else {
    return y0 + (x-x0)*(y1-y0)/(x1-x0);
  }
}

//===========================================================================
// interpole non given values
//===========================================================================
void ccruncher::Survival::fillHoles()
{
  double x0, x1, y0, y1;
  
  for(int i=0;i<nratings-1;i++)
  {
    x0 = 0.0;
    y0 = 1.0;
    
    for(int j=1;j<data[i].size();j++)
    {
      if (!isnan(data[i][j]))
      {
        x1 = double(j);
        y1 = data[i][j];
        
        for (int k=int(x0)+1;k<j;k++)
        {
          data[i][k] = interpole(double(k), x0, y0, x1, y1);
        }
        
        x0 = double(j);
        y0 = data[i][j];
      }
    }
  }
}

//===========================================================================
// evalue irating-survival function at time (in months) t
//===========================================================================
double ccruncher::Survival::evalue(const int irating, int t)
{
  assert(irating < nratings);
  assert(irating >= 0);
  
  // if default rating
  if (irating == nratings-1) {
    return 0.0;
  }

  if (t < data[irating].size())  
  {
    return data[irating][t];
  }
  else if (t > maxmonths)
  {
    return 0.0;
  }
  else 
  {
    double y0 = data[irating].back();
    double y1 = 0.0;
    double x0 = double(data[irating].size())-1.0;
    double x1 = double(maxmonths);
    
    return interpole(double(t), x0, y0, x1, y1);
  }
}

//===========================================================================
// evalue inverse irating-survival function at time (in months) t
//===========================================================================
int ccruncher::Survival::inverse(const int irating, double val)
{
  assert(irating < nratings);
  assert(irating >= 0);
  assert(val-1.0 < epsilon);
  assert(val > -epsilon);
  
  // if default rating
  if (irating == nratings-1) {
    return 0;
  }
  
  // if sure -> t=0
  if (val == 1.0) {
    return 0;
  }

  if (val > data[irating].back())
  {
    // TODO: non-optimal method, use bisection or cuasi-Newton 
    // instead of secuential scan
    int n = data[irating].size();
    for (int j=1;j<n;j++)
    {
      if (data[irating][j] < val)
      {
        double x0 = data[irating][j-1];
        double x1 = data[irating][j];
        double y0 = double(j-1);
        double y1 = double(j);
        
        double ret = interpole(val, x0, y0, x1, y1);

        return int(round(ret));
      }
    }
    // error if value not found
    assert(false);
  }
  else
  {
    double x0 = data[irating].back();
    double x1 = 0.0;
    double y0 = double(data[irating].size())-1.0;
    double y1 = double(maxmonths);
    
    double ret = interpole(val, x0, y0, x1, y1);
    
    return int(round(ret));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Survival::getXML(int ilevel) throw(Exception)
{
  string spc1 = Utils::blanks(ilevel);
  string spc2 = Utils::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<survival maxmonths='" + Parser::int2string(maxmonths) + "' ";
  ret += "epsilon='" + Parser::double2string(epsilon) + "'>\n";

  for(int i=0;i<nratings;i++)
  {
    for(int j=0;j<data[i].size();j++)
    {
      ret += spc2 + "<svalue ";
      ret += "rating='" + ratings->getName(i) + "' ";
      ret += "t='" + Parser::int2string(j) + "' ";
      ret += "value ='" + Parser::double2string(data[i][j]) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</survival>\n";

  return ret;
}
