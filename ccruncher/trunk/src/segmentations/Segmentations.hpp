
//***************************************************************************
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
// Segmentations.hpp - Segmentations header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#ifndef _Segmentations_
#define _Segmentations_

//---------------------------------------------------------------------------

#include <algorithm>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "Segmentation.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Segmentations
{

  private:

    vector<Segmentation> vsegmentations;

    void parseDOMNode(const DOMNode&) throw(Exception);
    void insertSegmentation(Segmentation &) throw(Exception);
    void validate() throw(Exception);


  public:

    Segmentations() {};
    Segmentations(const DOMNode &) throw(Exception);
    ~Segmentations();

    vector<Segmentation> getSegmentations();
    int getSegmentation(string name);
    char getComponents(string name);
    char getComponents(int iseg);
    int getSegment(string segmentation, string segment);
    string getSegmentationName(int isegmentation) throw(Exception);
    string getSegmentName(int isegmentation, int isegment) throw(Exception);

    void addSegment(string segmentation, string segment) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
