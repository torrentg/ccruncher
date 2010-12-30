
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

//===========================================================================
//
// Utility used to determine the bivariate t-student copula correlation 
// in function of construction correlation. Scans all tau and sigma values.
// These values will be used to determine an aproximate function obtained
// by least-squares in function of tau and sigma (see corrtab2).
//
// compilation commands:
// ----------------------------
// g++ -g -Wall -Wextra -Wshadow -O3 -lgsl -o corrtab1 corrtab1.cpp
//
// gnuplot commands:
// ----------------------------
// set ytics 0.1
// set xtics 0.1
// set grid
// show grid
// set xrange [0:1]
// set yrange [0:1]
// set title "Bivariate T-Student Correlations"
// set xlabel "construction correlation"
// set ylabel "observed correlation"
// plot "corrtab1.dat" using 2:3 with lines lt 1 title "t-Student(0.01)",
//      "corrtab1.dat" using 2:2 with lines lt -1  notitle,
//      "corrtab1.dat" using (-$2):(-$3) with lines lt 1 notitle,
//      "corrtab1.dat" using (-$2):(-$2) with lines lt -1 notitle
//      "corrtab1.dat" using 2:(6/pi * asin($2/2)) with lines title "Gaussian"
// plot "corrtab1.dat" using 2:(abs($3-$4)) with lines title "error"
//
//===========================================================================

#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

//---------------------------------------------------------------------------

using namespace std;

//===========================================================================
// global variables
//===========================================================================

// RNG object
gsl_rng *rng;

//===========================================================================
// numeric correlation of a bivariate t-student copula
// sqrt([1 a; a 1]) = [1 a; 0 sqrt(1-a^2)]
//===========================================================================
double ncorr2(double a, double ndf, int niterations)
{
  double sx=0.0, sy=0.0, sxy=0.0, sx2=0.0, sy2=0.0;
  double b = sqrt(1.0-a*a);
  int n=0;

  for(int i=0; i<niterations; i++)
  {
     double factor = sqrt(ndf/gsl_ran_chisq(rng, ndf));

     double x = gsl_ran_gaussian(rng, 1.0);
     double y = a*x + b*gsl_ran_gaussian(rng, 1.0);

     x = gsl_cdf_tdist_P(factor*x, ndf);
     y = gsl_cdf_tdist_P(factor*y, ndf);

     n++;
     sx += x;
     sy += y;
     sxy += x*y;
     sx2 += x*x;
     sy2 += y*y;
  } 

  return (n*sxy - sx*sy)/(sqrt(n*sx2-sx*sx)*sqrt(n*sy2-sy*sy));
}

//===========================================================================
// arg1: initial x value
// arg2: step size value
// arg3: number of steps
// arg4: number of simulated values for value
//===========================================================================
int main(int argc, char *argv[])
{
  if (argc != 6)
  {
    cerr << "error: invalid number of arguments" << endl;
    cerr << "usage: " << argv[0] << " nx ns t0 dt nt" << endl;
    cerr << "where:" << endl;
    cerr << "  nx  number of bins (introduced correlation)" << endl;
    cerr << "  ns  number of simulated values by step" << endl;
    cerr << "  t0  initial value (t-student degrees of freedom)" << endl;
    cerr << "  dt  step size (t-student degrees of freedom)" << endl;
    cerr << "  nt  number of steps (t-student degrees of freedom)" << endl;
    cerr << "example:" << endl;
    cerr << "  " << argv[0] << " 100 100000 0.05 0.05 199" << endl;
    return -1;
  }

  int nx = atoi(argv[1]);
  if (nx < 2)
  {
    cerr << "error: invalid number of bins" << endl;
    return -1;
  }

  int ns = atoi(argv[2]);
  if (ns < 2)
  {
    cerr << "error: invalid number of simulated values by step" << endl;
    return -1;
  }
  
  double t0 = atof(argv[3]);
  if (t0 <= 0.0)
  {
    cerr << "error: invalid t-student degrees of fredom initial value" << endl;
    return -1;
  }
  
  double dt = atof(argv[4]);
  if (dt <= 0.0)
  {
    cerr << "error: invalid step size" << endl;
    return -1;
  }
  
  int nt = atoi(argv[5]);
  if (nt <= 0)
  {
    cerr << "error: invalid number of steps" << endl;
    return -1;
  }
  
  double ndf = t0;
      
  rng = gsl_rng_alloc(gsl_rng_mt19937);

  while (ndf <= t0+dt*nt)
  {
    for(int i=0; i<=nx; i++)
    {
      double x = i/(double)nx;
      double y = ncorr2(x, ndf, ns);
      cout << ndf << "\t" << x << "\t" << y << endl;
    }
    ndf += dt;
  }
  
  gsl_rng_free(rng);
}

