
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#pragma once

#include <stdexcept>
#include <iostream>
#include <string>

namespace ccruncher {

/**************************************************************************//**
 * @brief   CCruncher's exception.
 *
 * @details It is derived from std::exception an can be instantiate using
 *          another exception (preserving its message).
 *
 * @see     http://www.cplusplus.com/reference/exception/exception/
 */
class Exception : public std::exception
{

  protected:

    //! Exception message.
    std::string msg;

  public:

    //! Default constructor.
    Exception(const std::string &str="") noexcept;
    //! Constructor.
    Exception(const std::exception &e) noexcept;
    //! Constructor.
    Exception(const std::exception &e, const std::string &str) noexcept;
    //! Destructor.
    virtual ~Exception() override {}
    //! Returns exception message.
    virtual const char * what() const noexcept override;
    //! Returns exception message.
    const std::string & toString() const noexcept;

};

//! Output operator.
std::ostream& operator << (std::ostream& os, const Exception &e);

} // namespace
