
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
// Date.hpp - Date header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release based on a Bernt A Oedegaard class
//
// 2005/07/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . converting to inline some critical methods
//
//===========================================================================

#ifndef _Date_
#define _Date_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <iostream>
#include <fstream>
#include <string>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Date
{

  protected:

    int year_;
    char month_;
    char day_;

  public:

    Date();
    Date(const string &str) throw(Exception);
    Date(const long d) throw(Exception);
    Date(const int d, const int m, const int y) throw(Exception);

    static bool valid(const int d, const int m, const int y);

    inline int getDay() const;
    inline int getMonth() const;
    inline int getYear() const;
    int getDayOfYear() const;

    void setDay (const int day) throw(Exception);
    void setMonth (const int month) throw(Exception);
    void setYear (const int year) throw(Exception);

    Date operator ++();    // prefix
    Date operator ++(int); // postfix
    Date operator --();    // prefix
    Date operator --(int); // postfix

    string toString() const; // dd/mm/yyyy
    long longDate();         // yyyymmdd

    static bool isLeapYear(int);
    bool isLeapYear();
    static int numDaysInYear(int);
    int numDaysInYear();
    static int numDaysInMonth(int m, int y);
    int numDaysInMonth();

  private:

    bool valid(void) const;

};

//---------------------------------------------------------------------------

// calculations
Date nextDate(const Date&);
// calculations
Date previousDate(const Date&);
// increment n number of months
Date addMonths(const Date& d, const int& no_months);
// increment 1 month
Date nextMonth(const Date& d);
// increment n number of days
Date operator +=(Date&, const int&);
// decrement n number of days
Date operator -=(Date&, const int&);
// comparation operator
bool operator == (const Date&, const Date&);
// comparation operator
bool operator != (const Date&, const Date&);
// comparation operator
bool operator <  (const Date&, const Date&);
// comparation operator
bool operator >  (const Date&, const Date&);
// comparation operator
bool operator <= (const Date&, const Date&);
// comparation operator
bool operator >= (const Date&, const Date&);
// output operator
ostream& operator << (ostream& os, const Date& d);
// increment n number of days
Date operator +(const Date&, const int&);
// decrement n number of days
Date operator -(const Date&, const int&);
// diferencia de dies entre 2 dates
int operator -(const Date&, const Date&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

//===========================================================================
// inline getXXX() methods
//===========================================================================
int ccruncher::Date::getDay() const
{
  return (int) day_;
}

int ccruncher::Date::getMonth() const
{
  return (int) month_;
}

int ccruncher::Date::getYear() const
{
  return year_;
}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
