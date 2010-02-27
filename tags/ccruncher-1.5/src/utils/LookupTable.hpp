
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _LookupTable_
#define _LookupTable_

//---------------------------------------------------------------------------

#include <cmath>
#include <vector>
#include "utils/config.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class LookupTable
{

  private:

    // minimum x value
    double xmin;
    // maximum x value
    double xmax;
    // number of steps
    double steplength;
    // array of values
    vector<double> values;
    // array of slopes
    vector<double> slope;

  public:

    // default constructor
    LookupTable();
    // constructor
    LookupTable(double minv, double maxv, const vector<double> &vals) throw(Exception);
    // lookup table initialization
    void init(double minv, double maxv, const vector<double> &vals) throw(Exception);
    // number of steps
    int size() const;
    // evalue the function at x value
    double evalue(double x) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// returns the function evaluated at x
//===========================================================================
inline double ccruncher::LookupTable::evalue(double x) const
{
  if (x <= xmin)
  {
    return values.front();
  }
  else if (x >= xmax)
  {
    return values.back();
  }
  else
  {
    // interpolation formula: y = y0 + (x-x0) x (y1-y0)/(x1-x0)
    // precomputed slope: (y1-y0)/(x1-x0)
    int i = (int) floor((x-xmin)/steplength);
    return values[i] + (x-(xmin+steplength*i)) * slope[i];
  }
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
