
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
// 07/02/2009 - Gerard Torrent
//   . added method getMonthsTo()
//
//===========================================================================

#include <cstdio>
#include <cstring>
#include <ctime>
#include "utils/Date.hpp"
#include "utils/Parser.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// Constructor
//===========================================================================
ccruncher::Date::Date(const int iDay, const int iMonth, const int iYear) throw(Exception)
{
  if (!valid(iDay, iMonth, iYear))
  {
    char buf[150];
    sprintf(buf, "invalid Date: %i/%i/%i", iDay, iMonth, iYear);
    throw Exception(buf);
  }

  lJulianDay = YmdToJd( iYear, iMonth, iDay );
}

//===========================================================================
// Constructor. Constructs an object initialized to the date
// represented by a system time value.
// example: Date date(time(NULL));
//===========================================================================
ccruncher::Date::Date( const time_t tSysTime )
{
  struct tm *ptm;
  int y, m, d;

  ptm = localtime( &tSysTime );
  y = ptm->tm_year + 1900;
  m = ptm->tm_mon + 1;
  d = ptm->tm_mday;
  lJulianDay = YmdToJd( y, m, d );
}

//===========================================================================
// Constructor from string (format string = dd/mm/yyyy)
//===========================================================================
ccruncher::Date::Date(const string &str) throw(Exception)
{
  parse(str.c_str());
}

//===========================================================================
// Constructor from string (format string = dd/mm/yyyy)
//===========================================================================
ccruncher::Date::Date(const char *str) throw(Exception)
{
  parse(str);
}

//===========================================================================
// Constructor from string (format string = dd/mm/yyyy)
//===========================================================================
void ccruncher::Date::parse(const char *str) throw(Exception)
{
  int d, m, y;
  char buf[10];

  int l = strlen(str);
  if (l < 8 || 10 < l)
  {
    throw Exception("invalid date: " + string(str) + " (non valid format)");
  }

  int rc = sscanf(str, "%d/%d/%d%s", &d, &m, &y, buf);
  if (rc != 3)
  {
    throw Exception("invalid date: " + string(str) + " (non valid format)");
  }

  if (!valid(d, m, y))
  {
    throw Exception("invalid Date: " + string(str) + " (non valid date)");
  }
  else
  {
    lJulianDay = YmdToJd(y, m, d);
  }
}

//===========================================================================
// YmdToJd
// Internal routine that does the physical conversion 
// from YMD form to Julian day number.
//===========================================================================
long ccruncher::Date::YmdToJd( const int iYear, const int iMonth, const int iDay )
{
    long jul_day;

#ifndef JULDATE_USE_ALTERNATE_METHOD

    int a,b;
    int year = iYear, month = iMonth, day = iDay;
    float year_corr;

    if ( year < 0 )
        year++;
    year_corr = ( year > 0 ? 0.0f : 0.75f );
    if ( month <= 2 )
    {
        year--;
        month += 12;
    }
    b = 0;
    if ( year * 10000.0 + month * 100.0 + day >= 15821015.0 )
    {
        a = year / 100;
        b = 2 - a + a / 4;
    }
    jul_day = (long) ( 365.25 * year - year_corr ) +
              (long) ( 30.6001 * ( month + 1 ) ) + day + 1720995L + b;

#else

    long lmonth = (long) iMonth, lday = (long) iDay, lyear = (long) iYear;

    // Adjust BC years
    if ( lyear < 0 )
        lyear++;

    jul_day = lday - 32075L +
        1461L * ( lyear + 4800L + ( lmonth - 14L ) / 12L ) / 4L +
        367L * ( lmonth - 2L - ( lmonth - 14L ) / 12L * 12L ) / 12L -
        3L * ( ( lyear + 4900L + ( lmonth - 14L ) / 12L ) / 100L ) / 4L;

#endif

    return jul_day;
}

//===========================================================================
// JdToYmd
// Internal routine that reverses the conversion, turning a Julian
// day number into YMD values.
//===========================================================================
void ccruncher::Date::JdToYmd( const long lJD, int *piYear, int *piMonth, int *piDay )
{
#ifndef JULDATE_USE_ALTERNATE_METHOD

    long a, b, c, d, e, z, alpha;

    z = lJD;
    if ( z < 2299161L )
        a = z;
    else
    {
        alpha = (long) ( ( z - 1867216.25 ) / 36524.25 );
        a = z + 1 + alpha - alpha / 4;
    }
    b = a + 1524;
    c = (long) ( ( b - 122.1 ) / 365.25 );
    d = (long) ( 365.25 * c );
    e = (long) ( ( b - d ) / 30.6001 );
    *piDay = (int) b - d - (long) ( 30.6001 * e );
    *piMonth = (int) ( e < 13.5 ) ? e - 1 : e - 13;
    *piYear = (int) ( *piMonth > 2.5 ) ? ( c - 4716 ) : c - 4715;
    if ( *piYear <= 0 )
        *piYear -= 1;

#else

    long t1, t2, yr, mo;

    t1 = lJD + 68569L;
    t2 = 4L * t1 / 146097L;
    t1 = t1 - ( 146097L * t2 + 3L ) / 4L;
    yr = 4000L * ( t1 + 1L ) / 1461001L;
    t1 = t1 - 1461L * yr / 4L + 31L;
    mo = 80L * t1 / 2447L;
    *piDay = (int) ( t1 - 2447L * mo / 80L );
    t1 = mo / 11L;
    *piMonth = (int) ( mo + 2L - 12L * t1 );
    *piYear = (int) ( 100L * ( t2 - 49L ) + yr + t1 );

    // Correct for BC years
    if ( *piYear <= 0 )
        *piYear -= 1;

#endif

    return;
}

//===========================================================================
// getters for day, month and year
//===========================================================================
int ccruncher::Date::getYear( void ) const
{
    int y, m, d;

    JdToYmd( lJulianDay, &y, &m, &d );
    return y;
}
int ccruncher::Date::getMonth( void ) const
{
    int y, m, d;

    JdToYmd( lJulianDay, &y, &m, &d );
    return m;
}
int ccruncher::Date::getDay( void ) const
{
    int y, m, d;

    JdToYmd( lJulianDay, &y, &m, &d );
    return d;
}

//===========================================================================
// DayOfYear
// returns the day of the year, with 1 Jan being day 1
//===========================================================================
int ccruncher::Date::getDayOfYear( void ) const
{
    int y, m, d;
    long soy;

    JdToYmd( lJulianDay, &y, &m, &d );
    soy = YmdToJd( y, 1, 1 );
    return (int) ( lJulianDay - soy + 1 );
}

//===========================================================================
// DayOfWeek
// return the day of the week, from 0 through 6
// 0 = Sunday, ...,  6 = Saturday
//===========================================================================
int ccruncher::Date::getDayOfWeek( void ) const
{
    return ( ( ( (int) ( lJulianDay % 7L ) ) + 1 ) % 7 );
}

//===========================================================================
// DayOfWorkWeek
// return the day of the week, from 0 through 6
// 0 = Monday, ..., 6 = Sunday
//===========================================================================
int ccruncher::Date::getDayOfWorkWeek( void ) const
{
    return ( (int) ( lJulianDay % 7L ) );
}

//===========================================================================
// isLeapYear
//===========================================================================
bool ccruncher::Date::isLeapYear( const int iYear )
{
    long jd1, jd2;
    jd1 = YmdToJd( iYear, 2, 28 );
    jd2 = YmdToJd( iYear, 3, 1 );
    return ( ( jd2 - jd1 ) > 1 );
}

//===========================================================================
// isLeapYear
//===========================================================================
bool ccruncher::Date::isLeapYear( void ) const
{
    int y, m, d;
    JdToYmd( lJulianDay, &y, &m, &d );
    return isLeapYear( y );
}

//===========================================================================
// YMD() puts theyear, month and day values directly into three integer 
// variables, for times when you need all three at the same time.
//===========================================================================
void ccruncher::Date::YMD( int *pY, int *pM, int *pD ) const
{
    JdToYmd( lJulianDay, pY, pM, pD );
    return;
}

//===========================================================================
// Converts the date to a time_t value
// representing midnight of that date.
//===========================================================================
time_t ccruncher::Date::ToSysTime( void ) const
{
    struct tm tmRep;
    int y, m, d;
    time_t t;
    
    JdToYmd( lJulianDay, &y, &m, &d );
    if ( y < 1970 )
    {
        y = 70;
        m = 1;
        d = 1;
    }
    tmRep.tm_year = y - 1900 ;
    tmRep.tm_mon = m-1;
    tmRep.tm_mday = d;
    tmRep.tm_hour = 0;
    tmRep.tm_min = 0;
    tmRep.tm_sec = 0;
    tmRep.tm_isdst = 0;
    
    t = mktime( &tmRep );
    return t;
}

//===========================================================================
// toString
//===========================================================================
string ccruncher::Date::toString() const
{
  int y, m, d;
  char aux[] = "dd/mm/yyyy ";
  aux[10] = 0;
  JdToYmd( lJulianDay, &y, &m, &d );
  sprintf(aux, "%02d/%02d/%04d", d, m, y);
  return string(aux);
}

//===========================================================================
// returns number of days in month
//===========================================================================
int ccruncher::Date::numDaysInMonth(int m, int y)
{
  if (m==4 || m==6 || m==9 || m==11)
  {
    return 30;
  }
  else if (m == 2)
  {
    if (Date::isLeapYear(y))
    {
      return 29;
    }
    else
    {
      return 28;
    }
  }
  else
  {
    return 31;
  }
}

//===========================================================================
// returns number of days in year
//===========================================================================
int ccruncher::Date::numDaysInYear(int y)
{
  if (isLeapYear(y))
  {
    return 366;
  }
  else
  {
    return 365;
  }
}

//===========================================================================
// output operator
//===========================================================================
ostream & ccruncher::operator << (ostream& os, const Date& d)
{
  os << d.toString();
  return os;
}

//===========================================================================
// increment n number of months
//===========================================================================
Date ccruncher::addMonths(const Date& inDate, const int& num_months)
{
  int y1, m1, d1;
  inDate.YMD( &y1, &m1, &d1 );

  if (num_months == 0) {
    return inDate;
  }

  int num_years = num_months/12;
  if (num_years != 0) {
    y1 += num_years;
  }

  int nmonths = num_months%12;
  if (nmonths == 0) {
    return Date(d1, m1, y1);
  }

  if (num_months > 0) {
    if (m1+nmonths<=12) {
      int nd = std::min(d1, Date::numDaysInMonth(m1+nmonths, y1));
      return Date(nd, m1+nmonths, y1);
    }
    else {
      int nd = std::min(d1, Date::numDaysInMonth(m1+nmonths-12, y1+1));
      return Date(nd, m1+nmonths-12, y1+1);
    }
  }
  else {
    if (m1+nmonths>=1) {
      int nd = std::min(d1, Date::numDaysInMonth(m1+nmonths, y1));
      return Date(nd, m1+nmonths, y1);
    }
    else {
      int nd = std::min(d1, Date::numDaysInMonth(m1+nmonths+12, y1-1));
      return Date(nd, m1+nmonths+12, y1-1);
    }
  }

  assert(false);
  return NAD;
}

//===========================================================================
// minimum of 2 dates
//===========================================================================
Date ccruncher::min(const Date &d1, const Date &d2)
{
  if (d1 < d2)
  {
    return d1;
  }
  else 
  {
    return d2;
  }
}

//===========================================================================
// maximum of 2 dates
//===========================================================================
Date ccruncher::max(const Date &d1, const Date &d2)
{
  if (d1 < d2)
  {
    return d2;
  }
  else 
  {
    return d1;
  }
}

//===========================================================================
// check if is a valid date
//===========================================================================
bool ccruncher::Date::valid(int d, int m, int y)
{
  if (y<0)
  {
    return false;
  }
  else if (m>12 || m<1)
  {
    return false;
  }
  else if (d < 1 || d > numDaysInMonth(m,y))
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================
// returns the number of months between this date and d
// fractional part are the remaining days / 30
//===========================================================================
double ccruncher::Date::getMonthsTo(const Date &d) const
{
  if (d < *this) {
    return -d.getMonthsTo(*this);
  }
  else {
    int y1, m1, d1;
    int y2, m2, d2;
    double ret = 0.0;
    JdToYmd( lJulianDay, &y1, &m1, &d1 );
    JdToYmd( d.lJulianDay, &y2, &m2, &d2 );
    ret += 12.0 * (y2 - y1);
    ret += 1.0 * (m2 - m1);
    ret += (d2 - d1) / 30.0;
    return ret;
  }
}


//===========================================================================
// Indicates if str is a interval
// true if str is interval, false if not
//===========================================================================
bool ccruncher::isInterval(const char *str) 
{ 
    if (!str)
    return false;
    size_t l = strlen(str);
    if (( str[l-1]!='D') && ( str[l-1]!='M') && ( str[l-1]!='Y'))
        return false;
    if ((str[0]!='+') && ( str[0]!='-') && ( (str[0]<'0') || ( str[0]>'9')))
        return false;
    for (size_t i=1;i<l-1;i++)
    {
        if (( str[i]<'0') || ( str[i]>'9'))
        return false;
    }
    return true;
}

//===========================================================================
// Increments the date in an interval periorde.
// Ejemp: Date + 450D, Date + 24M, Date + 5Y (days, months and years)
//===========================================================================
void ccruncher::Date::addIncrement(const char *str) throw(Exception)
{
    int y,m,d;
    int interval;
    char buffer[25];
    int l = strlen(str);
    if (l >= 25) 
      throw Exception("increment too long");
    strcpy(buffer,str);
    buffer[l-1] = '\0';
    interval = atoi(buffer);
    if (interval!=0) 
    {
      if (str[l-1]=='D') { // increment in days. Ejem 365D
        lJulianDay += interval;
      }
      else if (str[l-1]=='M') { // increment in Months. Ejem 3M
        JdToYmd(lJulianDay,&y,&m,&d);
        *this = addMonths(*this, interval);
      }
      else if (str[l-1]=='Y') { // increment in Years. Ejem 5Y
        JdToYmd(lJulianDay,&y,&m,&d);
        if (valid(d,m,y+interval)) lJulianDay = YmdToJd(y+interval,m,d);
        else lJulianDay = YmdToJd(y+interval,m,d-1); //29-Feb case
      }
      else {
        throw Exception("invalid increment");
      }
    }
    else
      throw Exception("invalid conversion to int from interval date: " + string(str) + " (non valid format)");
}

