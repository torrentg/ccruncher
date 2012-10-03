
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

#include <cmath>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "math/TMFCopula.hpp"
#include <cassert>

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::TMFCopula::TMFCopula(const TMFCopula &x, bool alloc) throw(Exception) :
    GMFCopula(static_cast<const GMFCopula&>(x), alloc), ndf(x.ndf), tcdf(x.ndf)
{
  // nothing to do
}

//===========================================================================
// constructor
// M: factors correlation matrix (diag = factor loadings)
// n: number of elements at each sector
// k: number of sectors
// ndf: degrees of freedom
//===========================================================================
ccruncher::TMFCopula::TMFCopula(const vector<vector<double> > &M, const vector<unsigned int> &dims, double ndf_) throw(Exception) :
    GMFCopula(M, dims), ndf(ndf_), tcdf(ndf_)
{
  if (ndf < 2.0) {
    finalize();
    throw Exception("degrees of freedom out of range [2,inf)");
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::TMFCopula::~TMFCopula()
{
  // ~GMFCopula called automatically
}

//===========================================================================
// clone
//===========================================================================
Copula* ccruncher::TMFCopula::clone(bool alloc)
{
  return new TMFCopula(*this, alloc);
}

//===========================================================================
// copula simulation. Put in values a random vector where each marginal
// follows a U[0,1] related by a t-Student copula
//===========================================================================
void ccruncher::TMFCopula::next()
{
  // simulate a multivariate gaussian
  rmvnorm();

  // simulate the chi-square value
  double chisq =  gsl_ran_chisq(rng, ndf);
  if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
  double factor = sqrt(ndf/chisq);

  // inverse sampling method
  for(unsigned int pos=0, i=0; i<k; i++)
  {
    for(unsigned int j=0; j<n[i]; j++)
    {
      //values[pos] = gsl_cdf_tdist_P(factor*values[pos], ndf);
      values[pos] = tcdf.eval(factor*values[pos]);
      pos++;
    }
  }
}

