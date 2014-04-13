
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

#ifndef _Segmentations_
#define _Segmentations_

#include <string>
#include <vector>
#include "params/Segmentation.hpp"
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

// forward declaration
class Asset;

/**************************************************************************//**
 * @brief List of segmentations.
 *
 * @see http://ccruncher.net/ifileref.html#segmentations
 */
class Segmentations : public ExpatHandlers
{

  private:

    //! List of enabled segmentations
    std::vector<Segmentation> enabled;
    //! List of disabled segmentations
    std::vector<Segmentation> disabled;
    //! Auxiliary variable (used by parser)
    Segmentation auxsegmentation;

  private:
  
    //! Insert a segmentation to list
    int insertSegmentation(Segmentation &) throw(Exception);
    //! Validate object content
    void validate() throw(Exception);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
  
  public:

    //! Constructor
    Segmentations() {}
    //! Number of enabled segmentations
    int size() const;
    //! [] operator (i>=0 -> enabled, i<0 -> disabled)
    const Segmentation& getSegmentation(int i) const;
    //! Return the index of the given segmentation
    int indexOfSegmentation(const std::string &sname) const throw(Exception);
    //! Return the index of the given segmentation
    int indexOfSegmentation(const char *sname) const throw(Exception);
    //! Add components to segmentations stats
    void addComponents(const Asset *);
    //! Remove unused segments
    void removeUnusedSegments();
    //! Recode segments removing unused segments
    void recodeSegments(Asset *);

};

} // namespace

#endif

