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
    void setSpline(int irating, const DefaultProbabilities &dprobs, vector<int> &days, vector<double> &cache);
    // return the worst unaccurate day
    int getWorstDay(int irating, const DefaultProbabilities &dprobs, vector<double> &cache);

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
    double evalueAsNum(int irating, double val) const;
    // evalue (return date)
    Date evalueAsDate(int irating, double val) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// evalueAsNum
//===========================================================================
inline double ccruncher::Inverses::evalueAsNum(int irating, double val) const
{
  assert(irating < (int)splines.size());

  if (splines[irating] == NULL)
  {
    // default rating
    return 0.0;
  }

  size_t n = splines[irating]->size-1;
  if (splines[irating]->x[n] < val)
  {
    // default date bigger than maximum date
    return splines[irating]->y[n] + 100.0;
  }
  else if (val <= splines[irating]->x[0])
  {
    // default in less than 1 day (or minday)
    return splines[irating]->y[0];
  }
  else
  {
    // we don't use accel because values are random
    return gsl_spline_eval(splines[irating], val, NULL);
  }
}

//===========================================================================
// evalueAsDate
//===========================================================================
inline Date ccruncher::Inverses::evalueAsDate(int irating, double val) const
{
  double days = evalueAsNum(irating, val);
  return t0 + (long)ceil(days);
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
