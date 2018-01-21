
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

#include <string>
#include <vector>
#include "portfolio/Obligor.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Portfolio segmentation.
 *
 * @details A segmentation is a list of segments. The segment 'unassigned'
 *          is created by default.
 *
 * @see http://ccruncher.net/ifileref.html#segmentations
 */
class Segmentation
{

  private:

    //! Segmentation name
    std::string mName;
    //! Enabled flag (true by default)
    bool mEnabled;
    //! List of segmentation segments
    std::vector<std::string> mSegments;

  private:
  
    //! Checks segment/segmentation name
    bool isValidName(const std::string &) const;

  public:
  
    //! Constructor
    Segmentation(const std::string &name="", bool enabled=true, bool hasUnassigned=true);
    //! Returns segmentation name
    const std::string& getName() const { return mName; }
    //! Sets segmentation name
    void setName(const std::string &);
    //! Returns enabled flag
    bool isEnabled() const { return mEnabled; }
    //! Sets the enabled flag
    void setEnabled(bool b);
    //! Number of segments
    unsigned short size() const { return (unsigned short) mSegments.size(); }
    //! Returns i-th segment name
    const std::string& getSegment(unsigned short i) const;
    //! Adds a segment to this segmentation
    void addSegment(const std::string &segment);
    //! Return the index of the given segment
    unsigned short indexOfSegment(const std::string &segment) const;
    //! Return the index of the given segment
    unsigned short indexOfSegment(const char *segment) const;
    //! Return the filename where segmentation simulation values are placed
    std::string getFilename(const std::string &path) const;

};

} // namespace
