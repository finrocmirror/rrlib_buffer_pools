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
/*!\file    rrlib/buffer_pools/policies/recycling/UseOwnerStorageInBuffer.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains UseOwnerStorageInBuffer
 *
 * \b UseOwnerStorageInBuffer
 *
 * The owner pool is stored in the buffer itself.
 * In order for this to work, the type T must be derived from tBufferPoolInfoStorage<...>.
 *
 * Pro: unique_ptr have size of single pointer => they are suitable for use in concurrent queues
 * Con: T must derive from tBufferPoolInfoStorage<...>. T is one pointer larger than it could be.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__recycling__UseOwnerStorageInBuffer_h__
#define __rrlib__buffer_pools__policies__recycling__UseOwnerStorageInBuffer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

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
//! SHORT_DESCRIPTION
/*!
 * Use owner storage in buffer. In order for this to work, the type T must be derived
 * from tBufferManagementInfo.
 */
template <typename T, typename TBufferManagementPolicy>
class UseOwnerStorageInBuffer
{
  static_assert(std::is_base_of<tBufferManagementInfo, T>::value, "Type T must be subclass of tBufferManagementInfo for this policy.");

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef T tManagedType;
  typedef std::unique_ptr<T, UseOwnerStorageInBuffer> tPointer;

  void operator()(T* p) const
  {
    tBufferManagementInfo& info_storage = static_cast<tBufferManagementInfo&>(*p);
    TBufferManagementPolicy::RecycleBuffer(info_storage, p);
  }

  static tPointer AddBuffer(TBufferManagementPolicy& buffer_management, std::unique_ptr<tManagedType> && buffer)
  {
    buffer_management.AddBuffer(buffer.get(), *buffer);
    return tPointer(buffer.release());
  }

  static tPointer GetUnusedBuffer(TBufferManagementPolicy& buffer_management)
  {
    tBufferManagementInfo info;
    return tPointer(buffer_management.GetUnusedBuffer(info));
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
