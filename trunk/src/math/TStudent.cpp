
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
// TStudent.cpp - TStudent code - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <cstdlib>
#include "math/TStudent.hpp"
#include "math/Normal.hpp"

//---------------------------------------------------------------------------

#define  M_2PI           6.283185307179586476925286766559  /* 2*pi */
#define  M_LN_SQRT_2PI   0.918938533204672741780329736406  /* log(sqrt(2*pi)) */
#define  M_LN_SQRT_PId2  0.225791352644727432363097614947  /* log(sqrt(pi/2)) */

//===========================================================================
// chebyshev_eval. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    int chebyshev_init(double *dos, int nos, double eta)
 *    double chebyshev_eval(double x, double *a, int n)
 *
 *  DESCRIPTION
 *
 *    "chebyshev_init" determines the number of terms for the
 *    double precision orthogonal series "dos" needed to insure
 *    the error is no larger than "eta".  Ordinarily eta will be
 *    chosen to be one-tenth machine precision.
 *
 *    "chebyshev_eval" evaluates the n-term Chebyshev series
 *    "a" at "x".
 *
 *  NOTES
 *
 *    These routines are translations into C of Fortran routines
 *    by W. Fullerton of Los Alamos Scientific Laboratory.
 *
 *    Based on the Fortran routine dcsevl by W. Fullerton.
 *    Adapted from R. Broucke, Algorithm 446, CACM., 16, 254 (1973).
 */
double ccruncher::TStudent::chebyshev_eval(double x, const double *a, const int n)
{
    double b0, b1, b2, twox;
    int i;

    if (n < 1 || n > 1000) return NAN; //ML_ERR_return_NAN;

    if (x < -1.1 || x > 1.1) return NAN; //ML_ERR_return_NAN;

    twox = x * 2;
    b2 = b1 = 0;
    b0 = 0;
    for (i = 1; i <= n; i++) {
		b2 = b1;
		b1 = b0;
		b0 = twox * b1 - b2 + a[n - i];
    }
    return (b0 - b2) * 0.5;
}

//===========================================================================
// lgammacor. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *  Copyright (C) 2000-2001 The R Development Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    #include <Rmath.h>
 *    double lgammacor(double x);
 *
 *  DESCRIPTION
 *
 *    Compute the log gamma correction factor for x >= 10 so that
 *
 *    log(gamma(x)) = .5*log(2*pi) + (x-.5)*log(x) -x + lgammacor(x)
 *
 *    [ lgammacor(x) is called	Del(x)	in other contexts (e.g. dcdflib)]
 *
 *  NOTES
 *
 *    This routine is a translation into C of a Fortran subroutine
 *    written by W. Fullerton of Los Alamos Scientific Laboratory.
 *
 *  SEE ALSO
 *
 *    Loader(1999)'s stirlerr() {in ./stirlerr.c} is *very* similar in spirit,
 *    is faster and cleaner, but is only defined "fast" for half integers.
 */
double ccruncher::TStudent::lgammacor(double x)
{
    const static double algmcs[15] = {
	+.1666389480451863247205729650822e+0,
	-.1384948176067563840732986059135e-4,
	+.9810825646924729426157171547487e-8,
	-.1809129475572494194263306266719e-10,
	+.6221098041892605227126015543416e-13,
	-.3399615005417721944303330599666e-15,
	+.2683181998482698748957538846666e-17,
	-.2868042435334643284144622399999e-19,
	+.3962837061046434803679306666666e-21,
	-.6831888753985766870111999999999e-23,
	+.1429227355942498147573333333333e-24,
	-.3547598158101070547199999999999e-26,
	+.1025680058010470912000000000000e-27,
	-.3401102254316748799999999999999e-29,
	+.1276642195630062933333333333333e-30
    };

    double tmp;

/* For IEEE double precision DBL_EPSILON = 2^-52 = 2.220446049250313e-16 :
 *   xbig = 2 ^ 26.5
 *   xmax = DBL_MAX / 48 =  2^1020 / 3 */
#define nalgm 5
#define xbig  94906265.62425156
#define xmax  3.745194030963158e306

    if (x < 10)
		return NAN; //ML_ERR_return_NAN
    else if (x >= xmax) {
		//ML_ERROR(ME_UNDERFLOW, "lgammacor");
	/* allow to underflow below */
    }
    else if (x < xbig) {
		tmp = 10 / x;
		return chebyshev_eval(tmp * tmp * 2 - 1, algmcs, nalgm) / x;
    }
    return 1 / (x * 12);
}

//===========================================================================
// gammafn. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *  Copyright (C) 2000-2001 The R Development Core Team
 *  Copyright (C) 2002-2004 The R Foundation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    #include <Rmath.h>
 *    double gammafn(double x);
 *
 *  DESCRIPTION
 *
 *    This function computes the value of the gamma function.
 *
 *  NOTES
 *
 *    This function is a translation into C of a Fortran subroutine
 *    by W. Fullerton of Los Alamos Scientific Laboratory.
 *    (e.g. http://www.netlib.org/slatec/fnlib/gamma.f)
 *
 *    The accuracy of this routine compares (very) favourably
 *    with those of the Sun Microsystems portable mathematical
 *    library.
 *
 *    MM specialized the case of  n!  for n < 50 - for even better precision
 */
double ccruncher::TStudent::gammafn(double x)
{
    const static double gamcs[42] = {
	+.8571195590989331421920062399942e-2,
	+.4415381324841006757191315771652e-2,
	+.5685043681599363378632664588789e-1,
	-.4219835396418560501012500186624e-2,
	+.1326808181212460220584006796352e-2,
	-.1893024529798880432523947023886e-3,
	+.3606925327441245256578082217225e-4,
	-.6056761904460864218485548290365e-5,
	+.1055829546302283344731823509093e-5,
	-.1811967365542384048291855891166e-6,
	+.3117724964715322277790254593169e-7,
	-.5354219639019687140874081024347e-8,
	+.9193275519859588946887786825940e-9,
	-.1577941280288339761767423273953e-9,
	+.2707980622934954543266540433089e-10,
	-.4646818653825730144081661058933e-11,
	+.7973350192007419656460767175359e-12,
	-.1368078209830916025799499172309e-12,
	+.2347319486563800657233471771688e-13,
	-.4027432614949066932766570534699e-14,
	+.6910051747372100912138336975257e-15,
	-.1185584500221992907052387126192e-15,
	+.2034148542496373955201026051932e-16,
	-.3490054341717405849274012949108e-17,
	+.5987993856485305567135051066026e-18,
	-.1027378057872228074490069778431e-18,
	+.1762702816060529824942759660748e-19,
	-.3024320653735306260958772112042e-20,
	+.5188914660218397839717833550506e-21,
	-.8902770842456576692449251601066e-22,
	+.1527474068493342602274596891306e-22,
	-.2620731256187362900257328332799e-23,
	+.4496464047830538670331046570666e-24,
	-.7714712731336877911703901525333e-25,
	+.1323635453126044036486572714666e-25,
	-.2270999412942928816702313813333e-26,
	+.3896418998003991449320816639999e-27,
	-.6685198115125953327792127999999e-28,
	+.1146998663140024384347613866666e-28,
	-.1967938586345134677295103999999e-29,
	+.3376448816585338090334890666666e-30,
	-.5793070335782135784625493333333e-31
    };

    int i, n;
    double y;
    double sinpiy, value;

/* For IEEE double precision DBL_EPSILON = 2^-52 = 2.220446049250313e-16 :
 * (xmin, xmax) are non-trivial, see ./gammalims.c
 * xsml = exp(.01)*DBL_MIN
 * dxrel = sqrt(DBL_EPSILON) = 2 ^ -26
*/
# define ngam 22
# define xmin -170.5674972726612
# undef xmax
# define xmax  171.61447887182298
# define xsml 2.2474362225598545e-308
# define dxrel 1.490116119384765696e-8

    if(isnan(x)) return x;

    /* If the argument is exactly zero or a negative integer
     * then return NaN. */
    if (x == 0 || (x < 0 && x == (long)x)) {
		//ML_ERROR(ME_DOMAIN, "gammafn");
		return NAN; //return ML_NAN;
    }

    y = fabs(x);

    if (y <= 10) {

		/* Compute gamma(x) for -10 <= x <= 10
		 * Reduce the interval and find gamma(1 + y) for 0 <= y < 1
		 * first of all. */

		n = x;
		if(x < 0) --n;
		y = x - n;/* n = floor(x)  ==>	y in [ 0, 1 ) */
		--n;
		value = chebyshev_eval(y * 2 - 1, gamcs, ngam) + .9375;
		if (n == 0)
			return value;/* x = 1.dddd = 1+y */

		if (n < 0) {
			/* compute gamma(x) for -10 <= x < 1 */

			/* exact 0 or "-n" checked already above */

			/* The answer is less than half precision */
			/* because x too near a negative integer. */
			if (x < -0.5 && fabs(x - (int)(x - 0.5) / x) < dxrel) {
				return NAN; //ML_ERROR(ME_PRECISION, "gammafn");
			}

			/* The argument is so close to 0 that the result would overflow. */
			if (y < xsml) {
				//ML_ERROR(ME_RANGE, "gammafn");
				if(x > 0) return +INFINITY; //ML_POSINF;
				else return -INFINITY; //ML_NEGINF;
			}

			n = -n;

			for (i = 0; i < n; i++) {
				value /= (x + i);
			}
			return value;
		}
		else {
			/* gamma(x) for 2 <= x <= 10 */

			for (i = 1; i <= n; i++) {
				value *= (y + i);
			}
			return value;
		}
    }
    else {
		/* gamma(x) for	 y = |x| > 10. */

		if (x > xmax) {			/* Overflow */
			//ML_ERROR(ME_RANGE, "gammafn");
			return +INFINITY; //ML_POSINF;
		}

		if (x < xmin) {			/* Underflow */
			//ML_ERROR(ME_UNDERFLOW, "gammafn");
			return 0.;
		}

		if(y <= 50 && y == (int)y) { /* compute (n - 1)! */
			value = 1.;
			for (i = 2; i < y; i++) value *= i;
		}
		else { /* normal case */
			value = exp((y - 0.5) * log(y) - y + M_LN_SQRT_2PI +
				((2*y == (int)2*y)? stirlerr(y) : lgammacor(y)));
		}
		if (x > 0)
			return value;

		if (fabs((x - (int)(x - 0.5))/x) < dxrel){

			/* The answer is less than half precision because */
			/* the argument is too near a negative integer. */

			//ML_ERROR(ME_PRECISION, "gammafn");
		}

		sinpiy = sin(M_PI * y);
		if (sinpiy == 0) {		/* Negative integer arg - overflow */
			//ML_ERROR(ME_RANGE, "gammafn");
			return +INFINITY; //ML_POSINF;
		}

		return -M_PI / (y * sinpiy * value);
    }
}

//===========================================================================
// lgammafn. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *  Copyright (C) 2000-2001 The R Development Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    #include <Rmath.h>
 *    double lgammafn_sign(double x, int *sgn);
 *    double lgammafn(double x);
 *
 *  DESCRIPTION
 *
 *    The function lgammafn computes log|gamma(x)|.  The function
 *    lgammafn_sign in addition assigns the sign of the gamma function
 *    to the address in the second argument if this is not NULL.
 *
 *  NOTES
 *
 *    This routine is a translation into C of a Fortran subroutine
 *    by W. Fullerton of Los Alamos Scientific Laboratory.
 *
 *    The accuracy of this routine compares (very) favourably
 *    with those of the Sun Microsystems portable mathematical
 *    library.
 */
double ccruncher::TStudent::lgammafn_sign(double x, int *sgn)
{
    double ans, y, sinpiy;

/* For IEEE double precision DBL_EPSILON = 2^-52 = 2.220446049250313e-16 :
   xmax  = DBL_MAX / log(DBL_MAX) = 2^1024 / (1024 * log(2)) = 2^1014 / log(2)
   dxrel = sqrt(DBL_EPSILON) = 2^-26 = 5^26 * 1e-26 (is *exact* below !)
 */
#undef xmax
#define xmax  2.5327372760800758e+305
#define dxrel 1.490116119384765696e-8

    if (sgn != NULL) *sgn = 1;

    if(isnan(x)) return x;

    if (x < 0 && fmod(floor(-x), 2.) == 0)
		if (sgn != NULL) *sgn = -1;

    if (x <= 0 && x == trunc(x)) { /* Negative integer argument */
		//ML_ERROR(ME_RANGE, "lgamma");
		return INFINITY;/* +Inf, since lgamma(x) = log|gamma(x)| */
    }

    y = fabs(x);

    if (y <= 10)
		return log(fabs(gammafn(x)));
    /*
      ELSE  y = |x| > 10 ---------------------- */

    if (y > xmax) {
		//ML_ERROR(ME_RANGE, "lgamma");
		return INFINITY;
    }

    if (x > 0) { /* i.e. y = x > 10 */
		if(x > 1e17)
	    	return(x*(log(x) - 1.));
		else if(x > 4934720.)
		    return(M_LN_SQRT_2PI + (x - 0.5) * log(x) - x);
		else
		    return M_LN_SQRT_2PI + (x - 0.5) * log(x) - x + lgammacor(x);
    }
    /* else: x < -10; y = -x */
    sinpiy = fabs(sin(M_PI * y));

    if (sinpiy == 0) { /* Negative integer argument ===
			  Now UNNECESSARY: caught above */
		//MATHLIB_WARNING(" ** should NEVER happen! *** [lgamma.c: Neg.int, y=%g]\n",y);
		return NAN; //ML_ERR_return_NAN;
    }

    ans = M_LN_SQRT_PId2 + (x - 0.5) * log(y) - x - log(sinpiy) - lgammacor(y);

    if(fabs((x - trunc(x - 0.5)) * ans / x) < dxrel) {

		/* The answer is less than half precision because
		 * the argument is too near a negative integer. */

		return NAN; //ML_ERROR(ME_PRECISION, "lgamma");
    }

    return ans;
}

double ccruncher::TStudent::lgammafn(double x)
{
    return lgammafn_sign(x, NULL);
}

//===========================================================================
// bd0. internal function
//===========================================================================
/*
 *  AUTHOR
 *	Catherine Loader, catherine@research.bell-labs.com.
 *	October 23, 2000.
 *
 *  Merge in to R:
 *	Copyright (C) 2000, The R Core Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *
 *  DESCRIPTION
 *	Evaluates the "deviance part"
 *	bd0(x,M) :=  M * D0(x/M) = M*[ x/M * log(x/M) + 1 - (x/M) ] =
 *		  =  x * log(x/M) + M - x
 *	where M = E[X] = n*p (or = lambda), for	  x, M > 0
 *
 *	in a manner that should be stable (with small relative error)
 *	for all x and M=np. In particular for x/np close to 1, direct
 *	evaluation fails, and evaluation is based on the Taylor series
 *	of log((1+v)/(1-v)) with v = (x-np)/(x+np).
 */
double ccruncher::TStudent::bd0(double x, double np)
{
  double ej, s, s1, v;
  int j;

  if(isinf(x) || isinf(np) || np == 0.0) {
    return NAN;
  }

  if (fabs(x-np) < 0.1*(x+np)) {
    v = (x-np)/(x+np);
    s = (x-np)*v;/* s using v -- change by MM */
    ej = 2*x*v;
    v = v*v;
    for (j=1; ; j++) { /* Taylor series */
      ej *= v;
      s1 = s+ej/((j<<1)+1);
      if (s1 == s) { /* last term was effectively 0 */
        return(s1);
      }
      s = s1;
    }
  }
  /* else:  | x - np |  is not too small */
  return(x*log(x/np)+np-x);
}

//===========================================================================
// stirlerr. internal function
//===========================================================================
/*
 *  AUTHOR
 *    Catherine Loader, catherine@research.bell-labs.com.
 *    October 23, 2000.
 *
 *  Merge in to R:
 *	Copyright (C) 2000, The R Core Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *
 *  DESCRIPTION
 *
 *    Computes the log of the error term in Stirling's formula.
 *      For n > 15, uses the series 1/12n - 1/360n^3 + ...
 *      For n <=15, integers or half-integers, uses stored values.
 *      For other n < 15, uses lgamma directly (don't use this to
 *        write lgamma!)
 *
 * Merge in to R:
 * Copyright (C) 2000, The R Core Development Team
 * R has lgammafn, and lgamma is not part of ISO C
 */

/* stirlerr(n) = log(n!) - log( sqrt(2*pi*n)*(n/e)^n )
 *             = log Gamma(n+1) - 1/2 * [log(2*pi) + log(n)] - n*[log(n) - 1]
 *             = log Gamma(n+1) - (n + 1/2) * log(n) + n - log(2*pi)/2
 *
 * see also lgammacor() in ./lgammacor.c  which computes almost the same!
 */
double ccruncher::TStudent::stirlerr(double n)
{

#define S0 0.083333333333333333333       /* 1/12 */
#define S1 0.00277777777777777777778     /* 1/360 */
#define S2 0.00079365079365079365079365  /* 1/1260 */
#define S3 0.000595238095238095238095238 /* 1/1680 */
#define S4 0.0008417508417508417508417508/* 1/1188 */

/*
  error for 0, 0.5, 1.0, 1.5, ..., 14.5, 15.0.
*/
    const static double sferr_halves[31] = {
	0.0, /* n=0 - wrong, place holder only */
	0.1534264097200273452913848,  /* 0.5 */
	0.0810614667953272582196702,  /* 1.0 */
	0.0548141210519176538961390,  /* 1.5 */
	0.0413406959554092940938221,  /* 2.0 */
	0.03316287351993628748511048, /* 2.5 */
	0.02767792568499833914878929, /* 3.0 */
	0.02374616365629749597132920, /* 3.5 */
	0.02079067210376509311152277, /* 4.0 */
	0.01848845053267318523077934, /* 4.5 */
	0.01664469118982119216319487, /* 5.0 */
	0.01513497322191737887351255, /* 5.5 */
	0.01387612882307074799874573, /* 6.0 */
	0.01281046524292022692424986, /* 6.5 */
	0.01189670994589177009505572, /* 7.0 */
	0.01110455975820691732662991, /* 7.5 */
	0.010411265261972096497478567, /* 8.0 */
	0.009799416126158803298389475, /* 8.5 */
	0.009255462182712732917728637, /* 9.0 */
	0.008768700134139385462952823, /* 9.5 */
	0.008330563433362871256469318, /* 10.0 */
	0.007934114564314020547248100, /* 10.5 */
	0.007573675487951840794972024, /* 11.0 */
	0.007244554301320383179543912, /* 11.5 */
	0.006942840107209529865664152, /* 12.0 */
	0.006665247032707682442354394, /* 12.5 */
	0.006408994188004207068439631, /* 13.0 */
	0.006171712263039457647532867, /* 13.5 */
	0.005951370112758847735624416, /* 14.0 */
	0.005746216513010115682023589, /* 14.5 */
	0.005554733551962801371038690  /* 15.0 */
    };
    double nn;

    if (n <= 15.0) {
	nn = n + n;
	if (nn == (int)nn) return(sferr_halves[(int)nn]);
	return(lgammafn(n + 1.) - (n + 0.5)*log(n) + n - M_LN_SQRT_2PI);
    }

    nn = n*n;
    if (n>500) return((S0-S1/nn)/n);
    if (n> 80) return((S0-(S1-S2/nn)/nn)/n);
    if (n> 35) return((S0-(S1-(S2-S3/nn)/nn)/nn)/n);
    /* 15 < n <= 35 : */
    return((S0-(S1-(S2-(S3-S4/nn)/nn)/nn)/nn)/n);
}

//===========================================================================
// probability density function TStudent(ndf)
//===========================================================================
/*
 *  AUTHOR
 *    Catherine Loader, catherine@research.bell-labs.com.
 *    October 23, 2000.
 *
 *  Merge in to R:
 *	Copyright (C) 2000, The R Core Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *
 * DESCRIPTION
 *
 *    The t density is evaluated as
 *         sqrt(n/2) / ((n+1)/2) * Gamma((n+3)/2) / Gamma((n+2)/2).
 *             * (1+x^2/n)^(-n/2)
 *             / sqrt( 2 pi (1+x^2/n) )
 *
 *    This form leads to a stable computation for all
 *    values of n, including n -> 0 and n -> infinity.
 */
double ccruncher::TStudent::pdf(double x, double n)
{
  double t, u;

  if (isnan(x) || isnan(n)) {
    return x + n;
  }

  if (n <= 0) {
    return NAN;
  }

  if(isinf(x)) {
    return 0.0;
  }

  if(isinf(n)) {
    return Normal::pdf(x);
  }

  t = -bd0(n/2.,(n+1)/2.) + stirlerr((n+1)/2.) - stirlerr(n/2.);
  if ( x*x > 0.2*n ) {
    u = log( 1+ x*x/n ) * n/2;
  }
  else {
    u = -bd0(n/2.,(n+x*x)/2.) + x*x/2.;
  }

  return exp(t-u)/sqrt(M_2PI*(1+x*x/n));
}

//===========================================================================
// cumulative distribution function TStudent(ndf)
//===========================================================================
double ccruncher::TStudent::cdf(double x, double ndf)
{
  //TODO
}

//===========================================================================
// cdfinv.
//===========================================================================
double ccruncher::TStudent::cdfinv(double p, double ndf)
{
  //TODO
}

