
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

#ifndef _Component_
#define _Component_

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "utils/Exception.hpp"

namespace ccruncher_inf {

/**************************************************************************//**
 * @brief   Variable estimated by the Metropolis-Hastings algorithm.
 *
 * @details Includes the current variable value, the desired acceptance
 *          level and the Robins-Monro procedure to attain this level.
 */
template <class T>
class Component
{

  private:

    //! Component name.
    std::string name;
    //! Circular queue (used to compute the acceptance rate).
    std::vector<bool> accepted;
    //! Current position in the circular queue.
    int pos;
    //! Desired acceptance rate (0 <= level < 1, =0 means no param update)
    double level;
    //! counter used by adapter
    int counter;

  private:

    //! Adaptive scaling param.
    void adapt(bool);

  public:

    //! Current state.
    T value;
    //! Tunning parameter (param>=0, if param=0 -> value not updated)
    double sigma;

  public:

    //! Default constructor.
    Component();
    //! Initialize component.
    void init(const std::string &, const T &v, double p, double l) throw(ccruncher::Exception);
    //! Resize circular queue.
    void resize(size_t n);
    //! check if this component is fixed.
    bool isFixed() const { return (sigma == 0.0); }
    //! Adds an observation.
    void setAccepted(bool);
    //! Returns acceptance rate.
    double getAccRate() const;
    //! Reset adapter.
    void reset();

};

//===========================================================================
// constructor
//===========================================================================
template <class T>
Component<T>::Component() : name("?"), accepted(0,false), pos(-1), level(NAN),
  counter(0), value(T()), sigma(NAN)
{
  // nothing to do
}

//===========================================================================
// init
//===========================================================================
template <class T>
void Component<T>::init(const std::string &str, const T &v, double s, double l) throw(ccruncher::Exception)
{
  name = str;
  value = v;

  if (s < 0.0) throw ccruncher::Exception("param out of range");
  else sigma = s;

  if (l < 0.0 || 1.0 <= l) throw ccruncher::Exception("level out of range");
  else level = l;

  if (level <= 0.0) counter = 0;
  else counter = (int)(5.0/(level*(1.0-level)));
}

//===========================================================================
// resize
//===========================================================================
template <class T>
void Component<T>::resize(size_t n)
{
  accepted.resize(n, false);
  pos = n-1;
}

//===========================================================================
// setAccepted
//===========================================================================
template <class T>
void Component<T>::setAccepted(bool b)
{
  adapt(b);

  if (accepted.size() > 0) {
    pos = (pos+1)%accepted.size();
    accepted[pos] = b;
  }
}

//===========================================================================
// getAccRate
//===========================================================================
template <class T>
double Component<T>::getAccRate() const
{
  if (accepted.size() > 0) {
    double ret = 0.0;
    for(unsigned int i=0; i<accepted.size(); i++) {
      if (accepted[i]) {
        ret += 1.0;
      }
    }
    return ret/accepted.size();
  }
  else {
    return NAN;
  }
}

//===========================================================================
// adaptive scaling param
// see 'Adaptative Optimal Scaling of Metropolis-Hastings Algorithms Using the Robbins-Monro process'
//===========================================================================
template <class T>
void Component<T>::adapt(bool b)
{
  if (level > 0.0)
  {
    counter++;
    if (b) {
      sigma *= 1.0 + 1.0/(level*counter);
    }
    else {
      sigma *= 1.0 - 1.0/((1.0-level)*counter);
    }
  }
}

//===========================================================================
// reset adapter
//===========================================================================
template <class T>
void Component<T>::reset()
{
  if (level <= 0.0) counter = 0;
  else counter = (int)(5.0/(level*(1.0-level)));
  if (sigma < 0.1) sigma = 0.1;
}

} // namespace

#endif

