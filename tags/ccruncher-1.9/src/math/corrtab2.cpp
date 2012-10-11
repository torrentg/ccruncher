
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
// Utility used to fit the points computed by corrtab1 to f(x)=sin(ax)/sin(a) 
// by least squares. corrtab2 output will be adjusted to h(x) by least squares 
// using gnuplot or other sofware (eg. Excel Solver).
//
// compilation commands:
// ----------------------------
// g++ -g -Wall -Wextra -Wshadow -O3 -lgsl -o corrtab2 corrtab2.cpp
//
// gnuplot commands:
// ----------------------------
// a=1
// b=1
// c=1
// h(y)=pi/6 + 1/(a+b*y+exp(c*y)) + d*atan(y)
// fit h(x) "corrtab2.dat" using 1:2 via a,b,c
//
// h(y)=pi/2 + 2/pi*atan(y)
//
//===========================================================================

#include <iostream>
#include <cstdio>
#include <cmath>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

//---------------------------------------------------------------------------

using namespace std;

//===========================================================================
// function parameters
//===========================================================================

struct parameters
{
	double *x;
	double *y;
	int n;
};

//===========================================================================
// root finding function
//===========================================================================
double f(double a, void *params)
{
	double ret = 0.0;
	int n = ((parameters*)params)->n;
	double *x = ((parameters*)params)->x;
	double *y = ((parameters*)params)->y;

	for(int i=0; i<n; i++)
	{
		ret += (2.0/sin(a))*(y[i]-sin(a*x[i])/sin(a))*((cos(a)/sin(a))*sin(a*x[i])-x[i]*cos(a*x[i]));
	}

	return ret;
}

//===========================================================================
// root finding procedure
//===========================================================================
double bisection(double *x, double *y, int n)
{
	int status;
	int iter = 0, max_iter = 100;
	const gsl_root_fsolver_type *T;
	gsl_root_fsolver *s;
	double r = 0;
	double x_lo = 0.01, x_hi = 100.0;
	gsl_function F;
	struct parameters params = {x, y, n};

	F.function = &f;
	F.params = &params;
	T = gsl_root_fsolver_bisection;
	s = gsl_root_fsolver_alloc (T);
	gsl_root_fsolver_set (s, &F, x_lo, x_hi);

	//printf ("%5s %9s %9s\n", "iter", "root", "err(est)");

	do
	{
		iter++;
		status = gsl_root_fsolver_iterate (s);
		r = gsl_root_fsolver_root (s);
		x_lo = gsl_root_fsolver_x_lower (s);
		x_hi = gsl_root_fsolver_x_upper (s);
		status = gsl_root_test_interval (x_lo, x_hi, 0, 0.0001);

		//if (status == GSL_SUCCESS) printf ("Converged:\n");

		//printf ("%5d %.7f %.7f\n", iter, r, x_hi - x_lo);
	}
	while (status == GSL_CONTINUE && iter < max_iter);

	gsl_root_fsolver_free (s);
	return r;
}

//===========================================================================
// main procedure
//===========================================================================
int main(int argc, char *argv[])
{
	double ndf, ndf_prev;
	double x[1000], y[1000];
	int n;

	n = 0;
	cin >> ndf;
	ndf_prev = ndf;
	x[n] = 1.0;
	y[n] = 1.0;
	n++;

	while (!cin.eof())
	{
		if (ndf != ndf_prev)
		{
			x[n] = 1.0;
			y[n] = 1.0;
			n++;
			double coef = bisection(x, y, n);
			cout << ndf << "\t" << coef << endl;
			
			x[0] = 0.0;
			y[0] = 0.0;
			n = 1;
			
		}
		
		cin >> y[n] >> x[n];
		
		if (y[n] == 0.0) x[n] = 0.0;
		//cout << y[n] << "\t" << x[n] << endl;
		n++;
		
		ndf_prev = ndf;
		cin >> ndf;
	}
	
	if (n > 1)
	{
		x[n] = 1.0;
		y[n] = 1.0;
		n++;
		double coef = bisection(x, y, n);
		cout << ndf << "\t" << coef << endl;
	}
	
}
