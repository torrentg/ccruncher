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

//===========================================================================
// factor
// return m!/(i!k!(m-k-i)!)
//===========================================================================
double factor(int m, int k, int i)
{
    assert(m-k-i+1 > 0);

    double ret;
    for(int j=0; j<=k+i-1; j++) ret += log(m-j);
    for(int j=1; j<=k; j++) ret -= log(j);
    for(int j=1; j<=i; j++) ret -= log(j);
    return (i%2==1?-1:+1)*exp(ret);
}

//===========================================================================
// prob
//===========================================================================
double prob(int k, int m, const vector<double> &values)
{
    double ret = 1.0;

    for(int i=1; i<(int)values.size(); i++)
    {
cout << "factor(" << i << ")=" << factor(m, k, i) << endl;
        ret += factor(m, k, i) * values[i-1];
    }

    return ret;
}

//===========================================================================
// approx
//===========================================================================
void approx(double sigma, double nu, double p, int n)
{
    int dims = n;
    double *aux = &sigma;
    double **C = &aux;
    BlockTStudentCopula copula(C, &dims, 1, nu, 0, false);
    vector<int> ndefaults(NUM_COPULA_REALIZATIONS, 0);
    vector<double> distr(n, 0);
    vector<double> values;

    for(int i=0; i<NUM_COPULA_REALIZATIONS; i++)
    {
        copula.next();
        for(int j=0; j<n; j++)
        {
            if (copula.get(j) < p)
            {
                ndefaults[i]++;
            }
        }

        distr[ndefaults[i]]++;
    }
/*
    for(int k=0; k<n; k++)
    {
        double sum = 0.0;

        for(int j=0; j<(int)ndefaults.size(); j++)
        {
            double prod = double(ndefaults[j])/double(n);
            for(int i=0; i<k; i++)
            {
                prod *= double(ndefaults[j]-i)/double(n-i);
            }
            sum += prod;
        }

        values.push_back(sum/ndefaults.size());
        cout << "pi\t" << (k+1) << "\t" << values.back() << endl;
        if (values[k] < 1e-12) break;
    }
*/

    for(int i=0; i<(int)distr.size(); i++)
    {
        distr[i] /= ndefaults.size();
        cout << "distr\t" << i << "\t" << distr[i] << endl; //"\t" << prob(i ,n, values) << endl;
    }

}

//===========================================================================
// main procedure
//===========================================================================
int main(int, char **)
{
    approx(0.1, 6, 0.05, 500);
    /*
    for(int i=0; i<10; i++) {
        cout << i << "\t" << factor(500, 3, i) << endl;
    }
    */
}
