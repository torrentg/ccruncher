
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include <limits>
#include <iostream>
#include "kernel/Input.hpp"
#include "kernel/InputTest.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1E-14

//===========================================================================
// getRatings
//===========================================================================
vector<Rating> ccruncher_test::InputTest::getRatings()
{
  vector<Rating> ratings;
  ratings.push_back(Rating("name1", "desc1"));
  ratings.push_back(Rating("name2", "desc2"));
  ratings.push_back(Rating("name3", "desc3"));
  ratings.push_back(Rating("name4", "desc4"));
  ratings.push_back(Rating("name5", "desc5"));
  return ratings;
}

//===========================================================================
// getFactors
//===========================================================================
vector<Factor> ccruncher_test::InputTest::getFactors()
{
  vector<Factor> factors;
  factors.push_back(Factor("name1", 0.1, "desc1"));
  factors.push_back(Factor("name2", 0.2, "desc2"));
  factors.push_back(Factor("name3", 0.3, "desc3"));
  factors.push_back(Factor("name4", 0.4, "desc4"));
  factors.push_back(Factor("name5", 0.5, "desc5"));
  return factors;
}

//===========================================================================
// getSegmentations
//===========================================================================
vector<Segmentation> ccruncher_test::InputTest::getSegmentations()
{
  vector<Segmentation> segmentations;
  segmentations.push_back(Segmentation("segmentation1"));
  segmentations.back().addSegment("segment11");
  segmentations.back().addSegment("segment12");
  segmentations.push_back(Segmentation("segmentation2"));
  segmentations.back().addSegment("segment21");
  segmentations.back().addSegment("segment22");
  segmentations.push_back(Segmentation("segmentation3"));
  segmentations.back().addSegment("segment31");
  segmentations.back().addSegment("segment32");
  return segmentations;
}

//===========================================================================
// getCDFs
//===========================================================================
vector<CDF> ccruncher_test::InputTest::getCDFs()
{
  vector<CDF> cdfs;
  cdfs.push_back(CDF({{0.0,0.0},{30,0.1},{60,0.2}}, 0.0));
  cdfs.push_back(CDF({{0.0,0.0},{30,0.2},{60,0.3}}, 0.0));
  cdfs.push_back(CDF({{0.0,1.0},{30,1.0},{60,1.0}}, 0.0));
  return cdfs;
}

//===========================================================================
// validateRatings + indexOfRating
//===========================================================================
void ccruncher_test::InputTest::validateRatings()
{
  // nominal case
  vector<Rating> ratings = getRatings();
  ASSERT(Input::validateRatings(ratings));
  ASSERT_NO_THROW(Input::validateRatings(ratings, true));
  ASSERT_EQUALS((unsigned char)0, Input::indexOfRating(ratings, "name1"));
  ASSERT_EQUALS((unsigned char)1, Input::indexOfRating(ratings, "name2"));
  ASSERT_EQUALS((unsigned char)2, Input::indexOfRating(ratings, "name3"));
  ASSERT_EQUALS((unsigned char)3, Input::indexOfRating(ratings, "name4"));
  ASSERT_EQUALS((unsigned char)4, Input::indexOfRating(ratings, "name5"));
  ASSERT_THROW(Input::indexOfRating(ratings, "name6"));

  // empty list
  ASSERT_THROW(Input::validateFactors(vector<Factor>(), true));
  ASSERT_EQUALS(false, Input::validateFactors(vector<Factor>()));

  // only one rating
  vector<Rating> ratings1;
  ratings1.push_back(Rating("default"));
  ASSERT(!Input::validateRatings(ratings1));

  // duplicated elements
  vector<Rating> ratings2 = getRatings();
  ratings2.push_back(Rating("xxx"));
  ratings2.push_back(Rating("xxx"));
  ASSERT(!Input::validateRatings(ratings2));
}

//===========================================================================
// validateFactors + indexOfFactor
//===========================================================================
void ccruncher_test::InputTest::validateFactors()
{
  // nominal case
  vector<Factor> factors = getFactors();
  ASSERT(Input::validateFactors(factors));
  ASSERT_NO_THROW(Input::validateFactors(factors, true));
  ASSERT_EQUALS((unsigned char)0, Input::indexOfFactor(factors, "name1"));
  ASSERT_EQUALS((unsigned char)1, Input::indexOfFactor(factors, "name2"));
  ASSERT_EQUALS((unsigned char)2, Input::indexOfFactor(factors, "name3"));
  ASSERT_EQUALS((unsigned char)3, Input::indexOfFactor(factors, "name4"));
  ASSERT_EQUALS((unsigned char)4, Input::indexOfFactor(factors, "name5"));
  ASSERT_THROW(Input::indexOfFactor(factors, "name6"));

  // empty list
  ASSERT_THROW(Input::validateFactors(vector<Factor>(), true));
  ASSERT_EQUALS(false, Input::validateFactors(vector<Factor>()));

  // too much factors
  vector<Factor> factors1;
  for(int i=0; i<numeric_limits<unsigned char>::max()+1; i++) {
    factors1.push_back(Factor("factor" + to_string(i)));
  }
  ASSERT(!Input::validateFactors(factors1));

  // duplicated elements
  vector<Factor> factors2 = getFactors();
  factors2.push_back(Factor("xxx"));
  factors2.push_back(Factor("xxx"));
  ASSERT(!Input::validateFactors(factors2));

  // factors loading values (default loading = NAN)
  vector<Factor> factors3;
  factors3.push_back(Factor("name"));
  ASSERT(!Input::validateFactors(factors2));

  // factors loading values (loading < 0)
  vector<Factor> factors4;
  factors4.push_back(Factor("name", -0.5));
  ASSERT(!Input::validateFactors(factors4));

  // factors loading values (loading > 1)
  vector<Factor> factors5;
  factors5.push_back(Factor("name", 1.5));
  ASSERT(!Input::validateFactors(factors5));
}

//===========================================================================
// validateFactorLoadings
//===========================================================================
void ccruncher_test::InputTest::validateFactorLoadings()
{
  // nominal case
  vector<double> fl = Input::getFactorLoadings(getFactors());
  ASSERT(Input::validateFactorLoadings(fl));
  ASSERT_NO_THROW(Input::validateFactorLoadings(fl, true));

  // empty list
  ASSERT_THROW(Input::validateFactorLoadings(vector<double>(), true));
  ASSERT_EQUALS(false, Input::validateFactorLoadings(vector<double>()));

  // too much elements
  vector<double> fl1 = vector<double>(numeric_limits<unsigned char>::max()+1);
  ASSERT(!Input::validateFactorLoadings(fl1));

  // out of range (x < 0)
  vector<double> fl2 = {0.1, -0.1, 0.2};
  ASSERT(!Input::validateFactorLoadings(fl2));

  // out of range (x > 1)
  vector<double> fl3 = {0.1, 0.2, 1.2};
  ASSERT(!Input::validateFactorLoadings(fl3));

  // out of range (NaN)
  vector<double> fl4 = {0.1, NAN, 0.2};
  ASSERT(!Input::validateFactorLoadings(fl4));
}

//===========================================================================
// validateCorrelations
//===========================================================================
void ccruncher_test::InputTest::validateCorrelations()
{
  // nominal case
  vector<vector<double>> matrix = {
    {1.00, 0.05, 0.15},
    {0.05, 1.00, 0.05},
    {0.15, 0.05, 1.00}};
  ASSERT(Input::validateCorrelations(matrix));
  ASSERT_NO_THROW(Input::validateCorrelations(matrix, true));

  // empty matrix
  ASSERT_THROW(Input::validateCorrelations(vector<vector<double>>(), true));
  ASSERT_EQUALS(false, Input::validateCorrelations(vector<vector<double>>()));

  // non-square matrix
  vector<vector<double>> matrix1 = {
    {1.00, 0.05, 0.15},
    {0.05, 1.00},
    {0.15, 0.05, 1.00}};
  ASSERT(!Input::validateCorrelations(matrix1));

  // diagonal distinct than 1
  vector<vector<double>> matrix2 = {{1.0, 0.5}, {0.5, 0.5}};
  ASSERT(!Input::validateCorrelations(matrix2));

  // correlation less than -1
  vector<vector<double>> matrix3 = {{1.0, -1.5}, {-1.5, 1.0}};
  ASSERT(!Input::validateCorrelations(matrix3));

  // correlation bigger than +1
  vector<vector<double>> matrix5 = {{1.0, 1.5}, {1.5, 1.0}};
  ASSERT(!Input::validateCorrelations(matrix5));

  // non-symmetric matrix
  vector<vector<double>> matrix6 = {{1.0, 0.5}, {0.7, 1.0}};
  ASSERT(!Input::validateCorrelations(matrix6));
}

//===========================================================================
// validateSegmentations + indexOfSegmentation
//===========================================================================
void ccruncher_test::InputTest::validateSegmentations()
{
  // nominal case
  vector<Segmentation> segmentations = getSegmentations();
  ASSERT(Input::validateSegmentations(segmentations));
  ASSERT_NO_THROW(Input::validateSegmentations(segmentations, true));
  ASSERT_EQUALS((unsigned short)0, Input::indexOfSegmentation(segmentations, "segmentation1"));
  ASSERT_EQUALS((unsigned short)1, Input::indexOfSegmentation(segmentations, "segmentation2"));
  ASSERT_EQUALS((unsigned short)2, Input::indexOfSegmentation(segmentations, "segmentation3"));
  ASSERT_THROW(Input::indexOfSegmentation(segmentations, "segmentation4"));

  // empty list
  ASSERT_THROW(Input::validateSegmentations(vector<Segmentation>(), true));
  ASSERT_EQUALS(false, Input::validateSegmentations(vector<Segmentation>()));

  // too much elements
  vector<Segmentation> segs1 = vector<Segmentation>();
  for(int i=0; i<numeric_limits<unsigned short>::max()+1; i++) {
    segs1.push_back(Segmentation("segmentation" + to_string(i)));
    segs1.back().addSegment("segment1");
  }
  ASSERT(!Input::validateSegmentations(segs1));

  // duplicated segmentation names
  vector<Segmentation> segs2 = vector<Segmentation>();
  segs2.push_back(Segmentation("segmentationX"));
  segs2.back().addSegment("segment1");
  segs2.push_back(Segmentation("segmentationX"));
  segs2.back().addSegment("segment1");
  ASSERT(!Input::validateSegmentations(segs2));

  // enabled segmentations not found
  vector<Segmentation> segs3 = vector<Segmentation>();
  segs3.push_back(Segmentation("segmentationX", false));
  segs3.back().addSegment("segment1");
  ASSERT(!Input::validateSegmentations(segs3));
}

//===========================================================================
// validateCDFs + indexOfDefaultRating
//===========================================================================
void ccruncher_test::InputTest::validateCDFs()
{
  // nominal case
  vector<CDF> cdfs = getCDFs();
  ASSERT(Input::validateCDFs(cdfs));
  ASSERT_NO_THROW(Input::validateCDFs(cdfs, true));
  ASSERT_EQUALS((unsigned char)2, Input::indexOfDefaultRating(cdfs));

  // less-than 2 ratings
  vector<CDF> cdfs1;
  cdfs1.push_back(CDF({{0.0,1.0},{30,1.0},{60,1.0}}, 0.0));
  ASSERT_THROW(Input::validateCDFs(cdfs1, true));
  ASSERT_EQUALS(false, Input::validateCDFs(cdfs1));

  // too much elements
  vector<CDF> cdfs2;
  for(int i=0; i<numeric_limits<unsigned char>::max()+1; i++) {
    cdfs2.push_back(CDF({{0.0,0.1},{30,0.2},{60,0.3}}, 0.0));
  }
  ASSERT(!Input::validateCDFs(cdfs2));

  // 2 default ratings
  vector<CDF> cdfs3 = getCDFs();
  cdfs3.push_back(CDF({{0.0,1.0},{30,1.0},{60,1.0}}, 0.0));
  ASSERT(!Input::validateCDFs(cdfs3));

  // non-defaulting rating
  vector<CDF> cdfs4 = getCDFs();
  cdfs4.push_back(CDF({{0.0,0.0},{30,0.0},{60,0.0}}, 0.0));
  ASSERT(!Input::validateCDFs(cdfs4));
}

