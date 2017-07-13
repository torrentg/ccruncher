
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <limits>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf_gamma.h>
#include "inference/Metropolis.hpp"

using namespace std;
using namespace libconfig;
using namespace ccruncher;
using namespace ccruncher_inf;

/*
  DEFAULT_NU has a low value because NU bigger hasn't slope
  to decide the direction to move.
  Caution: too low value also present problems.
  Solution: try diferents NU.value values reinitializing
  using *.state file.
 */

#define LOG2 0.693147180559945309417232121
#define DEFAULT_W 0.2
#define DEFAULT_R 0.0
#define DEFAULT_Z 0.0
#define DEFAULT_NU 15.0

//===========================================================================
// @param[in] seed RNG seed.
//===========================================================================
ccruncher_inf::Metropolis::Metropolis(unsigned long seed)
    : rng(nullptr), M0(nullptr), M1(nullptr)
{
  fstop = false;
  nfactors = 0;
  nratings = 0;
  nobs = 0;
  det0 = 0.0;
  det1 = 0.0;
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  if (seed == 0UL) {
    seed = (unsigned long) time(nullptr);
  }
  gsl_rng_set(rng, seed);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher_inf::Metropolis::~Metropolis()
{
  if (M0 != nullptr) gsl_matrix_free(M0);
  if (M1 != nullptr) gsl_matrix_free(M1);
  if (rng != nullptr) gsl_rng_free(rng);
}

//===========================================================================
// read array values from a configuration item
//===========================================================================
template<typename T>
vector<T> ccruncher_inf::Metropolis::readArray(const Setting &setting)
{
  if (!setting.isArray()) {
    throw Exception(setting.getPath() + " is not an array");
  }

  vector<T> ret;
  for(int i=0; i<setting.getLength(); i++) {
    T val = setting[i];
    ret.push_back(val);
  }

  return ret;
}

//===========================================================================
// read array values from a configuration item
//===========================================================================
template<typename T>
vector<T> ccruncher_inf::Metropolis::readArray(const Setting &setting, size_t nvals)
{
  vector<T> ret;

  if (setting.isArray()) {
    ret = readArray<T>(setting);
  }
  else {
    double aux = setting;
    ret = vector<T>(nvals, aux);
  }

  if (ret.size() != nvals) {
      throw Exception(setting.getPath() + " has invalid dim");
  }

  return ret;
}

//===========================================================================
// read matrix values from a configuration item
//===========================================================================
template<typename T>
vector<vector<T>> ccruncher_inf::Metropolis::readMatrix(const Setting &setting)
{
  if (!setting.isList()) {
    throw Exception(setting.getPath() + " is not a list of rows");
  }

  vector<vector<T>> ret;
  for(int i=0; i<setting.getLength(); i++)
  {
    vector<T> row = readArray<T>(setting[i]);
    ret.push_back(row);
    if (row.size() != ret[0].size()) {
      throw Exception(setting.getPath() + " with invalid row size");
    }
  }

  return ret;
}

//===========================================================================
// read matrix values from a configuration item
//===========================================================================
template<typename T>
vector<vector<T>> ccruncher_inf::Metropolis::readMatrix(const Setting &setting, size_t nrows, size_t ncols)
{
  vector<vector<T>> ret;

  if (setting.isList()) {
    ret = readMatrix<T>(setting);
  }
  else {
    vector<double> aux = readArray<T>(setting, ncols);
    ret = vector<vector<T>>(nrows, aux);
  }

  if (ret.size() != nrows || ret[0].size() != ncols) {
    throw Exception(setting.getPath() + " has invalid dim");
  }

  return ret;
}

//===========================================================================
// check for duplicates
//===========================================================================
template<typename T>
bool ccruncher_inf::Metropolis::hasDuplicates(const vector<T> &v)
{
  for(size_t i=0; i<v.size(); i++) {
    for(size_t j=i+1; j<v.size(); j++) {
      if (v[i] == v[j]) {
        return true;
      }
    }
  }
  return false;
}

//===========================================================================
// check if array constains the given value
//===========================================================================
template<typename T>
bool ccruncher_inf::Metropolis::containsValue(const vector<T> &v, const T &value)
{
  for(size_t i=0; i<v.size(); i++) {
    if (v[i] == value) {
      return true;
    }
  }
  return false;
}

//===========================================================================
// read factors
//===========================================================================
void ccruncher_inf::Metropolis::readFactors(const Config &config)
{
  factors = readArray<string>(config.lookup("factors"));
  if (factors.empty()) {
    throw Exception("no factors founds");
  }
  if (hasDuplicates(factors)) {
    throw Exception("found a duplicated factor");
  }
  nfactors = factors.size();
}

//===========================================================================
// read ratings and dprobs
//===========================================================================
void ccruncher_inf::Metropolis::readRatings(const Config &config)
{
  ratings = readArray<string>(config.lookup("ratings"));
  if (ratings.empty()) {
    throw Exception("no ratings found");
  }
  if (hasDuplicates(ratings)) {
    throw Exception("found a duplicated rating");
  }
  nratings = ratings.size();

  dprobs = readArray<double>(config.lookup("dprobs"));
  if (nratings != dprobs.size()) {
    throw Exception("length(ratings) !=  length(dprobs)");
  }
  for(size_t i=0; i<dprobs.size(); i++) {
    if(dprobs[i] <= 0.0 || 1.0 <= dprobs[i]) {
      throw Exception("dprobs["+to_string(i+1)+"] value out of range (0,1)");
    }
  }
}

//===========================================================================
// read the number of obligors
//===========================================================================
void ccruncher_inf::Metropolis::readNobligors(const Config &config)
{
  nobligors = readMatrix<int>(config.lookup("nobligors"));
  if (nobligors.size() < 3) throw Exception("nobligors has less than 3 rows");
  for(size_t i=0; i<nobligors.size(); i++) {
    if (nobligors[i].size() != nfactors*nratings) {
      throw Exception("nobligors["+to_string(i+1)+"] has invalid length");
    }
    for(size_t j=0; j<nobligors[i].size(); j++) {
      if(nobligors[i][j] < 0) {
        throw Exception("nobligors["+to_string(i+1)+","+to_string(j+1)+"] value out of range [0,inf)");
      }
    }
  }
  nobs = nobligors.size();
}

//===========================================================================
// read the number of defaults
//===========================================================================
void ccruncher_inf::Metropolis::readNdefaults(const Config &config)
{
  ndefaults = readMatrix<int>(config.lookup("ndefaults"));
  if (ndefaults.size() != nobs) throw Exception("nobligors and ndefaults have distinct length");
  for(size_t i=0; i<ndefaults.size(); i++) {
    if (ndefaults[i].size() != nobligors[i].size()) {
      throw Exception("ndefaults["+to_string(i+1)+"] has invalid length");
    }
    for(size_t j=0; j<ndefaults[i].size(); j++) {
      if(nobligors[i][j] < ndefaults[i][j]) {
        string str = "["+to_string(i+1)+","+to_string(j+1)+"]";
        throw Exception("ndefaults"+str+" value bigger than nobligors"+str);
      }
    }
  }
}

//===========================================================================
// read nu component
//===========================================================================
void ccruncher_inf::Metropolis::readNu(const Config &config)
{
  double sigma = config.lookup("NU.sigma");
  double level = config.lookup("NU.level");
  double value = DEFAULT_NU;
  if (sigma == 0.0 || config.exists("NU.value")) {
    value = config.lookup("NU.value");
  }
  if (value < 2.0) throw Exception("NU.value out of range [2,inf)");

  nu.init("NU", value, sigma, level);
}

//===========================================================================
// read W component
//===========================================================================
void ccruncher_inf::Metropolis::readW(const Config &config)
{
  vector<double> sigmas = readArray<double>(config.lookup("W.sigma"));
  if (sigmas.size() != nfactors) throw Exception("length(W.sigma) != length(factors)");

  vector<double> levels = readArray<double>(config.lookup("W.level"));
  if (levels.size() != nfactors) throw Exception("length(W.level) != length(nfactors)");

  vector<double> values(nfactors, DEFAULT_W);
  bool hasSigmaEq0 = containsValue(sigmas, 0.0);
  if (hasSigmaEq0 || config.exists("W.value")) {
    values = readArray<double>(config.lookup("W.value"));
    if (values.size() != nfactors) throw Exception("length(W.value) != length(nfactors)");
    for(size_t i=0; i<values.size(); i++) {
      if (values[i] <= 0.0 || 1.0 <= values[i]) {
        throw Exception("W.value["+to_string(i+1)+"] out of range (0,1)");
      }
    }
  }

  W.resize(nfactors);
  for(size_t i=0; i<W.size(); i++) {
    W[i].init("W", values[i], sigmas[i], levels[i]);
  }
}

//===========================================================================
// read R component
//===========================================================================
void ccruncher_inf::Metropolis::readR(const Config &config)
{
  size_t ncors = (nfactors*(nfactors-1))/2;

  vector<double> sigmas = readArray<double>(config.lookup("R.sigma"));
  if (sigmas.size() != ncors) throw Exception("length(R.sigma) != (nfactors*(nfactors-1))/2");

  vector<double> levels = readArray<double>(config.lookup("R.level"));
  if (levels.size() != ncors) throw Exception("length(R.level) != (nfactors*(nfactors-1))/2");

  vector<double> values(nfactors, DEFAULT_R);
  bool hasSigmaEq0 = containsValue(sigmas, 0.0);
  if (hasSigmaEq0 || config.exists("R.value")) {
    values = readArray<double>(config.lookup("R.value"));
    if (values.size() != ncors) throw Exception("length(R.value) != (nfactors*(nfactors-1))/2");
    for(size_t i=0; i<values.size(); i++) {
      if (values[i] <= -1.0 || 1.0 <= values[i]) {
        throw Exception("R.value["+to_string(i+1)+"] out of range (-1,+1)");
      }
    }
  }

  R.resize(nfactors, vector<Component<double>>(nfactors));
  for(size_t i=0,pos=0; i<R.size(); i++) {
    R[i][i].init("R", 1.0, 0.0, 0.0); // diagonal is 1
    for(size_t j=i+1; j<nfactors; j++) {
      R[i][j].init("R", values[pos], sigmas[pos], levels[pos]);
      pos++;
    }
  }
}

//===========================================================================
// read Z component
//===========================================================================
void ccruncher_inf::Metropolis::readZ(const Config &config)
{
  vector<double> sigmas = readArray<double>(config.lookup("Z.sigma"), nobs);
  vector<double> levels = readArray<double>(config.lookup("Z.level"), nobs);

  vector<vector<double>> values(nobs, vector<double>(nfactors, DEFAULT_Z));
  bool hasSigmaEq0 = containsValue(sigmas, 0.0);
  if (hasSigmaEq0 || config.exists("Z.value")) {
    values = readMatrix<double>(config.lookup("Z.value"), nobs, nfactors);
  }

  Z.resize(nobs);
  for(size_t i=0; i<Z.size(); i++) {
    Z[i].init("Z["+to_string(i+1)+"]", values[i], sigmas[i], levels[i]);
  }
}

//===========================================================================
// read S component (chi-squared latent variables)
//===========================================================================
void ccruncher_inf::Metropolis::readS(const Config &config)
{
  vector<double> sigmas = readArray<double>(config.lookup("S.sigma"), nobs);
  //TODO: check range (0,inf)
  vector<double> levels = readArray<double>(config.lookup("S.level"), nobs);
  //TODO: check range (0,1)

  vector<double> values(nobs, nu.value);
  bool hasSigmaEq0 = containsValue(sigmas, 0.0);
  if (hasSigmaEq0 || config.exists("S.value")) {
    values = readArray<double>(config.lookup("S.value"), nobs);
  }

  S.resize(nobs);
  for(size_t i=0; i<S.size(); i++) {
    if (values[i] <= 0) throw Exception("S.value value out of range (0,inf)");
    S[i].init("S", values[i], sigmas[i], levels[i]);
  }
}

//===========================================================================
// @param[in] config Configuration object.
// @throw Exception Invalid configuration values.
//===========================================================================
void ccruncher_inf::Metropolis::init(const Config &config)
{
  // reading data
  readFactors(config);
  readRatings(config);
  readNobligors(config);
  readNdefaults(config);
  readNu(config);
  readW(config);
  readR(config);
  readZ(config);
  readS(config);

  // allocating precomputed values (I)
  M0 = gsl_matrix_alloc(nfactors, nfactors);
  lb0 = vector<vector<double>>(nobs, vector<double>(nratings*nfactors));
  mz0.resize(nobs);
  cs0.resize(nobs);
  ti0.resize(nratings);

  // allocating precomputed values (II)
  M1 = gsl_matrix_alloc(nfactors, nfactors);
  lb1 = lb0;
  mz1 = mz0;
  cs1 = cs0;
  ti1 = ti0;

  // precomputing M0 and det0
  v2m(M0);
  gsl_linalg_cholesky_decomp(M0);
  det0 = det(M0);
  gsl_linalg_cholesky_invert(M0);

  // precomputing tcdfinv to avoid botlenecks
  for(size_t r=0; r<nratings; r++) {
    ti0[r] = gsl_cdf_tdist_Pinv(dprobs[r], nu.value);
  }

  // precomputing lbinom values
  for(size_t n=0; n<nobs; n++) {
    for(size_t s=0; s<nfactors; s++) {
      for(size_t r=0; r<nratings; r++) {
        size_t k = s*nratings + r;
        lb0[n][k] = lbinom(nobligors[n][k], ndefaults[n][k], ti0[r], nu.value,  W[s].value, Z[n].value[s], S[n].value);
      }
    }
  }

  // precomputing qprod values
  for(size_t n=0; n<nobs; n++) {
    mz0[n] = -0.5 * qprod(Z[n].value, M0);
  }

  // precomputing chi-square values
  for(size_t n=0; n<nobs; n++) {
    cs0[n] = (nu.value/2.0-1.0)*log(S[n].value);
  }
}

//===========================================================================
// @param[in] output Stream to write simulated values.
// @param[in] info Stream to write log info.
// @param[in] blocksize Number of items used to compute the acceptance rate.
// @param[in] maxiters Maximum number of iterations (=0 -> non-stop).
// @param[in] burnin Number of initial simulations skiped.
// @throw Exception Error running MCMC.
//===========================================================================
int ccruncher_inf::Metropolis::run(ostream &output, ostream &info, size_t blocksize, size_t maxiters, size_t burnin)
{
  size_t numsims = 0;
  fstop = false;

  // resize components to blocksize
  for(size_t i=0; i<W.size(); i++) W[i].resize(blocksize);
  for(size_t i=0; i<R.size(); i++) for(size_t j=i+1; j<nfactors; j++) R[i][j].resize(blocksize);
  for(size_t i=0; i<Z.size(); i++) Z[i].resize(blocksize);
  for(size_t i=0; i<S.size(); i++) S[i].resize(blocksize);
  nu.resize(blocksize);

  // formating traces
  info.setf(ios::fixed, ios::floatfield);
  info.precision(4);
  info << "CONCEPT\t"; writeHeader(info, nfactors);
  info.setf(ios::showpos);
  info << "VALUE\t"; writeValues(info); info << endl;
  writeHeader(output, nfactors);

  vector<int> shuffler(5);
  for(int i=0; i<5; i++) shuffler[i] = i;

  // main loop
  do
  {
    // shuffling components
    random_shuffle(shuffler.begin(), shuffler.end());

    // updating components
    for(size_t i=0; i<5; i++)
    {
      if (shuffler[i] == 0) updateNu();
      else if (shuffler[i] == 1) updateW();
      else if (shuffler[i] == 2) updateR();
      else if (shuffler[i] == 3) updateZ();
      else if (shuffler[i] == 4) updateS();
      else assert(false);
    }
    numsims++;

    if (numsims > burnin) {
      writeValues(output);
    }

    if ((blocksize > 0 && numsims%blocksize == 0) || fstop == true)
    {
      output.flush();

      info << "NSIMS\t" << numsims << "\n";
      info << "VALUE\t"; writeValues(info);
      info << "PARAM\t"; writeParams(info);
      info << "ACRAT\t"; writeAcRates(info);
      info << endl;
    }

    if (maxiters > 0 && numsims >= maxiters) {
      fstop = true;
    }

  }
  while(!fstop);

  return numsims;
}

//===========================================================================
// update factor loadings
// http://darrenjw.wordpress.com/2012/06/04/metropolis-hastings-mcmc-when-the-proposal-and-target-have-differing-support/
// we don't shuffle because they are independents
//===========================================================================
void ccruncher_inf::Metropolis::updateW()
{
  for(size_t s=0; s<nfactors; s++) {
    if (!W[s].isFixed()) {
      updateW(s);
    }
  }
}

void Metropolis::updateW(size_t s)
{
  assert(s < nfactors);

  double w0 = W[s].value;
  double w1 = w0 + gsl_ran_gaussian(rng, W[s].sigma);

  if (w1 < 0.0 || 1.0 < w1)
  {
    W[s].setAccepted(false);
  }
  else
  {
    double loga = 0.0;

#pragma omp parallel for reduction(+:loga) schedule(dynamic)
    for(size_t n=0; n<nobs; n++) {
      for(size_t r=0; r<nratings; r++) {
        size_t k = s*nratings+r;
        // binomial part
        lb1[n][k] = lbinom(nobligors[n][k], ndefaults[n][k], ti0[r], nu.value, w1, Z[n].value[s], S[n].value);
        loga += lb1[n][k] - lb0[n][k];
      }
    }

    double u = log(gsl_rng_uniform_pos(rng));
    bool accepted = (u<loga);
    W[s].setAccepted(accepted);

    if (accepted) {
      W[s].value = w1;
#pragma omp parallel for schedule(dynamic)
      for(size_t n=0; n<nobs; n++) {
        for(size_t r=0; r<nratings; r++) {
          size_t k = s*nratings+r;
          lb0[n][k] = lb1[n][k];
        }
      }
    }
  }
}

//===========================================================================
// update correlations
// http://darrenjw.wordpress.com/2012/06/04/metropolis-hastings-mcmc-when-the-proposal-and-target-have-differing-support/
//===========================================================================
void ccruncher_inf::Metropolis::updateR()
{
  vector<pair<size_t,size_t> > shuffler((nfactors*(nfactors-1))/2);
  size_t pos = 0;
  for(size_t s1=0; s1<nfactors; s1++) {
    for(size_t s2=s1+1; s2<nfactors; s2++) {
      shuffler[pos] = pair<size_t,size_t>(s1,s2);
      pos++;
    }
  }
  assert(pos == (nfactors*(nfactors-1))/2);
  random_shuffle(shuffler.begin(), shuffler.end());
  for(size_t i=0; i<shuffler.size(); i++) {
    size_t s1 = shuffler[i].first;
    size_t s2 = shuffler[i].second;
    if (!R[s1][s2].isFixed()) {
      updateR(s1, s2);
    }
  }
}

void Metropolis::updateR(size_t s1, size_t s2)
{
  assert(s1 < nfactors);
  assert(s1 < s2 && s2 < nfactors);

  double r0 = R[s1][s2].value;
  double r1 = r0 + gsl_ran_gaussian(rng, R[s1][s2].sigma);

  if (r1 < -1.0 || 1.0 < r1)
  {
    R[s1][s2].setAccepted(false);
  }
  else
  {
    v2m(M1);
    gsl_matrix_set(M1, s1, s2, r1);
    gsl_matrix_set(M1, s2, s1, r1);

    gsl_error_handler_t *eh = gsl_set_error_handler_off();
    int rc = gsl_linalg_cholesky_decomp(M1);
    gsl_set_error_handler(eh);
    if (rc != GSL_SUCCESS)
    {
      R[s1][s2].setAccepted(false);
    }
    else
    {
      det1 = det(M1);
      gsl_linalg_cholesky_invert(M1);

      double loga = 0.0;

#pragma omp parallel for reduction(+:loga) schedule(dynamic)
      for(size_t n=0; n<nobs; n++) {
        // multivariate normal part (I)
        mz1[n] = -0.5 * qprod(Z[n].value, M1);
        loga += mz1[n] - mz0[n];
      }

      // multivariate normal part (II)
      loga += -double(nobs)*log(det1); // detx = sqrt(det(Mx))
      loga -= -double(nobs)*log(det0); // detx = sqrt(det(Mx))

      double u = log(gsl_rng_uniform_pos(rng));
      bool accepted = (u<loga);
      R[s1][s2].setAccepted(accepted);

      if (accepted) {
        R[s1][s2].value = r1;
        gsl_matrix *aux = M0;
        M0 = M1;
        M1 = aux;
        det0 = det1;
        mz0 = mz1;
      }
    }
  }
}

//===========================================================================
// update normal latent variables
// we don't shuffle because they are independents
// TODO: actualitzar component a component?
//===========================================================================
void ccruncher_inf::Metropolis::updateZ()
{
#pragma omp parallel for schedule(dynamic)
  for(size_t n=0; n<nobs; n++) {
    if (!Z[n].isFixed()) {
      updateZ(n);
    }
  }
}

void ccruncher_inf::Metropolis::updateZ(size_t n)
{
  assert(n < nobs);

  double u = 0;
  vector<double> z0(Z[n].value);
  vector<double> z1(nfactors);

#pragma omp critical
{
  for(size_t s=0; s<nfactors; s++) {
    z1[s] = z0[s] + gsl_ran_gaussian(rng, Z[n].sigma);
  }
  u = gsl_rng_uniform_pos(rng);
}

  double loga = 0.0;

  // multivariate normal part
  mz1[n] = -0.5 * qprod(z1, M0);
  loga += mz1[n] - mz0[n];

  // binomial part
  for(size_t s=0; s<nfactors; s++) {
    for(size_t r=0; r<nratings; r++) {
      size_t k = s*nratings+r;
      lb1[n][k] = lbinom(nobligors[n][k], ndefaults[n][k], ti0[r], nu.value, W[s].value, z1[s], S[n].value);
      loga += lb1[n][k] - lb0[n][k];
    }
  }

  bool accepted = (log(u)<loga);
  Z[n].setAccepted(accepted);

  if (accepted) {
    Z[n].value = z1;
    mz0[n] = mz1[n];
    lb0[n] = lb1[n];
  }
}

//===========================================================================
// update chi-square latent variables
// we don't shuffle because they are independents
//===========================================================================
void ccruncher_inf::Metropolis::updateS()
{
#pragma omp parallel for schedule(dynamic)
  for(size_t n=0; n<nobs; n++) {
    if (!S[n].isFixed()) {
      updateS(n);
    }
  }
}

void Metropolis::updateS(size_t n)
{
  assert(n < nobs);

  double u = 0.0;
  double s0 = S[n].value;
  double s1 = 0.0;

#pragma omp critical
{
  s1 = s0 + gsl_ran_gaussian(rng, S[n].sigma);
  u = gsl_rng_uniform_pos(rng);
}

  if (s1 < 0.0)
  {
    S[n].setAccepted(false);
  }
  else
  {
    double loga = 0.0;
    // chi-square part
    cs1[n] = (nu.value/2.0-1.0)*log(s1);
    loga += (cs1[n] - s1/2.0) - (cs0[n] - s0/2.0);
    // binomial part
    for(size_t s=0; s<nfactors; s++) {
      for(size_t r=0; r<nratings; r++) {
        size_t k = s*nratings+r;
        lb1[n][k] = lbinom(nobligors[n][k], ndefaults[n][k], ti0[r], nu.value, W[s].value, Z[n].value[s], s1);
        loga += lb1[n][k] - lb0[n][k];
      }
    }

    bool accepted = (log(u)<loga);
    S[n].setAccepted(accepted);

    if (accepted) {
      S[n].value = s1;
      cs0[n] = cs1[n];
      lb0[n] = lb1[n];
    }
  }
}

//===========================================================================
// update degrees of freedom
//===========================================================================
void ccruncher_inf::Metropolis::updateNu()
{
  if (nu.isFixed()) {
    return;
  }

  double nu0 = nu.value;
  double nu1 = nu0 + gsl_ran_gaussian(rng, nu.sigma);

  if (nu1 < 2.0 || 1000.0 < nu1)
  {
    nu.setAccepted(false);
  }
  else
  {
    for(size_t r=0; r<nratings; r++) {
      ti1[r] = gsl_cdf_tdist_Pinv(dprobs[r], nu1);
    }

    double loga = 0.0;
#pragma omp parallel for reduction(+:loga) schedule(dynamic)
    for(size_t n=0; n<nobs; n++) {
      // chi-square part (I)
      cs1[n] = (nu1/2.0-1.0)*log(S[n].value);
      loga += cs1[n] - cs0[n];
      // binomial part
      for(size_t s=0; s<nfactors; s++) {
        for(size_t r=0; r<nratings; r++) {
          size_t k = s*nratings+r;
          lb1[n][k] = lbinom(nobligors[n][k], ndefaults[n][k], ti1[r], nu1, W[s].value, Z[n].value[s], S[n].value);
          loga += lb1[n][k] - lb0[n][k];
        }
      }
    }

    // chi-square part(II)
    loga += -double(nobs)*(0.5*nu1*LOG2 + gsl_sf_lngamma(nu1/2.0));
    loga -= -double(nobs)*(0.5*nu0*LOG2 + gsl_sf_lngamma(nu0/2.0));

    double u = log(gsl_rng_uniform_pos(rng));
    bool accepted = (u<loga);
    nu.setAccepted(accepted);

    if (accepted) {
      nu.value = nu1;
      ti0 = ti1;
      lb0 = lb1;
      cs0 = cs1;
    }
  }
}

//===========================================================================
// vector to matrix
//===========================================================================
void ccruncher_inf::Metropolis::v2m(gsl_matrix *m) const
{
  assert(m->size1 == nfactors);
  assert(m->size2 == nfactors);

  for(size_t i=0; i<nfactors; i++) {
    gsl_matrix_set(m, i, i, 1.0);
    for(size_t j=i+1; j<nfactors; j++) {
      gsl_matrix_set(m, i, j, R[i][j].value);
      gsl_matrix_set(m, j, i, R[i][j].value);
    }
  }
}

//===========================================================================
// determinant of a Cholesky matrix, returns det(L)
// obs: R=L.L' -> det(R)=det(L)^2 -> sqrt(det(R))=det(L)
//===========================================================================
double ccruncher_inf::Metropolis::det(gsl_matrix *L)
{
  assert(L->size1 == L->size2);

  double ret = 1.0;
  for(unsigned int i=0; i<L->size1; i++) ret *= gsl_matrix_get(L, i, i);
  return ret;
}

//===========================================================================
// quadratic prod (v'.M.v)
// where M is a definite-positive symmetric matrix
// TODO: allocar memoria i eliminar param aux
//===========================================================================
double ccruncher_inf::Metropolis::qprod(const vector<double> &v, const gsl_matrix *M)
{
  double ret = 0.0;
  gsl_vector x, *y;

  y = gsl_vector_alloc(v.size());
  gsl_vector_set_all(y, 0.0);

  x.size = v.size();
  x.stride = 1;
  x.data = (double *) &(v[0]);
  x.block = nullptr;
  x.owner = 0;

  gsl_blas_dsymv(CblasLower, 1.0, M, &x, 0.0, y);
  //gsl_blas_dgemv(CblasNoTrans, 1.0, M, &x, 0.0, y);
  gsl_blas_ddot(&x, y, &ret);

  gsl_vector_free(y);

  return ret;
}

//===========================================================================
// create a setting
//===========================================================================
Setting& ccruncher_inf::Metropolis::createSetting(const string &name, Setting &parent, enum Setting::Type type)
{
  if(parent.exists(name)) {
    parent.remove(name);
  }
  return parent.add(name, type);
}

template<typename T>
Setting& ccruncher_inf::Metropolis::writeArray(const string &name, Setting &parent, enum Setting::Type type, const vector<T> &values)
{
  Setting &setting = createSetting(name, parent, Setting::TypeArray);
  for(size_t i=0; i < values.size(); i++) {
    setting.add(type) = values[i];
  }
  return setting;
}

template<typename T>
Setting& ccruncher_inf::Metropolis::writeMatrix(const string &name, Setting &parent, enum Setting::Type type, const vector<vector<T>> &values)
{
  Setting &setting = createSetting(name, parent, Setting::TypeList);
  for(size_t i=0; i < values.size(); i++) {
    Setting &row = setting.add(Setting::TypeArray);
    for(size_t j=0; j<values[i].size(); j++) {
      row.add(type) = values[i][j];
    }
  }
  return setting;
}

//===========================================================================
// returns current state
//===========================================================================
Config& ccruncher_inf::Metropolis::getState()
{
  Setting &root = mConfig.getRoot();
  vector<double> vValues, vSigmas, vLevels;
  vector<vector<double>> mValues;

  mConfig.setOptions(Setting::OptionNone);
  mConfig.setTabWidth(2); // 0 = tab

  writeArray<string>("factors", root, Setting::TypeString, factors);
  writeArray<string>("ratings", root, Setting::TypeString, ratings);
  writeArray<double>("dprobs", root, Setting::TypeFloat, dprobs);
  writeMatrix<int>("nobligors", root, Setting::TypeInt, nobligors);
  writeMatrix<int>("ndefaults", root, Setting::TypeInt, ndefaults);

  Setting &sNU = createSetting("NU", root, Setting::TypeGroup);
  sNU.add("value", Setting::TypeFloat) = nu.value;
  sNU.add("sigma", Setting::TypeFloat) = nu.sigma;
  sNU.add("level", Setting::TypeFloat) = nu.level;

  vValues.clear();
  vSigmas.clear();
  vLevels.clear();
  for(size_t i=0; i<W.size(); i++) {
    vValues.push_back(W[i].value);
    vSigmas.push_back(W[i].sigma);
    vLevels.push_back(W[i].level);
  }
  Setting &sW = createSetting("W", root, Setting::TypeGroup);
  writeArray<double>("value", sW, Setting::TypeFloat, vValues);
  writeArray<double>("sigma", sW, Setting::TypeFloat, vSigmas);
  writeArray<double>("level", sW, Setting::TypeFloat, vLevels);

  vValues.clear();
  vSigmas.clear();
  vLevels.clear();
  for(size_t i=0; i<R.size(); i++) {
    for(size_t j=i+1; j<R.size(); j++) {
      vValues.push_back(R[i][j].value);
      vSigmas.push_back(R[i][j].sigma);
      vLevels.push_back(R[i][j].level);
    }
  }
  Setting &sR = createSetting("R", root, Setting::TypeGroup);
  writeArray<double>("value", sR, Setting::TypeFloat, vValues);
  writeArray<double>("sigma", sR, Setting::TypeFloat, vSigmas);
  writeArray<double>("level", sR, Setting::TypeFloat, vLevels);

  mValues.clear();
  vSigmas.clear();
  vLevels.clear();
  for(size_t i=0; i<Z.size(); i++) {
    mValues.push_back(Z[i].value);
    vSigmas.push_back(Z[i].sigma);
    vLevels.push_back(Z[i].level);
  }
  Setting &sZ = createSetting("Z", root, Setting::TypeGroup);
  writeMatrix<double>("value", sZ, Setting::TypeFloat, mValues);
  writeArray<double>("sigma", sZ, Setting::TypeFloat, vSigmas);
  writeArray<double>("level", sZ, Setting::TypeFloat, vLevels);

  vValues.clear();
  vSigmas.clear();
  vLevels.clear();
  for(size_t i=0; i<S.size(); i++) {
    vValues.push_back(S[i].value);
    vSigmas.push_back(S[i].sigma);
    vLevels.push_back(S[i].level);
  }
  Setting &sS = createSetting("S", root, Setting::TypeGroup);
  writeArray<double>("value", sS, Setting::TypeFloat, vValues);
  writeArray<double>("sigma", sS, Setting::TypeFloat, vSigmas);
  writeArray<double>("level", sS, Setting::TypeFloat, vLevels);

  return mConfig;
}

//===========================================================================
// writeHeader
//===========================================================================
void ccruncher_inf::Metropolis::writeHeader(ostream &s, int k) const
{
  s << "NU\t";
  for(int i=0; i<k; i++) s << "W" << i+1 << "\t";
  for(int i=0; i<k; i++) {
    for(int j=i+1; j<k; j++) {
      s << "R" << i+1 << j+1 << "\t";
    }
  }
  s << "Z\tS\n";
}

//===========================================================================
// write values
//===========================================================================
void ccruncher_inf::Metropolis::writeValues(ostream &s) const
{
  s << nu.value << "\t";
  for(size_t i=0; i<W.size(); i++) s << W[i].value << "\t";
  for(size_t i=0; i<nfactors; i++) {
    for(size_t j=i+1; j<nfactors; j++) {
      s << R[i][j].value << "\t";
    }
  }
  double avg = 0.0;
  for(size_t i=0; i<nobs; i++) for(size_t j=0; j<nfactors; j++) avg += Z[i].value[j];
  s << avg/double(nobs*nfactors) << "\t";
  avg = 0.0;
  for(size_t i=0; i<S.size(); i++) avg += S[i].value;
  s << avg/S.size();
  s << "\n";
}

//===========================================================================
// write acceptance rates
//===========================================================================
void ccruncher_inf::Metropolis::writeAcRates(ostream &s) const
{
  s << nu.getAccRate() << "\t";
  for(size_t i=0; i<W.size(); i++) s << W[i].getAccRate() << "\t";
  for(size_t i=0; i<nfactors; i++) {
    for(size_t j=i+1; j<nfactors; j++) {
      s << R[i][j].getAccRate() << "\t";
    }
  }
  double avg = 0.0;
  for(size_t i=0; i<Z.size(); i++) avg += Z[i].getAccRate();
  s << avg/Z.size() << "\t";
  avg = 0.0;
  for(size_t i=0; i<S.size(); i++) avg += S[i].getAccRate();
  s << avg/S.size() << "\n";
}

//===========================================================================
// write acceptance params
//===========================================================================
void ccruncher_inf::Metropolis::writeParams(ostream &s) const
{
  s << nu.sigma << "\t";
  for(size_t i=0; i<W.size(); i++) s << W[i].sigma << "\t";
  for(size_t i=0; i<nfactors; i++) {
    for(size_t j=i+1; j<nfactors; j++) {
      s << R[i][j].sigma << "\t";
    }
  }
  double avg = 0.0;
  for(size_t i=0; i<Z.size(); i++) avg += Z[i].sigma;
  s << avg/Z.size() << "\t";
  avg = 0.0;
  for(size_t i=0; i<S.size(); i++) avg += S[i].sigma;
  s << avg/S.size() << "\n";
}

//===========================================================================
// logbinom
// n: number of obligors
// k: number of defaults
// a: precomputed threshold (= tcdfinv(dprob,nu)) <- to speedup
// ndf: degrees of freedom
// w: factor loading
// z: normal latent variable Z~N(0,w)
// s: chi-square latent variable
//===========================================================================
double ccruncher_inf::Metropolis::lbinom(int n, int k, double a, double ndf, double w, double z, double s)
{
  assert(0 <= k && k <= n);
  assert(2.0 <= ndf);
  assert(0.0 <= w && w <= 1.0);
  assert(0.0 < s);

  if (n == 0) return 0.0;

  //double a = gsl_cdf_tdist_Pinv(p, ndf);
  double prob = gsl_cdf_gaussian_P(a*sqrt(s/ndf)-w*z, sqrt(1.0-w*w));
  // binomial coefficient (can be removed because it is constant)
  //double coef = gsl_sf_lngamma(n+1) - gsl_sf_lngamma(k+1) - gsl_sf_lngamma(n-k+1);
  return k*log(prob) + (n-k)*log(1.0-prob); // + coef;
}
