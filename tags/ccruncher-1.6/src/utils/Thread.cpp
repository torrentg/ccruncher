
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

#include "Thread.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Thread::Thread()
{
  status = fresh;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Thread::~Thread()
{
  cancel();
  
  if (status != fresh)
  {
    pthread_detach(thread);
  }
}

//===========================================================================
// launcher (static)
//===========================================================================
void* ccruncher::Thread::launcher(void *d)
{
  Thread *x = (Thread *)d;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  try 
  {
    x->status = running;
    x->run();
    x->status = finished;
  }
  catch(...) 
  {
    x->status = aborted;
    throw;
  }

  return d;
}

//===========================================================================
// start
//===========================================================================
void ccruncher::Thread::start()
{
  if (status == running) return;
  status = running;
  int rc = pthread_create(&thread, NULL, Thread::launcher, (void*)this);
  if (rc != 0) status = aborted;
}

//===========================================================================
// wait
//===========================================================================
void ccruncher::Thread::wait()
{
  if (status == running) 
  {
    int rc = pthread_join(thread, NULL);
    if (rc != 0) status = aborted;
    else status = finished;
  }
}

//===========================================================================
// cancel
//===========================================================================
void ccruncher::Thread::cancel()
{
  if (status == running) 
  {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    status = cancelled;
  }
}

//===========================================================================
// getStatus
//===========================================================================
ccruncher::ThreadStatus ccruncher::Thread::getStatus() const
{
  return status;
}

