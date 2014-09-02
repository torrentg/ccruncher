
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

    //! Segmentation name
    std::string mName;
    //! Type of components (obligors/assets)
    ComponentsType mType;
    //! Enabled flag (true by default)
    bool mEnabled;
    //! List of segmentation segments
    std::vector<std::string> mSegments;
    //! Number of elements per segment (used by recode)
    std::vector<size_t> mNumElements;
    //! Recode map (used by recode)
    std::vector<size_t> mRecodeMap;

  private:
  
    //! Checks segment/segmentation name
    bool isValidName(const std::string &);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:
  
    //! Constructor
    Segmentation(const std::string &name="", ComponentsType type=asset, bool enabled=true);
    //! Reset object content
    void reset();
    //! Returns segmentation name
    const std::string& getName() const { return mName; }
    //! Sets segmentation name
    void setName(const std::string &);
    //! Returns type of components
    ComponentsType getType() const { return mType; }
    //! Sets the type of components
    void setType(const ComponentsType &);
    //! Returns enabled flag
    bool isEnabled() const { return mEnabled; }
    //! Sets the enabled flag
    void setEnabled(bool b);
    //! Number of segments
    int size() const { return mSegments.size(); }
    //! Returns i-th segment name
    const std::string& getSegment(size_t i) const { return mSegments[i]; }
    //! Adds a segment to this segmentation
    void add(const std::string &segment);
    //! Return the index of the given segment
    size_t indexOf(const std::string &segment) const;
    //! Return the index of the given segment
    size_t indexOf(const char *segment) const;
    //! Return filename
    std::string getFilename(const std::string &path) const;
    //! Add components to segmentations stats
    void increaseSegmentCounter(size_t);
    //! Recodes segments
    void recode();
    //! Recodes a segment
    size_t recode(size_t) const;

};

} // namespace

#endif

