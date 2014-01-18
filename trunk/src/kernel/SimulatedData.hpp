
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
 * @brief Simulated asset.
 *
 * @details Unlike Asset class, this class only contains the strictly
 *          required data to do the simulation. The real size of this
 *          object is bigger than reported by sizeof(). Segments variable
 *          is not a unique value but a list of n elements (n = number of
 *          segmentations). The length is only know in execution time.
 *          We don't use allocated memory (eg. vector) for performance
 *          reasons.
 */
class SimulatedAsset
{

  private:

    //! Constructor (non-instanciable)
    SimulatedAsset() {}

  public:

    //! Minimum event date
    Date mindate;
    //! Maximum event date
    Date maxdate;
    //! Allocated datevalues
    DateValues *begin;
    //! Allocated datevalues
    DateValues *end;
    //! Segmentations indexes
    unsigned short segments;

  public:

    //! Initialize content
    void init(Asset *asset);
    //! Deallocate memory
    void free();

};

/**************************************************************************//**
 * @brief Simulated obligor.
 *
 * @details Unlike Obligor class, this class only contains the strictly
 *          required data to do the simulation. Variable ref contains
 *          (in the initialization stage) a reference to the obligor.
 *          Later, in the simulation stage, contains a reference to the first
 *          SimulatedAsset belonging by this Obligor. It is a void pointer
 *          to avoid the temptation to increase it (p++) -remind that
 *          SimulatedAsset has a real size distinct than reported by
 *          sizeof-.
 */
class SimulatedObligor
{

  public:

    //! Obligor's factor index
    unsigned char ifactor;
    //! Obligor's rating index
    unsigned char irating;
    //! Obligor's lgd
    LGD lgd;
    //! Number of assets
    unsigned short numassets;
    //! Data pointer
    union
    {
      //! obligor
      Obligor *obligor;
      //! simulated assets
      void *assets;
    } ref;

  public:

    //! Constructor
    SimulatedObligor(Obligor *obligor=NULL);
    //! Less-than operator
    bool operator<(const SimulatedObligor &obj) const;

};

} // namespace

#endif

