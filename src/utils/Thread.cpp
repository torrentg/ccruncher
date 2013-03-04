
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
  pthread_mutex_init(&mutex, NULL);
  setStatus(fresh);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Thread::~Thread()
{
  cancel();

  if (getStatus() != fresh)
  {
    pthread_detach(thread);
  }

  pthread_mutex_destroy(&mutex);
}

//===========================================================================
// launcher (static)
//===========================================================================
void* ccruncher::Thread::launcher(void *d)
{
  Thread *x = static_cast<Thread *>(d);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  try 
  {
    x->setStatus(running);
    x->run();
    x->setStatus(finished);
  }
  catch(...) 
  {
    x->setStatus(aborted);
    throw;
  }

  return d;
}

//===========================================================================
// start
//===========================================================================
void ccruncher::Thread::start()
{
  if (getStatus() == running) return;
  setStatus(running);
  int rc = pthread_create(&thread, NULL, Thread::launcher, (void*)this);
  if (rc != 0) setStatus(aborted);
}

//===========================================================================
// wait
//===========================================================================
void ccruncher::Thread::wait()
{
  if (getStatus() == running)
  {
    int rc = pthread_join(thread, NULL);
    if (rc != 0) setStatus(aborted);
    else setStatus(finished);
  }
}

//===========================================================================
// cancel
//===========================================================================
void ccruncher::Thread::cancel()
{
  if (getStatus() == running)
  {
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    setStatus(cancelled);
  }
}

//===========================================================================
// getStatus
//===========================================================================
ccruncher::Thread::ThreadStatus ccruncher::Thread::getStatus() const
{
  pthread_mutex_lock(&mutex);
  ThreadStatus s = status;
  pthread_mutex_unlock(&mutex);
  return s;
}

//===========================================================================
// setStatus
//===========================================================================
void ccruncher::Thread::setStatus(ThreadStatus s)
{
  pthread_mutex_lock(&mutex);
  status = s;
  pthread_mutex_unlock(&mutex);
}

