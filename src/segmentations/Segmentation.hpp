
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
// Segmentation.hpp - Segmentation header
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
//===========================================================================

#ifndef _Segmentation_
#define _Segmentation_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "segmentations/Segment.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

enum components_t {
  asset,
  client,
  undefined
};

//---------------------------------------------------------------------------

class Segmentation : public ExpatHandlers
{

  private:

    vector<Segment> vsegments;
    bool modificable;

    void insertSegment(Segment &) throw(Exception);


  public:

    string name;
    components_t components;

    Segmentation();
    ~Segmentation();

    vector<Segment> getSegments() const;
    int getSegment(string segname) const;
    string getSegmentName(int isegment) throw(Exception);

    void addSegment(string segname) throw(Exception);
    string getXML(int) throw(Exception);
    void reset();

    int getNumSegments();

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
