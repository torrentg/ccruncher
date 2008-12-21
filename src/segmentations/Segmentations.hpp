
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
// Segmentations.hpp - Segmentations header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/21 - Gerard Torrent [gerard@mail.generacio.com]
//   . added methods getNum*
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . class refactoring
//
//===========================================================================

#ifndef _Segmentations_
#define _Segmentations_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"
#include "segmentations/Segmentation.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Segmentations : public ExpatHandlers
{

  private:

    // list of segmentations
    vector<Segmentation> vsegmentations;
    // auxiliary variable (used by parser)
    Segmentation auxsegmentation;

    // insert a segmentation to list
    void insertSegmentation(Segmentation &) throw(Exception);
    // validate object content
    void validate() throw(Exception);


  public:

    // constructor
    Segmentations();
    // destructor
    ~Segmentations();

    // return the number of segments
    int size() const;
    // [] operator
    Segmentation& operator [] (int i);
    // [] operator
    Segmentation& operator [] (const string &name) throw(Exception);
    // add a segmentation-segment
    void addSegment(const string segmentation, const string segment) throw(Exception);
    // serialize object content as xml
    string getXML(int) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------