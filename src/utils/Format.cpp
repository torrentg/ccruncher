
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <iomanip>
#include "utils/Format.hpp"

using namespace std;

/**************************************************************************//**
 * @details Returns bytes as string (B, KB, MB, GB)
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::bytes(const size_t val)
{
  std::ostringstream oss;

  if (val < 1024) {
    oss << val << " B";
  }
  else if (val < 1024*1024) {
    double kbs = double(val)/double(1024);
    oss << fixed << setprecision(2) << kbs << " KB";
  }
  else {
    double mbs = double(val)/double(1024*1024);
    oss << fixed << setprecision(2) << mbs << " MB";
  }

  return oss.str();
}

