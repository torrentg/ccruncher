
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2008 Gerard Torrent
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
// TStudentTest.cpp - TStudentTest code - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include "math/TStudent.hpp"
#include "math/TStudentTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.000001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::TStudentTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::TStudentTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test probability distribution function
// validated using octave command: normal_pdf(x, mu, sigma)
//===========================================================================
void ccruncher_test::TStudentTest::test_pdf(void)
{
  double n[18] = {0.1, 0.5, 1.0, 2.0, 5.0, 7.5, 10.0, 15.0, 25.0, 40.0, 80.0,
    150.0, 250.0, 500.0, 1000.0, 5000.0, 20000.0, 50000.0};

  double x[21] = {-5.0, -4.5, -4.0, -3.5, -3.0, -2.5, -2.0, -1.5, -1.0, -0.5,
    0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};

  double y01[21] = { //-5.0
    7.091064e-03, 1.413075e-02, 1.224269e-02, 7.127781e-03, 1.757438e-03,
    7.585456e-04, 3.960011e-04, 1.534360e-04, 4.821457e-05, 1.886696e-05,
    6.554368e-06, 3.513206e-06, 2.545035e-06, 1.962337e-06, 1.712012e-06,
    1.529857e-06, 1.497416e-06, 1.490991e-06
  };

  double y02[21] = { //-4.5
    7.958322e-03, 1.649327e-02, 1.497929e-02, 9.528071e-03, 2.947540e-03,
    1.484630e-03, 8.832446e-04, 4.218269e-04, 1.765009e-04, 8.939357e-05,
    4.273463e-05, 2.805425e-05, 2.266476e-05, 1.912277e-05, 1.750437e-05,
    1.628019e-05, 1.605750e-05, 1.601322e-05
  };

  double y03[21] = { // -4.0
    0.0090526917, 0.0195865221, 0.0187241110, 0.0130945700, 0.0051237271,
    0.0030091758, 0.0020310339, 0.0011789997, 0.0006362067, 0.0004004712,
    0.0002470088, 0.0001891976, 0.0001656920, 0.0001492550, 0.0001414167,
    0.0001353274, 0.0001342036, 0.0001339795
  };

  double y04[21] = { //-3.5
    0.0104740619, 0.0237649906, 0.0240233876, 0.0185899278, 0.0092443541,
    0.0062997861, 0.0047836071, 0.0033073121, 0.0022138428, 0.0016582759,
    0.0012403928, 0.0010620160, 0.0009843556, 0.0009277827, 0.0009000464,
    0.0008781254, 0.0008740420, 0.0008732263
  };

  double y05[21] = { //-3.0
    0.012389654, 0.029633134, 0.031830989, 0.027410122, 0.017292579, 0.013526306,
    0.011400549, 0.009135184, 0.007253748, 0.006185947, 0.005301541, 0.004893205,
    0.004707892, 0.004569562, 0.004500625, 0.004445590, 0.004435283, 0.004433222
  };

  double y06[21] = { //-2.5
    0.01510102, 0.03829063, 0.04390481, 0.04220064, 0.03332624, 0.02935618,
    0.02693873, 0.02418442, 0.02171391, 0.02021321, 0.01889965, 0.01826688,
    0.01797342, 0.01775160, 0.01764013, 0.01755070, 0.01753390, 0.01753054
  };

  double y07[21] = { //-2.0
    0.01920883, 0.05189923, 0.06366198, 0.06804138, 0.06509031, 0.06273648,
    0.06114577, 0.05920773, 0.05736070, 0.05618831, 0.05512979, 0.05460871,
    0.05436450, 0.05417883, 0.05408517, 0.05400985, 0.05399569, 0.05399286
  };

  double y08[21] = { //-1.5
    0.02608823, 0.07508804, 0.09794150, 0.11413441, 0.12451734, 0.12653666,
    0.12744479, 0.12826094, 0.12883202, 0.12911545, 0.12932819, 0.12941960,
    0.12945964, 0.12948894, 0.12950335, 0.12951476, 0.12951689, 0.12951731
  };

  double y09[21] = { //-1.0
    0.03960646, 0.11830466, 0.15915494, 0.19245009, 0.21967980, 0.22670210,
    0.23036199, 0.23412477, 0.23721102, 0.23897736, 0.24046625, 0.24116639,
    0.24148759, 0.24172896, 0.24184979, 0.24194653, 0.24196468, 0.24196830
  };

  double y10[21] = { //-0.5
    0.07435232, 0.19896392, 0.25464791, 0.29629630, 0.32791853, 0.33570185,
    0.33969514, 0.34375457, 0.34704870, 0.34891946, 0.35048807, 0.35122305,
    0.35155967, 0.35181239, 0.35193883, 0.35204002, 0.35205900, 0.35206280
  };

  double y11[21] = { //0.0
    0.1480921, 0.2696763, 0.3183099, 0.3535534, 0.3796067, 0.3859007, 0.3891084,
    0.3923532, 0.3949738, 0.3964569, 0.3976976, 0.3982779, 0.3985435, 0.3987429,
    0.3988426, 0.3989223, 0.3989373, 0.3989403,
  };

  //y12[21] = y10 //0.5 (symmetric respect 0)
  //y13[21] = y09 //1.0 (symmetric respect 0)
  //y14[21] = y08 //1.5 (symmetric respect 0)
  //y15[21] = y07 //2.0 (symmetric respect 0)
  //y16[21] = y06 //2.5 (symmetric respect 0)
  //y17[21] = y05 //3.0 (symmetric respect 0)
  //y18[21] = y04 //3.5 (symmetric respect 0)
  //y19[21] = y03 //4.0 (symmetric respect 0)
  //y20[21] = y02 //4.5 (symmetric respect 0)
  //y21[21] = y01 //5.0 (symmetric respect 0)

  for (int i=0;i<18;i++)
  {
    ASSERT_EQUALS_EPSILON(y01[i], TStudent::pdf(x[0] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y02[i], TStudent::pdf(x[1] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y03[i], TStudent::pdf(x[2] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y04[i], TStudent::pdf(x[3] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y05[i], TStudent::pdf(x[4] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y06[i], TStudent::pdf(x[5] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y07[i], TStudent::pdf(x[6] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y08[i], TStudent::pdf(x[7] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y09[i], TStudent::pdf(x[8] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y10[i], TStudent::pdf(x[9] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y11[i], TStudent::pdf(x[10], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y10[i], TStudent::pdf(x[11], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y09[i], TStudent::pdf(x[12], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y08[i], TStudent::pdf(x[13], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y07[i], TStudent::pdf(x[14], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y06[i], TStudent::pdf(x[15], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y05[i], TStudent::pdf(x[16], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y04[i], TStudent::pdf(x[17], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y03[i], TStudent::pdf(x[18], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y02[i], TStudent::pdf(x[19], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y01[i], TStudent::pdf(x[20], n[i]), EPSILON);
  }

}

//===========================================================================
// test cumulative distribution function
// validated using octave command: normal_cdf(x, mu, sigma)
//===========================================================================
void ccruncher_test::TStudentTest::test_cdf(void)
{
  //TODO
}

//===========================================================================
// test cumulative distribution inverse function
// validated using octave command: normal_inv(x, mu, sigma)
//===========================================================================
void ccruncher_test::TStudentTest::test_cdfinv(void)
{
  //TODO
}
