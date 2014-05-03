
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

#ifndef _Rating_
#define _Rating_

#include <string>

namespace ccruncher {

/**************************************************************************//**
 * @brief Object representing a rating.
 *
 * @see http://ccruncher.net/ifileref.html#ratings
 */
class Rating
{

  private:

    //! Rating name
    std::string mName;
    //! Rating description
    std::string mDesc;

  public:

    //! Constructor
    Rating(const std::string &name="", const std::string &desc="");
    //! Returns the rating name
    const std::string & getName() const;
    //! Sets the rating name
    void setName(const std::string &name);
    //! Returns the rating description
    const std::string & getDescription() const;
    //! Sets the rating description
    void setDescription(const std::string &desc);

};

/**************************************************************************//**
 * @return Rating name.
 */
inline const std::string& ccruncher::Rating::getName() const
{
  return mName;
}

/**************************************************************************//**
 * @return Rating description.
 */
inline const std::string& ccruncher::Rating::getDescription() const
{
  return mDesc;
}

} // namespace

#endif

