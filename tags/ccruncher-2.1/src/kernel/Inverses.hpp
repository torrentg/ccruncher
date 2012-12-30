#ifndef _Inverses_
#define _Inverses_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>
#include <vector>
#include <gsl/gsl_spline.h>
#include "params/DefaultProbabilities.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Inverses
{

  private:

    // initial date
    Date t0;
    // ending date
    Date t1;
    // degrees of freedom (gaussian = negative or 0)
    double ndf;
    // cubic splines
    vector<gsl_spline*> splines;

  private:

    // inverse cumulative distribution function (gaussian/t-student)
    double icdf(double) const;
    // return minimum day
    int getMinDay(int irating, const DefaultProbabilities &dprobs) const;
    // set splines
    void setSplines(const DefaultProbabilities &dprobs) throw(Exception);
    // set spline
    void setSpline(int irating, vector<int> &days, vector<double> &cache);
    // return the worst unaccurate day
    int getWorstDay(int irating, vector<double> &cache);

  public:

    // default constructor
    Inverses();
    // constructor
    Inverses(double ndf, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception);
    //copy constructor
    Inverses(const Inverses &);
    // destructor
    ~Inverses();
    // assignment operator
    Inverses & operator=(const Inverses &);
    // initialize
    void init(double ndf, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception);
    // evalue (return days from t0)
    double evalue(int irating, double val) const;
    // evalue (return date)
    Date evalueAsDate(int irating, double val) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// evalue
//===========================================================================
inline double ccruncher::Inverses::evalue(int irating, double val) const
{
  assert(irating < (int)splines.size());

  gsl_spline *spline = splines[irating];

  if (spline == NULL)
  {
    // default rating
    return 0.0;
  }

  size_t n = spline->size-1;
  if (spline->x[n] < val)
  {
    // default date bigger than maximum date
    return spline->y[n] + 100.0;
  }
  else if (val <= spline->x[0])
  {
    // default in less than 1 day (or minday)
    return spline->y[0];
  }
  else
  {
    // we don't use accel because values are random
    return gsl_spline_eval(spline, val, NULL);
  }
}

//===========================================================================
// evalueAsDate
//===========================================================================
inline Date ccruncher::Inverses::evalueAsDate(int irating, double val) const
{
  double days = evalue(irating, val);
  return t0 + (long)ceil(days);
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
