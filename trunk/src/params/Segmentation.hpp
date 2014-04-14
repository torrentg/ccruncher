
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

#ifndef _Segmentation_
#define _Segmentation_

#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Portfolio segmentation.
 *
 * @details This class provides methods to read a segmentation from the
 *          xml input file. Offers a method to remove the unused segments
 *          in order to minimize the memory allocation/access in the simulation.
 *
 * @see http://ccruncher.net/ifileref.html#segmentations
 */
class Segmentation : public ExpatHandlers
{

  public:

    //! Type of segmentation's components
    enum ComponentsType
    {
      asset,     //!< Segmentation of assets
      obligor,   //!< Segmentation of obligors
      undefined  //!< Not defined
    };

  private:

    //! List of segments
    std::vector<std::string> segments;
    //! Number of components per segment
    std::vector<size_t> numcomponents;
    //! Recode map
    std::vector<int> recode_map;
    //! Enabled flag (true by default)
    bool enabled;

  private:
  
    //! Inserts a segment into the list
    void insertSegment(const std::string &);
    //! Check name
    bool isValidName(const std::string &);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Segmentation name
    std::string name;
    //! Type of components (obligors/assets)
    ComponentsType components;

  public:
  
    //! Constructor
    Segmentation();
    //! Number of segments
    int size() const;
    //! Returns i-th segment name
    const std::string& getSegment(int i) const;
    //! Return the index of the given segment
    int indexOfSegment(const std::string &sname) const;
    //! Return the index of the given segment
    int indexOfSegment(const char *sname) const;
    //! Reset object content
    void reset();
    //! Returns enabled flag
    bool isEnabled() const;
    //! Return filename
    std::string getFilename(const std::string &path) const;
    //! Add components to segmentations stats
    void addComponent(int);
    //! Remove unused segments
    void removeUnusedSegments();
    //! Recode segment (all segments -> removed unused segments)
    int recode(int) const;

};

} // namespace

#endif

