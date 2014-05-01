
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _SimulatedData_
#define _SimulatedData_

#include "portfolio/DateValues.hpp"
#include "portfolio/Obligor.hpp"
#include "portfolio/LGD.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Simulated obligor.
 *
 * @details Unlike Obligor class, this class only contains the strictly
 *          required data to do the simulation.
 */
class SimulatedObligor
{

  public:

    //! Obligor's factor index
    unsigned char ifactor;
    //! Obligor's rating index
    unsigned char irating;
    //! Number of active assets
    unsigned short numassets;
    //! obligor in initialization, lgd otherwise
    union UnionType
    {
      //! Obligor's lgd
      LGD lgd;
      //! Obligor's pointer
      Obligor *obligor;
      //! Union constructor
      UnionType() { new(&lgd) LGD(); }
    } ref;

  public:

    //! Constructor
    SimulatedObligor(Obligor *obligor=nullptr);
    //! Less-than operator
    bool operator<(const SimulatedObligor &obj) const;

};

/**************************************************************************//**
 * @brief Simulated asset.
 *
 * @details Unlike Asset class, this class only contains the strictly
 *          required data to do the simulation.
 */
class SimulatedAsset
{

  public:

    //! Minimum event date
    Date mindate;
    //! Maximum event date
    Date maxdate;
    //! Allocated datevalues
    DateValues *begin;
    //! Allocated datevalues
    DateValues *end;

  public:

    //! Constructor
    SimulatedAsset(Asset *asset=nullptr);
    //! Copy constructor
    SimulatedAsset(const SimulatedAsset &o);
    //! Destructor
    ~SimulatedAsset();
    //! Assignment operator
    SimulatedAsset& operator=(const SimulatedAsset &o);
    //! Initialize content
    void assign(Asset *asset);
    //! Deallocate memory
    void free();

};

} // namespace

#endif

