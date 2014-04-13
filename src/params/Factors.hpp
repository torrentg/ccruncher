
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _Factors_
#define _Factors_

#include <string>
#include <vector>
#include <cmath>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief List of factors.
 *
 * @see http://ccruncher.net/ifileref.html#factors
 */
class Factors : public ExpatHandlers
{

  private:

    //! Internal struct
    struct Factor
    {
        //! Factor name
        std::string name;
        //! Factor description
        std::string desc;
        //! Factor loading
        double loading;
        //! Constructor
        Factor(const std::string &n="", const std::string &d="", double v=NAN) : name(n), desc(d), loading(v) {}
    };

  private:

    //! List of factors
    std::vector<Factor> vfactors;

  private:
  
    //! Add a factor to list
    void insertFactor(const Factor &) throw(Exception);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
  
  public:

    //! Default constructor
    Factors() {}
    //! Return the number of factors
    int size() const;
    //! Return the index of the factor
    int getIndex(const char *name) const;
    //! Return the index of the factor
    int getIndex(const std::string &name) const;
    //! Return the i-th factor name
    const std::string& getName(int i) const;
    //! Return the i-th factor description
    const std::string& getDescription(int i) const;
    //! Return the i-th factor loading
    double getLoading(int i) const;
    //! Return factor loadings
    std::vector<double> getLoadings() const;

};

} // namespace

#endif

