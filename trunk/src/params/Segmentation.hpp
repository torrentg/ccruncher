
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

enum components_t {
  asset,
  obligor,
  undefined
};

//---------------------------------------------------------------------------

class Segmentation : public ExpatHandlers
{

  private:

    // list of segments
    vector<string> vsegments;
    // enabled flag (true by default)
    bool enabled;

  private:
  
    // inserts a segment into the list
    int insertSegment(const string &) throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // segmentation name
    string name;
    // type of components (obligors/assets)
    components_t components;

  public:
  
    // constructor
    Segmentation();
    // return the number of segments
    int size() const;
    // returns i-th segment
    const string& getSegment(int i);
    // return the index of the given segment
    int indexOfSegment(const string &sname) throw(Exception);
    // return the index of the given segment
    int indexOfSegment(const char *sname) throw(Exception);
    // serialize object content as xml
    string getXML(int) const throw(Exception);
    // reset object content
    void reset();
    // returns enabled flag
    bool isEnabled() const;
    // return filename
    string getFilename(const string &path) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
