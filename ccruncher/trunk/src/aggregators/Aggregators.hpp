
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
// Aggregators.hpp - Aggregators header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Aggregators_
#define _Aggregators_

//---------------------------------------------------------------------------

#include <algorithm>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "segmentations/Segmentations.hpp"
#include "ratings/Ratings.hpp"
#include "interests/Interests.hpp"
#include "portfolio/Client.hpp"
#include "Aggregator.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregators
{

  private:

    vector<Aggregator> vaggregators;

    void parseDOMNode(const DOMNode&, Segmentations *) throw(Exception);
    void insertAggregator(Aggregator &) throw(Exception);
    void validate() throw(Exception);


  public:

    Aggregators(const DOMNode &, Segmentations *) throw(Exception);
    ~Aggregators();

    vector<Aggregator> * getAggregators();
    void setOutputProperties(string path, bool force, int buffersize) throw(Exception);
    void initialize(Segmentations *, Date *, int, vector<Client *> *, int, int, Ratings*) throw(Exception);
    void append(int **rpaths, int, long, vector<Client *> *) throw(Exception);
    void flush(bool finalize) throw(Exception);
    void touch() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
