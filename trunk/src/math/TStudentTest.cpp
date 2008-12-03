
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
// validated using R command: dt(x,n)
//===========================================================================
void ccruncher_test::TStudentTest::test_pdf(void)
{
  double n[18] = {0.1, 0.5, 1.0, 2.0, 5.0, 7.5, 10.0, 15.0, 25.0, 40.0, 80.0,
    150.0, 250.0, 500.0, 1000.0, 5000.0, 20000.0, 50000.0};

  double x[21] = {-5.0, -4.5, -4.0, -3.5, -3.0, -2.5, -2.0, -1.5, -1.0, -0.5,
    0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};

  double y01[18] = { //-5.0
    7.091064e-03, 1.413075e-02, 1.224269e-02, 7.127781e-03, 1.757438e-03,
    7.585456e-04, 3.960011e-04, 1.534360e-04, 4.821457e-05, 1.886696e-05,
    6.554368e-06, 3.513206e-06, 2.545035e-06, 1.962337e-06, 1.712012e-06,
    1.529857e-06, 1.497416e-06, 1.490991e-06
  };

  double y02[18] = { //-4.5
    7.958322e-03, 1.649327e-02, 1.497929e-02, 9.528071e-03, 2.947540e-03,
    1.484630e-03, 8.832446e-04, 4.218269e-04, 1.765009e-04, 8.939357e-05,
    4.273463e-05, 2.805425e-05, 2.266476e-05, 1.912277e-05, 1.750437e-05,
    1.628019e-05, 1.605750e-05, 1.601322e-05
  };

  double y03[18] = { // -4.0
    0.0090526917, 0.0195865221, 0.0187241110, 0.0130945700, 0.0051237271,
    0.0030091758, 0.0020310339, 0.0011789997, 0.0006362067, 0.0004004712,
    0.0002470088, 0.0001891976, 0.0001656920, 0.0001492550, 0.0001414167,
    0.0001353274, 0.0001342036, 0.0001339795
  };

  double y04[18] = { //-3.5
    0.0104740619, 0.0237649906, 0.0240233876, 0.0185899278, 0.0092443541,
    0.0062997861, 0.0047836071, 0.0033073121, 0.0022138428, 0.0016582759,
    0.0012403928, 0.0010620160, 0.0009843556, 0.0009277827, 0.0009000464,
    0.0008781254, 0.0008740420, 0.0008732263
  };

  double y05[18] = { //-3.0
    0.012389654, 0.029633134, 0.031830989, 0.027410122, 0.017292579, 0.013526306,
    0.011400549, 0.009135184, 0.007253748, 0.006185947, 0.005301541, 0.004893205,
    0.004707892, 0.004569562, 0.004500625, 0.004445590, 0.004435283, 0.004433222
  };

  double y06[18] = { //-2.5
    0.01510102, 0.03829063, 0.04390481, 0.04220064, 0.03332624, 0.02935618,
    0.02693873, 0.02418442, 0.02171391, 0.02021321, 0.01889965, 0.01826688,
    0.01797342, 0.01775160, 0.01764013, 0.01755070, 0.01753390, 0.01753054
  };

  double y07[18] = { //-2.0
    0.01920883, 0.05189923, 0.06366198, 0.06804138, 0.06509031, 0.06273648,
    0.06114577, 0.05920773, 0.05736070, 0.05618831, 0.05512979, 0.05460871,
    0.05436450, 0.05417883, 0.05408517, 0.05400985, 0.05399569, 0.05399286
  };

  double y08[18] = { //-1.5
    0.02608823, 0.07508804, 0.09794150, 0.11413441, 0.12451734, 0.12653666,
    0.12744479, 0.12826094, 0.12883202, 0.12911545, 0.12932819, 0.12941960,
    0.12945964, 0.12948894, 0.12950335, 0.12951476, 0.12951689, 0.12951731
  };

  double y09[18] = { //-1.0
    0.03960646, 0.11830466, 0.15915494, 0.19245009, 0.21967980, 0.22670210,
    0.23036199, 0.23412477, 0.23721102, 0.23897736, 0.24046625, 0.24116639,
    0.24148759, 0.24172896, 0.24184979, 0.24194653, 0.24196468, 0.24196830
  };

  double y10[18] = { //-0.5
    0.07435232, 0.19896392, 0.25464791, 0.29629630, 0.32791853, 0.33570185,
    0.33969514, 0.34375457, 0.34704870, 0.34891946, 0.35048807, 0.35122305,
    0.35155967, 0.35181239, 0.35193883, 0.35204002, 0.35205900, 0.35206280
  };

  double y11[18] = { //0.0
    0.1480921, 0.2696763, 0.3183099, 0.3535534, 0.3796067, 0.3859007, 0.3891084,
    0.3923532, 0.3949738, 0.3964569, 0.3976976, 0.3982779, 0.3985435, 0.3987429,
    0.3988426, 0.3989223, 0.3989373, 0.3989403,
  };

  //y12[18] = y10 //0.5 (symmetric respect 0)
  //y13[18] = y09 //1.0 (symmetric respect 0)
  //y14[18] = y08 //1.5 (symmetric respect 0)
  //y15[18] = y07 //2.0 (symmetric respect 0)
  //y16[18] = y06 //2.5 (symmetric respect 0)
  //y17[18] = y05 //3.0 (symmetric respect 0)
  //y18[18] = y04 //3.5 (symmetric respect 0)
  //y19[18] = y03 //4.0 (symmetric respect 0)
  //y20[18] = y02 //4.5 (symmetric respect 0)
  //y21[18] = y01 //5.0 (symmetric respect 0)

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
// validated using octave command: pt(x, n)
//===========================================================================
void ccruncher_test::TStudentTest::test_cdf(void)
{
  double n[18] = {0.1, 0.5, 1.0, 2.0, 5.0, 7.5, 10.0, 15.0, 25.0, 40.0, 80.0,
    150.0, 250.0, 500.0, 1000.0, 5000.0, 20000.0, 50000.0};

  double x[21] = {-5.0, -4.5, -4.0, -3.5, -3.0, -2.5, -2.0, -1.5, -1.0, -0.5,
    0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};

  double y01[18] = { //-5.0
    3.552954e-01, 1.429957e-01, 6.283296e-02, 1.887478e-02, 2.052358e-03,
    6.381114e-04, 2.686668e-04, 7.918476e-05, 1.863554e-05, 5.920033e-06,
    1.659903e-06, 7.905908e-07, 5.399055e-07, 3.973208e-07, 3.383628e-07,
    2.964474e-07, 2.890757e-07, 2.876193e-07
  };

  double y02[18] = { //-4.5
    3.590498e-01, 1.506266e-01, 6.960449e-02, 2.300095e-02, 3.199768e-03,
    1.177162e-03, 5.715525e-04, 2.116489e-04, 6.809503e-05, 2.867233e-05,
    1.139363e-05, 6.771804e-06, 5.209601e-06, 4.230175e-06, 3.796536e-06,
    3.474755e-06, 3.416820e-06, 3.405322e-06
  };

  double y03[18] = { // -4.0
    3.632913e-01, 1.596100e-01, 7.797913e-02, 2.859548e-02, 5.161708e-03,
    2.253055e-03, 1.259166e-03, 5.796584e-04, 2.477218e-04, 1.329562e-04,
    7.021998e-05, 4.957793e-05, 4.173510e-05, 3.645803e-05, 3.400496e-05,
    3.212859e-05, 3.178514e-05, 3.171677e-05,
  };

  double y04[18] = { //-3.5
    0.3681566031, 0.1703926600, 0.0885855328, 0.0364136750, 0.0086422159,
    0.0044744360, 0.0028632527, 0.0016117655, 0.0008827477, 0.0005788533,
    0.0003817594, 0.0003064259, 0.0002753668, 0.0002534278, 0.0002428872,
    0.0002346582, 0.0002331353, 0.0002328315
  };

  double y05[18] = { //-3.0
    0.373847077, 0.183654078, 0.102416382, 0.047732983, 0.015049624, 0.009195469,
    0.006671828, 0.004486369, 0.003019090, 0.002315070, 0.001799199, 0.001581138,
    0.001486331, 0.001417246, 0.001383355, 0.001356555, 0.001351561, 0.001350563
  };

  double y06[18] = { //-2.5
    0.380677386, 0.200485265, 0.121118942, 0.064805860, 0.027245050, 0.019410129,
    0.015723422, 0.012252902, 0.009671564, 0.008310176, 0.007231975, 0.006747661,
    0.006530441, 0.006369288, 0.006289284, 0.006225558, 0.006213637, 0.006211254
  };

  double y07[18] = { //-2.0
    0.38917711, 0.22275745, 0.14758362, 0.09175171, 0.05096974, 0.04144850,
    0.03669402, 0.03197250, 0.02823799, 0.02616117, 0.02444692, 0.02365276,
    0.02329105, 0.02302034, 0.02288517, 0.02277713, 0.02275688, 0.02275283
  };

  double y08[18] = { //-1.5
    0.40033699, 0.25395090, 0.18716704, 0.13619656, 0.09695184, 0.08724029,
    0.08225366, 0.07718333, 0.07306923, 0.07073339, 0.06877536, 0.06785813,
    0.06743810, 0.06712277, 0.06696502, 0.06683877, 0.06681509, 0.06681036
  };

  double y09[18] = { //-1.0
    0.4163282, 0.3011216, 0.2500000, 0.2113249, 0.1816087, 0.1742380, 0.1704466,
    0.1665851, 0.1634460, 0.1616609, 0.1601628, 0.1594605, 0.1591387, 0.1588971,
    0.1587762, 0.1586794, 0.1586613, 0.1586577
  };

  double y10[18] = { //-0.5
    0.4432601, 0.3786590, 0.3524164, 0.3333333, 0.3191494, 0.3157055, 0.3139468,
    0.3121651, 0.3107239, 0.3099074, 0.3092238, 0.3089039, 0.3087574, 0.3086475,
    0.3085925, 0.3085485, 0.3085403, 0.3085386
  };

  double y11[18] = { //0.0
    0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5
  };

  //y12[18] = y10 //0.5 (symmetric respect 0)
  //y13[18] = y09 //1.0 (symmetric respect 0)
  //y14[18] = y08 //1.5 (symmetric respect 0)
  //y15[18] = y07 //2.0 (symmetric respect 0)
  //y16[18] = y06 //2.5 (symmetric respect 0)
  //y17[18] = y05 //3.0 (symmetric respect 0)
  //y18[18] = y04 //3.5 (symmetric respect 0)
  //y19[18] = y03 //4.0 (symmetric respect 0)
  //y20[18] = y02 //4.5 (symmetric respect 0)
  //y21[18] = y01 //5.0 (symmetric respect 0)

  for (int i=0;i<18;i++)
  {
    ASSERT_EQUALS_EPSILON(y01[i], TStudent::cdf(x[0] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y02[i], TStudent::cdf(x[1] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y03[i], TStudent::cdf(x[2] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y04[i], TStudent::cdf(x[3] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y05[i], TStudent::cdf(x[4] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y06[i], TStudent::cdf(x[5] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y07[i], TStudent::cdf(x[6] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y08[i], TStudent::cdf(x[7] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y09[i], TStudent::cdf(x[8] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y10[i], TStudent::cdf(x[9] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(y11[i], TStudent::cdf(x[10], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y10[i], TStudent::cdf(x[11], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y09[i], TStudent::cdf(x[12], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y08[i], TStudent::cdf(x[13], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y07[i], TStudent::cdf(x[14], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y06[i], TStudent::cdf(x[15], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y05[i], TStudent::cdf(x[16], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y04[i], TStudent::cdf(x[17], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y03[i], TStudent::cdf(x[18], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y02[i], TStudent::cdf(x[19], n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(1.0-y01[i], TStudent::cdf(x[20], n[i]), EPSILON);
  }

  ASSERT(isnan(TStudent::cdf(+0.5, -1.0)));
}

//===========================================================================
// test cumulative distribution inverse function
// validated using octave command: normal_inv(x, mu, sigma)
//===========================================================================
void ccruncher_test::TStudentTest::test_cdfinv(void)
{
  double n[18] = {0.1, 0.5, 1.0, 2.0, 5.0, 7.5, 10.0, 15.0, 25.0, 40.0, 80.0,
    150.0, 250.0, 500.0, 1000.0, 5000.0, 20000.0, 50000.0};

  double x[11] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

  double y01[18] = { //0.0
    -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY,
    -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY,
    -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY, -INFINITY
  };

  double y02[18] = { //0.1
    -1.60443e+06, -10.2703, 
    -3.077684, -1.885618, -1.475884, -1.405212, -1.372184, -1.340606, -1.316345,
    -1.303077, -1.292224, -1.287221, -1.284947, -1.283247, -1.282399, -1.281721,
    -1.281594, -1.281568
  };

  double y03[18] = { //0.2
    NAN, NAN, 
    -1.3763819, -1.0606602, -0.9195438, -0.8922093, -0.8790578, -0.8662450,
    -0.8562362, -0.8506998, -0.8461373, -0.8440241, -0.8430614, -0.8423407,
    -0.8419808, -0.8416931, -0.8416392, -0.8416284
  };

  double y04[18] = { //0.3
    NAN, NAN, 
    -0.7265425, -0.6172134, -0.5594296, -0.5474129, -0.5415280, -0.5357291,
    -0.5311538, -0.5286057, -0.5264965, -0.5255167, -0.5250698, -0.5247350,
    -0.5245677, -0.5244339, -0.5244089, -0.5244039
  };

  double y05[18] = { //0.4
    NAN, NAN, 
    -0.3249197, -0.2886751, -0.2671809, -0.2625019, -0.2601848, -0.2578853,
    -0.2560597, -0.2550387, -0.2541913, -0.2537969, -0.2536169, -0.2534819,
    -0.2534145, -0.2533606, -0.2533505, -0.2533485
  };

  double y06[18] = { //0.5
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  //double y07[18] = -y05 //0.6
  //double y08[18] = -y04 //0.7
  //double y09[18] = -y03 //0.8
  //double y10[18] = -y02 //0.9
  //double y11[18] = -y01 //1.0

  for (int i=0;i<2;i++)
  {
    ASSERT_EQUALS_EPSILON(+y01[i], TStudent::cdfinv(x[0] , n[i]), 100.0);
    ASSERT_EQUALS_EPSILON(+y02[i], TStudent::cdfinv(x[1] , n[i]), 100.0);
    ASSERT_EQUALS_EPSILON(-y02[i], TStudent::cdfinv(x[9] , n[i]), 100.0);
    ASSERT_EQUALS_EPSILON(-y01[i], TStudent::cdfinv(x[10], n[i]), 100.0);
  }

  for (int i=2;i<18;i++)
  {
    ASSERT_EQUALS_EPSILON(+y03[i], TStudent::cdfinv(x[2] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(+y04[i], TStudent::cdfinv(x[3] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(+y05[i], TStudent::cdfinv(x[4] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(+y06[i], TStudent::cdfinv(x[5] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(-y05[i], TStudent::cdfinv(x[6] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(-y04[i], TStudent::cdfinv(x[7] , n[i]), EPSILON);
    ASSERT_EQUALS_EPSILON(-y03[i], TStudent::cdfinv(x[8] , n[i]), EPSILON);
  }

  ASSERT(isnan(TStudent::cdfinv(+0.5, -1.0)));
  ASSERT(isnan(TStudent::cdfinv(-1.0, +1.0)));
  ASSERT(isnan(TStudent::cdfinv(+2.0, +1.0)));
}

