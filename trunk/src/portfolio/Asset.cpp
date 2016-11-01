
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#include <cmath>
#include <algorithm>
#include <cassert>
#include "portfolio/Asset.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] from Starting analysis date.
 * @param[in] to Ending analysis date.
 * @return True if this asset has credit risk in the given time range,
 *         false otherwise.
 */
bool ccruncher::Asset::isActive(const Date &from, const Date &/*to*/) const
{
  if (values.empty()) {
    return false;
  }
  else if (values.back().date < from) {
    return false;
  }
  else {
    return true;
  }
}

/**************************************************************************//**
 * @details Prepare data once they have been readed from input file.
 *          Data preparation consist on:
 *          - Remove data previous to starting date analysis.
 *          - Remove data just after ending analysis date (except the first one).
 *          - Compute Current Net Value of EAD's at starting date analysis.
 * @param[in] d1 Initial analysis date.
 * @param[in] d2 Ending analysis date.
 * @param[in] interest Yield curve in the analyzed time range.
 */
void ccruncher::Asset::prepare(const Date &d1, const Date &d2, const Interest &interest)
{
  assert(d1 <= d2);
  if (d1 >= d2) return;
  if (values.empty()) return;

  // sort values by date
  sort(values.begin(), values.end());

  // search range to preserve
  int pos1=-1, pos2=-1;
  for(int i=0; i<(int)values.size(); i++)
  {
    if (d1 < values[i].date) {
      if (pos1 < 0) {
        pos1 = i;
      }
      pos2 = i;
      if (d2 <= values[i].date) {
        break;
      }
    }
  }
  assert(pos1 <= pos2);

  // memory shrink
  if (pos1 < 0) {
    vector<DateValues>(0).swap(values);
  }
  else {
    assert(values.begin()+pos2 != values.end());
    vector<DateValues>(values.begin()+pos1, values.begin()+pos2+1).swap(values);
  }
  
  // computing Current Net Value
  for(DateValues &dv : values) {
    dv.ead.mult(interest.getFactor(dv.date));
  }
}

