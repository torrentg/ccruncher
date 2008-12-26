
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2008 Gerard Torrent
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
// toms708.h - toms708 header - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _TOMS708_
#define _TOMS708_

//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

  void bratio(double a, double b, double x, double y, double *w, double *w1, int *ierr, int log_p);

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

