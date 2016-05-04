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
/*!\file    rrlib/buffer_pools/tBufferContainer.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains tBufferContainer
 *
 * \b tBufferContainer
 *
 * Container with management information for buffer pool buffer.
 * Required for UseBufferContainer recycling policy.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__tBufferContainer_h__
#define __rrlib__buffer_pools__tBufferContainer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/concurrent_containers/tQueueable.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tBufferManagementInfo.h"

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
//! Buffer container with management information.
/*!
 * Container with management information for buffer pool buffer.
 * Required for UseBufferContainer recycling policy.
 */
template <typename T>
class tBufferContainer : public tBufferManagementInfo, public concurrent_containers::tQueueable<concurrent_containers::tQueueability::MOST_OPTIMIZED>
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param args Forwards all parameters to constructor of type T
   */
  template <typename... TArgs>
  tBufferContainer(TArgs && ... args) : buffer(std::forward<TArgs>(args)...) {}

  /*!
   * \return Wrapped Buffer
   */
  T& GetData()
  {
    return buffer;
  }

  /*!
   * \return Offset of buffer in tBufferContainer object
   */
  static size_t GetBufferOffset()
  {
    tBufferContainer<T>* container = nullptr;
    return reinterpret_cast<char*>(&container->buffer) - reinterpret_cast<char*>(container);
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Wrapped buffer */
  T buffer;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
