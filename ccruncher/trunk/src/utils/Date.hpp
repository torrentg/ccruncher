
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
// 01/08/2007 - Gerard Torrent
//   . code set in ccruncher form
//   . added required ccruncher methods and operators
//
//===========================================================================

#ifndef _Date_
#define _Date_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <time.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Date
{

  private:

    long lJulianDay;

    static long YmdToJd(const int iYear, const int iMonth, const int iDay);
    static void JdToYmd(const long lJD, int *piYear, int *piMonth, int *piDay);


  public:

    Date();
    Date(const Date& Orig);
    Date(const int iDay, const int iMonth, const int iYear) throw(Exception);
    explicit Date(const time_t tSysTime);
    Date(const string &str) throw(Exception);

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

    Date& operator=( const Date& Orig );
    friend Date operator+( const Date& Left, const long Right );
    friend Date operator+( const long Left, const Date& Right );
    Date& operator+=( const long Right );
    Date& operator++();
    Date operator++( int );
    friend Date operator-( const Date& Left, const long Right );
    friend Date operator-( const long Left, const Date& Right );
    Date& operator-=( const long Right );
    long operator-( const Date& Right );
    Date& operator--();
    Date operator--( int );

    int operator==( const Date& Right ) const;
    int operator!=( const Date& Right ) const;
    int operator<( const Date& Right ) const;
    int operator<=( const Date& Right ) const;
    int operator>( const Date& Right ) const;
    int operator>=( const Date& Right ) const;

};

//---------------------------------------------------------------------------

// output operator
ostream& operator << (ostream& os, const Date& d);
// increment n number of months
Date addMonths(const Date& d, const int& no_months);
// minimum of 2 dates
Date min(const Date&, const Date&);
// maximum of 2 dates
Date max(const Date&, const Date&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

