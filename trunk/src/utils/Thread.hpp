
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

#ifndef _Thread_
#define _Thread_

#include <thread>

namespace ccruncher {

/**************************************************************************//**
 * @brief   Base class to create a basic thread class.
 *
 * @details User extends this class implementing the Thread::run() method.
 *          Launch the defined task in a new thread calling Thread::start().
 */
class Thread
{

  private:

    //! Thread object
    std::thread mThread;

  private:

    //! Thread launcher
    static void launcher(Thread *) noexcept;

  protected:

    //! Constructor
    Thread() {}
    //! Non-copyable class
    Thread(const Thread &) = delete;
    //! Non-copyable class
    Thread& operator=(const Thread &) = delete;
    //! Destructor
    virtual ~Thread();

  public:

    //! Main procedure
    virtual void run() = 0;
    //! Starts the thread
    void start();
    //! Blocks until thread termination
    void join();

};

} // namespace

#endif

