
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

#ifndef _Segmentations_
#define _Segmentations_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"
#include "params/Segmentation.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Segmentations : public ExpatHandlers
{

  private:

    // list of enabled segmentations
    vector<Segmentation> enabled;
    // list of disabled segmentations
    vector<Segmentation> disabled;
    // auxiliary variable (used by parser)
    Segmentation auxsegmentation;

  private:
  
    // insert a segmentation to list
    int insertSegmentation(Segmentation &) throw(Exception);
    // validate object content
    void validate() throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
  
  public:

    // constructor
    Segmentations();
    // destructor
    ~Segmentations();
    // return the number of enabled segmentations
    int size() const;
    // [] operator (i>=0 -> enabled, i<0 -> disabled)
    Segmentation& getSegmentation(int i);
    // return the index of the given segmentation
    int indexOfSegmentation(const string &sname) throw(Exception);
    // return the index of the given segmentation
    int indexOfSegmentation(const char *sname) throw(Exception);
    // serialize object content as xml
    string getXML(int) const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
