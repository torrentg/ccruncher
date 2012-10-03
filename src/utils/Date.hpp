
//===========================================================================
// $Id: Date.h,v 1.3 2002/10/18 05:56:10 tknarr Exp $
// Copyright (C) 2000, 2002
// Todd T. Knarr <tknarr@silverglass.org>

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Description    :
//
// Provides a Date class which represents dates as
// Julian day numbers ( days since 1 Jan 4713 BC ).
// This class can handle all dates from  1 Jan 4713BC to 31 Dec 4999AD.
//
// Note: Years AD are positive, years BC are negative. There is
// no year 0AD, it goes from 1BC to 1AD. A year of 0 will be treated
// as 1BC. No attempt is made to validate ranges. Physical errors
// will not result from insane day-month combinations like the 87th
// day of the 45th month, but the results will obviously not make
// much sense.
//
// Date conversion routines  by Eric bergman-Terrell, Computer Language,
//     Dec 1990.
// Alternate method by David Burki, CUJ Feb 1993.
//
// To Do:
// * Add << and >> operators for iostreams. They should probably output
//   and input the string form.
//--------------------------------------------------------------------------
//
// 09/11/1995 - Todd Knarr
//   . initial release
//
//===========================================================================

#ifndef _Date_
#define _Date_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

// NAD = Not A Date = Date(0,0,0)
static const long nad[1] = { 1721026L };
#define NAD (*(const Date *) nad)

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Date
{

  private:

    long lJulianDay;


  private:

    static long YmdToJd(const int iYear, const int iMonth, const int iDay);
    static void JdToYmd(const long lJD, int *piYear, int *piMonth, int *piDay);
    void parse(const char *) throw(Exception);


  public:

    Date();
    Date(const int iDay, const int iMonth, const int iYear) throw(Exception);
    explicit Date(const time_t tSysTime);
    Date(const string &str) throw(Exception);
    Date(const char *str) throw(Exception);

    string toString() const;
    static bool valid(const int d, const int m, const int y);
    static int numDaysInMonth(int m, int y);
    static int numDaysInYear(int);
    int getYear( void ) const;
    int getMonth( void ) const;
    int getDay( void ) const;
    int getDayOfYear( void ) const;
    int getDayOfWeek( void ) const;
    int getDayOfWorkWeek( void ) const;
    static bool isLeapYear(const int iYear);
    bool isLeapYear( void ) const;
    void YMD( int *pY, int *pM, int *pD ) const;
    time_t ToSysTime( void ) const;
    void add(const char *str) throw(Exception);

    Date& operator=( const Date& Orig );
    friend Date operator+( const Date& Left, const long Right );
    friend Date operator+( const long Left, const Date& Right );
    Date& operator+=( const long Right );
    Date& operator++();
    Date operator++( int );
    friend Date operator-( const Date& Left, const long Right );
    friend Date operator-( const long Left, const Date& Right );
    Date& operator-=( const long Right );
    long operator-( const Date& Right ) const;
    Date& operator--();
    Date operator--( int );

    bool operator==( const Date& Right ) const;
    bool operator!=( const Date& Right ) const;
    bool operator<( const Date& Right ) const;
    bool operator<=( const Date& Right ) const;
    bool operator>( const Date& Right ) const;
    bool operator>=( const Date& Right ) const;

};

//---------------------------------------------------------------------------

// output operator
ostream& operator << (ostream& os, const Date& d);
// increment n number of days/months/years
Date add(const Date &d, int num, char units);
// difference between 2 dates  in days/months/years
double diff(const Date &d1, const Date &d2, char units);
// minimum of 2 dates
Date min(const Date&, const Date&);
// maximum of 2 dates
Date max(const Date&, const Date&);
// check if has format ([+-])?([0-9])+[DMY]
bool isInterval(const char *str);

//---------------------------------------------------------------------------

}

//===========================================================================
// Constructor. constructs a new object with non-valid date (NAD)
//===========================================================================
inline ccruncher::Date::Date()
{
  lJulianDay = nad[0];
}

//===========================================================================
// asignation operator
//===========================================================================
inline Date& ccruncher::Date::operator=( const Date& Orig )
{
    lJulianDay = Orig.lJulianDay;
    return *this;
}

//===========================================================================
// adds the number of days in the integral type to the Date
//===========================================================================
inline Date ccruncher::operator+( const Date& Left, const long Right )
{
    Date Temp = Left;
    Temp.lJulianDay += Right;
    return Temp;
}
inline Date ccruncher::operator+( const long Left, const Date& Right )
{
    Date Temp = Right;
    Temp.lJulianDay += Left;
    return Temp;
}
inline Date& ccruncher::Date::operator+=( const long Right )
{
    lJulianDay += Right;
    return *this;
}
inline Date& ccruncher::Date::operator++()
{
    lJulianDay++;
    return *this;
}
inline Date ccruncher::Date::operator++( int )
{
    Date Temp = *this;
    lJulianDay++;
    return Temp;
}

//===========================================================================
// subtracts two dates yields the difference between them in days.
//===========================================================================
inline Date ccruncher::operator-( const Date& Left, const long Right )
{
    Date Temp = Left;
    Temp.lJulianDay -= Right;
    return Temp;
}
inline Date ccruncher::operator-( const long Left, const Date& Right )
{
    Date Temp = Right;
    Temp.lJulianDay -= Left;
    return Temp;
}
inline Date& ccruncher::Date::operator-=( const long Right )
{
    lJulianDay -= Right;
    return *this;
}
inline long ccruncher::Date::operator-( const Date& Right ) const
{
    return lJulianDay - Right.lJulianDay;
}
inline Date& ccruncher::Date::operator--()
{
    lJulianDay--;
    return *this;
}
inline Date ccruncher::Date::operator--( int )
{
    Date Temp = *this;
    lJulianDay--;
    return Temp;
}

//===========================================================================
// comparison operators
// non-zero if the comparison succeeds 0 if it fails
//===========================================================================
inline bool ccruncher::Date::operator==( const Date& Right ) const
{
    return lJulianDay == Right.lJulianDay;
}
inline bool ccruncher::Date::operator!=( const Date& Right ) const
{
    return lJulianDay != Right.lJulianDay;
}
inline bool ccruncher::Date::operator<( const Date& Right ) const
{
    return lJulianDay < Right.lJulianDay;
}
inline bool ccruncher::Date::operator<=( const Date& Right ) const
{
    return lJulianDay <= Right.lJulianDay;
}
inline bool ccruncher::Date::operator>( const Date& Right ) const
{
    return lJulianDay > Right.lJulianDay;
}
inline bool ccruncher::Date::operator>=( const Date& Right ) const
{
    return lJulianDay >= Right.lJulianDay;
}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

