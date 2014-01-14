
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

#include <pthread.h>

namespace ccruncher {

/**************************************************************************//**
 * @brief   Base class to create a posix thread class.
 *
 * @details User extends this class implementing the procedure to execute
 *          in a new thread in the Thread::run() method. He launch the task
 *          in a new thread calling Thread::start(). The rest of methods
 *          provides support to manage the running thread (stop, getStatus, etc.)
 */
class Thread
{

  public:

    //! Thread status
    enum ThreadStatus
    {
      fresh,     //!< Thread not started
      running,   //!< Running thread (see start method)
      aborted,   //!< Aborted thread (problem during execution)
      cancelled, //!< Cancelled thread (see cabcel method)
      finished   //!< Finished thread (run method finish)
    };

  private:
  
    //! Thread object
    pthread_t thread;
    //! Thread status
    ThreadStatus status;
    //! Ensures data consistence
    mutable pthread_mutex_t mutex;

  private:

    //! Thread launcher
    static void* launcher(void *d);

    //! Non-copyable class
    Thread(const Thread &);
    //! Non-copyable class
    Thread& operator=(const Thread &);
    //! Set thread status
    void setStatus(ThreadStatus s);

  protected:

    //! Constructor
    Thread();
    //! Destructor
    virtual ~Thread();

  public:

    //! Main procedure
    virtual void run() = 0;
    //! Starts the thread
    void start();
    //! Blocks until thread termination
    void wait();
    //! Cancel the running thread
    void cancel();
    //! Returns thread status
    ThreadStatus getStatus() const;

};

} // namespace

#endif

