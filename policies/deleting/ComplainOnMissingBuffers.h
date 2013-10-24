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
/*!\file    rrlib/buffer_pools/policies/deleting/ComplainOnMissingBuffers.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-30
 *
 * \brief   Contains ComplainOnMissingBuffers
 *
 * \b ComplainOnMissingBuffers
 *
 * Deleting policy that will complain if buffers are in use when pool is deleted.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__buffer_pools__policies__deleting__ComplainOnMissingBuffers_h__
#define __rrlib__buffer_pools__policies__deleting__ComplainOnMissingBuffers_h__

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
namespace deleting
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Complain if buffers are in use
/*!
 * Deleting policy that will complain if buffers are in use when pool is deleted.
 */
template <typename TBufferManagementPolicy>
class ComplainOnMissingBuffers : public TBufferManagementPolicy
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  ~ComplainOnMissingBuffers()
  {
    int missing_buffers = TBufferManagementPolicy::DeleteGarbage();
    if (missing_buffers > 0)
    {
      RRLIB_LOG_PRINT(ERROR, "At least ", missing_buffers, " buffers have not been returned to buffer pool. This will result in segmentation violations when the remaining buffers are recycled.\
                             If you cannot ensure that all buffers are returned, use different deleting policy.");
    }
  }

  TBufferManagementPolicy& GetBufferManagement()
  {
    return *this;
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
