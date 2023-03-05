
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include <cassert>
#include "Thread.hpp"

using namespace std;

/**************************************************************************//**
 * @details If there is a running task, this task will be stoped.
 *
 */
ccruncher::Thread::~Thread()
{
  assert(!mThread.joinable());
}

/**************************************************************************//**
 * @details Internal method used to launch threads.
 * @param[in] x Thread to launch.
 */
void ccruncher::Thread::launcher(Thread *x) noexcept
{
  try {
    assert(x != nullptr);
    x->run();
  }
  catch(...) {
    // nothing to do
  }
}

/**************************************************************************//**
 * @details If you want to execute the main class procedure in the current
 *          execution thread use Thread::run(). This method executes the
 *          Thread::run() method in a new thread.
 * @exception system_error System is unable to create a new thread.
 */
void ccruncher::Thread::start()
{
  assert(!mThread.joinable());
  mThread = std::thread(Thread::launcher, this);
}

/**************************************************************************//**
* @exception system_error Invalid thread usage.
*/
void ccruncher::Thread::join()
{
  if (mThread.joinable()) {
    mThread.join();
  }
}

