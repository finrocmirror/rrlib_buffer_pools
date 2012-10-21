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
/*!\file    rrlib/buffer_pools/tBufferManagementInfo.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains tBufferManagementInfo
 *
 * \b tBufferManagementInfo
 *
 * Stores information on buffer pool a buffer originates from.
 * Required for some recycling policies.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__tBufferManagementInfo_h__
#define __rrlib__buffer_pools__tBufferManagementInfo_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/concurrent_containers/tConcurrency.h"

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
namespace management
{
template <typename T, concurrent_containers::tConcurrency CONCURRENCY, typename>
class ArrayAndFlagBased;

template <typename T, concurrent_containers::tConcurrency CONCURRENCY>
class QueueBased;
}

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Buffer management info
/*!
 * Stores information required for buffer management.
 * UseBufferContainer recycling policy requires that a type T derives from this class.
 * The content is determined and interpreted by buffer management policy.
 */
class tBufferManagementInfo
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tBufferManagementInfo() : buffer_management_info(NULL) {}

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  template <typename T, concurrent_containers::tConcurrency CONCURRENCY, typename>
  friend class management::ArrayAndFlagBased;

  template <typename T, concurrent_containers::tConcurrency CONCURRENCY>
  friend class management::QueueBased;

  /*!
   * Information set and interpreted by buffer management policy.
   * The buffer management policy can choose to use either of union members.
   */
  union
  {
    void* buffer_management_info;
    //std::atomic<size_t> buffer_management_info_atomic;  // TODO: Does uncommenting this influence performance?
  };
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
