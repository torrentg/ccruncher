
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Segmentation : public ExpatHandlers
{

  public:

    enum ComponentsType
    {
      asset,
      obligor,
      undefined
    };

  private:

    // list of segments
    std::vector<std::string> vsegments;
    // enabled flag (true by default)
    bool enabled;

  private:
  
    // inserts a segment into the list
    int insertSegment(const std::string &) throw(Exception);
    // check name
    bool isValidName(const std::string &);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);

  public:

    // segmentation name
    std::string name;
    // type of components (obligors/assets)
    ComponentsType components;

  public:
  
    // constructor
    Segmentation();
    // return the number of segments
    int size() const;
    // returns i-th segment
    const std::string& getSegment(int i) const;
    // return the index of the given segment
    int indexOfSegment(const std::string &sname) const throw(Exception);
    // return the index of the given segment
    int indexOfSegment(const char *sname) const throw(Exception);
    // reset object content
    void reset();
    // returns enabled flag
    bool isEnabled() const;
    // return filename
    std::string getFilename(const std::string &path) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
