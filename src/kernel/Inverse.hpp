#ifndef _Inverse_
#define _Inverse_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <cmath>
#include "params/Survivals.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Inverse
{

  private:

    // cubic spline coefficients
    struct csc
    {
      double coef[4];
    };

  private:

    // initial date
    Date t0;
    // ending date
    Date t1;
    // degrees of freedom (gaussian = negative or 0)
    double ndf;
    // limits
    vector<pair<double,double> > range;
    // interpolation coeficients
    vector<vector<csc> > data;

  private:

    // set ranges
    void setRanges(const Survivals &survivals);
    // set interpolation coefficients
    void setCoefs(const Survivals &survivals) throw(Exception);
    // set interpolation coefficients
    vector<csc> getCoefs(int irating, const Survivals &survivals, int nbreaks) throw(Exception);

  public:

    // constructor
    Inverse(double ndf, Date t0, Date t1, const Survivals &) throw(Exception);
    // evalue
    inline Date evalue(int irating, double val) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// evalue
//===========================================================================
Date ccruncher::Inverse::evalue(int irating, double val) const
{
  assert(!isnan(range[irating].first) && !isnan(range[irating].second));
  assert(data[irating].size() > 0);

  if (range[irating].second < val)
  {
    // defaulted in range (t1,inf)
    return t1+1;
  }
  else if (val <= range[irating].first)
  {
    // defaulted in range [t0,t0+1]
    return t0+1;
  }
  else
  {
    // defaulted in range (t0+1,t1]
    double val = data[irating].size()*(val-range[irating].first)/(range[irating].second-range[irating].first);
    double intpart;
    double x = modf(val, &intpart);
    int pos = floor(intpart);

    assert(0 <= pos && pos < data[irating].size());
    assert(0.0 <= x && x <= 1.0);

    const double *a = data[irating][pos].coef;
    double days = a[0] + x*(a[1]+x*(a[2]+x*a[3]));

    assert(1.0 <= days);

    return t0 + (long)days;
  }
}

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
