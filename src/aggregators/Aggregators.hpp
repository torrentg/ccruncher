
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
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#ifndef _Aggregators_
#define _Aggregators_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <algorithm>
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "segmentations/Segmentations.hpp"
#include "ratings/Ratings.hpp"
#include "interests/Interests.hpp"
#include "portfolio/Client.hpp"
#include "aggregators/Aggregator.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregators : public ExpatHandlers
{

  private:

    vector<Aggregator> vaggregators;
    Segmentations *segmentations;
    Aggregator auxaggregator;

    void insertAggregator(Aggregator &) throw(Exception);
    void validate() throw(Exception);


  public:

    Aggregators(Segmentations *);
    ~Aggregators();

    vector<Aggregator> * getAggregators();
    void setOutputProperties(string path, bool force, int buffersize) throw(Exception);
    void initialize(Date *, int, vector<Client *> *, int, int, Ratings*, Interests *) throw(Exception);
    void append(int **rpaths, int, long, vector<Client *> *) throw(Exception);
    void flush(bool finalize) throw(Exception);
    void touch() throw(Exception);
    string getXML(int) throw(Exception);
    long getNumSegments() throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
