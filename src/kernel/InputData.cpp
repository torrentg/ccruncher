
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <limits>
#include <cassert>
#include "kernel/InputData.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12

/**************************************************************************//**
 * @details Creates an identity matrix of dimension dim x dim
 */
void ccruncher::InputData::initCorrelations(size_t dim)
{
  correlations.assign(dim, vector<double>(dim, 0.0));
  for(size_t i=0; i<dim; i++) {
    correlations[i][i] = 1.0;
  }
}

/**************************************************************************//**
 * @throw Exception Error setting factor loadings.
 */
void ccruncher::InputData::fillFLoadings()
{
  floadings = Input::getFactorLoadings(factors);
}

/**************************************************************************//**
 * @throw Exception Error setting CDFs.
 */
void ccruncher::InputData::fillCDFs()
{
  // set the default time functions
  if (!dprobs.empty()) {
    cdfs = dprobs;
  }
  else {
    // obtain the 1-month transitions matrix
    Transitions tone = transitions.scale(1);

    // computing default probability functions using transition matrix
    Date time0 = params.getTime0();
    Date timeT = params.getTimeT();
    int months = (int) ceil(diff(time0, timeT, 'M'));
    cdfs = tone.getCDFs(time0, months+1);
    assert(Input::validateCDFs(cdfs));
  }
}

/**************************************************************************//**
 * @details Recode portfolio (assets) segments and segmentations.
 */
void ccruncher::InputData::removeUnusedSegments()
{
  if (obligors.empty()) return;
  vector<vector<unsigned short>> table = getRecodeTable();
  recodePortfolioSegments(table);
  recodeSegmentations(table);
}

/**************************************************************************//**
 * @return A list of arrays containing the new segment indexes. Note that
 *         segments order is preserved. USHRT_MAX means that this index is
 *         unused.
 */
vector<vector<unsigned short>> ccruncher::InputData::getRecodeTable() const
{
  vector<vector<size_t>> hits = getHitsPerSegment();
  vector<vector<unsigned short>> table(hits.size());

  for(size_t isegmentation=0; isegmentation<hits.size(); isegmentation++) {
    table[isegmentation]= getRecodeTableRow(hits[isegmentation]);
  }

  return table;
}

/**************************************************************************//**
 * @return A list of arrays where the (i,j) element is the number of obligors
 *         in the j-th segment of the i-th segmentation.
 */
vector<vector<size_t>> ccruncher::InputData::getHitsPerSegment() const
{
  vector<vector<size_t>> hits(segmentations.size());
  for(size_t i=0; i<segmentations.size(); i++) {
    hits[i].assign(segmentations[i].size(), 0);
  }

  for(const Obligor &obligor : obligors) {
    for(const Asset &asset : obligor.assets) {
      for(size_t isegmentation=0; isegmentation<asset.segments.size(); isegmentation++) {
        unsigned short isegment = asset.segments[isegmentation];
        hits[isegmentation][isegment]++;
      }
    }
  }

  return hits;
}

/**************************************************************************//**
 * @param[in] hits Number of counts per segment.
 * @return Recoded segment indexes.
 */
vector<unsigned short> ccruncher::InputData::getRecodeTableRow(const vector<size_t> &hits) const
{
  vector<unsigned short> row;
  unsigned short numSegments = 0;

  // recoding segment indexes
  for(size_t i=0; i<hits.size(); i++)
  {
    if (hits[i] == 0) {
      row.push_back(numeric_limits<unsigned short>::max());
    }
    else {
      row.push_back(numSegments);
      numSegments++;
    }
  }

  // moving unassigned segment to the end
  if (hits[0] > 0)
  {
    assert(numSegments > 0);
    row[0] = numSegments-1;
    for(size_t i=1; i<row.size(); i++) {
      assert(row[i] != 0);
      if (row[i] != numeric_limits<unsigned short>::max()) row[i]--;
    }
  }

  return row;
}


/**************************************************************************//**
 * @param[in] table Transformation map.
 */
void ccruncher::InputData::recodePortfolioSegments(const vector<vector<unsigned short>> &table)
{
  for(Obligor &obligor : obligors) {
    for(Asset &asset : obligor.assets) {
      vector<unsigned short> &segments = asset.segments;
      for(size_t isegmentation=0; isegmentation<segments.size(); isegmentation++) {
        unsigned short oldSegment = segments[isegmentation];
        unsigned short newSegment = table[isegmentation][oldSegment];
        segments[isegmentation] = newSegment;
      }
    }
  }
}

/**************************************************************************//**
 * @param[in] table Transformation map.
 */
void ccruncher::InputData::recodeSegmentations(const vector<vector<unsigned short>> &table)
{
  vector<Segmentation> recodedSegmentations;

  for(size_t isegmentation=0; isegmentation<segmentations.size(); isegmentation++)
  {
    const Segmentation &segmentation = segmentations[isegmentation];
    if (segmentation.isEnabled())
    {
      recodedSegmentations.push_back(Segmentation(segmentation.getName(), true, false));
      Segmentation &recodedSegmentation = recodedSegmentations.back();
      bool hasUnassignedSegment = (table[isegmentation][0] != numeric_limits<unsigned short>::max());
      for(size_t i=(hasUnassignedSegment?1:0); i<table[isegmentation].size(); i++)
      {
        if (table[isegmentation][i] != numeric_limits<unsigned short>::max()) {
          // recode table preserves order (except unassigned segment)
          assert(recodedSegmentation.size() == table[isegmentation][i]);
          string segmentName = segmentation.getSegment(i);
          recodedSegmentation.addSegment(segmentName);
        }
      }
      if (hasUnassignedSegment) {
        assert(recodedSegmentation.size() == table[isegmentation][0]);
        string segmentName = segmentation.getSegment(0);
        recodedSegmentation.addSegment(segmentName);
      }
    }
  }

  segmentations = recodedSegmentations;
}

