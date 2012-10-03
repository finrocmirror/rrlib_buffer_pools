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
/*!\file    rrlib/buffer_pools/policies/recycling/UseBufferContainer.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains UseBufferContainer
 *
 * \b UseBufferContainer
 *
 * The owner pool is stored in a container the buffer is allocated inside.
 * Buffers that are added, must be allocated as tBufferContainer<T>.
 *
 * Pro: unique_ptr have size of single pointer => they are suitable for use in concurrent queues
 * Con: T must be movable. Buffers are one pointer larger than they could be.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__recycling__UseBufferContainer_h__
#define __rrlib__buffer_pools__policies__recycling__UseBufferContainer_h__

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
 * The owner pool is stored in a container the buffer is allocated inside.
 * Buffers that are added must be allocated as tBufferContainer<T>.
 *
 * Pro: unique_ptr have size of single pointer => they are suitable for use in concurrent queues
 * Con: Buffers must be allocated as tBufferContainer<T> (unsuitable for factories). Buffers are one pointer larger than they could be.
 */
template <typename T, typename TBufferManagementPolicy>
class UseBufferContainer
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef tBufferContainer<T> tManagedType;
  typedef std::unique_ptr<T, UseBufferContainer> tPointer;

  size_t GetOffset(tBufferContainer<T>* buffer = NULL) const
  {
    return ((char*)&buffer->GetData()) - ((char*)buffer);
  }

  void operator()(T* p) const
  {
    tBufferContainer<T>* buffer = reinterpret_cast<tBufferContainer<T>*>(((char*)p) - GetOffset());
    TBufferManagementPolicy::RecycleBuffer(*buffer, buffer);
  }

  static tPointer AddBuffer(TBufferManagementPolicy& buffer_management, std::unique_ptr<tManagedType> && buffer)
  {
    buffer_management.AddBuffer(buffer.get(), *buffer);
    return tPointer(&(buffer.release()->GetData()));
  }

  static tPointer GetUnusedBuffer(TBufferManagementPolicy& buffer_management)
  {
    tBufferManagementInfo info;
    tManagedType* buffer = buffer_management.GetUnusedBuffer(info);
    return tPointer(buffer ? & (buffer->GetData()) : NULL);
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
