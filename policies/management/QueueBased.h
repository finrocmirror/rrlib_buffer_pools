//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/buffer_pools/policies/management/QueueBased.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains QueueBased
 *
 * \b QueueBased
 *
 * Buffer management based on collecting unused buffers in a concurrent queue.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__management__QueueBased_h__
#define __rrlib__buffer_pools__policies__management__QueueBased_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/concurrent_containers/tQueue.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tNotifyOnRecycle.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace buffer_pools
{
namespace management
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Queue-based buffer management
/*!
 * Buffer management based on collecting unused buffers in a concurrent queue.
 *
 * Pro: Scales well with many buffers
 * Con: Types T must be queueable => memory overhead & possibly difficult to achieve
 */
template <typename T, concurrent_containers::tConcurrency CONCURRENCY, typename TBufferDeleter>
class QueueBased
{

  static_assert(std::is_base_of<concurrent_containers::queue::tQueueableMost, T>::value ||
                (CONCURRENCY == concurrent_containers::tConcurrency::NONE && std::is_base_of<concurrent_containers::queue::tQueueableSingleThreaded, T>::value),
                "Only queueable types may be used with queue-based policy. Choosing UseBufferContainer Recycling policy might be an alternative.");

  /*! Pointer type used in internal queue (we don't want any auto-recycling here) */
  typedef std::unique_ptr<T, TBufferDeleter> tQueuePointer;

  /*!
   * Type of queue backend.
   * TODO: maybe we should make dequeue mode choosable.
   * Using fast queue implementation will reduce computational overhead.
   * On the other hand there is always one additional unused buffer in pool.
   */
  typedef concurrent_containers::tQueue<tQueuePointer, CONCURRENCY, concurrent_containers::tDequeueMode::FIFO_FAST> tQueueType;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  QueueBased() :
    unused_buffers(),
    buffer_count(0)
  {}

  void AddBuffer(T* buffer, tBufferManagementInfo& info)
  {
    buffer_count++;
    info.buffer_management_info = this;
  }

  /*!
   * \return Number of buffers that have not been returned yet
   */
  int DeleteGarbage()
  {
    bool success = true;
    while (true)
    {
      unused_buffers.Dequeue(success);
      if (!success)
      {
        break;
      }
      buffer_count--;
    }
    return buffer_count - tQueueType::cMINIMUM_ELEMENTS_IN_QEUEUE;
  }

  T* GetUnusedBuffer(tBufferManagementInfo& info)
  {
    info.buffer_management_info = this;
    return unused_buffers.Dequeue().release();
  }

  static void RecycleBuffer(const tBufferManagementInfo& info, T* buffer)
  {
    assert(info.buffer_management_info && "Received empty buffer_management_info. This is not allowed using this policy.");
    QueueBased* owner_pool = static_cast<QueueBased*>(info.buffer_management_info);
    NotifyOnRecycle(buffer);
    owner_pool->unused_buffers.Enqueue(tQueuePointer(buffer));
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Queue containing the unused buffers of this pool */
  tQueueType unused_buffers;

  /*! Number of buffers in this pool */
  std::atomic<int> buffer_count;


  static inline void NotifyOnRecycle(void*) {}
  static inline void NotifyOnRecycle(tNotifyOnRecycle* recycled)
  {
    static_cast<T*>(recycled)->OnRecycle();
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
