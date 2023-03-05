
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

#pragma once

#include <vector>
#include "utils/MiniCppUnit.hxx"
#include "params/Segmentation.hpp"
#include "params/Factor.hpp"
#include "params/Rating.hpp"
#include "params/CDF.hpp"

namespace ccruncher_test {

class InputTest : public TestFixture<InputTest>
{

  private:

    std::vector<ccruncher::Rating> getRatings();
    std::vector<ccruncher::Factor> getFactors();
    std::vector<ccruncher::Segmentation> getSegmentations();
    std::vector<ccruncher::CDF> getCDFs();

    void validateRatings();
    void validateFactors();
    void validateFactorLoadings();
    void validateCorrelations();
    void validateSegmentations();
    void validateCDFs();


  public:

    TEST_FIXTURE(InputTest)
    {
      TEST_CASE(validateRatings);
      TEST_CASE(validateFactors);
      TEST_CASE(validateFactorLoadings);
      TEST_CASE(validateCorrelations);
      TEST_CASE(validateSegmentations);
      TEST_CASE(validateCDFs);
    }

};

REGISTER_FIXTURE(InputTest)

} // namespace
