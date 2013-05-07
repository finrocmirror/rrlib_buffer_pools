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
/*!\file    rrlib/buffer_pools/tBufferPool.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-28
 *
 * \brief   Contains tBufferPool
 *
 * \b tBufferPool
 *
 * This class manages a pool of reusable buffers.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__tBufferPool_h__
#define __rrlib__buffer_pools__tBufferPool_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/logging/messages.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tBufferContainer.h"
#include "rrlib/buffer_pools/policies/deleting/CollectGarbage.h"
#include "rrlib/buffer_pools/policies/deleting/ComplainOnMissingBuffers.h"
#include "rrlib/buffer_pools/policies/management/ArrayAndFlagBased.h"
#include "rrlib/buffer_pools/policies/management/QueueBased.h"
#include "rrlib/buffer_pools/policies/recycling/StoreOwnerInUniquePointer.h"
#include "rrlib/buffer_pools/policies/recycling/UseOwnerStorageInBuffer.h"
#include "rrlib/buffer_pools/policies/recycling/UseBufferContainer.h"

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
// Class declaration
//----------------------------------------------------------------------
//! This class manages a pool of reusable buffers.
/*!
 * This class manages a pool of reusable buffers.
 * Buffers obtained from this class are stored in std::unique_ptr pointers (tBufferPool::tPointer).
 * Buffers are returned to their pool when these pointers go out of scope.
 * It is recommandable to use this type, as it avoids memory leaks and makes code exception safe.
 *
 * Sometimes, it is not suitable to have buffers in a unique_ptr though.
 * In these cases, it is possible to switch to manual buffer recycling -
 * obtaining the raw pointers via unique_ptr::release().
 * Buffers can be recycled later by temporarily placing them in a tBufferPool::tPointer again.
 *
 * The buffer pool is implemented lock-free.
 *
 * (As they allocate memory internally, we opted not to return shared_ptrs from this class.
 *  Memory allocation typically being an issue with respect to real-time code and efficiency,
 *  this did not appear to be a good idea).
 *
 * T  Type of buffers
 * CONCURRENCY  specifies if threads can return (write) and retrieve (read) buffers from the pool concurrently.
 * TBufferManagementPolicy  Determines how buffers are managed
 * TDeletingPolicy  If buffers are still in use when a buffer pool is deleted, they will cause a segmentation violation
 *                  when finally recycled. If this can occur, alternative deleting policies should be used.
 * TRecycling  Determines how buffers are (automatically) recycled. The main thing about this is where information
 *             about the originating buffer pool is stored. The Recycler determines what kinds of types T can be used,
 *             how unique pointer obtained from this class look like, and whether buffers can be added to concurrent queues.
 *             Custom recycling policies can make a lot of sense.
 * TBufferDeleter
 * TBufferManagementPolicyArgs  Any additional arguments for the BufferManagementPolicy (apart from T and CONCURRENCY)
 */
template < typename T,
         concurrent_containers::tConcurrency CONCURRENCY,
         template <typename, concurrent_containers::tConcurrency, typename ...> class TBufferManagementPolicy = management::QueueBased,
         template <typename> class TDeletingPolicy = deleting::ComplainOnMissingBuffers,
         template <typename, typename> class TRecycling = recycling::StoreOwnerInUniquePointer,
         typename TBufferDeleter = std::default_delete<typename TRecycling<T, int>::tManagedType>,
         typename... TBufferManagementPolicyArgs >
class tBufferPool : boost::noncopyable
{
//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! Buffer management backend */
  typedef TBufferManagementPolicy<typename TRecycling<T, int>::tManagedType, CONCURRENCY, TBufferDeleter, TBufferManagementPolicyArgs...> tBufferManagement;

  /*! Recycling policy */
  typedef TRecycling<T, tBufferManagement> tRecycler;

  /*!
   * Pointer type to preferably use to operate with buffers in client code.
   * It is a std::unique_ptr with custom deleter that returns buffers to pool
   * instead of deleting them when going out of scope.
   * It is highly recommandable to use this type, as it avoids memory leaks and
   * makes code very exception safe.
   */
  typedef std::unique_ptr<T, tRecycler> tPointer;

  /*!
   * Type of Buffer actually managed in the backend.
   * Added buffers need to be of this type.
   * Usually, this is T - except of when using UseBufferContainer recycling policy.
   */
  typedef typename TRecycling<T, int>::tManagedType tManagedType;


  tBufferPool() :
    buffer_management()
  {
  }

  /*!
   * Add new buffer to pool.
   * Naturally, a buffer may only be added to one pool.
   *
   * \param buffer Buffer to add. unique_ptr is empty after call. tManagedType type is T with most recycling policies.
   * \return Buffer reference. May be used as unused buffer reference immediately (otherwise its automatically recycled by tPointer)
   */
  tPointer AddBuffer(std::unique_ptr<tManagedType> && buffer)
  {
    assert(buffer);
    return tRecycler::AddBuffer(buffer_management.GetBufferManagement(), std::forward<std::unique_ptr<tManagedType>>(buffer));
  }

  /*!
   * Obtain pointer to unused buffer in pool.
   * The buffer will be marked in use as long as the returned unique_ptr
   * refers to it.
   *
   * Sometimes, it is not suitable to have buffers in a unique_ptr though.
   * In these cases, it is possible to switch to manual buffer recycling -
   * obtaining the raw pointers via unique_ptr::release().
   * Buffers can be recycled later by temporarily placing them in a tBufferPool::tPointer again.
   *
   * \return Unused Buffer - Null if there is no unused buffer in pool
   */
  tPointer GetUnusedBuffer()
  {
    return tRecycler::GetUnusedBuffer(buffer_management.GetBufferManagement());
  }

  /*!
   * \return Returns internal buffer management backend for special manual tweaking of
   * buffer pool. In most cases, it should not be necessary to access internals.
   */
  tBufferManagement& InternalBufferManagement()
  {
    return buffer_management.GetBufferManagement();
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Buffer Pool backend */
  TDeletingPolicy<tBufferManagement> buffer_management;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
