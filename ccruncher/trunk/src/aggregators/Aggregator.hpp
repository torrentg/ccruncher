
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
// Aggregator.hpp - Aggregator header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Aggregator_
#define _Aggregator_

//---------------------------------------------------------------------------

#include <string>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "interests/Interests.hpp"
#include "segmentations/Segmentations.hpp"
#include "ratings/Ratings.hpp"
#include "portfolio/Client.hpp"
#include "SegmentAggregator.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregator
{

  private:

    string name;
    int isegmentation;
    components_t components;
    bool bvalues; // true=values, false=ratings
    bool bfull;

    long numsegments;
    SegmentAggregator *saggregators;

    void reset();
    void parseProperty(const DOMNode&, Segmentations *) throw(Exception);
    void parseDOMNode(const DOMNode&, Segmentations *) throw(Exception);  
    void validate(Segmentations *) throw(Exception);


  public:

    Aggregator();
    Aggregator(const DOMNode &, Segmentations *) throw(Exception);
    ~Aggregator();

    string getName() const;
    int getISegmentation() const;
    bool getBValues() const;
    bool getBFull() const;

    void initialize(Segmentations *, Date *, int, vector<Client *> *, long, int, Ratings *) throw(Exception);
    void setOutputProperties(string path, bool force, int buffersize) throw(Exception);
    void append(int **rpaths, int, long, vector<Client *> *) throw(Exception);
    void flush(bool finalize) throw(Exception);
    void touch() throw(Exception);
};

//---------------------------------------------------------------------------

// comparation operator
bool operator == (const Aggregator&, const Aggregator&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
