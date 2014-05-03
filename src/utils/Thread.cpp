
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

#include "Thread.hpp"
#include <exception>
#include <cassert>

using namespace std;

/**************************************************************************/
ccruncher::Thread::Thread()
{
  setStatus(fresh);
}

/**************************************************************************//**
 * @details If there is a running task, this task will be stoped.
 */
ccruncher::Thread::~Thread()
{
  if (mThread.joinable()) {
    assert(false);
    mThread.detach();
  }
}

/**************************************************************************//**
 * @details Internal method used to launch threads.
 * @param[in] x Thread to launch.
 */
void ccruncher::Thread::launcher(Thread *x) noexcept
{
  try
  {
    assert(x != nullptr);
    x->setStatus(running);
    x->run();
    x->setStatus(finished);
  }
  catch(...) 
  {
    x->setStatus(aborted);
  }
}

/**************************************************************************//**
 * @details If you want to execute the main class procedure in the current
 *          execution thread use Thread::run(). This method executes the
 *          Thread::run() method in a new thread.
 */
void ccruncher::Thread::start()
{
  if (getStatus() == running) return;
  assert(!mThread.joinable());
  if (mThread.joinable()) return;
  try {
    setStatus(running);
    mThread = std::thread(Thread::launcher, this);
  }
  catch(std::exception &) {
    setStatus(aborted);
  }
}

/**************************************************************************/
void ccruncher::Thread::wait()
{
  if (getStatus() == running || mThread.joinable())
  {
    try {
      assert(mThread.joinable());
      mThread.join();
      setStatus(finished);
    }
    catch(std::exception &) {
      setStatus(aborted);
    }
  }
}

/**************************************************************************/
ccruncher::Thread::ThreadStatus ccruncher::Thread::getStatus() const
{
  mMutex.lock();
  ThreadStatus s = mStatus;
  mMutex.unlock();
  return s;
}

/**************************************************************************//**
 * @details This method avoids synchronization problems using a mutex.
 * @param s New thread status.
 */
void ccruncher::Thread::setStatus(ThreadStatus s)
{
  mMutex.lock();
  mStatus = s;
  mMutex.unlock();
}

