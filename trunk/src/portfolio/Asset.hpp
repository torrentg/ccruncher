
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

#ifndef _Asset_
#define _Asset_

#include <vector>
#include "portfolio/LGD.hpp"
#include "portfolio/DateValues.hpp"
#include "params/Interest.hpp"
#include "params/Segmentations.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Asset with credit risk.
 *
 * @details Asset is described in terms of LGD and EAD. If the number of
 *          assets is high (eg. 10000) then memory is used to preserve
 *          unused data (like DateValues out of the analyzed range or
 *          data that is duplicated). For this reason, unused data is
 *          deleted when they are no longer needed.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Asset : public ExpatHandlers
{

  private:

    //! Segmentation-segment relations
    std::vector<int> vsegments;
    //! Asset identifier
    std::string id;
    //! Asset creation date
    Date date;
    //! EAD-LGD values
    std::vector<DateValues> data;
    //! Default LGD
    LGD dlgd;
    //! Pointer to segmentations list (used by parser)
    Segmentations *segmentations;
    //! Auxiliary variable (used by parser)
    bool have_data;

  protected:
  
    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    //! Constructor
    Asset(Segmentations *);
    //! Return asset identifier
    const std::string& getId() const { return id; }
    //! Add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    //! Prepare data
    void prepare(const Date &d1, const Date &d2, const Interest &interest);
    //! Check if this asset belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment) const;
    //! Return the asset's segment of the given segmentation
    int getSegment(int isegmentation) const;
    //! Set the given segment to segmentation
    void setSegment(int isegmentation, int isegment);
    //! Indicates if this asset has info in date1-date2
    bool isActive(const Date &, const Date &);
    //! Returns minimum event date (restricted to prepared events)
    Date getMinDate() const;
    //! Returns maximum event date (restricted to prepared events)
    Date getMaxDate() const;
    //! Used to test SimulationThread::simule lower_bound
    const DateValues& getValues(const Date t) const;
    //! Indicates if this asset use obligor lgd
    bool hasObligorLGD() const;
    //! Returns reference to data
    const std::vector<DateValues>& getData() const;
    //! Clears data
    void clearData();

};

} // namespace

#endif

