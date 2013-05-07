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
/*!\file    rrlib/buffer_pools/policies/recycling/StoreOwnerInUniquePointer.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains StoreOwnerInUniquePointer
 *
 * \b StoreOwnerInUniquePointer
 *
 * The owner pool is stored in the Deleter of the unique_ptr obtained from GetUnusedBuffer().
 * Thus, the object itself does not need to store a pointer to the pool.
 * However, because the Deleter has a non-zero size, buffer obtained from this pool
 * cannot (efficiently) be enqueued in other concurrent queues.
 *
 * Pro: any C++ type can be used in pool
 * Con: unique_ptr have size of two pointers => they are not suitable for use in concurrent queues
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__recycling__StoreOwnerInUniquePointer_h__
#define __rrlib__buffer_pools__policies__recycling__StoreOwnerInUniquePointer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace buffer_pools
{
namespace recycling
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Stores owner information in unique_ptrs only.
/*!
 * The owner pool is stored in the Deleter of the unique_ptr obtained from GetUnusedBuffer().
 * Thus, the object itself does not need to store a pointer to the pool.
 * However, because the Deleter has a non-zero size, buffer obtained from this pool
 * cannot (efficiently) be enqueued in other concurrent queues.
 *
 * Pro: Memory consumption: Any C++ type can be used in pool directly (needs to be queueable type for QueueBased strategy though)
 * Con: unique_ptr have size of two pointers => they are not suitable for use in concurrent queues
 */
template <typename T, typename TBufferManagementPolicy>
class StoreOwnerInUniquePointer
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef T tManagedType;
  typedef std::unique_ptr<T, StoreOwnerInUniquePointer> tPointer;

  StoreOwnerInUniquePointer() : buffer_management_info() {}
  StoreOwnerInUniquePointer(const tBufferManagementInfo& info) : buffer_management_info(info) {}

  void operator()(T* p) const
  {
    TBufferManagementPolicy::RecycleBuffer(buffer_management_info, p);
  }

  static tPointer AddBuffer(TBufferManagementPolicy& buffer_management, std::unique_ptr<tManagedType> && buffer)
  {
    tBufferManagementInfo info;
    buffer_management.AddBuffer(buffer.get(), info);
    return tPointer(buffer.release(), StoreOwnerInUniquePointer(info));
  }

  static tPointer GetUnusedBuffer(TBufferManagementPolicy& buffer_management)
  {
    tBufferManagementInfo info;
    T* unused_buffer = buffer_management.GetUnusedBuffer(info);
    return tPointer(unused_buffer, StoreOwnerInUniquePointer(info));
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Buffer pool that buffer belongs to */
  tBufferManagementInfo buffer_management_info;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
