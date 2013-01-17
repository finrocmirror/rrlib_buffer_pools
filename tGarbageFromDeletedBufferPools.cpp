//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/buffer_pools/tGarbageFromDeletedBufferPools.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 */
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tGarbageFromDeletedBufferPools.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tThread.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace buffer_pools
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
namespace internal
{
struct tDeletionList
{
  /*! Mutex to synchronize access on list */
  rrlib::thread::tMutex mutex;

  /*! List with pools that have not been completely deleted yet */
  std::list<tGarbageFromDeletedBufferPools*> garbage_pools;

  ~tDeletionList()
  {
    tGarbageFromDeletedBufferPools::DeleteGarbage();
    if (garbage_pools.size())
    {
      RRLIB_LOG_PRINT_STATIC(WARNING, garbage_pools.size(), " buffer pools have not been completely deleted.");
    }
  }
};

typedef rrlib::design_patterns::tSingletonHolder<tDeletionList, rrlib::design_patterns::singleton::Longevity> tDeletionListInstance;
static inline unsigned int GetLongevity(tDeletionList*)
{
  return 0xFF000000; // should exist longer than any reusable (and longer than any thread objects)
}

}

void tGarbageFromDeletedBufferPools::AddPool(tGarbageFromDeletedBufferPools* pool)
{
  internal::tDeletionList& list = internal::tDeletionListInstance::Instance();
  thread::tLock lock(list.mutex);
  list.garbage_pools.push_back(pool);
}

void tGarbageFromDeletedBufferPools::DeleteGarbage()
{
  internal::tDeletionList& list = internal::tDeletionListInstance::Instance();
  thread::tLock lock(list.mutex);
  for (auto it = list.garbage_pools.begin(); it != list.garbage_pools.end(); ++it)
  {
    int remaining = (*it)->DeleteBufferPoolGarbage();
    if (remaining == 0)
    {
      delete *it;
      it = list.garbage_pools.erase(it);
      --it;
    }
  }
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
