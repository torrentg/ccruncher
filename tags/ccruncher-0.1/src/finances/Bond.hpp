
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
// Bond.hpp - Bond header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Bond_
#define _Bond_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "Asset.hpp"
#include "DateValues.hpp"
#include "segmentations/Segmentations.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Bond : public Asset
{

  private:

    /** fecha de emision del bono */
    Date issuedate;
    /** plazo (en meses) del bono */
    int term;
    /** importe nominal del bono */
    double nominal;
    /** tipo interes anual de cada cupon (en tanto por 1) */
    double rate;
    /** numeros de cupones */
    int ncoupons;
    /** fecha de adquisicion */
    Date adquisitiondate;
    /** precio de compra */
    double adquisitionprice;

    bool filled;
    DateValues *simulate();
    int length;

  protected:

    DateValues* getIEvents() throw(Exception);
    int getISize() throw(Exception);


  public:

    Bond();
    ~Bond();

    void setProperty(string name, string value) throw(Exception);
    bool validate();
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------