
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _ccmpi_
#define _ccmpi_

//---------------------------------------------------------------------------

#include "utils/config.h"

#ifdef USE_MPI

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include <mpi.h>
#define MPI_VAL_WORK 1025   // work tag (see task variable)
#define MPI_VAL_STOP 1026   // stop tag (see task variable)
#define MPI_TAG_DATA 1027   // data tag (used to send results)
#define MPI_TAG_INFO 1028   // info tag (used to send results)
#define MPI_TAG_TASK 1029   // task tag (used to send results)
#define MPI_TAG_EXIT 1030   // task tag (used to send results)

#endif

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

