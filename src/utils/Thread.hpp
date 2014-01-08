
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

#ifndef _Thread_
#define _Thread_

//---------------------------------------------------------------------------

#include <pthread.h>

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Thread
{

  public:

    // thread status
    enum ThreadStatus
    {
      fresh,     // thread not started
      running,   // running thread (see start method)
      aborted,   // aborted thread (problem during execution)
      cancelled, // cancelled thread (see cabcel method)
      finished   // finished thread (run method finish)
    };

  private:
  
    // thread object
    pthread_t thread;
    // thread status
    ThreadStatus status;
    // ensures data consistence
    mutable pthread_mutex_t mutex;

  private:

    // thread launcher
    static void* launcher(void *d);
    // non-copyable class
    Thread(const Thread &);
    // non-copyable class
    Thread& operator=(const Thread &);
    // set thread status
    void setStatus(ThreadStatus s);

  protected:

    // constructor
    Thread();
    // destructor
    virtual ~Thread();

  public:

    // executes main procedure as a thread
    virtual void run() = 0;
    // starts the thread
    void start();
    // blocks until thread termination
    void wait();
    // cancel the running thread
    void cancel();
    // returns thread status
    ThreadStatus getStatus() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
