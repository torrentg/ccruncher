
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include <vector>
#include <libconfig.h++>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_matrix.h>
#include "utils/Exception.hpp"
#include "inference/Component.hpp"

namespace ccruncher_inf {

/**************************************************************************//**
 * @brief   Executes the CCruncher Metropolis-Hastings algorithm.
 *
 * @details Save state + performance tips.
 */
class Metropolis
{

  private:

    //! Current configuration
    libconfig::Config mConfig;
    //! Random number generator
    gsl_rng *rng;
    //! Stop flag
    bool fstop;

    //! List of factors
    std::vector<std::string> factors;
    //! List of ratings
    std::vector<std::string> ratings;
    //! Number of factors
    size_t nfactors;
    //! Number of ratings
    size_t nratings;
    //! Number of observations
    size_t nobs;
    //! Default probabilities
    std::vector<double> dprobs;
    //! Number of obligors
    std::vector<std::vector<int>> nobligors;
    //! Number of defaults
    std::vector<std::vector<int>> ndefaults;

    //! Degrees-of-freedom
    Component<double> nu;
    //! Factor loadings
    std::vector<Component<double>> W;
    //! Correlation coefficients
    std::vector<std::vector<Component<double>>> R;
    //! Normal latent variables
    std::vector<Component<std::vector<double>>> Z;
    //! Chi-squared latent variables
    std::vector<Component<double>> S;

    //! Inverse of correlation matrix [dim=nfactors.nfactors]
    gsl_matrix *M0, *M1;
    //! Determinant of sqrt(R)
    double det0, det1;
    //! Precomputed tcdfinv(nu) values [dim=nratings]
    std::vector<double> ti0, ti1;
    //! Precomputed lbinom values [dim=nobs.(nratings.nfactors)]
    std::vector<std::vector<double>> lb0, lb1;
    //! Precomputed qprod(z,M) values [dim=nobs]
    std::vector<double> mz0, mz1;
    //! Precomputed (nu/2-1)*log(s) [dim=nobs]
    std::vector<double> cs0, cs1;

  private:

    //! Binomial part
    static double lbinom(int n, int k, double a, double ndf, double w, double z, double s);
    //! Determinant of a Cholesky matrix
    static double det(gsl_matrix *L);
    //! Quadratic prod (v'.M.v)
    static double qprod(const std::vector<double> &v, const gsl_matrix *M);
    //! Return correlations as gsl matrix
    void v2m(gsl_matrix *m) const;
    //! Update factor loadings
    void updateW();
    void updateW(size_t s);
    //! Update correlations
    void updateR();
    void updateR(size_t s1, size_t s2);
    //! Update normal latent variables
    void updateZ();
    void updateZ(size_t);
    //! Update chi-squared variables
    void updateS();
    void updateS(size_t);
    //! Update degrees of freedom
    void updateNu();

    //! Check for duplicates
    template<typename T>
    bool hasDuplicates(const std::vector<T> &v);
    //! Read array values from a configuration item.
    template<typename T>
    std::vector<T> readArray(const libconfig::Setting &setting);
    template<typename T>
    std::vector<T> readArray(const libconfig::Setting &setting, size_t);
    //! Read matrix values from a configuration item
    template<typename T>
    std::vector<std::vector<T>> readMatrix(const libconfig::Setting &setting);
    template<typename T>
    std::vector<std::vector<T>> readMatrix(const libconfig::Setting &setting, size_t, size_t);
    //! Check if array constains the given value
    template<typename T>
    bool containsValue(const std::vector<T> &v, const T &value);
    //! Create setting
    libconfig::Setting& createSetting(const std::string &name, libconfig::Setting &parent, enum libconfig::Setting::Type type);
    //! Write array to the configuration file
    template<typename T>
    libconfig::Setting& writeArray(const std::string &name, libconfig::Setting &parent, enum libconfig::Setting::Type type, const std::vector<T> &values);
    template<typename T>
    libconfig::Setting& writeMatrix(const std::string &name, libconfig::Setting &parent, enum libconfig::Setting::Type type, const std::vector<std::vector<T>> &values);

    //! Read factors from configuration file
    void readFactors(const libconfig::Config &config);
    //! Read ratings + dprobs from configuration file
    void readRatings(const libconfig::Config &config);
    //! Read nobligors from configuration file
    void readNobligors(const libconfig::Config &config);
    //! Read ndefaults from configuration file
    void readNdefaults(const libconfig::Config &config);
    //! Read nu.XXX from configuration file
    void readNu(const libconfig::Config &config);
    //! Read W.XXX from configuration file
    void readW(const libconfig::Config &config);
    //! Read R.XXX from configuration file
    void readR(const libconfig::Config &config);
    //! Read Z.XXX from configuration file
    void readZ(const libconfig::Config &config);
    //! Read S.XXX from configuration file
    void readS(const libconfig::Config &config);

    //! Set component names as header
    void writeHeader(std::ostream &s) const;
    //! Write values
    void writeValues(std::ostream &s) const;
    //! Write acceptance rates
    void writeAcRates(std::ostream &s) const;
    //! Write scale parameters
    void writeParams(std::ostream &s) const;

  public:

    //! Constructor
    Metropolis(unsigned long seed);
    //! Destructor
    ~Metropolis();
    //! Initializes object
    void init(const libconfig::Config &config);
    //! Run simulation
    int run(std::ostream &output, std::ostream &info, size_t blocksize, size_t maxiters_, size_t burnin);
    //! Returns current state
    libconfig::Config& getState();
    //! Stops current run
    void stop() { fstop = true; }

};

} // namespace
