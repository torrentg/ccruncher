
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
//   . https://svn.r-project.org/R/trunk/src/nmath/
//
//===========================================================================

#include <cmath>
#include <cstdlib>
#include "math/TStudent.hpp"
#include "math/Normal.hpp"
#include "math/toms708.h"

//---------------------------------------------------------------------------

#define M_2PI           6.283185307179586476925286766559  /* 2*pi */
#define M_LN_SQRT_2PI   0.918938533204672741780329736406  /* log(sqrt(2*pi)) */
#define M_LN_SQRT_PId2  0.225791352644727432363097614947  /* log(sqrt(pi/2)) */
#ifndef M_1_PI
#define M_1_PI          0.318309886183790671537767526745  /* 1/pi */
#endif
#ifndef M_PI
#define M_PI            3.141592653589793238462643383280  /* pi */
#endif
#ifndef M_SQRT2
#define M_SQRT2         1.414213562373095048801688724210  /* sqrt(2) */
#endif
#ifndef M_PI_2
#define M_PI_2          1.570796326794896619231321691640  /* pi/2 */
#endif
#ifndef M_LN2
#define M_LN2           0.693147180559945309417232121458  /* ln(2) */
#endif

//===========================================================================
// pbeta. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 2006 The R Development Core Team
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
 * #include <Rmath.h>
 *
 * double pbeta_raw(double x, double pin, double qin, int lower_tail, int log_p)
 * double pbeta	   (double x, double pin, double qin, int lower_tail, int log_p)
 *
 *  DESCRIPTION
 *
 *	Returns distribution function of the beta distribution.
 *	( = The incomplete beta ratio I_x(p,q) ).
 *
 *  NOTES
 *
 *      As from R 2.3.0, a wrapper for TOMS708
 *      as from R 2.6.0, 'log_p' partially improved over log(p..)
 */
#define R_D__0	(log_p ? -INFINITY : 0.)		/* 0 */
#define R_D__1	(log_p ? 0. : 1.)			/* 1 */
#define R_DT_0	(lower_tail ? R_D__0 : R_D__1)		/* 0 */
#define R_DT_1	(lower_tail ? R_D__1 : R_D__0)		/* 1 */
double ccruncher::TStudent::pbeta_raw(double x, double pin, double qin, int lower_tail, int log_p)
{
    double x1 = 0.5 - x + 0.5, w, wc;
    int ierr;
    bratio(pin, qin, x, x1, &w, &wc, &ierr, log_p); /* -> ./toms708.c */
    //if(ierr) 
		//MATHLIB_WARNING(_("pbeta_raw() -> bratio() gave error code %d"), ierr);
    return lower_tail ? w : wc;
} /* pbeta_raw() */

double ccruncher::TStudent::pbeta(double x, double pin, double qin, int lower_tail, int log_p)
{
    if (isnan(x) || isnan(pin) || isnan(qin)) return x + pin + qin;

    if (pin <= 0 || qin <= 0) return NAN; //ML_ERR_return_NAN;

    if (x <= 0)
		return R_DT_0;
    if (x >= 1)
		return R_DT_1;
    return pbeta_raw(x, pin, qin, lower_tail, log_p);
}
#undef R_D__0
#undef R_D__1
#undef R_DT_0
#undef R_DT_1

//===========================================================================
// lbeta. internal function
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *  Copyright (C) 2000 The R Development Core Team
 *  Copyright (C) 2003 The R Foundation
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
 *    double lbeta(double a, double b);
 *
 *  DESCRIPTION
 *
 *    This function returns the value of the log beta function.
 *
 *  NOTES
 *
 *    This routine is a translation into C of a Fortran subroutine
 *    by W. Fullerton of Los Alamos Scientific Laboratory.
 */
double ccruncher::TStudent::lbeta(double a, double b)
{
    double corr, p, q;

    p = q = a;
    if(b < p) p = b;/* := min(a,b) */
    if(b > q) q = b;/* := max(a,b) */

    if(isnan(a) || isnan(b))
		return a + b;

    /* both arguments must be >= 0 */

    if (p < 0)
		return NAN; //ML_ERR_return_NAN
    else if (p == 0) {
		return +INFINITY; //ML_POSINF;
    }
    else if (isinf(q)) {
		return -INFINITY; //ML_NEGINF;
    }

    if (p >= 10) {
		/* p and q are big. */
		corr = lgammacor(p) + lgammacor(q) - lgammacor(p + q);
		return log(q) * -0.5 + M_LN_SQRT_2PI + corr
			+ (p - 0.5) * log(p / (p + q)) + q * log1p(-p / (p + q));
    }
    else if (q >= 10) {
		/* p is small, but q is big. */
		corr = lgammacor(q) - lgammacor(p + q);
		return lgammafn(p) + corr + p - p * log(p + q)
			+ (q - 0.5) * log1p(-p / (p + q));
    }
    else
		/* p and q are small: p <= q < 10. */
		return log(gammafn(p) * (gammafn(q) / gammafn(p + q)));
}

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
/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
 *  Copyright (C) 2000-2007   The R Development Core Team
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
 */
double ccruncher::TStudent::cdf(double x, double n)
{
/* return  P[ T <= x ]	where
 * T ~ t_{n}  (t distrib. with n degrees of freedom).

 *	--> ./pnt.c for NON-central
 */
    double val, nx;
    bool lower_tail = true;

    if (isnan(x) || isnan(n))
		return x + n;

    if (n <= 0.0) return NAN; //ML_ERR_return_NAN;

    if(isinf(x))
		return (x < 0) ? 0.0 : 1.0;
    if(isinf(n))
		return Normal::cdf(x); //pnorm(x, 0.0, 1.0, lower_tail, log_p);

    if (n > 4e5) { /*-- Fixme(?): test should depend on `n' AND `x' ! */
		/* Approx. from	 Abramowitz & Stegun 26.7.8 (p.949) */
		val = 1./(4.*n);
		return Normal::cdf(x*(1. - val)/sqrt(1. + x*x*2.*val));
    }

    nx = 1 + (x/n)*x;
    /* FIXME: This test is probably losing rather than gaining precision,
     * now that pbeta(*, log_p = TRUE) is much better.
     * Note however that a version of this test *is* needed for x*x > D_MAX */
    if(nx > 1e100) { /* <==>  x*x > 1e100 * n  */
		/* Danger of underflow. So use Abramowitz & Stegun 26.5.4
		   pbeta(z, a, b) ~ z^a(1-z)^b / aB(a,b) ~ z^a / aB(a,b),
		   with z = 1/nx,  a = n/2,  b= 1/2 :
		*/
		double lval;
		lval = -0.5*n*(2*log(fabs(x)) - log(n))
			    - lbeta(0.5*n, 0.5) - log(0.5*n);
		val = exp(lval);
    } else {
		val = (n > x * x)
			? pbeta (x * x / (n + x * x), 0.5, n / 2., /*lower_tail*/0, /*log_p*/0)
			: pbeta (1. / nx,             n / 2., 0.5, /*lower_tail*/1, /*log_p*/0);
    }

    /* Use "1 - v"  if	lower_tail  and	 x > 0 (but not both):*/
    if(x <= 0.)
		lower_tail = !lower_tail;
    //
    //if(log_p) {
	//	if(lower_tail) return log1p(-0.5*exp(val));
	//	else return val - M_LN2; /* = log(.5* pbeta(....)) */
	//}
	//else {
		val /= 2.;
		return (lower_tail ? (0.5 - (val) + 0.5) : (val));	/*  1 - p */ //R_D_Cval(val);
	//}
}

//===========================================================================
// cdfinv.
//===========================================================================
/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1998 Ross Ihaka
 *  Copyright (C) 2000-2007 The R Development Core Team
 *  Copyright (C) 2003	    The R Foundation
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
 *  DESCRIPTION
 *
 *	The "Student" t distribution quantile function.
 *
 *  NOTES
 *
 *	This is a C translation of the Fortran routine given in:
 *	Hill, G.W (1970) "Algorithm 396: Student's t-quantiles"
 *	CACM 13(10), 619-620.
 *
 *	Supplemented by inversion for 0 < ndf < 1.
 *
 *  ADDITIONS:
 *	- lower_tail, log_p
 *	- using	 expm1() : takes care of  Lozy (1979) "Remark on Algo.", TOMS
 *	- Apply 2-term Taylor expansion as in
 *	  Hill, G.W (1981) "Remark on Algo.396", ACM TOMS 7, 250-1
 *	- Improve the formula decision for 1 < df < 2
 */
#define R_Q_P01_boundaries(p, _LEFT_, _RIGHT_)		\
    if (log_p) {									\
		if(p > 0)									\
			return NAN; /*ML_ERR_return_NAN;*/      \
		if(p == 0) /* upper bound*/					\
			return lower_tail ? _RIGHT_ : _LEFT_;	\
		if(p == -INFINITY/*ML_NEGINF*/)				\
			return lower_tail ? _LEFT_ : _RIGHT_;	\
    }												\
    else { /* !log_p */								\
		if(p < 0 || p > 1)							\
			return NAN; /*ML_ERR_return_NAN;*/ 		\
		if(p == 0)									\
			return lower_tail ? _LEFT_ : _RIGHT_;	\
		if(p == 1)									\
			return lower_tail ? _RIGHT_ : _LEFT_;	\
    }
#define R_DT_qIv(p)	(log_p ? (lower_tail ? exp(p) : - expm1(p)) \
			       : R_D_Lval(p))
#define R_D_qIv(p)	(log_p	? exp(p) : (p))		/*  p  in qF(p,..) */
#define R_D_Lval(p)	(lower_tail ? (p) : (0.5 - (p) + 0.5))	/*  p  */
#define R_D_Cval(p)	(lower_tail ? (0.5 - (p) + 0.5) : (p))	/*  1 - p */
#define R_Log1_Exp(x)   ((x) > -M_LN2 ? log(-expm1(x)) : log1p(-exp(x)))
double fmin2(double x, double y)
{
	if (isnan(x) || isnan(y))
		return x + y;
	return (x < y) ? x : y;
}
#include <float.h>
double ccruncher::TStudent::cdfinv(double p, double ndf)
{
    bool lower_tail = true;
    bool log_p = false;
    const static double eps = 1.e-12;

    double P, q;
    bool neg;

    if (isnan(p) || isnan(ndf))
		return p + ndf;

    R_Q_P01_boundaries(p, -INFINITY, +INFINITY);

    if (ndf <= 0) return NAN; //ML_ERR_return_NAN;

    if (ndf < 1) { /* based on qnt */
		const static double accu = 1e-13;
		const static double Eps = 1e-11; /* must be > accu */

		double ux, lx, nx, pp, pu, pl;

		p = R_DT_qIv(p);

		/* Invert pt(.) :
		 * 1. finding an upper and lower bound */
		if(p > 1 - DBL_EPSILON) return INFINITY; //ML_POSINF;
		pp = fmin2(1 - DBL_EPSILON, p * (1 + Eps));
		for(ux = 1.; ux < DBL_MAX && (pu = cdf(ux, ndf)/*pt(ux, ndf, TRUE, FALSE)*/) < pp; ux *= 2){};
		pp = p * (1 - Eps);
		for(lx =-1.; lx > -DBL_MAX&& (pl = cdf(lx, ndf)/*pt(lx, ndf, TRUE, FALSE)*/) > pp; lx *= 2){};

		/* 2. interval (lx,ux)  halving : nx = 0.5 * (lx + ux);
		 *    lower but secure: bisection method: */
		int cont=0;
		do {
			cont++;
			nx = lx + (ux - lx)/2.0;
			pp = cdf(nx, ndf);
			if (pp > p) {
				ux = nx; pu = pp;
			} else {
				lx = nx; pl = pp;
			}
		}
		while ((ux - lx) / fabs(nx) > accu && cont < 100);

		/* 2. interval (lx,ux)  halving : nx = 0.5 * (lx + ux);
		 *    faster: use regula falsi: */
/*
		do {
			nx = lx + (ux - lx)* (p - pl)/(pu - pl);
			pp = cdf(nx, ndf)//pt(nx, ndf, TRUE, FALSE)

			if ((pp = cdf(nx, ndf)) > p) { //pt(nx, ndf, TRUE, FALSE)
				ux = nx; pu = pp;
			} else {
				lx = nx; pl = pp;
			}
		}
		while ((ux - lx) / fabs(nx) > accu);
*/
		return 0.5 * (lx + ux);
    }

    /* Old comment:
     * FIXME: "This test should depend on  ndf  AND p  !!
     * -----  and in fact should be replaced by
     * something like Abramowitz & Stegun 26.7.5 (p.949)"
     *
     * That would say that if the qnorm value is x then
     * the result is about x + (x^3+x)/4df + (5x^5+16x^3+3x)/96df^2
     * The differences are tiny even if x ~ 1e5, and qnorm is not
     * that accurate in the extreme tails.
     */
    if (ndf > 1e20) return Normal::cdfinv(p); //qnorm(p, 0., 1., lower_tail, log_p);

    P = R_D_qIv(p); /* if exp(p) underflows, we fix below */

    neg = (!lower_tail || P < 0.5) && (lower_tail || P > 0.5);
    if(neg)
		P = 2 * (log_p ? (lower_tail ? P : -expm1(p)) : R_D_Lval(p));
    else
		P = 2 * (log_p ? (lower_tail ? -expm1(p) : P) : R_D_Cval(p));
    /* 0 <= P <= 1 ; P = 2*min(P', 1 - P')  in all cases */

/* Use this if(log_p) only : */
#define P_is_exp_2p (lower_tail == neg) /* both TRUE or FALSE == !xor */

	if (fabs(ndf - 2) < eps) {	/* df ~= 2 */
		if(P > DBL_MIN) {
			if(3* P < DBL_EPSILON) /* P ~= 0 */
				q = 1 / sqrt(P);
			else if (P > 0.9)	   /* P ~= 1 */
				q = (1 - P) * sqrt(2 /(P * (2 - P)));
			else /* eps/3 <= P <= 0.9 */
				q = sqrt(2 / (P * (2 - P)) - 2);
		}
		else { /* P << 1, q = 1/sqrt(P) = ... */
			if(log_p)
				q = P_is_exp_2p ? exp(- p/2) / M_SQRT2 : 1/sqrt(-expm1(p));
			else
				q = INFINITY; //ML_POSINF;
		}
    }
    else if (ndf < 1 + eps) { /* df ~= 1  (df < 1 excluded above): Cauchy */
		if(P > 0)
			q = 1/tan(P * M_PI_2);/* == - tan((P+1) * M_PI_2) -- suffers for P ~= 0 */

		else { /* P = 0, but maybe = 2*exp(p) ! */
			if(log_p) /* 1/tan(e) ~ 1/e */
				q = P_is_exp_2p ? M_1_PI * exp(-p) : -1./(M_PI * expm1(p));
			else
				q = INFINITY; //ML_POSINF;
		}
    }
    else {		/*-- usual case;  including, e.g.,  df = 1.1 */
		double x = 0., y, log_P2 = 0./* -Wall */,
			a = 1 / (ndf - 0.5),
			b = 48 / (a * a),
			c = ((20700 * a / b - 98) * a - 16) * a + 96.36,
			d = ((94.5 / (b + c) - 3) / b + 1) * sqrt(a * M_PI_2) * ndf;

		bool P_ok1 = P > DBL_MIN || !log_p,  P_ok = P_ok1;
		if(P_ok1) {
			y = pow(d * P, 2 / ndf);
			P_ok = (y >= DBL_EPSILON);
		}
		if(!P_ok) { /* log_p && P very small */
			log_P2 = P_is_exp_2p ? p : R_Log1_Exp(p); /* == log(P / 2) */
			x = (log(d) + M_LN2 + log_P2) / ndf;
			y = exp(2 * x);
		}

		if ((ndf < 2.1 && P > 0.5) || y > 0.05 + a) { /* P > P0(df) */
			/* Asymptotic inverse expansion about normal */
			if(P_ok)
				x = Normal::cdf(0.5 * P);//qnorm(0.5 * P, 0., 1., /*lower_tail*/TRUE,  /*log_p*/FALSE);
			else /* log_p && P underflowed */
				x = Normal::cdf(log_P2);//qnorm(log_P2,  0., 1., lower_tail,	        /*log_p*/ TRUE);

			y = x * x;
			if (ndf < 5)
			c += 0.3 * (ndf - 4.5) * (x + 0.6);
			c = (((0.05 * d * x - 5) * x - 7) * x - 2) * x + b + c;
			y = (((((0.4 * y + 6.3) * y + 36) * y + 94.5) / c
			  - y - 3) / b + 1) * x;
			y = expm1(a * y * y);
			q = sqrt(ndf * y);
		} else { /* re-use 'y' from above */

			if(!P_ok && x < - M_LN2 * DBL_MANT_DIG) {/* 0.5* log(DBL_EPSILON) */
				/* y above might have underflown */
				q = sqrt(ndf) * exp(-x);
			}
			else {
				y = ((1 / (((ndf + 6) / (ndf * y) - 0.089 * d - 0.822)
					   * (ndf + 2) * 3) + 0.5 / (ndf + 4))
					 * y - 1) * (ndf + 1) / (ndf + 2) + 1 / y;
				q = sqrt(ndf * y);
			}
		}


		/* Now apply 2-term Taylor expansion improvement (1-term = Newton):
		 * as by Hill (1981) [ref.above] */

		/* FIXME: This can be far from optimal when log_p = TRUE
		 *      but is still needed, e.g. for qt(-2, df=1.01, log=TRUE).
		 *	Probably also improvable when  lower_tail = FALSE */

		if(P_ok1) {
			int it=0;
			while(it++ < 10 && (y = pdf/*dt*/(q, ndf/*, FALSE*/)) > 0 &&
			  !isinf/*R_FINITE*/(x = (1.0-cdf/*pt*/(q, ndf/*, FALSE, FALSE*/) - P/2) / y) &&
			  fabs(x) > 1e-14*fabs(q))
			/* Newton (=Taylor 1 term):
			 *  q += x;
			 * Taylor 2-term : */
			q += x * (1. + x * q * (ndf + 1) / (2 * (q * q + ndf)));
		}
    }
    if(neg) q = -q;
    return q;
}

