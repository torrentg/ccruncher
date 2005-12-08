
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
// Date.cpp - Date code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release based on a Bernt A Oedegaard class
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/07/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . converting to inline some critical methods
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added assert support
//   . added getCentury() methods
//   . added getDayOfWeek() method
//
//===========================================================================

#include <vector>
#include <ctime>
#include <cstdio>
#include <cmath>
#include "utils/Date.hpp"
#include "utils/Strings.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Date::Date()
{
  time_t now = time(NULL);
  tm lt = *(localtime(&now));

  year_ = lt.tm_year + 1900;
  month_ = (char) (lt.tm_mon + 1);
  day_ = (char) lt.tm_mday;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Date::Date(int d, int m, int y) throw(Exception)
{
  day_ = (char) d;
  month_ = (char) m;
  year_ = y;

  if (!valid())
  {
    throw Exception("Date::Date(int,int,int): invalid Date");
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Date::Date(const string &str) throw(Exception)
{
  vector<string> tokens;
  Strings::tokenize(str, tokens, "/");

  if (tokens.size() != 3)
  {
    throw Exception("Date::Date(str): num tokens distinct 3");
  }

  try
  {
    day_ = (char) Parser::intValue(tokens[0]);
    month_ = (char) Parser::intValue(tokens[1]);
    year_ = Parser::intValue(tokens[2]);
  }
  catch(Exception &e)
  {
    throw Exception(e, "Date::Date(str): invalid day or month or year");
  }

  if (!valid())
  {
    throw Exception("Date::Date(str): invalid Date");
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Date::Date(const long date) throw(Exception)
{
  year_  = date/10000;
  month_ = (char) ((date%10000)/100);
  day_   = (char) (date%100);

  if (!valid())
  {
    throw Exception("Date::Date(long): invalid Date (" + Format::long2string(date) + ")");
  }
}

//===========================================================================
// setXXX() methods
//===========================================================================
void ccruncher::Date::setDay (const int day) throw(Exception)
{
  Date::day_ = (char) day;

  if (!valid())
  {
    throw Exception("Date::setDay(): invalid Date");
  }
}

void ccruncher::Date::setMonth(const int month) throw(Exception)
{
  Date::month_ = (char) month;

  if (!valid())
  {
    throw Exception("Date::setMonth(): invalid Date");
  }
}

void ccruncher::Date::setYear (const int year) throw(Exception)
{
  Date::year_ = year;

  if (!valid())
  {
    throw Exception("Date::setYear(): invalid Date");
  }
}

//===========================================================================
// returns century
//===========================================================================
int ccruncher::Date::getCentury(const int y)
{
  return y - (y % 100);
}

//===========================================================================
// returns century
//===========================================================================
int ccruncher::Date::getCentury() const
{
  return getCentury(getYear());
}

//===========================================================================
// returns day of year (1...364/365)
//===========================================================================
int ccruncher::Date::getDayOfYear() const
{
  int ret = 0;

  for (int i=1;i<month_;i++)
  {
    ret += numDaysInMonth(i, getYear());
  }

  return ret + (int) day_;
}

//===========================================================================
// returns day of week
// 0=sunday, 1=monday, 2=tuesday, 3=wednesday, 4=thursday, 5=friday, 6=saturday
// implements Doomsday rule
// http://www.cst.cmich.edu/users/graha1sw/Pub/Doomsday/Doomsday.html
//===========================================================================
int ccruncher::Date::getDayOfWeek() const
{
  int x, y, aux;
  int ddm = getDoomsDayMonth(getMonth(), getYear());
  int ddc = getDoomsDayCentury(getYear());

  if( ddc < 0 ) return -1;
  if( ddm < 0 ) return -1;

  if( ddm > getDay() )
  {
    aux = (7 - ((ddm-getDay()) % 7 ) + ddm);
  }
  else
  {
    aux = getDay();
  }

  x = (aux - ddm) % 7;
  x %= 7;
  y = ddc + (getYear()-getCentury()) + (floor((getYear()-getCentury())/4));
  y %= 7;

  return (x+y)%7;
}

//===========================================================================
// returns dooms day month (internal function required by getDayOfWeek)
//===========================================================================
int ccruncher::Date::getDoomsDayMonth(int m, int y)
{
  switch(m)
  {
    case 1: //january
      if( isLeapYear(y) ) return 32;
      else return 31;
    case 2: //february
      if( isLeapYear(y) ) return 29;
      else return 28;
    case 3: //march
      return 7;
    case 4: //april
      return 4;
    case 5: //may
      return 9;
    case 6: //june
      return 6;
    case 7: //july
      return 11;
    case 8: //august
      return 8;
    case 9: //september
      return 5;
    case 10: //october
      return 10;
    case 11: //november
      return 7;
    case 12: //december
      return 12;
    default:
      assert(false);
  }
}

//===========================================================================
// returns dooms day century (internal function required by getDayOfWeek)
//===========================================================================
int ccruncher::Date::getDoomsDayCentury(int y)
{
  switch( getCentury(y) % 400 )
  {
    case 0:
      return 2;
    case 100:
      return 0;
    case 200:
      return 5;
    case 300:
      return 3;
    default:
      assert(false);
  }
}

//===========================================================================
// check if given year is a leap year
//===========================================================================
bool ccruncher::Date::isLeapYear(int y)
{
  if (y%4!=0 || y%100==0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================
// check if given year is a leap year
//===========================================================================
bool ccruncher::Date::isLeapYear()
{
  return isLeapYear(getYear());
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
// returns number of days in year
//===========================================================================
int ccruncher::Date::numDaysInYear()
{
  return numDaysInYear(getYear());
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
    if (isLeapYear(y))
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
// returns number of days in month
//===========================================================================
int ccruncher::Date::numDaysInMonth()
{
  return numDaysInMonth(getMonth(), getYear());
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
// check if is a valid date
//===========================================================================
bool ccruncher::Date::valid() const
{
  return valid(getDay(), getMonth(), getYear());
}

//===========================================================================
// postfix operator
//===========================================================================
Date ccruncher::Date::operator ++(int)
{
  Date d = *this;
  *this = nextDate(d);
  return d;
}

//===========================================================================
// prefix operator
//===========================================================================
Date ccruncher::Date::operator ++()
{
  *this = nextDate(*this);
  return *this;
}

//===========================================================================
// postfix operator, return current value
//===========================================================================
Date ccruncher::Date::operator --(int)
{
  Date d = *this;
  *this = previousDate(*this);
  return d;
}

//===========================================================================
// prefix operator, return new value
//===========================================================================
Date ccruncher::Date::operator --()
{
  *this = previousDate(*this);
  return *this;
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator == (const Date& d1, const Date& d2)
{
  if ((d1.getDay()==d2.getDay()) && (d1.getMonth()==d2.getMonth()) && (d1.getYear()==d2.getYear()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator !=(const Date& d1, const Date& d2)
{
  return !(d1==d2);
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator < (const Date& d1, const Date& d2)
{
  if (d1.getYear()<d2.getYear())
  {
    return true;
  }
  else if (d1.getYear()>d2.getYear())
  {
    return false;
  }
  else
  {
    if (d1.getMonth()<d2.getMonth())
    {
      return true;
    }
    else if (d1.getMonth()>d2.getMonth())
    {
      return false;
    }
    else
    {
      if (d1.getDay()<d2.getDay())
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator > (const Date& d1, const Date& d2)
{
  if (d1==d2)
  {
    return false;
  }
  else if (d1<d2)
  {
    return false;
  }
  else
  {
    return true;
  }
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator <=(const Date& d1, const Date& d2)
{
  if (d1==d2)
  {
    return true;
  }
  else
  {
    return (d1<d2);
  }
}

//===========================================================================
// comparation operator
//===========================================================================
bool ccruncher::operator >=(const Date& d1, const Date& d2)
{
  if (d1==d2)
  {
    return true;
  }
  else
  {
    return (d1>d2);
  }
}

//===========================================================================
// roll-up 1 day the Date
//===========================================================================
Date ccruncher::nextDate(const Date& d)
{
  if (d.getDay()+1 <= Date::numDaysInMonth(d.getMonth(), d.getYear()))
  {
    return Date(d.getDay()+1, d.getMonth(), d.getYear());
  }
  else if (d.getMonth()+1 <= 12)
  {
    return Date(1, d.getMonth()+1, d.getYear());
  }
  else
  {
    return Date(1, 1, d.getYear()+1);
  }
}

//===========================================================================
// roll-down 1 day the Date
//===========================================================================
Date ccruncher::previousDate(const Date& d)
{
  if (d.getDay() > 1)
  {
    return Date(d.getDay()-1, d.getMonth(), d.getYear());
  }
  else if (d.getMonth() > 1)
  {
    int nd = Date::numDaysInMonth(d.getMonth()-1, d.getYear());
    return Date(nd, d.getMonth()-1, d.getYear());
  }
  else
  {
    return Date(31, 12, d.getYear()-1);
  }
}

//===========================================================================
// return long with yyyymmdd
//===========================================================================
long ccruncher::Date::longDate()
{
  return getYear() * 10000 + getMonth() * 100 + getDay();
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
// increment n number of days
//===========================================================================
Date ccruncher::operator + (const Date& d, const int& days)
{
  if (days<0)
  {
    return (d-(-days));
  }
  else
  {
    Date ret = d;

    for (int day=1;day<=days;++day)
    {
      ret = ccruncher::nextDate(ret);
    }

    return ret;
  }
}

//===========================================================================
// increment n number of days
//===========================================================================
Date ccruncher::operator += (Date& d, const int& days)
{
  d = (d+days);
  return d;
}

//===========================================================================
// decrement n number of days
//===========================================================================
Date ccruncher::operator - (const Date& d, const int& days)
{
  if (days<0)
  {
    return (d+(-days));
  }
  else
  {
    Date ret=d;

    for (int day=0; day<days; ++day)
    {
      ret = ccruncher::previousDate(ret);
    }

    return ret;
  }
}

//===========================================================================
// decrement n number of days
//===========================================================================
Date ccruncher::operator -= (Date& d, const int& days)
{
  d = (d-days);
  return d;
}

//===========================================================================
// increment n number of months
//===========================================================================
Date ccruncher::addMonths(const Date& inDate, const int& no_months)
{
  Date d=inDate;

  if (no_months == 0)
  {
    return inDate;
  }

  int no_years = no_months/12;

  if (no_years != 0)
  {
    d.setYear(inDate.getYear()+no_years);
  }

  int nmonths = no_months%12;

  if (nmonths == 0)
  {
    return d;
  }

  if (no_months > 0)
  {
    if (d.getMonth()+nmonths<=12)
    {
      int nd = min(d.getDay(), Date::numDaysInMonth(d.getMonth()+nmonths,d.getYear()));
      return Date(nd, d.getMonth()+nmonths,d.getYear());
    }
    else
    {
      int nd = min(d.getDay(), Date::numDaysInMonth(d.getMonth()+nmonths-12,d.getYear()+1));
      return Date(nd, d.getMonth()+nmonths-12,d.getYear()+1);
    }
  }
  else
  {
    if (d.getMonth()+nmonths>=1)
    {
      int nd = min(d.getDay(), Date::numDaysInMonth(d.getMonth()+nmonths,d.getYear()));
      return Date(nd, d.getMonth()+nmonths,d.getYear());
    }
    else
    {
      int nd = min(d.getDay(), Date::numDaysInMonth(d.getMonth()+nmonths+12,d.getYear()-1));
      return Date(nd, d.getMonth()+nmonths+12,d.getYear()-1);
    }
  }

  cerr << "Date::AddMonths():  error!" << endl;
  return inDate;  // something wroong if arrive here
}

//===========================================================================
// increment 1 month
//===========================================================================
Date ccruncher::nextMonth(const Date& d)
{
  return ccruncher::addMonths(d, 1);
}

//===========================================================================
// toString
//===========================================================================
string ccruncher::Date::toString() const
{
  char aux[] = "dd/mm/yyyy ";
  aux[10] = 0;

  sprintf(aux, "%02d/%02d/%04d", getDay(), getMonth(), getYear());

  return string(aux);
}

//===========================================================================
// return days between date1 and date2
//===========================================================================
int ccruncher::operator -(const Date &d1, const Date &d2)
{
  int ret = 0;

  if (d1 == d2)
  {
    return 0;
  }
  else if (d1 < d2)
  {
    return -(d2-d1);
  }

  if (d1.getYear() != d2.getYear())
  {
    ret += Date::numDaysInYear(d2.getYear()) - d2.getDayOfYear();

    for (int i=d2.getYear()+1;i<d1.getYear();i++)
    {
      ret += Date::numDaysInYear(i);
    }

    return ret + d1.getDayOfYear();
  }
  else
  {
    return d1.getDayOfYear() - d2.getDayOfYear();
  }
}
