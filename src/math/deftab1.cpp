
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
// Utility used to determine the distribution of the number of defaults of
// n individuals, where its defaults times are modeled by a t-student copula
// with sigma as correlation, nu as degrees-of-freedom and p the threshold
// to consider an individual as defaulted
//
//===========================================================================

#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <cassert>
#include <gsl/gsl_sf_psi.h>
#include <gsl/gsl_roots.h>
#include "math/BlockGaussianCopula.hpp"
#include "math/BlockTStudentCopula.hpp"
#include "utils/Timer.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;

#define NUM_COPULA_REALIZATIONS 30000
#define NBINS_SIGMA 100
#define NBINS_P 20
#define NUM_NS 13

int ns[NUM_NS] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2500, 5000, 10000 };


struct estimate
{
    double n;
    double p;
};

struct fparams
{
    double *k;
    int N;
};

double f(double r, void *params)
{
    double ret=0.0, sum=0.0;
    fparams *p = (fparams*) params;

    for(int i=0; i<p->N; i++)
    {
        ret += gsl_sf_psi(p->k[i]+r);
        sum += p->k[i];
    }
    ret -= p->N*gsl_sf_psi(r);
    ret += p->N*log(r/(r+sum/p->N));

    return ret;
}

//===========================================================================
// fit
// fit values to binomial negative
// http://en.wikipedia.org/wiki/Negative_binomial_distribution
//===========================================================================
estimate fit(vector<double> &values)
{
    int status;
    int iter = 0, max_iter = 100;
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;
    double r = 0;
    double x_lo = 0.00001, x_hi = 1000.0;
    gsl_function F;
    fparams params;

    params.k = &values[0];
    params.N = values.size();

    F.function = &f;
    F.params = (void*) &params;
/*
for(int i=1; i<=200; i++)
{
    double x = 1000.0*i;
    cout << x << "\t" << F.function(x,F.params) << endl;
}
*/
    T = gsl_root_fsolver_bisection;
    s = gsl_root_fsolver_alloc(T);
    gsl_root_fsolver_set(s, &F, x_lo, x_hi);
//    printf ("%5s [%9s, %9s] %9s\n", "iter", "lower", "upper", "root");

    do
    {
        iter++;
        status = gsl_root_fsolver_iterate(s);
        r = gsl_root_fsolver_root(s);
        x_lo = gsl_root_fsolver_x_lower(s);
        x_hi = gsl_root_fsolver_x_upper(s);
        status = gsl_root_test_interval(x_lo, x_hi, 0, 0.00001);
/*
        if (status == GSL_SUCCESS) printf("Converged:\n");
        printf ("%5d [%.7f, %.7f] %.7f\n", iter, x_lo, x_hi, r);
*/
    }
    while (status == GSL_CONTINUE && iter < max_iter);

    double aux = 0.0;
    for(int i=0; i<(int)values.size(); i++) {
        aux += values[i];
    }
    aux /= values.size();

    estimate ret;
    ret.n = r;
    ret.p = aux/(r+aux);

    gsl_root_fsolver_free(s);
    return ret;
}

//===========================================================================
// approx_t
//===========================================================================
estimate approx(double sigma, double nu, double p, int n)
{
    Timer timer1, timer2;
    int dims = n;
    double *aux = &sigma;
    double **C = &aux;
    timer1.start();
    Copula *copula = NULL;
    if (nu > 30000) copula = new BlockGaussianCopula(C, &dims, 1, 0, false);
    else copula = new BlockTStudentCopula(C, &dims, 1, nu, 0, false);
    timer1.stop();
    vector<double> ndefaults(NUM_COPULA_REALIZATIONS, 0);

    timer2.start();
    for(int i=0; i<NUM_COPULA_REALIZATIONS; i++)
    {
        copula->next();
        for(int j=0; j<n; j++)
        {
            if (copula->get(j) < p)
            {
                ndefaults[i]++;
            }
        }
    }
    timer2.stop();
    delete copula;
    for(int i=0; i<(int)ndefaults.size(); i++) ndefaults[i] *= 1000.0/double(n);
//cout << "times(" << n << ") = (" << timer1.read() << ", " << timer2.read() << ")" << endl;
    return fit(ndefaults);
}

//===========================================================================
// main procedure
//===========================================================================
int main(int argc, char *argv[])
{
    for(int i1=2; i1<=30+10; i1++)
    {
        double nu = 0.0;
        if (i1 <= 30) nu = double(i1);
        else if (i1 <= 40) nu = double(i1-30)*100;
        else assert(false);

        for(int i2=0; i2<NBINS_SIGMA; i2++)
        {
            double sigma = i2/double(NBINS_SIGMA);

            for(int i3=1; i3<=NBINS_P; i3++)
            {
                double p = i3*0.1/double(NBINS_P);

                //for(int i4=1; i4<=100; i4++)
                {
                    int n = 1000; //100*i4; //ns[i4];

                    try
                    {
                        estimate bn = approx(sigma, nu, p, n);
                        cout << sigma << "\t" << nu << "\t" <<
                                p << "\t" << n << "\t" <<
                                bn.n << "\t" << bn.p <<
                                endl;
                    }
                    catch(...)
                    {
                        // not feasible parameters
                    }
                }
            }
        }
    }
}
