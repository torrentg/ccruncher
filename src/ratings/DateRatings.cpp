
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
// DateRatings.cpp - DateRatings code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "ratings/DateRatings.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateRatings::DateRatings()
{
  ocurrences = NULL;
  nratings = 0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateRatings::DateRatings(Date date_, int numratings)
{
  initialize(date_, numratings);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::DateRatings::~DateRatings()
{
  if (ocurrences != NULL)
  {
    delete [] ocurrences;
    ocurrences = NULL;
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::DateRatings::initialize(Date date_, int numratings) throw(Exception)
{
  date = date_;

  try
  {
    nratings = numratings;
    ocurrences = new long[nratings];
    reset();
  }
  catch(std::exception &e)
  {
    throw Exception(e, "DateRatings::initialize(): not enougth space");
  }
}

//===========================================================================
// append
//===========================================================================
void ccruncher::DateRatings::append(int irating)
{
  if (irating >= nratings)
  {
    throw Exception("DateRatings::append(): panic: rating index out of range");
  }

  ocurrences[irating]++;
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::DateRatings::reset()
{
  for(int i=0;i<nratings;i++)
  {
    ocurrences[i] = 0L;
  }
}
