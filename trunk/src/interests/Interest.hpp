
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#ifndef _Interest_
#define _Interest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

enum InterestType
{ 
  Simple,
  Compound,
  Continuous
};

//---------------------------------------------------------------------------

class Interest : public ExpatHandlers
{

  private:

    class Rate
    {
      public:

        // time (as string)
        string t_str;
        // time (in days from interest curve date)
        int d;
        // time (in years from interest curve date)
        double y;
        // yearly rate value (0.025 -> 2.5% per year)
        double r;

      public:

        // constructor
        Rate(int d_, double y_=0.0, double r_=1.0, const string &str="") : t_str(str), d(d_), y(y_), r(r_) {}
        // comparator
        bool operator< (const Rate &right ) const { return (d < right.d); }
    };

  private:

    // interest type
    InterestType type;
    // interest curve date
    Date date;
    // rate values
    vector<Rate> vrates;

  private:

    // insert a rate to list
    void insertRate(const Rate &) throw(Exception);
    // given a time, returns the rate (interpolated)
    void getValues(int, double *, double *) const;

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // default constructor
    Interest(const Date &date_=Date());
    // return interest type
    InterestType getType() const;
    // numbers of rates
    int size() const;
    // returns upsilon value
    double getFactor(const Date &date1) const;
    // serialize object content as xml
    string getXML(int) const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
