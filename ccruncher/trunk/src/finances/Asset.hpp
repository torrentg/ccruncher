
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
// Asset.hpp - Asset header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#ifndef _Asset_
#define _Asset_

//---------------------------------------------------------------------------

#include <map>
#include "interests/Interests.hpp"
#include "segmentations/Segmentations.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/XMLUtils.hpp"
#include "DateValues.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------


class Asset
{

  private:

    map<int,int> belongsto;
    string id;
    DateValues *vevents;
    int nevents;
    Interests *interests;

    double getVCashFlow(Date date1, Date date2, DateValues *events, int n);
    double getVExposure(Date date1, Date date2, DateValues *events, int n);
    void insertBelongsTo(int iconcept, int tsegment) throw(Exception);


  protected:
  
    virtual DateValues* getIEvents() throw(Exception) = 0;
    virtual int getISize() throw(Exception) = 0;


  public:

    Asset();
    virtual ~Asset();

    static Asset* getInstanceByClassName(string) throw(Exception);
    static Asset* parseDOMNode(const DOMNode &, Segmentations *, Interests *) 
                  throw(Exception);

    virtual void setProperty(string name, string value) throw(Exception) = 0;
    void setInterests(Interests *);
    void setId(string id_);
    void addBelongsTo(int iconcept, int isegment) throw(Exception);

    virtual bool validate() = 0;
    void initialize() throw(Exception);
    void getVertexes(Date *dates, int n, DateValues *ret);
    string getId(void);
    int getSize();
    DateValues *getEvents();
    bool belongsTo(int iconcept, int isegment);
    int getSegment(int iconcept);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
