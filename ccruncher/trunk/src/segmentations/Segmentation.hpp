
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Segmentation.hpp - Segmentation header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added method getNumSegments
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class refactoring
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
//===========================================================================

#ifndef _Segmentation_
#define _Segmentation_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "segmentations/Segment.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

enum components_t {
  asset,
  borrower,
  undefined
};

//---------------------------------------------------------------------------

class Segmentation : public ExpatHandlers
{

  private:

    // list of segments
    vector<Segment> vsegments;
    // if generic pattern (eg: *) -> modificable=true, false otherwise
    bool modificable;

    // inserts a segment into the list
    void insertSegment(const Segment &) throw(Exception);


  public:

    // segmentation order
    int order;
    // segmentation name
    string name;
    // type of components (borrowers/assets)
    components_t components;

    // constructor
    Segmentation();
    // destructor
    ~Segmentation();

    // return the number of segments
    int size() const;
    // [] operator
    Segment& operator [] (int i);
    // [] operator
    Segment& operator [] (const string &sname) throw(Exception);
    // add a segment to list
    void addSegment(const string segname) throw(Exception);
    // serialize object content as xml
    string getXML(int) const throw(Exception);
    // reset object content
    void reset();

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
