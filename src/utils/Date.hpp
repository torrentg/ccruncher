
//===========================================================================
//
// $Id: Date.h,v 1.3 2002/10/18 05:56:10 tknarr Exp $
// Copyright (C) 2000, 2002
// Todd T. Knarr <tknarr@silverglass.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//===========================================================================

#ifndef _Date_
#define _Date_

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Specific instant in time with day precision.
 *
 * @details Provides a Date class which represents dates as
 *          Julian day numbers ( days since 1 Jan 4713 BC ).
 *          This class can handle all dates from  1 Jan 4713BC to
 *          31 Dec 4999AD.
 *
 * @author  Todd T. Knarr (09/11/1995, initial release).
 * @author  Gerard Torrent (minor modifications and updates)
 *
 * @version 1.3 2002-10-18, Todd T. Knarr
 * @version 2.0 2011-08-16, Todd T. Knarr
 * @version ccruncher, Gerard Torrent
 *
 * @see     http://www.silverglass.org/code/date-class
 *
 * @copyright GNU Public License.
 *
 * @note    Years AD are positive, years BC are negative. There is
 *          no year 0AD, it goes from 1BC to 1AD. A year of 0 will be
 *          treated as 1BC. No attempt is made to validate ranges. Physical
 *          errors will not result from insane day-month combinations like
 *          the 87th day of the 45th month, but the results will obviously
 *          not make much sense.
 *
 * @note    Date conversion routines by:
 *              Eric bergman-Terrell, Computer Language, Dec 1990.
 *          Alternate method by:
 *              David Burki, CUJ Feb 1993.
 *
 * @note    The Date class is used extensively in the CCruncher code
 *          (instantiation, memory space, and methods invocation).
 */
class Date
{

  private:

    //! Date as julian day
    long lJulianDay;


  private:

    //! Converts a year-month-day to julian day
    static long YmdToJd(const int year, const int month, const int day);
    //! Converts a julian day to year-month-day
    static void JdToYmd(const long lJD, int *piYear, int *piMonth, int *piDay);

    //! Parse a date in format 'dd/MM/yyyy'
    void parse(const char *);


  public:

    //! Default constructor
    constexpr Date();
    //! Constructor
    Date(const int day, const int month, const int year);
    //! Constructor
    explicit Date(const time_t tSysTime);
    //! Constructor
    explicit Date(const std::string &str);
    //! Constructor
    explicit Date(const char *str);

    //! Checks if the given day-month-year is a valid date
    static bool valid(const int d, const int m, const int y);
    //! Returns the number of days in the given month-year
    static int numDaysInMonth(int m, int y);
    //! Returns the number of days in the given year
    static int numDaysInYear(int);
    //! Check if the given year is a leap year
    static bool isLeapYear(const int year);

    //! Format Date as 'dd/MM/yyyy'
    std::string toString() const;
    //! Returns current Date in time_t format
    time_t toSysTime( void ) const;
    //! Returns the Date year
    int getYear( void ) const;
    //! Returns the Date month
    int getMonth( void ) const;
    //! Returns the Date day
    int getDay( void ) const;
    //! Returns the day of year (1...366)
    int getDayOfYear( void ) const;
    //! Returns the day of week (0...6)
    int getDayOfWeek( void ) const;
    //! Returns the day of week (0...6)
    int getDayOfWorkWeek( void ) const;
    //! Checks if date has a leap year
    bool isLeapYear( void ) const;
    //! Returns the year, month and day
    void YMD( int *pY, int *pM, int *pD ) const;
    //! Increments the current date x days/months/years
    void add(const char *str);

    //! Assignment operator
    Date& operator=( const Date& date );
    //! Addition operator
    friend Date operator+( const Date& date, const long num );
    //! Addition operator
    friend Date operator+( const long num, const Date& date );
    //! Addition assignment operator
    Date& operator+=( const long num );
    //! Increment prefix operator
    Date& operator++();
    //! Increment postfix operator
    Date operator++( int );
    //! Subtraction operator
    friend Date operator-( const Date& date, const long num );
    //! Subtraction operator
    friend Date operator-( const long num, const Date& date );
    //! Subtraction assignment operator
    Date& operator-=( const long num );
    //! Subtraction operator
    long operator-( const Date& date ) const;
    //! Decrement prefix operator
    Date& operator--();
    //! Decrement postfix operator
    Date operator--( int );
    //! Equal-to operator
    bool operator==( const Date& date ) const;
    //! Not-equal-to operator
    bool operator!=( const Date& date ) const;
    //! Less-than operator
    bool operator<( const Date& date ) const;
    //! Less-than-or-equal-to operator
    bool operator<=( const Date& date ) const;
    //! Greater-than operator
    bool operator>( const Date& date ) const;
    //! Greater-than-or-equal-to operator
    bool operator>=( const Date& date ) const;

};

//! Prints current date to an output stream
std::ostream& operator << (std::ostream& os, const Date& d);
//! Increments n number of days/months/years
Date add(const Date &d, int num, char units);
//! Difference between 2 dates in days/months/years
double diff(const Date &d1, const Date &d2, char units);
//! Minimum of 2 dates
Date min(const Date&, const Date&);
//! Maximum of 2 dates
Date max(const Date&, const Date&);
//! Checks if given string is a valid interval
bool isInterval(const char *str);

/**************************************************************************//**
 * @details Create a new object with a non-valid date (NAD). Current
 *          timestamp is not used due to performance reasons.
 */
inline constexpr ccruncher::Date::Date() : lJulianDay(1721026L)
{
    // nothing to do
}

/**************************************************************************//**
 * @details NAD means Not-A-Date value. Use it as a macro.
 *          NAD = 0/0/0 = Date(0,0,0) = Date()
 */
constexpr Date NAD;

/**************************************************************************//**
 * @param[in] date Date to assign.
 * @return Reference to current Date.
 */
inline Date& ccruncher::Date::operator=( const Date& date )
{
    lJulianDay = date.lJulianDay;
    return *this;
}

/**************************************************************************//**
 * @details Adds the number of days in the integral type to the Date.
 * @param[in] date Starting date.
 * @param[in] num Number of days to be added.
 * @return date + num days.
 */
inline Date operator+( const Date& date, const long num )
{
    Date temp = date;
    temp.lJulianDay += num;
    return temp;
}

/**************************************************************************//**
 * @details Adds the number of days in the integral type to the Date.
 * @param[in] num Number of days to be added.
 * @param[in] date Starting date.
 * @return date + num days.
 */
inline Date operator+( const long num, const Date& date )
{
    Date temp = date;
    temp.lJulianDay += num;
    return temp;
}

/**************************************************************************//**
 * @details Adds the given number of days to the Date.
 * @param[in] num Number of days to be added.
 * @return Reference to the current Date.
 */
inline Date& ccruncher::Date::operator+=( const long num )
{
    lJulianDay += num;
    return *this;
}

/**************************************************************************//**
 * @details Adds 1 day to the Date.
 * @return Reference to the current Date.
 */
inline Date& ccruncher::Date::operator++()
{
    lJulianDay++;
    return *this;
}

/**************************************************************************//**
 * @details Adds 1 day to the Date.
 * @return current date previous to increase.
 */
inline Date ccruncher::Date::operator++( int )
{
    Date temp = *this;
    lJulianDay++;
    return temp;
}

/**************************************************************************//**
 * @details Subtracts the number of days in the integral type to the Date.
 * @param[in] date Starting date.
 * @param[in] num Number of days to be subtracted.
 * @return date - num days.
 */
inline Date operator-( const Date& date, const long num )
{
    Date temp = date;
    temp.lJulianDay -= num;
    return temp;
}

/**************************************************************************//**
 * @details Subtracts the number of days in the integral type to the Date.
 * @param[in] num Number of days to be subtracted.
 * @param[in] date Starting date.
 * @return date - num days.
 */
inline Date operator-( const long num, const Date& date )
{
    Date temp = date;
    temp.lJulianDay -= num;
    return temp;
}

/**************************************************************************//**
 * @details Subtracts the given number of days to the Date.
 * @param[in] num Number of days to be subtracted.
 * @return Reference to the current Date.
 */
inline Date& ccruncher::Date::operator-=( const long num )
{
    lJulianDay -= num;
    return *this;
}

/**************************************************************************//**
 * @details Subtracts two dates yields the difference between them in days.
 * @param[in] date Date to subtract.
 * @return The difference between dates in days.
 */
inline long ccruncher::Date::operator-( const Date& date ) const
{
    return lJulianDay - date.lJulianDay;
}

/**************************************************************************//**
 * @details Subtracts 1 day to the Date.
 * @return Reference to the current Date (decreased).
 */
inline Date& ccruncher::Date::operator--()
{
    lJulianDay--;
    return *this;
}

/**************************************************************************//**
 * @details Subtracts 1 day to the Date.
 * @return current date previous to decrease.
 */
inline Date ccruncher::Date::operator--( int )
{
    Date temp = *this;
    lJulianDay--;
    return temp;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator==( const Date& date ) const
{
    return lJulianDay == date.lJulianDay;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator!=( const Date& date ) const
{
    return lJulianDay != date.lJulianDay;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator<( const Date& date ) const
{
    return lJulianDay < date.lJulianDay;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator<=( const Date& date ) const
{
    return lJulianDay <= date.lJulianDay;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator>( const Date& date ) const
{
    return lJulianDay > date.lJulianDay;
}

/**************************************************************************//**
 * @param date Date to compare.
 * @return Non-zero if the comparison succeeds 0 if it fails.
 */
inline bool ccruncher::Date::operator>=( const Date& date ) const
{
    return lJulianDay >= date.lJulianDay;
}

} // namespace

#endif

